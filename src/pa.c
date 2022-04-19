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
#include "pa.h"
#include "dna.h"
#include "mem.h"
#include "msg.h"
#include "buffer.h"
#include "common.h"
#include "strings.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - P A I N T - - - - - - - - - - - - - - - -
void PaintStreams(PA_PARAMETERS *P)
  {
  FILE *Plot = Fopen("plot.svg", "w");
  char backColor[] = "#ffffff";
  uint64_t init, end;
  Painter *Paint;
  uint32_t tar;

  P->chrSize = (uint64_t *) Calloc(P->tar->nFiles, sizeof(uint64_t));

  // SET MAXIMUM FROM FILE:
  FILE *XReader = Fopen(P->tar->names[0], "r");
  uint64_t x_size = 0, i_size = 0;
  if(fscanf(XReader, "#%"PRIu64"#%"PRIu64"", &x_size, &i_size) != 2)
    {
    fprintf(stderr, "Error: unknown segmented file!\n");
    exit(1);
    }
  P->max = x_size;
  fclose(XReader);

  SetScale(P->max);
  Paint = CreatePainter(GetPoint(P->max), backColor);

  PrintHead(Plot, (2 * DEFAULT_CX) + (((Paint->width + DEFAULT_SPACE) *
  P->tar->nFiles) - DEFAULT_SPACE), Paint->size + EXTRA);
  Rect(Plot, (2 * DEFAULT_CX) + (((Paint->width + DEFAULT_SPACE) *
  P->tar->nFiles) - DEFAULT_SPACE), Paint->size + EXTRA, 0, 0, backColor);

  for(tar = 0 ; tar < P->tar->nFiles ; ++tar)
    {
    FILE *Reader = Fopen(P->tar->names[tar], "r");

    if(fscanf(Reader, "#%"PRIu64"#%"PRIu64"\n", &x_size, &i_size) != 2)
      {
      fprintf(stderr, "Error: unknown segmented file!\n");
      exit(1);
      }

    P->chrSize[tar] = i_size;

    while(fscanf(Reader, "%"PRIu64":%"PRIu64"", &init, &end) == 2)
      {
      Rect(Plot, Paint->width, GetPoint(end-init+1+P->enlarge), Paint->cx,
      Paint->cy + GetPoint(init), GetRgbColor(LEVEL_HUE));
      }

    Chromosome(Plot, Paint->width, GetPoint(P->chrSize[tar]), Paint->cx,
    Paint->cy);

    if(P->tar->nFiles > 0) 
      Paint->cx += DEFAULT_WIDTH + DEFAULT_SPACE;

    fclose(Reader);
    }

  PrintFinal(Plot);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

