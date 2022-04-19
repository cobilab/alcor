#include "stats.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Stats *CreateStats(uint32_t size){
  
  Stats *S    = (Stats  *) Calloc(1, sizeof(Stats));
  S->size     = size;
  S->average  = (double *) Calloc(size, sizeof(double));
  S->variance = (double *) Calloc(size, sizeof(double));
  S->stddev   = (double *) Calloc(size, sizeof(double));

  return S;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintStats(Stats *S, uint32_t min_ctx){

  uint32_t x;
  fprintf(stderr, "+-------------------------------------------------------+\n");
  fprintf(stderr, "|                Overall mRAWs statistics               |\n");
  fprintf(stderr, "+--------+---------------+--------------+---------------+\n");
  fprintf(stderr, "|  kmer  |    Average    |   Variance   |  Stand. Dev.  |\n");
  fprintf(stderr, "+--------+---------------+--------------+---------------+\n");
  for(x = 0 ; x < S->size ; ++x)
    fprintf(stderr, "| %6u | %13.2lf | %12.2lf | %13.2lf |\n", x + min_ctx,
    S->average[x], S->variance[x], S->stddev[x]);  
  fprintf(stderr, "+--------+---------------+--------------+---------------+\n");

  return;	
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveStats(Stats *S){

  Free(S->average);	
  Free(S->variance);	
  Free(S->stddev);	
  Free(S);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
