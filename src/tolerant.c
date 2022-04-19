#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tolerant.h"
#include "strings.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TOLERANT *CreateTolerantModel(uint32_t e, uint32_t d, uint32_t c, uint32_t n,
uint32_t i)
  {
  TOLERANT *T  = (TOLERANT *) Calloc(1, sizeof(TOLERANT));
  T->mask      = (uint8_t *) Calloc(BGUARD, sizeof(uint8_t));
  T->seq       = CreateCBuffer(BUFFER_SIZE, BGUARD);
  T->in        = 0;
  T->idx       = 0;
  T->threshold = e;
  T->ctx       = c;
  T->nSym      = n;
  T->den       = d;
  T->ir        = i;
  return T;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t BestId(uint32_t *f, uint32_t sum, uint32_t nSym)
  {
  if(sum == nSym) return -2; // ZERO COUNTERS
  
  uint32_t x, best = 0, max = f[0];

  for(x = 1 ; x < nSym ; ++x)
    if(f[x] > max)
      {
      max = f[x];
      best = x;
      }

  for(x = 0 ; x < nSym ; ++x) 
    if(best != x && max == f[x]) 
      return -1;

  return best;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Fail(TOLERANT *T)
  {
  uint32_t x, fails = 0;

  for(x = 0 ; x < T->ctx ; ++x) 
    if(T->mask[x] != 0) 
      ++fails;

  if(fails <= T->threshold) ShiftBuffer(T->mask, T->ctx, 1);
  else                      T->in = 0;

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Hit(TOLERANT *T)
  {
  ShiftBuffer(T->mask, T->ctx, 0);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateTolerantModel(TOLERANT *T, PMODEL *PM, uint8_t sym)
  {
  int32_t best = BestId(PM->freqs, PM->sum, T->nSym);

  switch(best)
    {
    case -2:  // IT IS A ZERO COUNTER [NOT SEEN BEFORE]
      if(T->in != 0) Hit(T);   // BETTER FOR AMINOACIDS (CHANGE IF DNA)
    break;
    case -1:  // IT HAS AT LEAST TWO MAXIMUM FREQS [CONFUSION FREQS]
      if(T->in != 0) Fail(T);  // BETTER FOR AMINOACIDS (CHANGE IF DNA)
    break;
    default:  // IT HAS ONE MAXIMUM FREQ
      if(T->in == 0)
        { // IF IS OUT
        T->in = 1;
        memset(T->mask, 0, T->ctx);
        }
      else
        { // IF IS IN
        if(best == sym) Hit(T);
        else
	  {
          Fail(T);
          T->seq->buf[T->seq->idx] = best; 
          } // UPDATE BUFFER WITH NEW SYMBOL
        }
    }

  UpdateCBuffer(T->seq);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveTolerantModel(TOLERANT *T)
  {
  RemoveCBuffer(T->seq);
  Free(T->mask);
  Free(T);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


