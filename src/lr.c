#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>

#include "lr.h"
#include "defs.h"
#include "threads.h"
#include "mem.h"
#include "msg.h"
#include "dna.h"
#include "buffer.h"
#include "levels.h"
#include "param.h"
#include "alphabet.h"
#include "common.h"
#include "cache.h"
#include "positions.h"
#include "pmodels.h"
#include "strings.h"
#include "cm.h"

LR_PARAMETERS *P; // FOR THREAD SHARING

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - P R I N T   R E G I O N   T O   F I L E - - - - - - - - - - - 

void PrintRegionToFile(uint64_t i, uint64_t e, uint32_t color, FILE *F)
  {
  fprintf(F, "%"PRIu64"\t%"PRIu64"\t%u\n", i, e, color);
  return;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - P R I N T   R E G I O N - - - - - - - - - - - - - - 

void PrintRegion(uint64_t i, uint64_t e, uint32_t color)
  {
  fprintf(stdout, "%"PRIu64"\t%"PRIu64"\t%u\n", i, e, color);
  return;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - P R I N T   H E A D E R - - - - - - - - - - - - - -

void PrintHeader(uint64_t idx, uint64_t size)
  {
  fprintf(stdout, "#Length %"PRIu64" (id: %"PRIu64" )\n", size, idx);
  return;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - C O M P R E S S O R   D N A - - - - - - - - - - - - - -

void CompressTargetDNA(THREADS T)
  {
  char        in_name [4096];
  char        out_name[4096];
  sprintf(in_name,  ".lrcr_%u.seq", T.id + 1);
  sprintf(out_name, ".lrcr_%u.inf", T.id + 1);
  FILE        *Reader  = Fopen(in_name,  "r");
  FILE        *Writter = Fopen(out_name, "w");
  uint32_t    n, k, cModel, totModels, idxPos;
  uint64_t    compressed = 0, nSymbols = 0, nBases = 0, i = 0;
  uint8_t     *readBUF, sym, irSym, *pos;
  PMODEL      **PM, *MX;
  FPMODEL     *PT;
  CMWEIGHT    *WM;
  CBUF        *symbBUF;
  CMODEL      **cModels;
  CACHE       *C;
  double      bits = 0, bps = 0;
  
  // EXTRA MODELS DERIVED FROM EDITS
  totModels = P->nModels;
  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].edits != 0)
      totModels += 1;

  PM            = (PMODEL  **) Calloc(totModels, sizeof(PMODEL *));
  for(n = 0 ; n < totModels ; ++n)
        PM[n]   = CreatePModel      (P->nSym);
  MX            = CreatePModel      (P->nSym);
  PT            = CreateFloatPModel (P->nSym);
  WM            = CreateWeightModel (totModels);

  readBUF  = (uint8_t *) Calloc(DEF_BUF_SIZE, sizeof(uint8_t));
  symbBUF  = CreateCBuffer(DEF_BUF_SIZE, DEF_BUF_GUARD);

  cModels = (CMODEL **) Malloc(P->nModels * sizeof(CMODEL *));
  for(n = 0 ; n < P->nModels ; ++n)
    cModels[n] = CreateCModel(P->model[n].ctx, P->model[n].den,
    TARGET, P->model[n].edits, P->model[n].eDen, P->nSym, 
    P->model[n].gamma, P->model[n].eGamma, P->model[n].ir, 
    P->model[n].eIr, P->model[n].memory);

  // GIVE SPECIFIC GAMMA:
  int pIdx = 0;
  for(n = 0 ; n < P->nModels ; ++n)
    {
    WM->gamma[pIdx++] = cModels[n]->gamma;
    if(P->model[n].edits != 0)
      WM->gamma[pIdx++] = cModels[n]->eGamma;
    }

  float dv = 2.0; 
  compressed = 0;
  while((k = fread(readBUF, 1, DEF_BUF_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos)
      {
      sym = toupper(readBUF[idxPos]);

      // FINAL FILTERING DNA CONTENT
      if(sym != 'A' && sym != 'C' && sym != 'G' && sym != 'T')
        {
        fprintf(Writter, "%lf\n", 2.0);  // FORCE HIGH COMPLEXITY
        continue;
        }

      symbBUF->buf[symbBUF->idx] = sym = DNASymToNum(sym);
      memset((void *)PT->freqs, 0, P->nSym * sizeof(double));

      n = 0;
      pos = &symbBUF->buf[symbBUF->idx-1];
      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
        {
        CMODEL *CM = cModels[cModel];
        GetPModelIdx(pos, CM);
        ComputePModel(CM, PM[n], CM->pModelIdx, CM->alphaDen);
        ComputeWeightedFreqs(WM->weight[n], PM[n], PT, CM->nSym);

        if(CM->edits != 0)
	  {
          ++n;
          CM->TM->seq->buf[CM->TM->seq->idx] = sym;
          CM->TM->idx = GetPModelIdxCorr(CM->TM->seq->buf+
          CM->TM->seq->idx-1, CM, CM->TM->idx);
          ComputePModel(CM, PM[n], CM->TM->idx, CM->TM->den);
          ComputeWeightedFreqs(WM->weight[n], PM[n], PT, CM->nSym);
          }
        ++n;
        }

      ComputeMXProbs(PT, MX, cModels[0]->nSym);

      bits += (bps = PModelNats(MX, sym) / M_LN2);
      fprintf(Writter, "%.3g\n", bps);
      //fwrite(&bps, sizeof(float), 1, Writter);
      CalcDecayment(WM, PM, sym);

      // ADD COUNTERS
      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
        {
        CMODEL *CM = cModels[cModel];
        switch(CM->ir)
	  {
          case 0:
          UpdateCModelCounter(CM, sym, CM->pModelIdx);
          break;
          case 1:
          UpdateCModelCounter(CM, sym, CM->pModelIdx);
          irSym = GetPModelIdxIR(symbBUF->buf+symbBUF->idx, CM);
          UpdateCModelCounter(CM, irSym, CM->pModelIdxIR);
          break;
          case 2:
          irSym = GetPModelIdxIR(symbBUF->buf+symbBUF->idx, CM);
          UpdateCModelCounter(CM, irSym, CM->pModelIdxIR);
          break;
          default:
          UpdateCModelCounter(CM, sym, CM->pModelIdx);
          break;
          }
        }

      // UPDATE INDEXES & SYM CACHE
      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
        {
        CMODEL *CM = cModels[cModel];
        if(CM->memory != 0)
	  {
          uint32_t mem_pos = (CM->M.pos) % CM->M.size;
          switch(CM->ir)
	    {
            case 0:
            CM->M.idx[mem_pos] = CM->pModelIdx;
            CM->M.sym[mem_pos] = sym;
            break;
            case 1:
            CM->M.idx[mem_pos]    = CM->pModelIdx;
            CM->M.sym[mem_pos]    = sym;
            CM->M.idx_ir[mem_pos] = CM->pModelIdxIR;
            CM->M.sym_ir[mem_pos] = irSym;
            break;
            case 2:
            CM->M.idx_ir[mem_pos] = CM->pModelIdxIR;
            CM->M.sym_ir[mem_pos] = irSym;
            break;
            default:
            PrintWarning("no store action!");
            exit(1);
            }
          }
        }

      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
	{
        CMODEL *CM = cModels[cModel];
        if(CM->memory != 0)
	  {
          uint32_t mps = (CM->M.pos + 1) % CM->M.size;
          if(compressed > CM->M.size)
	    {
            switch(CM->ir)
	      {
              case 0:
              UnUpdateCModelCounter(CM, CM->M.sym[mps], CM->M.idx[mps]);
              break;
              case 1:
              UnUpdateCModelCounter(CM, CM->M.sym   [mps], CM->M.idx   [mps]);
              UnUpdateCModelCounter(CM, CM->M.sym_ir[mps], CM->M.idx_ir[mps]);
              break;
              case 2:
              UnUpdateCModelCounter(CM, CM->M.sym_ir[mps], CM->M.idx_ir[mps]);
              break;
              default:
	      PrintWarning("no update action!");
              exit(1);
              }
            }
          }
        }

      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
	{
        CMODEL *CM = cModels[cModel];
        if(CM->memory != 0)
	  {
          if(CM->M.pos >= CM->M.size - 1) CM->M.pos = 0;
          else                            CM->M.pos++;
          }
        }

      RenormalizeWeights(WM);

      for(cModel = 0, n = 0 ; cModel < P->nModels ; ++cModel, ++n)
        if(cModels[cModel]->edits != 0)
          UpdateTolerantModel(cModels[cModel]->TM, PM[++n], sym);

      UpdateCBuffer(symbBUF);
      ++compressed;
      }

  fclose(Writter);
  Free(MX);
  
   for(n = 0 ; n < P->nModels ; ++n)
     RemoveCModel(cModels[n]);

  for(n = 0 ; n < totModels ; ++n){
    Free(PM[n]->freqs);
    Free(PM[n]);
    }
  Free(PM);
  Free(PT);
  Free(readBUF);
  RemoveCBuffer(symbBUF);
  fclose(Reader);

  return;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - C O M P R E S S O R   A A - - - - - - - - - - - - - - -
//
void CompressTargetAA(THREADS T)
  {
  char        in_name [4096];
  char        out_name[4096];
  sprintf(in_name,  ".lrcr_%u.seq", T.id + 1);
  sprintf(out_name, ".lrcr_%u.inf", T.id + 1);
  FILE        *Reader  = Fopen(in_name,  "r");
  FILE        *Writter = Fopen(out_name, "w");

  uint32_t    n, k, cModel, totModels, idxPos;
  uint64_t    compressed = 0, nSymbols = 0, nBases = 0, i = 0;
  uint8_t     *readBUF, sym, irSym, *pos;
  PMODEL      **PM, *MX;
  FPMODEL     *PT;
  CMWEIGHT    *WM;
  CBUF        *symbBUF;
  CMODEL      **cModels;
  CACHE       *C;
  double      bits = 0, bps = 0;
  
  // EXTRA MODELS DERIVED FROM EDITS
  totModels = P->nModels;
  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].edits != 0)
      totModels += 1;

  PM            = (PMODEL  **) Calloc(totModels, sizeof(PMODEL *));
  for(n = 0 ; n < totModels ; ++n)
        PM[n]   = CreatePModel      (P->nSym);
  MX            = CreatePModel      (P->nSym);
  PT            = CreateFloatPModel (P->nSym);
  WM            = CreateWeightModel (totModels);

  readBUF  = (uint8_t *) Calloc(DEF_BUF_SIZE, sizeof(uint8_t));
  symbBUF  = CreateCBuffer(DEF_BUF_SIZE, DEF_BUF_GUARD);

  cModels = (CMODEL **) Malloc(P->nModels * sizeof(CMODEL *));
  for(n = 0 ; n < P->nModels ; ++n)
    cModels[n] = CreateCModel(P->model[n].ctx, P->model[n].den,
    TARGET, P->model[n].edits, P->model[n].eDen, P->nSym, 
    P->model[n].gamma, P->model[n].eGamma, P->model[n].ir, 
    P->model[n].eIr, P->model[n].memory);
  
  // GIVE SPECIFIC GAMMA:
  int pIdx = 0;
  for(n = 0 ; n < P->nModels ; ++n)
    {
    WM->gamma[pIdx++] = cModels[n]->gamma;
    if(P->model[n].edits != 0)
      WM->gamma[pIdx++] = cModels[n]->eGamma;
    }

  compressed = 0;
  while((k = fread(readBUF, 1, DEF_BUF_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos)
      {
      sym = readBUF[idxPos];
      symbBUF->buf[symbBUF->idx] = sym = P->A->revMap[sym];
      
      memset((void *)PT->freqs, 0, P->nSym * sizeof(double));

      n = 0;
      pos = &symbBUF->buf[symbBUF->idx-1];
      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
        {
        CMODEL *CM = cModels[cModel];
        GetPModelIdx(pos, CM);
        ComputePModel(CM, PM[n], CM->pModelIdx, CM->alphaDen);
        ComputeWeightedFreqs(WM->weight[n], PM[n], PT, CM->nSym);

        if(CM->edits != 0)
	  {
          ++n;
          CM->TM->seq->buf[CM->TM->seq->idx] = sym;
          CM->TM->idx = GetPModelIdxCorr(CM->TM->seq->buf+
          CM->TM->seq->idx-1, CM, CM->TM->idx);
          ComputePModel(CM, PM[n], CM->TM->idx, CM->TM->den);
          ComputeWeightedFreqs(WM->weight[n], PM[n], PT, CM->nSym);
          }
        ++n;
        }

      ComputeMXProbs(PT, MX, P->nSym);
      bits += (bps = PModelNats(MX, sym) / M_LN2);

      fprintf(Writter, "%.3g\n", bps);
      CalcDecayment(WM, PM, sym);

      // ADD COUNTERS
      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
        {
        CMODEL *CM = cModels[cModel];
        switch(CM->ir)
	  {
          case 0:
          UpdateCModelCounter(CM, sym, CM->pModelIdx);
          break;
          case 1:
          UpdateCModelCounter(CM, sym, CM->pModelIdx);
          irSym = GetPModelIdxR(symbBUF->buf+symbBUF->idx, CM);
          UpdateCModelCounter(CM, irSym, CM->pModelIdxIR);
          break;
          case 2:
          irSym = GetPModelIdxR(symbBUF->buf+symbBUF->idx, CM);
          UpdateCModelCounter(CM, irSym, CM->pModelIdxIR);
          break;
          default:
          UpdateCModelCounter(CM, sym, CM->pModelIdx);
          break;
          }
        }

      // UPDATE INDEXES & SYM CACHE
      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
        {
        CMODEL *CM = cModels[cModel];
        if(CM->memory != 0)
	  {
          uint32_t mem_pos = (CM->M.pos) % CM->M.size;
          switch(CM->ir)
	    {
            case 0:
            CM->M.idx[mem_pos] = CM->pModelIdx;
            CM->M.sym[mem_pos] = sym;
            break;
            case 1:
            CM->M.idx[mem_pos]    = CM->pModelIdx;
            CM->M.sym[mem_pos]    = sym;
            CM->M.idx_ir[mem_pos] = CM->pModelIdxIR;
            CM->M.sym_ir[mem_pos] = irSym;
            break;
            case 2:
            CM->M.idx_ir[mem_pos] = CM->pModelIdxIR;
            CM->M.sym_ir[mem_pos] = irSym;
            break;
            default:
            PrintWarning("no store action!");
            exit(1);
            }
          }
        }

      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
	{
        CMODEL *CM = cModels[cModel];
        if(CM->memory != 0)
	  {
          uint32_t mps = (CM->M.pos + 1) % CM->M.size;
          if(compressed > CM->M.size)
            {
            switch(CM->ir)
	      {
              case 0:
              UnUpdateCModelCounter(CM, CM->M.sym[mps], CM->M.idx[mps]);
              break;
              case 1:
              UnUpdateCModelCounter(CM, CM->M.sym   [mps], CM->M.idx   [mps]);
              UnUpdateCModelCounter(CM, CM->M.sym_ir[mps], CM->M.idx_ir[mps]);
              break;
              case 2:
              UnUpdateCModelCounter(CM, CM->M.sym_ir[mps], CM->M.idx_ir[mps]);
              break;
              default:
	      PrintWarning("no update action!");
              exit(1);
              }
            }
          }
        }

      for(cModel = 0 ; cModel < P->nModels ; ++cModel)
	{
        CMODEL *CM = cModels[cModel];
        if(CM->memory != 0)
	  {
          if(CM->M.pos >= CM->M.size - 1) CM->M.pos = 0;
          else                            CM->M.pos++;
          }
        }

      RenormalizeWeights(WM);

      for(cModel = 0, n = 0 ; cModel < P->nModels ; ++cModel, ++n)
        if(cModels[cModel]->edits != 0)
          UpdateTolerantModel(cModels[cModel]->TM, PM[++n], sym);

      UpdateCBuffer(symbBUF);
      ++compressed;
      }

  fclose(Writter);
  Free(MX);
  
  for(n = 0 ; n < P->nModels ; ++n)
    RemoveCModel(cModels[n]);

  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(PM[n]);

  Free(PM);
  Free(PT);
  Free(readBUF);
  RemoveCBuffer(symbBUF);
  fclose(Reader);

  return;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - F   T H R E A D I N G - - - - - - - - - - - - - - -

void *CompressThread(void *Thr)
  {
  THREADS *T = (THREADS *) Thr;
  if(P->dna == 1) CompressTargetDNA(T[0]);
  else            CompressTargetAA (T[0]);
  pthread_exit(NULL);
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - C O M P R E S S O R   M A I N - - - - - - - - - - - -

void CompressAction(THREADS *T)
  {
  pthread_t t[2];
  uint32_t n;

  pthread_create(&(t[1]), NULL, CompressThread, (void *) &(T[0]));
  pthread_create(&(t[2]), NULL, CompressThread, (void *) &(T[1]));
    
  pthread_join(t[1], NULL);
  pthread_join(t[2], NULL);

  return;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - M O V I N G   A V E R A G E   F I L T E R - - - - - - - -
//
double MovAvgInstant(double *f, double *pS, uint32_t p, uint32_t l, double n)
  {
  *pS = *pS - f[p] + n;
  f[p] = n;
  return *pS / l;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - L O C A L   R E D U N D A N C Y - - - - - - - - - - - -
//
void LocalRedundancy(LR_PARAMETERS *MAP)
  {
  int32_t   n;
  uint32_t  k;
  uint64_t  totalBytes, totalSize;
  THREADS   *T;

  T = (THREADS *) Calloc(2, sizeof(THREADS));
  T[0].id = 0;
  T[1].id = 1;

  // ALPHABET ==================================================================

  P = *&MAP;

  uint64_t x = 0;

  if(P->verbose) PrintMessage("Loading alphabet ...");
  P->A = CreateAlphabet();
  LoadAlphabet(P->A, P->filename);
  P->nSym = P->A->cardinality;
  if(P->verbose) PrintAlphabet(P->A);

  if(P->dna == 1)
    {
    P->nSym = 4;
    if(P->verbose)
      PrintMessage("Adapting alphabet to 4 symbols {ACGT} (flag --dna on)");
    }
  
  if(P->threshold == 0)
    P->threshold = (double) log2(P->nSym) / 2.0;
  if(P->verbose)
    fprintf(stderr, "[>] Threshold: %.3lf\n", P->threshold);

  // ===========================================================================

  if(P->verbose) PrintMessage("Spliting and reversing streams ...");

  FILE *IN   = Fopen(P->filename, "r");
  FILE *OUT1 = Fopen(".lrcr_1.seq", "w");
  FILE *OUT2 = Fopen(".lrcr_2.seq", "w");

  int sym;
  uint64_t nValues = 0;
  while((sym = getc(IN)) != EOF){
    if(sym == '>') // HEADER FOUND!
      while((sym = fgetc(IN)) != EOF && sym != '\n')
        ;
    if(sym == EOF) break;
    if(sym == '\n') continue;
    fprintf(OUT1, "%c", sym);
    ++nValues;
    }
  fclose(IN);
  fclose(OUT1);

  FILE *IN2 = Fopen(".lrcr_1.seq", "r");
  uint64_t nBytes = NBytesInFile(IN2);
  
  if(P->verbose) fprintf(stderr, "[>] Number of bytes: %"PRIu64"\n", nBytes);

  fseek(IN2, -1L, 2);
  while(nBytes--){
    sym = fgetc(IN2); //this moves the logical pointer ahead be 1 char
    fputc(sym, OUT2);
    fseek(IN2, -2L, 1);
    }

  fclose(IN2);
  fclose(OUT2);

  if(P->verbose) PrintMessage("Done!");

  // COMPRESSING ==============================================================
  // 
 
  if(P->verbose) PrintMessage("Compressing streams ...");
  
  CompressAction(T);
  
  remove(".lrcr_1.seq");
  remove(".lrcr_2.seq");

  if(P->verbose) PrintMessage("Done!");

  // GET THE MINIMUM OF LR & RL DIRECTIONS, FILTER & SEGMENT ==================
  //
  
  FILE *IN_LR = Fopen(".lrcr_1.inf", "r");
  FILE *IN_RL = Fopen(".lrcr_2.inf", "r");
  FILE *P_MIN = Fopen(".lrcr_1_2.inf", "w");
  FILE *PROF  = Fopen(Cat(P->filename, ".info"), "w");

  fseek(IN_RL, 0, SEEK_END);
  char line_LR[1024];
  char line_RL[1024];

  if(P->verbose) fprintf(stderr, "[>] Computing the minimum profile ...\n");
  
  while(Fgets_backwards(line_RL, 1024, IN_RL) != NULL)
    { 
    Reverse(line_RL);	  
    if(!fgets(line_LR, 1024, IN_LR))
      {
      PrintWarning("information files have been changed!");
      exit(1);
      }
    float RL = atof(line_RL);
    float LR = atof(line_LR);
    float min = RL < LR ? RL : LR;
    fprintf(P_MIN, "%.3g\n", min);
    }
  fclose(P_MIN);
  fclose(IN_RL);
  fclose(IN_LR);

  remove(".lrcr_1.inf");
  remove(".lrcr_2.inf");

  if(P->verbose) fprintf(stderr, "[>] Smoothing the minimum profile ...\n");

  uint64_t idx = 0;
  double   newAvg = 0;
  double   sum = 0;
  char     buffer[1024];
  double   *filt = (double *) Calloc(P->window + 1, sizeof(double));
  FILE     *MIN_IN = Fopen(".lrcr_1_2.inf", "r");

  while(fgets(buffer, 1024, MIN_IN))
    {
    newAvg = MovAvgInstant(filt, &sum, idx, P->window, atof(buffer));
    fprintf(PROF, "%.3g\n", (float) newAvg);
    if(++idx == P->window) idx = 0;
    }
  fclose(MIN_IN);
  fclose(PROF);
  
  remove(".lrcr_1_2.inf");

  if(P->verbose) fprintf(stderr, "[>] Segmenting the regions ...\n");

  FILE *PROF_IN = Fopen(Cat(P->filename, ".info"), "r");
  if(!P->nosize && P->renormalize == 0) 
    fprintf(stdout, "#Length\t%"PRIu64"\n", nValues);
 
  int region;
  double smooth, min;
  uint64_t initPos = 1;

  POS *PO = CreatePositions(10000);

  if(fgets(buffer, 1024, PROF_IN))
    {
    min = atof(buffer);
    region = min < P->threshold ? 0 : 1;
    }

  idx = 1; 
  while(fgets(buffer, 1024, PROF_IN))
    {
    ++idx;
    smooth = atof(buffer);
    if(smooth >= P->threshold) // LOW REGION = 0, HIGH REGION = 1
      {
      if(region == 0)
        {              
        region = 1; 
        if(idx - initPos > P->ignore)
	  {	
          if(MAP->renormalize != 1)
	    PrintRegion(initPos, idx, P->color);
          UpdatePositions(PO, initPos, idx);
	  }
        }
      }
    else // val < threshold ====> LOW_REGION
      {
      if(region == 1)
        {
        region  = 0;
        initPos = idx;
        }
      }
    }

  if(region == 0)
    {
    if(idx - initPos > P->ignore)
      {
      if(MAP->renormalize != 1)
	PrintRegion(initPos, idx, P->color);
      UpdatePositions(PO, initPos, idx);
      }
    }

  fclose(PROF_IN);

  if(P->hide) remove(Cat(P->filename, ".info"));

  if(MAP->mask == 1) // MASK FASTA
    {
    if(P->verbose) fprintf(stderr, "[>] Masking sequence ...\n");
    
    FILE *IN  = Fopen(P->filename,   "r");
    FILE *OUT = Fopen(P->outputmask, "w");

    int sym;
    PO->idx = 0;
    uint64_t position = 1;
    while((sym = getc(IN)) != EOF)
      {
      if(sym == '>')
        { // HEADER FOUND!
	fprintf(OUT, "%c", sym);
        while((sym = fgetc(IN)) != EOF && sym != '\n')
          fprintf(OUT, "%c", sym);
	}
      
      if(sym == EOF ) { break; }
      if(sym == '\n') { fprintf(OUT, "%c", sym); continue; }

      if(position >= PO->init[PO->idx] && position <= PO->end[PO->idx]) 
        fprintf(OUT, "%c", tolower(sym));
      else
        fprintf(OUT, "%c", toupper(sym));

      if(PO->idx < PO->size && position > PO->end[PO->idx]) 
        PO->idx++;

      assert(PO->idx > PO->size);
      ++position;
      }

    fclose(IN);
    fclose(OUT);
    
    if(P->verbose) fprintf(stderr, "[>] Done!\n");
    }

  if(MAP->renormalize == 1) // RENORMALIZE POSITIONS
    {
    if(P->verbose) fprintf(stderr, "[>] Renormalize positions ...\n");

    SA *S = CreateSA();
    GetCumulativeReadsLength(S, P->filename);
    uint64_t last = 0;
    for(idx = 0 ; idx < S->idx ; ++idx)
      {
      if(P->verbose)
        fprintf(stderr, "[>] Cumulative read length %"PRIu64": %"PRIu64" "
        "(size: %"PRIu64" )\n", idx+1, S->array[idx], S->array[idx] - last);
      last = S->array[idx];
      }

    uint64_t i_pos = 0, e_pos = 0;
    uint64_t i_cum = 0, e_cum = 0;
    uint64_t idx_pos = 0, idx_cum = 0, tmp = 0;
    uint64_t c_array[S->idx+1];
    c_array[0] = 0;
    for(idx_cum = 0 ; idx_cum < S->idx ; ++idx_cum)
      c_array[idx_cum+1] = S->array[idx_cum];
    
    FILE **OP = (FILE **) Malloc((S->idx + 1) * sizeof(FILE *));
    for(idx_cum = 1 ; idx_cum <= S->idx ; ++idx_cum)
      {
      int size = snprintf(NULL, 0, "%s%"PRIu64".txt", P->prefix, idx_cum);
      char *name = (char *) malloc(size + 1 * sizeof(char));
      sprintf(name, "%s%"PRIu64".txt", P->prefix, idx_cum);
      OP[idx_cum] = Fopen(name, "w");
      if(!P->nosize)
        {
        //fprintf(OP[idx_cum], "#Length %"PRIu64" (id: %"PRIu64" )\n", 
        //c_array[idx_cum]-c_array[idx_cum-1], idx_cum);
        fprintf(OP[idx_cum], "#Length %"PRIu64"\n", 
        c_array[idx_cum]-c_array[idx_cum-1]);
	}
      }

    for(idx_cum = 1 ; idx_cum <= S->idx ; ++idx_cum) // FOR EACH INTERVAL
      {
      i_cum = c_array[idx_cum-1];
      e_cum = c_array[idx_cum];

      for(idx_pos = 0 ; idx_pos < PO->idx ; ++idx_pos) // FOR EACH POSITION PAIR
        {
        i_pos = PO->init[idx_pos];
        e_pos = PO->end [idx_pos];

        if(i_pos > i_cum && i_pos <= e_cum && e_pos > i_cum && e_pos <= e_cum)
          {
	  PrintRegionToFile(i_pos-i_cum, e_pos-i_cum, P->color, OP[idx_cum]);
	  continue;
          }

	if(i_pos > i_cum && i_pos <= e_cum && e_pos > i_cum && e_pos >  e_cum)
          {
          PrintRegionToFile(i_pos-i_cum, e_cum-i_cum, P->color, OP[idx_cum]);
	  for(tmp = idx_cum + 1 ; tmp <= S->idx ; ++tmp)
            {
            uint64_t i_tmp = c_array[tmp-1];
            uint64_t e_tmp = c_array[tmp];
	    if(e_pos < e_tmp)
              {
              PrintRegionToFile(1, e_pos-i_tmp, P->color, OP[tmp]);
              break;
              }
            else
              PrintRegionToFile(1, e_tmp-i_tmp, P->color, OP[tmp]);
            }
          }
        } 
      }

    for(idx_cum = 1 ; idx_cum <= S->idx ; ++idx_cum)
      fclose(OP[idx_cum]);
    RemoveSA(S);

    if(P->verbose) fprintf(stderr, "[>] Done!\n");
    }

  if(P->verbose) fprintf(stderr, "[>] Cleaning structures ...\n");
  RemovePositions(PO);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
