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
#include "ex.h"
#include "if.h"
#include "vi.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

LR_PARAMETERS  *MAP;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void P_Visual(char **p, int c)
  {
  int n;

  VI_PARAMETERS *MAP = (VI_PARAMETERS *) Calloc(1, sizeof(VI_PARAMETERS));

  MAP->help      = ArgsState  (DEF_VI_HELP,    p, c, "-h", "--help");
  MAP->verbose   = ArgsState  (DEF_VI_VERBOSE, p, c, "-v", "--verbose");
  MAP->corner    = ArgsState  (DEF_VI_CORNER,  p, c, "-c", "--strict-corner");
  MAP->space     = ArgsNum    (DEF_VI_SPACE,   p, c, "-s", "--space",
                              1, 99999);
  MAP->width     = ArgsNum    (DEF_VI_WIDTH,   p, c, "-w", "--width",
                              1, 99999);
  MAP->enlarge   = ArgsNum    (DEF_VI_ENLARGE, p, c, "-e", "--enlarge",
                              0, 9999999);

  if(c < MIN_NPARAM_FOR_PROGS+1 || MAP->help)
    {
    PrintMenuVi();
    return;
    }

  int exists = 0;
  for(n = 1 ; n < c ; ++n)
    if(strcmp(p[n], "-b") == 0 || strcmp(p[n], "--back-color") == 0)
      {
      MAP->backColor = CloneString(p[n+1]);
      CheckHexaColor(MAP->backColor);
      exists = 1;
      break;
      }
  if(exists == 0)
    MAP->backColor = CloneString("ffffff");

  exists = 0;
  for(n = 1 ; n < c ; ++n)
    if(strcmp(p[n], "-a") == 0 || strcmp(p[n], "--border-color") == 0)
      {
      MAP->borderColor = CloneString(p[n+1]);
      CheckHexaColor(MAP->borderColor);
      exists = 1;
      break;
      }
  if(exists == 0) 
    MAP->borderColor = CloneString("262626");

  exists = 0;
  for(n = 1 ; n < c ; ++n)
    if(strcmp(p[n], "-o") == 0 || strcmp(p[n], "--output") == 0)
      {
      MAP->output = CloneString(p[n+1]);
      exists = 1;
      break;
      }
  if(exists == 0)
    MAP->output = CloneString("map.svg");

  MAP->tar = ReadFNames(MAP, p[c-1]);
  
  if(MAP->verbose) PrintMessage("Running visual ...");

  if(MAP->verbose) PrintParametersVI(MAP);

  Visual(MAP);

  if(MAP->verbose) fprintf(stderr, "[>] Done!\n");

  free(MAP);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void P_Information(char **p, int c)
  {
  int n;

  IF_PARAMETERS *MAP = (IF_PARAMETERS *) Calloc(1, sizeof(IF_PARAMETERS));

  MAP->help        = ArgsState  (DEF_IF_HELP,    p, c, "-h", "--help");
  MAP->verbose     = ArgsState  (DEF_IF_VERBOSE, p, c, "-v", "--verbose");

  if(c < MIN_NPARAM_FOR_PROGS+1 || MAP->help)
    {
    PrintMenuIf();
    return;
    }

  MAP->filename = p[c-1];
  CheckFileEmpty(MAP->filename);

  if(MAP->verbose) PrintParametersIF(MAP);

  // PRINT HEADER
  if(MAP->verbose) fprintf(stderr, "[>] Information from the FASTA reads: \n");

  Information(MAP);

  if(MAP->verbose) fprintf(stderr, "[>] Done!\n");

  free(MAP);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void P_Extract(char **p, int c)
  {
  int n;

  EX_PARAMETERS *MAP = (EX_PARAMETERS *) Calloc(1, sizeof(EX_PARAMETERS));

  MAP->help        = ArgsState  (DEF_EX_HELP,    p, c, "-h", "--help");
  MAP->verbose     = ArgsState  (DEF_EX_VERBOSE, p, c, "-v", "--verbose");
  MAP->fasta       = ArgsState  (DEF_EX_FASTA  , p, c, "-f", "--fasta");
  MAP->init        = ArgsNum    (DEF_EX_INIT,    p, c, "-i", "--init",
                                1, 999999999);
  MAP->end         = ArgsNum    (DEF_EX_END,     p, c, "-e", "--end",
                                1, 999999999);


  if(c < MIN_NPARAM_FOR_PROGS+1 || MAP->help)
    {
    PrintMenuEx();
    return;
    }

  MAP->filename = p[c-1];
  CheckFileEmpty(MAP->filename);
  
  if(MAP->verbose) PrintParametersEX(MAP);

  // PRINT HEADER
  if(MAP->fasta) fprintf(stdout, ">Extracted sequence [%"PRIu64";%"PRIu64"]\n",
		 MAP->init, MAP->end);

  Extract(MAP);

  // PRINT TAIL
  fprintf(stdout, "\n");

  if(MAP->verbose) fprintf(stderr, "[>] Done!\n");

  free(MAP);
  return;
  }

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

  MAP->help        = ArgsState  (DEF_LR_HELP,        p, c, "-h", "--help");
  MAP->verbose     = ArgsState  (DEF_LR_VERBOSE,     p, c, "-v", "--verbose");
  MAP->hide        = ArgsState  (DEF_LR_HIDE,        p, c, "-e", "--hide");
  MAP->dna         = ArgsState  (DEF_LR_DNA,         p, c, "-d", "--dna");
  MAP->mask        = ArgsState  (DEF_LR_MASK,        p, c, "-k", "--mask");
  MAP->nosize      = ArgsState  (DEF_LR_NOSIZE,      p, c, "-n", "--no-size");
  MAP->renormalize = ArgsState  (DEF_LR_RENORMALIZE, p, c, "-r", "--renormalize");
  MAP->threshold   = ArgsDouble (DEF_LR_THRESHOLD,   p, c, "-t", "--threshold");
  MAP->color       = ArgsNum    (DEF_LR_COLOR,       p, c, "-c", "--color",
                                0, 255);
  MAP->window      = ArgsNum    (DEF_LR_WINDOW,      p, c, "-w", "--window",
                                0, 999999999);
  MAP->ignore      = ArgsNum    (DEF_LR_IGNORE,      p, c, "-i", "--ignore",
                                0, 999999999);
  MAP->level       = ArgsNum    (0,                  p, c, "-l", "--level",
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

  int out_mask = 0;
  for(n = 1 ; n < c ; ++n)
    if(strcmp(p[n], "-o") == 0 || strcmp(p[n], "--output-mask") == 0)
      {
      MAP->outputmask = CloneString(p[n+1]);
      out_mask = 1;
      break;
      }
  if(out_mask == 0 && MAP->mask == 1)
    MAP->outputmask = CloneString(Cat("masked-", MAP->filename));

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
    case K2: P_Information                   (argv, argc);  break;
    case K3: P_Extract                       (argv, argc);  break;
    case K4: P_LocalRedundancy               (argv, argc);  break;
    case K5: P_Simulation                    (argv, argc);  break;
    case K6: P_Visual                        (argv, argc);  break;

    default:
    PrintWarning("unknown menu option!");
    PrintMenu();
    }

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
