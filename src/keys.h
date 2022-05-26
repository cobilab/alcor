#ifndef KEYS_H_INCLUDED
#define KEYS_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#include "defs.h"

#define K0  0
#define K1  1
#define K2  2
#define K3  3
#define K4  4
#define K5  5

typedef struct
  {
  char *key;
  int val;
  }
K_STRUCT;

static K_STRUCT LT_KEYS[] = 
  { 
    { "help"          , K1  },  // HELP MENU
    { "extract"       , K2  },  // EXTRACTION
    { "mapper"        , K3  },  // LOW-COMPLEXITY MAPPER
    { "simulation"    , K4  },  // SIMULATION
    { "visual"        , K5  }   // VISUALIZATION 
  };

#define NKEYS (sizeof(LT_KEYS)/sizeof(K_STRUCT))

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int KeyString (char *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

