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
#include "pmodels.h"
#include "strings.h"
#include "cm.h"

LR_PARAMETERS *P; // FOR THREAD SHARING

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
  if(!P->nosize) fprintf(stdout, "#Length\t%"PRIu64"\n", nValues);
 
  int region;
  double smooth, min;
  uint64_t initPos = 1;

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
          fprintf(stdout, "%"PRIu64"\t%"PRIu64"\n", initPos, idx);
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
    fprintf(stdout, "%"PRIu64"\t%"PRIu64"\n", initPos, idx);
    }

  fclose(PROF_IN);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
