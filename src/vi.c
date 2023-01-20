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
#include "vi.h"
#include "dna.h"
#include "mem.h"
#include "msg.h"
#include "buffer.h"
#include "common.h"
#include "strings.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - P A I N T - - - - - - - - - - - - - - - -

void Visual(VI_PARAMETERS *P)
  {
  FILE *Plot = Fopen(P->output, "w");
  uint64_t init, end;
  Painter *Paint;
  uint32_t tar, cl;

  P->chrSize = (uint64_t *) Calloc(P->tar->nFiles, sizeof(uint64_t));

  // SET MAXIMUM FROM FILE:
  P->max = 0;
  uint64_t x_size = 0;
  for(tar = 0 ; tar < P->tar->nFiles ; ++tar)
    {
    FILE *XReader = Fopen(P->tar->names[tar], "r");
    if(fscanf(XReader, "#Length %"PRIu64"", &x_size) != 1)
      {
      fprintf(stderr, "Error: unknown segmented file!\n");
      exit(1);
      }

    if(P->max < x_size) P->max = x_size;
    fclose(XReader);
    }

  SetScale(P->max);
  Paint = CreatePainter(GetPoint(P->max), P->backColor, P->width, P->space, 
	  P->enlarge);

  PrintHead(Plot, (2 * DEFAULT_CX) + (((Paint->width + Paint->space) *
  P->tar->nFiles) - Paint->space), Paint->size + EXTRA);
  Rect(Plot, (2 * DEFAULT_CX) + (((Paint->width + Paint->space) *
  P->tar->nFiles) - Paint->space), Paint->size + EXTRA, 0, 0, P->backColor);

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar)
    {
    FILE *Reader = Fopen(P->tar->names[tar], "r");

    if(fscanf(Reader, "#Length %"PRIu64"\n", &x_size) != 1)
      {
      fprintf(stderr, "Error: unknown segmented file!\n");
      exit(1);
      }

    P->chrSize[tar] = x_size;

    while(fscanf(Reader, "%"PRIu64"\t%"PRIu64"\t%u", &init, &end, &cl) == 3)
      {
      assert(cl >= 0 && cl <= 255);
      Rect(Plot, Paint->width, GetPoint(end-init+1+Paint->enlarge), Paint->cx,
      Paint->cy + GetPoint(init), GetRgbColor(cl));
      }

    Chromosome(Plot, Paint->width, GetPoint(P->chrSize[tar]), Paint->cx,
    Paint->cy, P->corner, P->borderColor, P->backColor);

    if(P->tar->nFiles > 0) 
      Paint->cx += Paint->width + Paint->space;

    fclose(Reader);
    }

  PrintFinal(Plot);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

