#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <limits.h>
#include <ctype.h>

#include "ma.h"
#include "mem.h"
#include "msg.h"
#include "buffer.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - M O V I N G   A V E R A G E   F I L T E R - - - - - - - -
//
double MovAvgInstant(double *f, double *pS, uint32_t p, uint32_t l, double n)
  {
  *pS = *pS - f[p] + n;
  f[p] = n;
  return *pS / l;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - F I L T E R   M A I N - - - - - - - - - - - - - -
//
void MovingAverage(MA_PARAMETERS *MAP)
  {
  uint64_t row = 0;
  uint32_t idx = 0;
  double   newAvg = 0;
  double   sum = 0;
  char     buffer[1024];
  double   *filt = (double *) Calloc(MAP->window_size + 1, sizeof(double));

  while(fgets(buffer, 1024, stdin))
    {
    uint32_t col = 0;
    char *value = strtok(buffer, "\t");
    while(value) 
      {
      if(++col == MAP->column) 
        {
        if(MAP->ignore == 1 && row == 0) break;
        newAvg = MovAvgInstant(filt, &sum, idx, MAP->window_size, atof(value));
	if(row >= MAP->window_size)
	  {
	  if(MAP->show_pos == 1)	
	    fprintf(stdout, "%"PRIu64"\t%lf\n", row-MAP->window_size+1, newAvg);
	  else
	    fprintf(stdout, "%lf\n", newAvg);
	  }
        if(++idx == MAP->window_size) idx = 0;
        }
      value = strtok(NULL, "\t");
      }
    ++row;
    }

  if(MAP->verbose)
    {
    fprintf(stderr, "[>] Processed %"PRIu64" entries.\n", row);
    fprintf(stderr, "[>] Done\n");
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

