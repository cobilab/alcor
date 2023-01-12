#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "filter.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static double CalcMean(Entry *entries, int nEntries, int n, int M, double *w){

  int k;
  double sum = 0, wSum = 0;
  for(k = -M ; k <= M ; k++)
    if(n + k >= 0 && n + k < nEntries){
      sum += w[M+k] * entries[n+k].value;
      wSum += w[M+k];
      }

  return sum / wSum;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Filter(FILE *IN, FILE *OUT){

  int M = 20, drop = 0, wType = 0;

  int n, k, nEntries, position;
  double value, *w;
  Entry *entries = NULL;

  nEntries = 0;
  while(fscanf(IN, "%lf", &value) == 1){
    entries = (Entry *) Realloc(entries, (nEntries+1) * sizeof(Entry), 
    sizeof(Entry));
    entries[nEntries].position = nEntries;
    entries[nEntries].value = value;
    ++nEntries;
    }
  
  w = (double *) Malloc((2*M+1) * sizeof(double));

  switch(wType){
    case W_HAMMING:
      for(k = -M ; k <= M ; k++)
        w[M+k] = 0.54 + 0.46 * cos((2 * M_PI * k) / (2 * M + 1));
    break;
    case W_HANN:
      for(k = -M ; k <= M ; k++)
        w[M+k] = 0.5 * (1 + cos((2 * M_PI * k) / (2 * M + 1)));
    break;
    case W_BLACKMAN:
      for(k = -M ; k <= M ; k++)
        w[M+k] = 0.42 + 0.5 * cos((2 * M_PI * k) / (2 * M + 1)) +
	0.08 * cos((4 * M_PI * k) / (2 * M + 1));
    break;
    case W_RECTANGULAR:
      for(k = -M ; k <= M ; k++)
        w[M+k] = 1;
    break;
    }

  for(n = 0 ; n < nEntries ; n++)
    if(n % (drop + 1) == 0)
      fprintf(OUT, "%.3lf\n", CalcMean(entries, nEntries, n, M, w));

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
