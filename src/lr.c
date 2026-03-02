#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
#include <stdatomic.h>

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
// - - - - - - - - - - P R O G R E S S   H E L P E R S - - - - - - - - - - - -

static _Atomic uint64_t g_comp_done[2];
static _Atomic uint64_t g_comp_total[2];
static _Atomic int      g_comp_stop = 0;

static uint64_t FileSize(FILE *F)
  {
  off_t cur = ftello(F);
  fseeko(F, 0, SEEK_END);
  off_t end = ftello(F);
  fseeko(F, cur, SEEK_SET);
  return (uint64_t) end;
  }

static void ProgressPrintPct(const char *label, uint64_t done, uint64_t total)
  {
  if(!P || !P->verbose) return;

  double pct = 100.0;
  if(total > 0) pct = (100.0 * (double)done) / (double)total;
  if(pct > 100.0) pct = 100.0;

  fprintf(stderr, "\r[%-14s] %6.2f%%", label, pct);
  fflush(stderr);
  }

static void ProgressDoneLine(void)
  {
  if(!P || !P->verbose) return;
  fprintf(stderr, "\n");
  fflush(stderr);
  }

static void SleepMs(unsigned ms)
  {
  struct timespec ts;
  ts.tv_sec  = (time_t)(ms / 1000u);
  ts.tv_nsec = (long)(ms % 1000u) * 1000000L;

  while(nanosleep(&ts, &ts) == -1 && errno == EINTR) { }
  }

static void *CompressProgressThread(void *arg)
  {
  (void)arg;

  if(!P || !P->verbose) pthread_exit(NULL);

  while(atomic_load(&g_comp_stop) == 0)
    {
    uint64_t d0 = atomic_load(&g_comp_done[0]);
    uint64_t d1 = atomic_load(&g_comp_done[1]);
    uint64_t t0 = atomic_load(&g_comp_total[0]);
    uint64_t t1 = atomic_load(&g_comp_total[1]);

    uint64_t done  = d0 + d1;
    uint64_t total = t0 + t1;

    ProgressPrintPct("Compressing", done, total);
    SleepMs(200);
    }

  {
  uint64_t d0 = atomic_load(&g_comp_done[0]);
  uint64_t d1 = atomic_load(&g_comp_done[1]);
  uint64_t t0 = atomic_load(&g_comp_total[0]);
  uint64_t t1 = atomic_load(&g_comp_total[1]);
  ProgressPrintPct("Compressing", d0 + d1, t0 + t1);
  ProgressDoneLine();
  }

  pthread_exit(NULL);
  }

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
  char        in_name [8192];
  char        out_name[8192];
  sprintf(in_name,  ".lrcr_%u.seq", T.id + 1);
  sprintf(out_name, ".lrcr_%u.inf", T.id + 1);
  FILE        *Reader  = Fopen(in_name,  "rb");
  FILE        *Writter = Fopen(out_name, "wb");
  setvbuf(Reader,  NULL, _IOFBF, 1<<20);
  setvbuf(Writter, NULL, _IOFBF, 1<<20);
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
  float       fbps = 0.0f;

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
    {
    // progress: count processed bytes per thread 
    atomic_fetch_add(&g_comp_done[T.id], (uint64_t)k);

    for(idxPos = 0 ; idxPos < k ; ++idxPos)
      {
      sym = (uint8_t) toupper(readBUF[idxPos]);

      // FINAL FILTERING DNA CONTENT
      if(sym != 'A' && sym != 'C' && sym != 'G' && sym != 'T')
        {
        fbps = 2.0f; // FORCE HIGH COMPLEXITY
        fwrite(&fbps, sizeof(float), 1, Writter);
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

      fbps = (float) bps;
      fwrite(&fbps, sizeof(float), 1, Writter);

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
    }

  fclose(Writter);

  Free(MX->freqs);
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
  char        in_name [8192];
  char        out_name[8192];
  sprintf(in_name,  ".lrcr_%u.seq", T.id + 1);
  sprintf(out_name, ".lrcr_%u.inf", T.id + 1);
  FILE        *Reader  = Fopen(in_name,  "rb");
  FILE        *Writter = Fopen(out_name, "wb");
  setvbuf(Reader,  NULL, _IOFBF, 1<<20);
  setvbuf(Writter, NULL, _IOFBF, 1<<20);
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
  float       fbps = 0.0f;

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
    {
    // progress: count processed bytes per thread 
    atomic_fetch_add(&g_comp_done[T.id], (uint64_t)k);

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

      fbps = (float) bps;
      fwrite(&fbps, sizeof(float), 1, Writter);

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
    }

  fclose(Writter);

  Free(MX->freqs);
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

  pthread_create(&(t[0]), NULL, CompressThread, (void *) &(T[0]));
  pthread_create(&(t[1]), NULL, CompressThread, (void *) &(T[1]));

  pthread_join(t[0], NULL);
  pthread_join(t[1], NULL);

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

  // ALPHABET ================================================================
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

  // =========================================================================

  if(P->verbose) PrintMessage("Spliting and reversing streams ...");

  // FAST: buffered write + block-wise reverse
  FILE *IN   = Fopen(P->filename, "rb");
  FILE *OUT1 = Fopen(".lrcr_1.seq", "wb");

  const size_t WBUF_SZ = (size_t)1u << 20; // 1 MiB
  uint8_t *wbuf = (uint8_t *) Malloc(WBUF_SZ);
  size_t widx = 0;

  int sym;
  uint64_t nValues = 0;

  while((sym = getc(IN)) != EOF)
    {
    if(sym == '>') // header line 
      {
      while((sym = fgetc(IN)) != EOF && sym != '\n')
        ;
      continue;
      }
    if(sym == '\n' || sym == '\r')
      continue;

    wbuf[widx++] = (uint8_t) sym;
    if(widx == WBUF_SZ)
      {
      fwrite(wbuf, 1, widx, OUT1);
      widx = 0;
      }
    ++nValues;
    }

  if(widx) fwrite(wbuf, 1, widx, OUT1);

  fclose(IN);
  fclose(OUT1);

  if(P->verbose) fprintf(stderr, "[>] Number of symbols: %"PRIu64"\n", nValues);

  FILE *IN2  = Fopen(".lrcr_1.seq", "rb");
  FILE *OUT2 = Fopen(".lrcr_2.seq", "wb");

  uint64_t remaining = nValues;
  while(remaining > 0)
    {
    size_t chunk = (remaining > WBUF_SZ) ? WBUF_SZ : (size_t) remaining;
    uint64_t off = remaining - chunk;

    fseeko(IN2, (off_t) off, SEEK_SET);

    if(fread(wbuf, 1, chunk, IN2) != chunk)
      {
      PrintWarning("reverse read failed!");
      exit(1);
      }

    // reverse in-place 
    for(size_t i = 0, j = (chunk ? chunk - 1 : 0) ; i < j ; ++i, --j)
      {
      uint8_t tmp = wbuf[i];
      wbuf[i] = wbuf[j];
      wbuf[j] = tmp;
      }

    if(fwrite(wbuf, 1, chunk, OUT2) != chunk)
      {
      PrintWarning("reverse write failed!");
      exit(1);
      }

    remaining -= chunk;
    }

  Free(wbuf);
  fclose(IN2);
  fclose(OUT2);

  if(P->verbose) PrintMessage("Done!");

  // COMPRESSING ============================================================
  //
  if(P->verbose) PrintMessage("Compressing streams ...");

  // progress init: both streams have nValues bytes
  atomic_store(&g_comp_done[0], 0);
  atomic_store(&g_comp_done[1], 0);
  atomic_store(&g_comp_total[0], nValues);
  atomic_store(&g_comp_total[1], nValues);
  atomic_store(&g_comp_stop, 0);

  pthread_t pthr;
  if(P->verbose) pthread_create(&pthr, NULL, CompressProgressThread, NULL);

  CompressAction(T);

  atomic_store(&g_comp_stop, 1);
  if(P->verbose) pthread_join(pthr, NULL);

  remove(".lrcr_1.seq");
  remove(".lrcr_2.seq");

  if(P->verbose) PrintMessage("Done!");

  // ======================================================================
  // MINIMUM (LR vs RL), SMOOTH + SEGMENT IN THE SAME PASS OVER MIN FILE
  // ======================================================================

  FILE *IN_LR = Fopen(".lrcr_1.inf", "rb");
  FILE *IN_RL = Fopen(".lrcr_2.inf", "rb");

  uint64_t bytesLR = FileSize(IN_LR);
  uint64_t bytesRL = FileSize(IN_RL);
  uint64_t nFloatsLR = bytesLR / sizeof(float);
  uint64_t nFloatsRL = bytesRL / sizeof(float);

  if(nFloatsLR != nFloatsRL)
    {
    PrintWarning("information files have been changed!");
    exit(1);
    }

  const uint64_t L = nFloatsLR;

  // MINIMUM
  if(P->verbose) fprintf(stderr, "[>] Computing the minimum profile ...\n");

  FILE *MIN_BIN = Fopen(".lrcr_1_2.bin", "wb");

  const size_t BF = (size_t)1u << 18; // 262144 floats ~= 1 MiB 
  float *bufLR = (float *) Malloc(BF * sizeof(float));
  float *bufRL = (float *) Malloc(BF * sizeof(float));

  uint64_t done = 0;
  while(done < L)
    {
    size_t chunk = (L - done > BF) ? BF : (size_t)(L - done);

    if(fread(bufLR, sizeof(float), chunk, IN_LR) != chunk)
      {
      PrintWarning("failed reading LR profile!");
      exit(1);
      }

    off_t pos = (off_t)((L - done - chunk) * (uint64_t)sizeof(float));
    fseeko(IN_RL, pos, SEEK_SET);

    if(fread(bufRL, sizeof(float), chunk, IN_RL) != chunk)
      {
      PrintWarning("failed reading RL profile!");
      exit(1);
      }

    // compute min aligned 
    for(size_t j = 0 ; j < chunk ; ++j)
      {
      float LRv = bufLR[j];
      float RLv = bufRL[chunk - 1 - j];
      bufLR[j] = (RLv < LRv) ? RLv : LRv;
      }

    if(fwrite(bufLR, sizeof(float), chunk, MIN_BIN) != chunk)
      {
      PrintWarning("failed writing MIN profile!");
      exit(1);
      }

    done += chunk;
    ProgressPrintPct("Minimum", done, L);
    }

  ProgressDoneLine();

  fclose(MIN_BIN);
  fclose(IN_RL);
  fclose(IN_LR);

  Free(bufLR);
  Free(bufRL);

  remove(".lrcr_1.inf");
  remove(".lrcr_2.inf");

  // SMOOTH + SEGMENT (in single pass)
  if(P->verbose) fprintf(stderr, "[>] Smoothing and segmenting ...\n");

  // region output header (as before) 
  if(!P->nosize && P->renormalize == 0)
    fprintf(stdout, "#Length\t%"PRIu64"\n", nValues);

  // positions storage for downstream masking/renormalize 
  POS *PO = CreatePositions(10000);

  FILE *MIN_IN = Fopen(".lrcr_1_2.bin", "rb");

  // If hide==1, don't even create/write the .info file (avoid I/O).
  FILE *PROF = NULL;
  if(!P->hide)
    PROF = Fopen(Cat(P->filename, ".info"), "w");

  float  *bufM = (float *) Malloc(BF * sizeof(float));
  double *filt = (double *) Calloc(P->window + 1, sizeof(double));
  double sum = 0.0, newAvg = 0.0;
  uint32_t ring = 0;

  int region = 0;
  uint64_t initPos = 1;
  uint64_t idx = 0;

  done = 0;
  while(done < L)
    {
    size_t chunk = (L - done > BF) ? BF : (size_t)(L - done);

    if(fread(bufM, sizeof(float), chunk, MIN_IN) != chunk)
      {
      PrintWarning("failed reading MIN profile!");
      exit(1);
      }

    for(size_t j = 0 ; j < chunk ; ++j)
      {
      ++idx;

      // smoothing
      newAvg = MovAvgInstant(filt, &sum, ring, P->window, (double)bufM[j]);
      if(++ring == P->window) ring = 0;

      // write .info only if not hidden 
      if(PROF)
        fprintf(PROF, "%.3g\n", (float)newAvg);

      // segmentation ONLINE 
      if(idx == 1)
        {
        region  = ((double)newAvg < P->threshold) ? 0 : 1;
        initPos = 1;
        }
      else
        {
        if((double)newAvg >= P->threshold) // HIGH 
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
        else // LOW 
          {
          if(region == 1)
            {
            region  = 0;
            initPos = idx;
            }
          }
        }
      }

    done += chunk;
    ProgressPrintPct("Smoothing", done, L);
    }

  // finalize last region (as original) 
  if(region == 0)
    {
    if(idx - initPos > P->ignore)
      {
      if(MAP->renormalize != 1)
        PrintRegion(initPos, idx, P->color);
      UpdatePositions(PO, initPos, idx);
      }
    }

  ProgressDoneLine();

  fclose(MIN_IN);
  if(PROF) fclose(PROF);

  Free(bufM);
  Free(filt);

  remove(".lrcr_1_2.bin");

  // If hide==1 we never created .info, so nothing to remove. 
  // (Old behavior was: create then delete; now it's skipped entirely.) 

  //========================================================================
  // MASK / RENORMALIZE use PO computed during smoothing                    
  //========================================================================

  if(MAP->mask == 1) // MASK FASTA
    {
    if(P->verbose) fprintf(stderr, "[>] Masking sequence ...\n");

    FILE *INM  = Fopen(P->filename,   "r");
    FILE *OUTM = Fopen(P->outputmask, "w");

    int sym2;
    PO->idx = 0;
    uint64_t position = 1;
    while((sym2 = getc(INM)) != EOF)
      {
      if(sym2 == '>')
        { // HEADER FOUND!
        fprintf(OUTM, "%c", sym2);
        while((sym2 = fgetc(INM)) != EOF && sym2 != '\n')
          fprintf(OUTM, "%c", sym2);
        }

      if(sym2 == EOF ) { break; }
      if(sym2 == '\n') { fprintf(OUTM, "%c", sym2); continue; }

      if(position >= PO->init[PO->idx] && position <= PO->end[PO->idx])
        fprintf(OUTM, "%c", tolower(sym2));
      else
        fprintf(OUTM, "%c", toupper(sym2));

      if(PO->idx < PO->size && position > PO->end[PO->idx])
        PO->idx++;

      assert(PO->idx <= PO->size);
      ++position;
      }

    fclose(INM);
    fclose(OUTM);

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
      char *name = (char *) malloc((size_t)size + 1u);
      sprintf(name, "%s%"PRIu64".txt", P->prefix, idx_cum);
      OP[idx_cum] = Fopen(name, "w");
      free(name);

      if(!P->nosize)
        fprintf(OP[idx_cum], "#Length %"PRIu64"\n",
        c_array[idx_cum]-c_array[idx_cum-1]);
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
