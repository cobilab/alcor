#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include "mem.h"
#include "common.h"
#include "dna.h"
#include "msg.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CountFastaReadsAndMax(FASTA_READS *FA, char *fn)
  {
  uint8_t  buffer[BUFFER_SIZE], sym = 0, header = 1;
  uint32_t k, idx;
  uint64_t n_symbols = 0;

  FILE *F = Fopen(fn, "r");
  
  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      {
      sym = buffer[idx];
      if(sym == '>')
        { 
	header = 1;
	if(n_symbols > FA->max_nsym)
	  FA->max_nsym = n_symbols;
	n_symbols = 0;
	++FA->nReads; 
	continue; 
	}
      if(sym == '\n' && header == 1){ header = 0; continue; }
      if(sym == '\n') continue;
      if(header == 1) continue;
      ++n_symbols;
      }

  if(n_symbols > FA->max_nsym)
    FA->max_nsym = n_symbols;

  fclose(F);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SA *CreateSA(void)
  {
  SA *S    = (SA *) Calloc(1, sizeof(SA));
  S->array = (uint64_t *) Calloc(1, sizeof(uint64_t));
  S->size  = 1;
  S->idx   = 0;
  return S;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateSA(SA *S, uint64_t len)
  {
  if(S->idx == 0) S->array[S->idx] = len;
  else            S->array[S->idx] = S->array[S->idx-1] + len;
  S->array = (uint64_t *) Realloc(S->array, ++S->size * sizeof(uint64_t), 
	     sizeof(uint64_t));
  ++S->idx;
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveSA(SA *S)
  {
  free(S->array);
  free(S);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetCumulativeReadsLength(SA *S, char *fn)
  {
  uint8_t buffer[BUFFER_SIZE], sym = 0, header = 1;
  uint32_t k, idx;
  FILE *F = Fopen(fn, "r");
  uint64_t seqLen = 0;
  uint64_t readIdx = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      {
      sym = buffer[idx];
      if(sym == '>')
        {
        if(readIdx > 0)
	  UpdateSA(S, seqLen);

        header = 1;
        ++readIdx;
        seqLen = 0;
        continue;
        }
      if(sym == '\n' && header == 1)
        {
        header = 0;
        continue;
        }
      if(sym == '\n') continue;
      if(header == 1) continue;

      ++seqLen;
      }

  UpdateSA(S, seqLen);

  fclose(F);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FASTA_READS *CreateFastaReads(void)
  {
  FASTA_READS *FA = (FASTA_READS *) Calloc(1, sizeof(FASTA_READS));
  FA->nReads      = 0;
  FA->max_nsym    = 0;
  return FA;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveFastaReads(FASTA_READS *FA)
  {
  free(FA);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NDNASyminFile(FILE *F)
  {
  uint8_t  buffer[BUFFER_SIZE];
  uint32_t k, idx;
  uint64_t nSymbols = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      switch(buffer[idx])
        {
        case 'A': ++nSymbols; break;
        case 'T': ++nSymbols; break;
        case 'C': ++nSymbols; break;
        case 'G': ++nSymbols; break;
        }

  rewind(F);
  return nSymbols;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NDNASymInFasta(FILE *F)
  {
  uint8_t  buffer[BUFFER_SIZE], sym = 0, header = 1;
  uint32_t k, idx;
  uint64_t nSymbols = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      {
      sym = buffer[idx];
      if(sym == '>'){ header = 1; continue; }
      if(sym == '\n' && header == 1){ header = 0; continue; }
      if(sym == '\n') continue;
      if(sym == 'N' ) continue;
      if(header == 1) continue;
      // FINAL FILTERING DNA CONTENT
      if(sym != 'A' && sym != 'C' && sym != 'G' && sym != 'T') continue;
      ++nSymbols;
      }

  rewind(F);
  return nSymbols;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NSymInFasta(FILE *F)
  {
  uint8_t  buffer[BUFFER_SIZE], sym = 0, header = 1;
  uint32_t k, idx;
  uint64_t nSymbols = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      {
      sym = buffer[idx];
      if(sym == '>'){ header = 1; continue; }
      if(sym == '\n' && header == 1){ header = 0; continue; }
      if(sym == '\n') continue;
      if(header == 1) continue;
      // FINAL FILTERING DNA CONTENT
      ++nSymbols;
      }

  rewind(F);
  return nSymbols;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NDNASymInFastq(FILE *F)
  {
  uint8_t  buffer[BUFFER_SIZE], sym = 0, line = 0, dna = 0;
  uint32_t k, idx;
  uint64_t nSymbols = 0;

  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      {
      sym = buffer[idx];
      switch(line)
        {
        case 0: if(sym == '\n'){ line = 1; dna = 1; } break;
        case 1: if(sym == '\n'){ line = 2; dna = 0; } break;
        case 2: if(sym == '\n'){ line = 3; dna = 0; } break;
        case 3: if(sym == '\n'){ line = 0; dna = 0; } break;
        }
      if(dna == 0 || sym == '\n') continue;
      if(dna == 1 && sym == 'N' ) continue;

      // FINAL FILTERING DNA CONTENT
      if(sym != 'A' && sym != 'C' && sym != 'G' && sym != 'T') continue;
      ++nSymbols;
      }

  rewind(F);
  return nSymbols;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t DNASymToNum(uint8_t symbol)
  {
  switch(symbol)
    {
    case 'A': case 'a': return 0;
    case 'T': case 't': return 3;
    case 'C': case 'c': return 1;
    case 'G': case 'g': return 2;
    default : return 4;
    }
  return 5;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t NumToDNASym(uint8_t symbol)
  {
  switch(symbol)
    {
    case 0: return 'A';
    case 3: return 'T';
    case 1: return 'C';
    case 2: return 'G';
    default: fprintf(stderr, "Error: unknown numerical symbols\n"); 
    exit(1);
    }
  return 'X';
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetCompSym(uint8_t symbol)
  {
  switch(symbol)
    {
    case 'A': case 'a': return 'T';
    case 'C': case 'c': return 'G';
    case 'G': case 'g': return 'C';
    case 'T': case 't': return 'A';
    default:  return symbol;
    }
  return 'X';
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetCompNum(uint8_t symbol)
  {
  switch(symbol)
    {
    case 0: return 3;
    case 1: return 2;
    case 2: return 1;
    case 3: return 0;
    default: fprintf(stderr, "symbol\n");
    return symbol;
    }
  return 5;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComplementArrayStack(uint8_t array[], uint32_t size)
  {
  for(uint32_t x = 0 ; x < size ; ++x)
    switch(array[x])
      {
      case 'A': case 'a': array[x] = 'T'; break; 
      case 'C': case 'c': array[x] = 'G'; break;
      case 'G': case 'g': array[x] = 'C'; break;
      case 'T': case 't': array[x] = 'A'; break;
      default: break;
      }
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CheckInputIsFASTA(void)
  {
  int c;
  if((c = fgetc(stdin)) != '>') // THIS IS ONLY A FAST/BASIC CHECK
    {
    PrintWarning("input requires a FASTA file!");
    exit(1);
    }
  ungetc(c, stdin); // put back
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CheckFileIsFASTA(char *s)
  {
  FILE *F = Fopen(s, "r");
  int c = fgetc(F);

  if(c != '>') // THIS IS ONLY A FAST/BASIC CHECK
    {
    PrintWarning("input requires a FASTA file!");
    exit(1);
    }

  fclose(F);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ExtractSeqFromFASTA (uint32_t init, uint32_t end, char *name, char seq[],
char *alphabet, uint32_t dna)
  {
  uint8_t buffer[BUFFER_SIZE], sym = 0, header = 1;
  uint32_t k, idx, nSymbols = 0, x = 0;

  FILE *F = Fopen(name, "r");

  assert(end > init);

  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(idx = 0 ; idx < k ; ++idx)
      {
      sym = buffer[idx];
      if(sym == '>'){ header = 1; continue; }
      if(sym == '\n' && header == 1){ header = 0; continue; }
      if(sym == '\n') continue;
      if(header == 1) continue;
      if(dna == 1 && strchr(alphabet, sym) == NULL) continue;
      if(++nSymbols >= init && nSymbols <= end && x <= (end-init))
        seq[x++] = sym;
      }

  fclose(F);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

