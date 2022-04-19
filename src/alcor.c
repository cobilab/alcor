#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "defs.h"
#include "keys.h"
#include "msg.h"
#include "mem.h"
#include "dna.h"
#include "param.h"
#include "common.h"
#include "levels.h"
#include "strings.h"
#include "lr.h"
#include "si.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

LR_PARAMETERS  *MAP;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void P_Simulation(char **p, int c)
  {
  int n;

  SI_PARAMETERS *MAP = (SI_PARAMETERS *) Calloc(1, sizeof(SI_PARAMETERS));

  MAP->help        = ArgsState  (DEF_SI_HELP,     p, c, "-h", "--help");
  MAP->verbose     = ArgsState  (DEF_SI_VERBOSE,  p, c, "-v", "--verbose");
  MAP->dna         = ArgsState  (DEF_SI_DNA,      p, c, "-n", "--no-dna");
  MAP->alphabet    = ArgsString (DEF_SI_ALPHABET, p, c, "-a", "--alphabet");
  MAP->nSym        = strlen(MAP->alphabet);

  if(c < MIN_NPARAM_FOR_PROGS+1 || MAP->help)
    {
    PrintMenuSi();
    return;
    }

  MAP->nModels = 0;
  for(n = 1 ; n < c ; ++n)
    if(!strcmp(p[n], "-fs") || !strcmp(p[n], "--file-segment") ||
       !strcmp(p[n], "-rs") || !strcmp(p[n], "--rand-segment") || 
       !strcmp(p[n], "-ms") || !strcmp(p[n], "--model-segment"))
      MAP->nModels += 1;

  if(MAP->nModels == 0)
    {
    PrintWarning("at least you need to use a generation model!");
    exit(1);
    }

  MAP->model = (MODEL_PAR *) Calloc(MAP->nModels, sizeof(MODEL_PAR));

  int k = 0;
  for(n = 1 ; n < c ; ++n){
    if(!strcmp(p[n], "-fs") || !strcmp(p[n], "--file-segment"))
      MAP->model[k++] = ArgsUniqModelSI(p[n+1], 0);
    if(!strcmp(p[n], "-rs") || !strcmp(p[n], "--rand-segment"))
      MAP->model[k++] = ArgsUniqModelSI(p[n+1], 1);
    if(!strcmp(p[n], "-ms") || !strcmp(p[n], "--model-segment"))
      MAP->model[k++] = ArgsUniqModelSI(p[n+1], 2);
    }

  if(MAP->verbose) PrintParametersSI(MAP);

  // PRINT HEADER
  fprintf(stdout, ">Simulated sequence\n");

  // SIMULATION OF SEGMENTS  
  MAP->iBase = 0;
  Simulation(MAP);

  // PRINT TAIL
  fprintf(stdout, "\n");

  if(MAP->verbose) fprintf(stderr, "[>] Done!\n");

  free(MAP);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void P_LocalRedundancy(char **p, int c)
  {
  char    **xargv, *xpl = NULL;
  int32_t n, xargc = 0;

  MAP = (LR_PARAMETERS *) Calloc(1, sizeof(LR_PARAMETERS));

  MAP->help      = ArgsState  (DEF_LR_HELP,      p, c, "-h", "--help");
  MAP->verbose   = ArgsState  (DEF_LR_VERBOSE,   p, c, "-v", "--verbose");
  MAP->dna       = ArgsState  (DEF_LR_DNA,       p, c, "-d", "--dna");
  MAP->nosize    = ArgsState  (DEF_LR_NOSIZE,    p, c, "-n", "--no-size");
  MAP->threshold = ArgsDouble (DEF_LR_THRESHOLD, p, c, "-t", "--threshold");
  MAP->weight    = ArgsDouble (DEF_LR_WEIGHT,    p, c, "-w", "--weight");
  MAP->ignore    = ArgsNum    (DEF_LR_IGNORE,    p, c, "-i", "--ignore",
                              0, 999999999);
  MAP->level     = ArgsNum    (0,                p, c, "-l", "--level",
                              DEF_LR_MIN_LEVEL, DEF_LR_MAX_LEVEL);

  if(c < MIN_NPARAM_FOR_PROGS + 1 || MAP->help)
    {
    PrintMenuLR();
    return;
    }

  if(ArgsState(0, p, c, "-s", "--show-levels"))
    {
    PrintLevelsLR();
    exit(1);
    }

  if(ArgsState(0, p, c, "-p", "--show-parameters"))
    {
    PrintModels();
    exit(1);
    }

  MAP->nModels = 0;
  for(n = 1 ; n < c ; ++n)
    if(strcmp(p[n], "-m") == 0)
      MAP->nModels += 1;

  if(MAP->nModels == 0 && MAP->level == 0)
    MAP->level = DEF_LR_LEVEL;

  if(MAP->level != 0)
    {
    xpl = GetLevelsLR(MAP->level);
    xargc = StrToArgv(xpl, &xargv);
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-m") == 0)
        MAP->nModels += 1;
    }

  if(MAP->nModels == 0)
    {
    PrintWarning("at least you need to use a context model!");
    exit(1);
    }

  MAP->model = (MODEL_PAR *) Calloc(MAP->nModels, sizeof(MODEL_PAR));

  int k = 0;
  for(n = 1 ; n < c ; ++n)
    if(strcmp(p[n], "-m") == 0)
      MAP->model[k++] = ArgsUniqModelLR(p[n+1], 0);
  if(MAP->level != 0)
    {
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-m") == 0)
        MAP->model[k++] = ArgsUniqModelLR(xargv[n+1], 0);
    }
  
  MAP->filename = p[c-1];
  CheckFileEmpty(MAP->filename);

  if(MAP->verbose) PrintParametersLR(MAP);

  LocalRedundancy(MAP);

  if(MAP->verbose) fprintf(stderr, "[>] Done!\n");

  free(MAP);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[])
  {
  char **p = *&argv;

  if(ArgsState(0, p, argc, "-V", "--version"))
    {
    PrintVersion();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_HELP, p, argc, "-h", "--help") && argc == 2 || argc < 2)
    {
    PrintMenu();
    return EXIT_SUCCESS;
    }

  switch(KeyString(argv[1]))
    {
    case K1: PrintMenu();                                   break;
    //case K2: P_Extract                       (argv, argc);  break;
    case K3: P_LocalRedundancy               (argv, argc);  break;
    case K4: P_Simulation                    (argv, argc);  break;
    //case K5: P_Visualization                 (argv, argc);  break;

    default:
    PrintWarning("unknown menu option!");
    PrintMenu();
    }

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
