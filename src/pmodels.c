#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "defs.h"
#include "mem.h"
#include "math.h"
#include "common.h"
#include "pmodels.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PMODEL *CreatePModel(uint32_t n){
  PMODEL *PM = (PMODEL   *) Calloc(1, sizeof(PMODEL));
  PM->freqs  = (uint32_t *) Calloc(n, sizeof(uint32_t));
  return PM;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CMWEIGHT *CreateWeightModel(uint32_t size){
  uint32_t n;
  CMWEIGHT *CMW    = (CMWEIGHT *) Calloc(1, sizeof(CMWEIGHT));
  CMW->totModels   = size;
  CMW->totalWeight = 0;
  CMW->gamma       = (double *) Calloc(CMW->totModels, sizeof(double));
  CMW->weight      = (double *) Calloc(CMW->totModels, sizeof(double));
  double fraction  = 1.0 / CMW->totModels;
  for(n = 0 ; n < CMW->totModels ; ++n){
    CMW->weight[n] = fraction;
    CMW->gamma[n]  = DEFAULT_GAMMA;
    }
  return CMW;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetWeightModel(CMWEIGHT *CMW){
  uint32_t n;
  double fraction = 1.0 / CMW->totModels;
  CMW->totalWeight = 0;
  for(n = 0 ; n < CMW->totModels ; ++n){
    CMW->weight[n] = fraction;
    CMW->gamma[n]  = DEFAULT_GAMMA;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RenormalizeWeights(CMWEIGHT*CMW){
  uint32_t n;
  for(n = 0 ; n < CMW->totModels ; ++n) 
    CMW->weight[n] /= CMW->totalWeight;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CalcDecayment(CMWEIGHT *CMW, PMODEL **PM, uint8_t sym){
  uint32_t n;
  CMW->totalWeight = 0;
  for(n = 0 ; n < CMW->totModels ; ++n){
    CMW->weight[n] = Power(CMW->weight[n], CMW->gamma[n]) * 
                     (double) PM[n]->freqs[sym] / PM[n]->sum;
    CMW->totalWeight += CMW->weight[n];
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveWeightModel(CMWEIGHT *CMW){
  Free(CMW->weight);
  Free(CMW->gamma);
  Free(CMW);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComputeMXProbs(FPMODEL *PT, PMODEL *MX, uint32_t nSym){
  uint32_t x;
  MX->sum = 0;
  for(x = 0 ; x < nSym ; ++x)
    MX->sum += MX->freqs[x] = 1 + (unsigned) (PT->freqs[x] * MX_PMODEL);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemovePModel(PMODEL *PM){
  Free(PM->freqs);
  Free(PM);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FPMODEL *CreateFloatPModel(uint32_t n){
  FPMODEL *F = (FPMODEL *) Calloc(1, sizeof(FPMODEL));
  F->freqs   = (double  *) Calloc(n, sizeof(double));
  return F;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveFPModel(FPMODEL *FM){
  Free(FM->freqs);
  Free(FM);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComputeWeightedFreqs(double w, PMODEL *P, FPMODEL *PT, uint32_t nSym){
  uint32_t x;
  double f = w / P->sum;
  for(x = 0 ; x < nSym ; ++x)
    PT->freqs[x] += (double) P->freqs[x] * f;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double PModelNats(PMODEL *P, uint32_t s){
  return log((double) P->sum / P->freqs[s]);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double PModelStat(PMODEL *P, uint32_t s){
  return (double) P->sum / P->freqs[s];
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

