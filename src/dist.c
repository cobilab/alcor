#include "dist.h"
#include "mem.h"
#include "common.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Dist *CreateDist(uint32_t size, uint32_t nSym){ // size = nKmers, nSym = 4
 
  uint32_t x;
  Dist *D     = (Dist *) Calloc(1, sizeof(Dist));
  D->size     = size;
  D->C        = (Counts *) Calloc(size, sizeof(Counts));
  for(x = 0 ; x < size ; ++x){
    D->C[x].nSym = nSym; 	  
    D->C[x].sym  = (uint64_t *) Calloc(nSym, sizeof(uint64_t));
    }

  return D;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintDist(Dist *D, uint32_t min_ctx){

  int x;
  fprintf(stderr, "+--------------------------------------------------------+\n");
  fprintf(stderr, "|             mRAWs nucleotide distribution              |\n");
  fprintf(stderr, "+--------+-----------+-----------+-----------+-----------+\n");
  fprintf(stderr, "|  kmer  |     A     |     C     |     G     |     T     |\n");
  fprintf(stderr, "+--------+-----------+-----------+-----------+-----------+\n");
  for(x = 0 ; x < D->size ; ++x)
    fprintf(stderr, "| %6u | %9"PRIu64" | %9"PRIu64" | %9"PRIu64" | %9"PRIu64" |\n", 
    x + min_ctx, D->C[x].sym[0], D->C[x].sym[1], D->C[x].sym[2], D->C[x].sym[3]);		  
  fprintf(stderr, "+--------+-----------+-----------+-----------+-----------+\n");

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintDistAA(Dist *D, ALPHABET *A, uint32_t min_ctx){

  int x, y, z;
  fprintf(stderr, "         +-----------------------------------------------+\n");
  fprintf(stderr, "         |         mRAWs aminoacid distribution          |\n");
  fprintf(stderr, "         +-----------+-----------+-----------+-----------+\n");
  fprintf(stderr, "         |                      kmer                     |\n");
  fprintf(stderr, "+--------+");
  z = D->size < 4 ? 4 : D->size;
  for(x = 0 ; x < z ; ++x)
    fprintf(stderr, "-----------+");
  fprintf(stderr, "\n");
  fprintf(stderr, "| Symbol |");
  for(x = 0 ; x < D->size ; ++x)
    fprintf(stderr, "    %2d     |", x + min_ctx);
  fprintf(stderr, "\n");
  fprintf(stderr, "+--------+");
  for(x = 0 ; x < D->size ; ++x)
    fprintf(stderr, "-----------+");
  fprintf(stderr, "\n");
  for(x = 0 ; x < A->cardinality ; ++x){
    fprintf(stderr, "|    %c   |", A->toChars[x]);
    for(y = 0 ; y < D->size ; ++y){
      fprintf(stderr, " %9"PRIu64" |", D->C[y].sym[x]);
      }
    fprintf(stderr, "\n");
    }
  fprintf(stderr, "+--------+");
  for(x = 0 ; x < D->size ; ++x)
  fprintf(stderr, "-----------+");
  fprintf(stderr, "\n");

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintDistAAPerc(Dist *D, ALPHABET *A, uint32_t min_ctx, int8_t p, int8_t v){

  int x, y, z;
  fprintf(stderr, "         +-----------------------------------------------+\n");
  fprintf(stderr, "         |          mRAWs aminoacids Percentage          |\n");
  fprintf(stderr, "         +-----------+-----------+-----------+-----------+\n");
  fprintf(stderr, "         |                      kmer                     |\n");
  fprintf(stderr, "+--------+");
  z = D->size < 4 ? 4 : D->size;
  for(x = 0 ; x < z ; ++x)
    fprintf(stderr, "-----------+");
  fprintf(stderr, "\n");
  fprintf(stderr, "| Symbol |");
  double totals[D->size];
  for(y = 0 ; y < D->size ; ++y)
    for(x = 0 ; x < A->cardinality ; ++x)
      totals[y] += D->C[y].sym[x];
  for(x = 0 ; x < D->size ; ++x)
    fprintf(stderr, "    %2d     |", x + min_ctx);
  fprintf(stderr, "\n");
  fprintf(stderr, "+--------+");
  for(x = 0 ; x < D->size ; ++x)
    fprintf(stderr, "-----------+");
  fprintf(stderr, "\n");
  for(x = 0 ; x < A->cardinality ; ++x){
    fprintf(stderr, "|    %c   |", A->toChars[x]);
    for(y = 0 ; y < D->size ; ++y)
      fprintf(stderr, "   %5.2lf   |", (double) D->C[y].sym[x] / totals[y] * 100.0);
    fprintf(stderr, "\n");
    }
  fprintf(stderr, "+--------+");
  for(x = 0 ; x < D->size ; ++x)
  fprintf(stderr, "-----------+");
  fprintf(stderr, "\n");

  FILE *OUT2 = Fopen("AAplot.sh",  "w");
  //FILE *IN   = Fopen("AA-data.eg", "r");
  if(p){
	  /*
    fprintf(OUT2, "#!/bin/sh\n");
    fprintf(OUT2, "echo 'reset\n");
    fprintf(OUT2, "set terminal pdfcairo enhanced font \"Verdana,12\"\n");
    fprintf(OUT2, "set output \"AAplot.pdf\"\n");
    fprintf(OUT2, "set style line 11 lc rgb \"#808080\" lt 1\n");
    fprintf(OUT2, "set border 3 back ls 11\n");
    fprintf(OUT2, "set tics nomirror\n");
    fprintf(OUT2, "set size ratio 1\n");
    fprintf(OUT2, "set style line 12 lc rgb \"#808080\" lt 0 lw 2\n");
    fprintf(OUT2, "set grid back ls 12\n");
    fprintf(OUT2, "set style line 1 lc rgb \"#8b1a0e\" pt 1 ps 1 lt 1 lw 3\n");
    fprintf(OUT2, "set style line 2 lc rgb \"#5e9c36\" pt 6 ps 1 lt 1 lw 3\n");
    fprintf(OUT2, "set key bottom right\n");
    fprintf(OUT2, "set xlabel \"k-mer\"\n");
    fprintf(OUT2, "set ylabel \"Distribution (%%)\"\n");
    fprintf(OUT2, "set xrange [%u:%u]\n", min_ctx, min_ctx + D->size - 1);
    fprintf(OUT2, "set yrange [%u:%u]\n", 0, 100);
    fprintf(OUT2, "plot \"AA-data.eg\" u 1:2 t \"AT\" w lp ls 1, "
                  " \"AA-data.eg\" u 1:3 t \"CG\" w lp ls 2 ' | gnuplot -persist\n");
*/
    if(v) fprintf(stderr, "[>] Shell plot with AA %% in AAplot.sh\n");
    }
  //fclose(IN);
  fclose(OUT2);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintCG(Dist *D, uint32_t min_ctx, uint32_t p, uint32_t v){

  int x;
  uint64_t tot = 0, at = 0, cg = 0;
  double at100 = 0, cg100 = 0;
  FILE *OUT = Fopen("CG-data.eg", "w");
  fprintf(stderr, "+--------------------------------------------------+\n");
  fprintf(stderr, "|              mRAWs AT/CG distribution            |\n");
  fprintf(stderr, "+--------+--------------------+--------------------+\n");
  fprintf(stderr, "|  kmer  |         AT         |         CG         |\n");
  fprintf(stderr, "+--------+--------------------+--------------------+\n");
  for(x = 0 ; x < D->size ; ++x){
    tot = D->C[x].sym[0]+D->C[x].sym[1]+D->C[x].sym[2]+D->C[x].sym[3];
    at  = D->C[x].sym[0]+D->C[x].sym[3];
    cg  = D->C[x].sym[1]+D->C[x].sym[2];
    if(at != 0) at100 = (double) at/tot*100;
    if(cg != 0) cg100 = (double) cg/tot*100;
    if(at == 0 || cg == 0)
      fprintf(stderr, "| %6u | %10"PRIu64" [--.-%%] | %10"PRIu64" [--.-%%] |\n", 
      x + min_ctx, at, cg);
    else
      fprintf(stderr, "| %6u | %10"PRIu64" [%3.1lf%%] | %10"PRIu64" [%3.1lf%%] |\n", 
      x + min_ctx, at, at100, cg, cg100);
    if(at != 0 && cg != 0)
      fprintf(OUT, "%u\t%.3lf\t%.3lf\n", x + min_ctx, at100, cg100);
    }
  fclose(OUT);

  fprintf(stderr, "+--------+--------------------+--------------------+\n");

  FILE *OUT2 = Fopen("CGplot.sh",  "w");
  FILE *IN   = Fopen("CG-data.eg", "r");
  if(p){
    fprintf(OUT2, "#!/bin/sh\n");
    fprintf(OUT2, "echo 'reset\n");
    fprintf(OUT2, "set terminal pdfcairo enhanced font \"Verdana,12\"\n");
    fprintf(OUT2, "set output \"CGplot.pdf\"\n");
    fprintf(OUT2, "set style line 11 lc rgb \"#808080\" lt 1\n");
    fprintf(OUT2, "set border 3 back ls 11\n");
    fprintf(OUT2, "set tics nomirror\n");
    fprintf(OUT2, "set size ratio 1\n");
    fprintf(OUT2, "set style line 12 lc rgb \"#808080\" lt 0 lw 2\n");
    fprintf(OUT2, "set grid back ls 12\n");
    fprintf(OUT2, "set style line 1 lc rgb \"#8b1a0e\" pt 1 ps 1 lt 1 lw 3\n");
    fprintf(OUT2, "set style line 2 lc rgb \"#5e9c36\" pt 6 ps 1 lt 1 lw 3\n");
    fprintf(OUT2, "set key bottom right\n");
    fprintf(OUT2, "set xlabel \"k-mer\"\n");
    fprintf(OUT2, "set ylabel \"Distribution (%%)\"\n");
    fprintf(OUT2, "set xtics 1\n");
    fprintf(OUT2, "set xrange [%u:%u]\n", min_ctx, min_ctx + D->size - 1);
    fprintf(OUT2, "set yrange [%u:%u]\n", 0, 100);
    fprintf(OUT2, "plot \"CG-data.eg\" u 1:2 t \"AT\" w lp ls 1, "
                  " \"CG-data.eg\" u 1:3 t \"CG\" w lp ls 2 ' | gnuplot -persist\n");
    
    if(v) fprintf(stderr, "[>] Shell plot with CG %% in CGplot.sh\n");	  
    }
  fclose(IN);
  fclose(OUT2);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveDist(Dist *D){

  int x;	
  for(x = 0 ; x < D->size ; ++x)	
    Free(D->C[x].sym);	
  Free(D->C);	
  Free(D);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
