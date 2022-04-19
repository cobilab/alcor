#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "defs.h"
#include "dna.h"
#include "mem.h"
#include "msg.h"
#include "common.h"
#include "alphabet.h"
#include "cm.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CMODEL *CreateCModel(uint32_t ctx, uint32_t aDen, uint8_t ref, uint32_t edits, 
uint32_t eDen, uint32_t nSym, double gamma, double eGamma, uint32_t ir, 
uint32_t eIr, uint32_t memory)
  {
  CMODEL    *M = (CMODEL *) Calloc(1, sizeof(CMODEL));
  uint64_t  prod = 1, *mult;
  uint32_t  n;

  M->nSym        = nSym;
  mult           = (uint64_t *) Calloc(ctx, sizeof(uint64_t));
  M->nPModels    = (uint64_t) pow(M->nSym, ctx);
  M->ctx         = ctx;
  M->ir          = ir;
  M->alphaDen    = aDen;
  M->edits       = edits;
  M->gamma       = gamma;
  M->eGamma      = eGamma;
  M->memory      = memory;
  M->pModelIdx   = 0;
  M->pModelIdxIR = M->nPModels - 1;
  M->ref         = ref == 0 ? 0 : 1;

  if(M->nPModels > pow(2, 50))
    {
    PrintWarning("Context size is too higher for the cardinality!");
    exit(1);
    }

  if(M->nPModels * M->nSym * sizeof(ACC) >> 20 > MAX_ARRAY_MEMORY)
    {
    M->mode = HASH_TABLE_MODE;
    M->HT   = CreateHashTable(M->nSym);
    }
  else
    {
    M->mode = ARRAY_MODE;
    M->AT   = CreateArrayTable(M->nSym, M->nPModels);
    }

  for(n = 0 ; n < M->ctx ; ++n)
    {
    mult[n] = prod;
    prod *= M->nSym;
    }

  M->multiplier = mult[M->ctx-1];

  if(edits != 0)
    M->TM = CreateTolerantModel(edits, eDen, M->ctx, nSym, eIr);

  if(memory != 0)
    {
    M->M.pos    = 0;
    M->M.size   = memory;
    M->M.idx    = (uint64_t *) Calloc(memory + 1, sizeof(uint64_t));
    M->M.idx_ir = (uint64_t *) Calloc(memory + 1, sizeof(uint64_t));
    M->M.sym    = (uint8_t  *) Calloc(memory + 1, sizeof(uint8_t ));
    M->M.sym_ir = (uint8_t  *) Calloc(memory + 1, sizeof(uint8_t ));
    }

  Free(mult);
  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CMODEL *CloneCModel(CMODEL *M)
  {
  CMODEL *C = (CMODEL *) Calloc(1, sizeof(CMODEL));
  uint32_t  n;

  C->nSym        = M->nSym;
  C->nPModels    = M->nPModels;
  C->ctx         = M->ctx;
  C->ir          = M->ir;
  C->alphaDen    = M->alphaDen;
  C->edits       = M->edits;
  C->gamma       = M->gamma;
  C->eGamma      = M->eGamma;
  C->memory      = M->memory;
  C->pModelIdx   = 0;
  C->pModelIdxIR = C->nPModels - 1;
  C->ref         = M->ref;

  if(M->nPModels * M->nSym * sizeof(ACC) >> 20 > MAX_ARRAY_MEMORY)
    {
    C->mode      = M->mode;
    C->HT        = CloneHashTable(M->HT);
    }
  else
    {
    C->mode      = M->mode;
    C->AT        = CloneArrayTable(M->AT);
    }

  C->multiplier  = M->multiplier;

  if(C->edits != 0)                    
    C->TM = CreateTolerantModel(C->edits, M->TM->den, C->ctx, C->nSym, 
    M->TM->ir);

  if(M->memory != 0)
    {
    C->M.pos    = 0;
    C->M.size   = M->memory;
    C->M.idx    = (uint64_t *) Calloc(M->memory + 1, sizeof(uint64_t));
    C->M.idx_ir = (uint64_t *) Calloc(M->memory + 1, sizeof(uint64_t));
    C->M.sym    = (uint8_t  *) Calloc(M->memory + 1, sizeof(uint8_t ));
    C->M.sym_ir = (uint8_t  *) Calloc(M->memory + 1, sizeof(uint8_t ));
    }

  return C;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetCModelIdx(CMODEL *M)
  {
  M->pModelIdx   = 0;
  M->pModelIdxIR = M->nPModels - 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetPModelIdx(uint8_t *p, CMODEL *M)
  {
  M->pModelIdx = ((M->pModelIdx-*(p-M->ctx)*M->multiplier)*M->nSym)+*p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetPModelIdxIR(uint8_t *p, CMODEL *M)
  {
  M->pModelIdxIR = (M->pModelIdxIR>>2)+GetCompNum(*p)*M->multiplier;
  return GetCompNum(*(p-M->ctx));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetPModelIdxR(uint8_t *p, CMODEL *M)
  {
  M->pModelIdxIR = (M->pModelIdxIR / M->nSym) + ((*p) * M->multiplier);
  return (*(p-M->ctx));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t GetPModelIdxCorr(uint8_t *p, CMODEL *M, uint64_t idx)
  {
  return (((idx-*(p-M->ctx)*M->multiplier)*M->nSym)+*p);
  }
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComputePModel(CMODEL *M, PMODEL *PM, uint64_t idx, uint32_t aDen)
  {
  ACC *ac;
  HCC *hc;
  uint32_t x;
  switch(M->mode)
    {
    case HASH_TABLE_MODE:
      if(!(hc = GetHCCounters(M->HT, idx)))
        hc = (HCC *) M->HT->zeroCounters;
      PM->sum = 0;
      for(x = 0 ; x < M->nSym ; ++x)
        {
        PM->freqs[x] = 1 + aDen * hc[x];
        PM->sum += PM->freqs[x];
        }
    break;
    case ARRAY_MODE:

      ac = &M->AT->counters[idx*M->nSym];
      PM->sum = 0;
      for(x = 0 ; x < M->nSym ; ++x)
        {
        PM->freqs[x] = 1 + aDen * ac[x];
        PM->sum += PM->freqs[x];
        }
      break;
    default:
    fprintf(stderr, "Error: not implemented!\n");
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateCModelCounter(CMODEL *M, uint32_t sym, uint64_t idx)
  {
  if(M->mode == HASH_TABLE_MODE) UpdateHashCounter (M->HT, sym, idx);
  else                           UpdateArrayCounter(M->AT, sym, idx);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UnUpdateCModelCounter(CMODEL *M, uint32_t sym, uint64_t idx)
  {
  if(M->mode == HASH_TABLE_MODE) UnUpdateHashCounter (M->HT, sym, idx);
  else                           UnUpdateArrayCounter(M->AT, sym, idx);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ModelSequence(CMODEL *CM, char *name, uint32_t nSym, ALPHABET *AL, char 
*alphabet)
  {
  uint8_t sym = 0, header = 1;
  uint64_t k, idxPos, idx = 0;
  uint8_t *readBuf = Calloc(BUFFER_SIZE, sizeof(uint8_t));
  CBUF *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  FILE *Reader = Fopen(name, "r");

  while((k = fread(readBuf, 1, DEF_BUF_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos)
      {
      sym = readBuf[idxPos];

      if(sym == '>'){ header = 1; continue; }
      if(sym == '\n' && header == 1){ header = 0; continue; }
      if(sym == '\n') continue;
      if(header == 1) continue;
      if(strchr(alphabet, sym) == NULL) continue;
      
      symBuf->buf[symBuf->idx] = sym = AL->revMap[sym];
      GetPModelIdx(symBuf->buf+symBuf->idx-1, CM);
      if(idx++ >= CM->ctx) 
        UpdateCModelCounter(CM, sym, CM->pModelIdx);
      UpdateCBuffer(symBuf);
      }

  fclose(Reader);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SimulateSequence(CMODEL *CM, char *sequence, uint32_t size, G_RAND *GR,
ALPHABET *AL) 
  {
  uint32_t x, sym;
  uint64_t sum, idx = 0; 
  CBUF *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  PMODEL *PM = CreatePModel(CM->nSym);
  srand48(GR->seed);

  // RANDOM PRIOR FOR CONTEXT INITIALIZATION
  CM->pModelIdx = 0; 
  for(x = 0 ; x < CM->ctx ; ++x)
    {
    symBuf->buf[symBuf->idx] = RandValue(GR) % CM->nSym;
    GetPModelIdx(symBuf->buf+symBuf->idx-1, CM);
    UpdateCBuffer(symBuf);
    }

  // GENERATED SYMBOLS WITH THE MODEL USING THE PRIOR
  for(x = 0 ; x < size ; ++x)
    {
    GetPModelIdx(symBuf->buf+symBuf->idx-1, CM);
    ComputePModel(CM, PM, CM->pModelIdx, CM->alphaDen);
    double rnd = drand48() * PM->sum;  
    
    sum = 0;
    for(sym = 0 ; sym < CM->nSym ; ++sym)
      if(rnd <= sum + PM->freqs[sym])
        break;
      else
        sum += PM->freqs[sym];

    symBuf->buf[symBuf->idx] = sym;
    sequence[idx++] = AL->toChars[sym];
    UpdateCBuffer(symBuf);
    }

  RemovePModel(PM);
  RemoveCBuffer(symBuf);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveCModel(CMODEL *M)
  {
  if(M->mode == HASH_TABLE_MODE) RemoveHashTable (M->HT);
  else                           RemoveArrayTable(M->AT);

  if(M->edits > 0)               RemoveTolerantModel(M->TM);
  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
