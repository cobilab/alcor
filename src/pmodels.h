#ifndef CM_PMODELS_H_INCLUDED
#define CM_PMODELS_H_INCLUDED

#include "defs.h"

#define DEFAULT_GAMMA  0.90
#define MX_PMODEL      65535

typedef struct{
  uint32_t  *freqs;
  uint32_t  sum;
  }
PMODEL;

typedef struct{
  double    *freqs;
  int32_t   nSym;
  }
FPMODEL;

typedef struct{
  uint32_t  totModels;
  double    *weight;
  double    *gamma;
  double    totalWeight;
  }
CMWEIGHT;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PMODEL      *CreatePModel        (uint32_t);
FPMODEL     *CreateFloatPModel   (uint32_t);
void        RemovePModel         (PMODEL *);
void        RemoveFPModel        (FPMODEL *);
void        ComputeMXProbs       (FPMODEL *, PMODEL *, uint32_t);
void        ComputeWeightedFreqs (double, PMODEL *, FPMODEL *, uint32_t);
CMWEIGHT    *CreateWeightModel   (uint32_t);
void        ResetWeightModel     (CMWEIGHT *);
void        RenormalizeWeights   (CMWEIGHT *);
void        CalcDecayment        (CMWEIGHT *, PMODEL **, uint8_t);
void        RemoveWeightModel    (CMWEIGHT *);
double      PModelNats           (PMODEL *, uint32_t);
double      PModelStat           (PMODEL *, uint32_t);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
