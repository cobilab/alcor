#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

#include "defs.h"
#include "ex.h"
#include "dna.h"
#include "mem.h"
#include "msg.h"
#include "buffer.h"
#include "common.h"
#include "strings.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - E X T R A C T   M A I N - - - - - - - - - - - -
//
void Extract(EX_PARAMETERS *P)
  {
  uint8_t buffer[BUFFER_SIZE], sym = 0, header = 1;
  uint32_t k, idx, nSymbols = 0, x = 0;
  FILE *F = Fopen(P->filename, "r");

  assert(P->end > P->init);

  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      {
      sym = buffer[idx];
      if(sym == '>'){ header = 1; continue; }
      if(sym == '\n' && header == 1){ header = 0; continue; }
      if(sym == '\n') continue;
      if(header == 1) continue;
      if(++nSymbols >= P->init && nSymbols <= P->end)
	{
        fprintf(stdout, "%c", sym);
	if(P->fasta && nSymbols % 70 == 0)
          fprintf(stdout, "\n");
	}
      }

  fclose(F);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

