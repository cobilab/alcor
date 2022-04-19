#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <limits.h>
#include <ctype.h>

#include "defs.h"
#include "si.h"
#include "cm.h"
#include "dna.h"
#include "mem.h"
#include "msg.h"
#include "rand.h"
#include "buffer.h"
#include "common.h"
#include "strings.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - S I M U L A T I O N   M A I N - - - - - - - - - - - -
//
void Simulation(SI_PARAMETERS *MAP)
  {
  uint32_t x = 0;
  uint64_t n = 0;

  if(MAP->verbose)
    fprintf(stderr, "[>] Analyzing alphabet ...\n");

  if(MAP->nSym < 2)
    {
    PrintWarning("alphabet must contain more than 1 symbol");
    exit(1);
    }

  StringHasUniqueCharacters(MAP->alphabet);

  if(MAP->verbose) 
    fprintf(stderr, "[>] Simulating %u segment(s):\n", MAP->nModels);

  for(x = 0 ; x < MAP->nModels ; ++x)
    {
    char sequence[MAP->model[x].size + 1];
    G_RAND *GR = CreateRand(MAP->model[x].seed, 1103515245, 12345, 32768);
    
    // GET SEQUENCE FROM FILE, RAND, OR MODEL
    switch(MAP->model[x].type)
      {
      case 0: 
        if(MAP->verbose) 
          fprintf(stderr, "  [+] Simulating file segment %u, size %u "
	  "(%u to %u) ...\n", x+1, MAP->model[x].size, MAP->model[x].init, 
	  MAP->model[x].end);
	ALPHABET *ALE = CreateAlphabet();
	if(MAP->dna)
          LoadAlphabetWithExceptions(ALE, MAP->model[x].name, MAP->alphabet);
	else
	  LoadAlphabet(ALE, MAP->model[x].name);
	if(ALE->length < MAP->model[x].end)
	  {
	  PrintWarning("end position > number of symbols in file!");
          exit(1);
	  }
        ExtractSeqFromFASTA(MAP->model[x].init, MAP->model[x].end,
	MAP->model[x].name, sequence, MAP->alphabet, MAP->dna);
	RemoveAlphabet(ALE);
      break;

      case 1:
        if(MAP->verbose)
          fprintf(stderr, "  [+] Simulating rand segment %u, size %u ...\n",
          x+1, MAP->model[x].size);
	for(n = 0 ; n < MAP->model[x].size ; ++n)
          sequence[n] = MAP->alphabet[RandValue(GR) % MAP->nSym];
      break;
      
      case 2:
        if(MAP->verbose) 
	  fprintf(stderr, "  [+] Simulating model segment %u, size %u ...\n", 
	  x+1, MAP->model[x].size);
	CMODEL *CM = CreateCModel(MAP->model[x].ctx, MAP->model[x].bet, 1, 0,
        0, MAP->nSym, 0.9, 0.9, MAP->model[x].ir, 0, 0);
	ALPHABET *ALM = CreateAlphabet();
        LoadAlphabetWithExceptions(ALM, MAP->model[x].name, MAP->alphabet);
	ModelSequence(CM, MAP->model[x].name, MAP->nSym, ALM, MAP->alphabet);
        SimulateSequence(CM, sequence, MAP->model[x].size, GR, ALM); 
	RemoveCModel(CM);
	RemoveAlphabet(ALM);
      break;
      }

    // REVERSE
    if(MAP->model[x].ir == 1)
      {
      ReverseArrayStack(sequence, MAP->model[x].size);
      if(MAP->dna == 1)
        ComplementArrayStack(sequence, MAP->model[x].size);
      }

    // MUTATIONS:
    for(n = 0 ; n < MAP->model[x].size ; ++n)
      {
      // DELETION MUTATION :: DO NOT CHANGE ORDER!
      if(RandValue(GR) / (GR->module + 1.0) < MAP->model[x].dels_rate)
        goto ADDS;

      // SUBSTITUTION MUTATION
      if(RandValue(GR) / (GR->module + 1.0) < MAP->model[x].subs_rate)
        putchar(MAP->alphabet[RandValue(GR) % MAP->nSym]);
      else
        putchar(sequence[n]);

      if(++MAP->iBase % SI_OUT_FA_LS == 0) fprintf(stdout, "\n");

      ADDS:
      // ADDITION MUTATION   GR->module instead of RAND_MAX and rand()
      if(RandValue(GR) / (GR->module + 1.0) < MAP->model[x].adds_rate)
	{
        putchar(MAP->alphabet[RandValue(GR) % MAP->nSym]);
        if(++MAP->iBase % SI_OUT_FA_LS == 0) fprintf(stdout, "\n");
	}
      }

    RemoveRand(GR);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

