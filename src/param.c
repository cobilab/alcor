#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include "mem.h"
#include "common.h"
#include "param.h"
#include "msg.h"
#include "dna.h"
#include "strings.h"
#include "keys.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint32_t ArgsNum(uint32_t d, char *a[], uint32_t n, char *s, char *s2,
uint32_t l, uint32_t u)
  {
  uint32_t x;
  for( ; --n ; ) if(!strcmp(s, a[n]) || !strcmp(s2, a[n]))
    {
    if((x = atol(a[n+1])) < l || x > u)
      {
      fprintf(stderr, "[x] Invalid number! Interval: [%u;%u].\n", l, u);
      exit(EXIT_FAILURE);
      }
    return x;
    }
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double ArgsDouble(double d, char *a[], uint32_t n, char *s, char *s2)
  {
  for( ; --n ; )
    if(!strcmp(s, a[n]) || !strcmp(s2, a[n]))
      return atof(a[n+1]);
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t ArgsState(uint8_t d, char *a[], uint32_t n, char *s, char *s2)
  {
  for( ; --n ; )
    if(!strcmp(s, a[n]) || !strcmp(s2, a[n]))
      return d == 0 ? 1 : 0;
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsString(char *d, char *a[], uint32_t n, char *s, char *s2)
  {
  for( ; --n ; )
    if(!strcmp(s, a[n]) || !strcmp(s2, a[n]))
      {
      if(a[n+1] == NULL || strlen(a[n+1]) < 1)
        {
        PrintWarning("string is empty!");
	exit(1);
        }
      return a[n+1];
      }
  return d;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

MODEL_PAR ArgsUniqModelLR(char *str, uint8_t type)
  {
  uint32_t  ctx, den, ir, hash, edits, eDen, memory, models_exit = 0;
  double    gamma, eGamma;
  MODEL_PAR Mp;


  if(sscanf(str, "%u:%u:%u:%u:%u:%lf/%u:%u:%lf", &ctx, &den, &memory, &ir, 
    &hash, &gamma, &edits, &eDen, &eGamma) == 9)
    {

    if(ctx > LR_MAX_CTX || ctx < LR_MIN_CTX)
      {
      fprintf(stderr, "ERROR: Invalid Context!\n");
      models_exit = 1;
      }

    if(den > LR_MAX_DEN || den < LR_MIN_DEN)
      {
      fprintf(stderr, "ERROR: Invalid Alpha denominator!\n");
      models_exit = 1;
      }

    if(memory > LR_MAX_MEM)
      {
      fprintf(stderr, "ERROR: Invalid cache memory model!\n");
      models_exit = 1;
      }

    if(ir > 2)
      {
      fprintf(stderr, "ERROR: Invalid IR!\n");
      models_exit = 1;
      }

    if(hash > 255)
      {
      fprintf(stderr, "ERROR: Invalid cache-hash size!\n");
      models_exit = 1;
      }

    if(gamma < 0 || gamma > 0.999999)
      {
      fprintf(stderr, "ERROR: Invalid gamma!\n");
      models_exit = 1;
      }

    if(edits > 20)
      {
      fprintf(stderr, "ERROR: Invalid number of editions (substitutions)!\n");
      models_exit = 1;
      }

    if(eDen > LR_MAX_DEN)
      {
      fprintf(stderr, "ERROR: Invalid Alpha denominator (substitutions)!\n");
      models_exit = 1;
      }

    if(gamma < 0 || gamma > 0.999999)
      {
      fprintf(stderr, "ERROR: Invalid gamma (substitutions)!\n");
      models_exit = 1;
      }

    if(models_exit == 1)
      {
      PrintModels();
      fprintf(stderr, "\nReset models according to the above description.\n");
      exit(1);
      }

    Mp.ctx      = ctx;
    Mp.den      = den;
    Mp.memory   = memory;
    Mp.ir       = ir;
    Mp.hashSize = hash;
    Mp.gamma    = ((int)( gamma * 65534)) / 65534.0;
    Mp.eGamma   = ((int)(eGamma * 65534)) / 65534.0;
    Mp.edits    = edits;
    Mp.eDen     = eDen;
    Mp.type     = type;
    return Mp;
    }
  else{
    fprintf(stderr, "Error: unknown scheme for model arguments!\n");
    PrintModels();
    fprintf(stderr, "\nReset models according to the above description.\n");
    exit(1);
    }

  return Mp;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

MODEL_PAR ArgsUniqModelSI(char *str, uint8_t type)
  {
  char *name = (char *) calloc(5010, sizeof(char));
  uint32_t  init, end, size, ctx, bet, ir, seed, models_exit = 0;
  double    subs, adds, dels;
  MODEL_PAR Mp;

  if(type == 0)
    {
    if(sscanf(str, "%u:%u:%u:%u:%lf:%lf:%lf:%5000s", &init, &end, &ir,
    &seed, &subs, &adds, &dels, name) == 8)
      {
      if(init > SI_MAX_INIT || init < SI_MIN_INIT)
        {
        fprintf(stderr, "ERROR: Invalid size!\n");
        models_exit = 1;
        }

      if(end > SI_MAX_END || end < SI_MIN_END)
        {
        fprintf(stderr, "ERROR: Invalid context order!\n");
        models_exit = 1;
        }

      if(end < init)
	{
        fprintf(stderr, "ERROR: initial position higher or equal to end!\n");
        models_exit = 1;
        }

      if(ir > 2)
        {
        fprintf(stderr, "ERROR: Invalid IR!\n");
        models_exit = 1;
        }

      if(seed > SI_MAX_SEED || seed < SI_MIN_SEED)
        {
        fprintf(stderr, "ERROR: Invalid seed!\n");
        models_exit = 1;
        }

      if(subs < 0 || subs > 1)
        {
        fprintf(stderr, "ERROR: Invalid substitution rate!\n");
        models_exit = 1;
        }

      if(adds < 0 || adds > 1)
        {
        fprintf(stderr, "ERROR: Invalid addition rate!\n");
        models_exit = 1;
        }

      if(dels < 0 || dels > 1)
        {
        fprintf(stderr, "ERROR: Invalid deletion rate!\n");
        models_exit = 1;
        }

      if(models_exit == 1)
        {
        PrintWarning("Invalid file pattern");
        exit(1);
        }

      TestReadFile(name);
      CheckFileIsFASTA(name);

      Mp.type      = type;
      Mp.init      = init;
      Mp.end       = end;
      Mp.size      = end - init + 1;  
      Mp.ir        = ir;
      Mp.seed      = seed;
      Mp.subs_rate = subs;
      Mp.adds_rate = adds;
      Mp.dels_rate = dels;
      Mp.name      = &name[0];
      }
    }
  else if(type == 1)
    {
    if(sscanf(str, "%u:%u:%u:%lf:%lf:%lf", &size, &ir, &seed, &subs, &adds,
    &dels) == 6)
      {
      if(size > SI_MAX_SIZE || size < SI_MIN_SIZE)
        {
        fprintf(stderr, "ERROR: Invalid size!\n");
        models_exit = 1;
        }

      if(ir > 2)
        {
        fprintf(stderr, "ERROR: Invalid IR!\n");
        models_exit = 1;
        }

      if(seed > SI_MAX_SEED || seed < SI_MIN_SEED)
        {
        fprintf(stderr, "ERROR: Invalid seed!\n");
        models_exit = 1;
        }

      if(subs < 0 || subs > 1)
        {
        fprintf(stderr, "ERROR: Invalid substitution rate!\n");
        models_exit = 1;
        }

      if(adds < 0 || adds > 1)
        {
        fprintf(stderr, "ERROR: Invalid addition rate!\n");
        models_exit = 1;
        }

      if(dels < 0 || dels > 1)
        {
        fprintf(stderr, "ERROR: Invalid deletion rate!\n");
        models_exit = 1;
        }

      if(models_exit == 1)
        {
        PrintWarning("Invalid Rand pattern");
        exit(1);
        }
      
      Mp.type      = type;
      Mp.size      = size;
      Mp.ir        = ir;
      Mp.seed      = seed;
      Mp.subs_rate = subs;
      Mp.adds_rate = adds;
      Mp.dels_rate = dels;
      }
    else
      {
      goto UMA;
      }
    }
  else if(type == 2)
    {
    if(sscanf(str, "%u:%u:%u:%u:%u:%lf:%lf:%lf:%5000s", &size, &ctx, &bet, &ir,
    &seed, &subs, &adds, &dels, name) == 9)
      {
      if(size > SI_MAX_SIZE || size < SI_MIN_SIZE)
        {
        fprintf(stderr, "ERROR: Invalid size!\n");
        models_exit = 1;
        }

      if(ctx > SI_MAX_CTX || ctx < SI_MIN_CTX)
        {
        fprintf(stderr, "ERROR: Invalid context order!\n");
        models_exit = 1;
        }

      if(bet > SI_MAX_BET || bet < SI_MIN_BET)
        {
        fprintf(stderr, "ERROR: Invalid bet!\n");
        models_exit = 1;
        }

      if(ir > 2)
        {
        fprintf(stderr, "ERROR: Invalid IR!\n");
        models_exit = 1;
        }

      if(seed > SI_MAX_SEED || seed < SI_MIN_SEED)
        {
        fprintf(stderr, "ERROR: Invalid seed!\n");
        models_exit = 1;
        }

      if(subs < 0 || subs > 1)
        {
        fprintf(stderr, "ERROR: Invalid substitution rate!\n");
        models_exit = 1;
        }

      if(adds < 0 || adds > 1)
        {
        fprintf(stderr, "ERROR: Invalid addition rate!\n");
        models_exit = 1;
        }

      if(dels < 0 || dels > 1)
        {
        fprintf(stderr, "ERROR: Invalid deletion rate!\n");
        models_exit = 1;
        }

      if(models_exit == 1)
        {
        PrintWarning("Invalid Model pattern");
        exit(1);
        }

      TestReadFile(name);
      CheckFileIsFASTA(name);

      Mp.type      = type;
      Mp.size      = size;
      Mp.ctx       = ctx;
      Mp.bet       = bet;
      Mp.ir        = ir;
      Mp.seed      = seed;
      Mp.subs_rate = subs;
      Mp.adds_rate = adds;
      Mp.dels_rate = dels;
      Mp.name      = &name[0];
      }
    else
      {
      goto UMA;
      }
    }
  else
    {
    UMA:
    fprintf(stderr, "Error: unknown scheme for simulation!\n");
    exit(1);
    }

  return Mp;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ArgsFiles(char *arg[], uint32_t argc, char *str)
  {
  int32_t n = argc;

  for( ; --n ; )
    if(!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);

  return Cat(Cat(arg[argc-2], arg[argc-1]), ".svg");
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SFILES *ReadFNames(VI_PARAMETERS *P, char *arg)
  {
  SFILES *SF = (SFILES *) Calloc(1, sizeof(SFILES));
  uint32_t nFiles = 1, k = 0, argLen = strlen(arg);
  for( ; k != argLen ; ++k)
    if(arg[k] == ':')
      ++nFiles;
  SF->names = (char **) Malloc(nFiles * sizeof(char *));
  SF->names[0] = strtok(arg, ":");
  TestReadFile(SF->names[0]);
  for(k = 1 ; k < nFiles ; ++k){
    SF->names[k] = strtok(NULL, ":");
    TestReadFile(SF->names[k]);
    }
  SF->nFiles = nFiles;
  return SF;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintParametersLR(LR_PARAMETERS *M)
  {
  uint32_t n;

  fprintf(stderr, "[>] Force mode ..................... %s\n", M->force == 0 ?
  "no" : "yes");
  fprintf(stderr, "[>] DNA mode ....................... %s\n", M->dna == 0 ? 
  "no" : "yes");
  fprintf(stderr, "[>] Applying mask .................. %s\n", M->mask == 0 ? 
  "no" : "yes");
  fprintf(stderr, "[>] Renormalize .................... %s\n", M->renormalize 
  == 0 ? "no" : "yes");
  if(M->renormalize == 1)
    fprintf(stderr, "[>] Prefix ......................... %s\n", M->prefix); 
  fprintf(stderr, "[>] Color for visualization ........ %u\n", M->color);
  fprintf(stderr, "[>] Low-pass window size ........... %u\n", M->window);
  for(n = 0 ; n < M->nModels ; ++n){
    fprintf(stderr, "[>] Target model %d:\n", n+1);
    fprintf(stderr, "  [+] Context order ................ %u\n",
    M->model[n].ctx);
    fprintf(stderr, "  [+] Alpha denominator ............ %u\n",
    M->model[n].den);
    fprintf(stderr, "  [+] Cache memory model ........... %u\n",
    M->model[n].memory);
    switch(M->model[n].ir){
      case 0:
      fprintf(stderr, "  [+] Inverted repeats ............. no (regular)\n");
      break;
      case 1:
      fprintf(stderr, "  [+] Inverted repeats ............. mix (reg & inv)\n");
      break;
      case 2:
      fprintf(stderr, "  [+] Inverted repeats ............. inverted only\n");
      break;
      }
    fprintf(stderr, "  [+] Cache-hash size .............. %u\n",
    M->model[n].hashSize);
    fprintf(stderr, "  [+] Gamma ........................ %.3lf\n",
    M->model[n].gamma);
    fprintf(stderr, "  [+] Allowable substitutions ...... %u\n",
    M->model[n].edits);
    if(M->model[n].edits != 0){
      fprintf(stderr, "  [+] Substitutions alpha den ...... %u\n",
      M->model[n].eDen);
      fprintf(stderr, "  [+] Substitutions gamma .......... %.3lf\n",
      M->model[n].eGamma);
      }
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintParametersSI(SI_PARAMETERS *M)
  {
  uint32_t n;

  fprintf(stderr, "[>] DNA mode ....................... %s\n", M->dna == 0 ?
  "no" : "yes");
  fprintf(stderr, "[>] Number of segments ............. %u\n", M->nModels);
  fprintf(stderr, "[>] Alphabet ....................... %s\n", M->alphabet);
  fprintf(stderr, "[>] Alphabet size .................. %u\n", M->nSym);
  for(n = 0 ; n < M->nModels ; ++n)
    {
    if(M->model[n].type == 0)
      {
      fprintf(stderr, "[>] File segment %d:\n", n+1);
      fprintf(stderr, "  [+] Initial position ............. %u\n",
      M->model[n].init);
      fprintf(stderr, "  [+] End position ................. %u\n",
      M->model[n].end);
      fprintf(stderr, "  [+] Inverted ..................... %s\n",
      M->model[n].ir ? "yes" : "no");
      fprintf(stderr, "  [+] Seed ......................... %u\n",
      M->model[n].seed);
      fprintf(stderr, "  [+] Substitution rate ............ %.3lf\n",
      M->model[n].subs_rate);
      fprintf(stderr, "  [+] Addition rate ................ %.3lf\n",
      M->model[n].adds_rate);
      fprintf(stderr, "  [+] Deletion rate ................ %.3lf\n",
      M->model[n].dels_rate);
      fprintf(stderr, "  [+] Filename ..................... %s\n",
      M->model[n].name);
      }
    else if(M->model[n].type == 1)
      {
      fprintf(stderr, "[>] Random segment %d:\n", n+1);
      fprintf(stderr, "  [+] Segment size ................. %u\n", 
      M->model[n].size);
      fprintf(stderr, "  [+] Inverted ..................... %s\n", 
      M->model[n].ir ? "yes" : "no");
      fprintf(stderr, "  [+] Seed ......................... %u\n",   
      M->model[n].seed);
      fprintf(stderr, "  [+] Substitution rate ............ %.3lf\n",
      M->model[n].subs_rate);
      fprintf(stderr, "  [+] Addition rate ................ %.3lf\n",
      M->model[n].adds_rate);
      fprintf(stderr, "  [+] Deletion rate ................ %.3lf\n",
      M->model[n].dels_rate);
      }
    else if(M->model[n].type == 2)
      {
      fprintf(stderr, "[>] Model segment %d:\n", n+1);
      fprintf(stderr, "  [+] Region size .................. %u\n",
      M->model[n].size);
      fprintf(stderr, "  [+] Context size ................. %u\n",
      M->model[n].ctx);
      fprintf(stderr, "  [+] Bet parameter ................ %u\n",
      M->model[n].bet);
      fprintf(stderr, "  [+] Inverted ..................... %s\n",
      M->model[n].ir ? "yes" : "no");
      fprintf(stderr, "  [+] Seed ......................... %u\n",
      M->model[n].seed);
      fprintf(stderr, "  [+] Substitution rate ............ %.3lf\n",
      M->model[n].subs_rate);
      fprintf(stderr, "  [+] Addition rate ................ %.3lf\n",
      M->model[n].adds_rate);
      fprintf(stderr, "  [+] Deletion rate ................ %.3lf\n",
      M->model[n].dels_rate);
      fprintf(stderr, "  [+] Learning filename ............ %s\n",
      M->model[n].name);
      }
    else
      {
      PrintWarning("Unknown function in PrintParametersSI");
      exit(1);
      }
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintParametersEX(EX_PARAMETERS *M)
  {
  uint32_t n;

  fprintf(stderr, "[>] FASTA output ................... %s\n", M->fasta == 0 ?
  "no" : "yes");
  fprintf(stderr, "[>] Initial position ............... %"PRIu64"\n", M->init);
  fprintf(stderr, "[>] End position ................... %"PRIu64"\n", M->end);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintParametersIF(IF_PARAMETERS *M)
  {
  uint32_t n;

  fprintf(stderr, "[>] Header maximum length .......... %"PRIu64"\n", 
  M->headerMax);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintParametersVI(VI_PARAMETERS *M)
  {
  uint32_t n;

  fprintf(stderr, "[>] Enlarge ........................ %"PRIu64"\n", M->enlarge);
  fprintf(stderr, "[>] Space .......................... %"PRIu64"\n", M->space);
  fprintf(stderr, "[>] Width .......................... %"PRIu64"\n", M->width);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

