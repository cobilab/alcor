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
#include "if.h"
#include "dna.h"
#include "mem.h"
#include "msg.h"
#include "buffer.h"
#include "common.h"
#include "strings.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - P R I N T   E N T R Y - - - - - - - - - - - - - 
//
void PrintEntryRead(uint64_t idx, uint64_t len, double GC, uint8_t *header, 
uint32_t size)
  {
  uint32_t x;
 
  fprintf(stdout, "%"PRIu64"\t%"PRIu64"\t%.4g\t", idx, len, GC / len * 100);

  for(x = 1 ; x < size ; ++x)
    fprintf(stdout, "%c", header[x]);
  fprintf(stdout, "\n");

return;
}

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - I N F O R M A T I O N   M A I N - - - - - - - - - - -
//
void Information(IF_PARAMETERS *P)
  {
  uint8_t buffer[BUFFER_SIZE], sym = 0, header = 1;
  uint32_t k, idx; 
  FILE *F = Fopen(P->filename, "r");

  uint32_t headerMax = P->headerMax;
  uint8_t  headerStr[headerMax + 3];
  headerStr[headerMax + 2] = '\0';
  uint32_t headerLen = 0;
  
  uint64_t seqLen = 0; 
  uint64_t seqGC = 0; 
  
  uint64_t readIdx = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      {
      sym = buffer[idx];
      if(sym == '>')
        {
        if(headerLen > headerMax)
	  headerLen = headerMax;

	if(readIdx > 0)
          PrintEntryRead(readIdx, seqLen, (double) seqGC, headerStr, headerLen);

	header = 1;
	headerStr[0] = '>';
	headerLen = 1;

	++readIdx;
	seqLen = 0;
        seqGC = 0;
	continue; 
	}
      if(sym == '\n' && header == 1)
        { 
	header = 0;
	continue; 
	}
      if(sym == '\n') continue;
      if(header == 1) 
        {
	++headerLen;
	if(headerLen < headerMax)
          headerStr[headerLen] = sym;
	continue;
	}
      
      ++seqLen;
      if(sym == 'C' || sym == 'c' || sym == 'G' || sym == 'g')
        ++seqGC;
      
      }

  if(headerLen > headerMax)
    headerLen = headerMax;

  PrintEntryRead(readIdx, seqLen, (double) seqGC, headerStr, headerLen);

  fclose(F);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

