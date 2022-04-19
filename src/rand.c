#include <stdio.h>
#include "rand.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

G_RAND *CreateRand(uint64_t seed, uint64_t a, uint64_t b, uint64_t module)
  {
  G_RAND *GR = (G_RAND *) Calloc(1, sizeof(G_RAND));
  
  GR->seed   = seed;
  GR->next   = GR->seed;
  GR->a      = a;
  GR->b      = b;
  GR->module = module;

  return GR;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Srand(G_RAND *GR, uint64_t seed)
  {
  GR->next = seed;
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint32_t RandValue(G_RAND *GR)
  {
  GR->next = GR->next * GR->a + GR->b;
  return ((uint32_t) (GR->next / 65536) % GR->module);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveRand(G_RAND *GR)
  {
  Free(GR);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

