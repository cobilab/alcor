#ifndef PARAM_H_INCLUDED
#define PARAM_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "defs.h"
#include "alphabet.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LR_MAX_MEM                9999999
#define LR_MIN_MEM                0
#define LR_MAX_LEVEL              24
#define LR_MIN_LEVEL              1
#define LR_MAX_CTX                31
#define LR_MIN_CTX                1
#define LR_MAX_DEN                1000000
#define LR_MIN_DEN                1

#define SI_MAX_SIZE               UINT_MAX
#define SI_MIN_SIZE               1
#define SI_MAX_SEED               UINT_MAX
#define SI_MIN_SEED               1
#define SI_MAX_CTX                20
#define SI_MIN_CTX                1
#define SI_MAX_BET                50000
#define SI_MIN_BET                1
#define SI_MAX_INIT               UINT_MAX
#define SI_MIN_INIT               1
#define SI_MAX_END                UINT_MAX
#define SI_MIN_END                1
#define SI_OUT_FA_LS              80

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct
  {
  char      **names;
  uint32_t  nFiles;
  }
SFILES;

typedef struct
  {
  U32       ctx;
  U32       den;
  U32       ir;
  U32       eIr;
  U32       memory;
  U32       hashSize;
  double    gamma;
  U32       edits;
  U32       eDen;
  double    eGamma;
  U8        type;
  U32       bet;
  double    subs_rate;
  double    adds_rate;
  double    dels_rate;
  U32       seed;
  U32       size;
  U32       init;
  U32       end;
  char      *name;
  }
MODEL_PAR;

typedef struct
  {
  U32       help;
  U32       verbose;
  char      *alphabet;
  U8        dna;
  U32       nSym;
  MODEL_PAR *model;
  U32       nModels;
  U64       iBase;
  }
SI_PARAMETERS;

typedef struct
  {
  ALPHABET  *A;
  char      *alphabet;
  U32       nSym;
  U8        help;
  U8        verbose;
  U8        force;
  U8        level;
  U8        dna;
  double    threshold;
  double    weight;
  U32       region;
  MODEL_PAR *model;
  U64       size;
  U64       ignore;
  U32       nModels;
  U32       hs;
  U32       memory;
  double    lr;
  char      *reference;
  char      *output;
  char      *filename;
  SFILES    *ref;
  }
LR_PARAMETERS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint32_t    ArgsNum            (uint32_t , char *[], uint32_t, char *, char *,
                               uint32_t, uint32_t);
double      ArgsDouble         (double, char *[], uint32_t, char *, char *);
uint8_t     ArgsState          (uint8_t  , char *[], uint32_t, char *, char *);
char        *ArgsString        (char    *, char *[], uint32_t, char *, char *);
char        *ArgsFiles         (char *[], uint32_t, char *);
MODEL_PAR   ArgsUniqModelLR    (char *, uint8_t);
MODEL_PAR   ArgsUniqModelSI    (char *, uint8_t);
void        PrintParametersLR  (LR_PARAMETERS *);
void        PrintParametersSI  (SI_PARAMETERS *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
