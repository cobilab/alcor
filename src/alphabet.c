#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "alphabet.h"
#include "mem.h"
#include "common.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintID(ALPHABET *A, int id)
  {
  switch(id)
    {
    case 9:
      fprintf(stderr, "  [+] %3d :'\\t' ( %"PRIu64" )\n", id, A->counts[id]);
    break;
    case 10:
      fprintf(stderr, "  [+] %3d :'\\n' ( %"PRIu64" )\n", id, A->counts[id]);
    break;
    default:
      fprintf(stderr, "  [+] %3d :'%c' ( %"PRIu64" )\n", id, id, A->counts[id]);
    break;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ALPHABET *CreateAlphabet(void)
  {
  ALPHABET *A     = (ALPHABET *) Calloc(1,                 sizeof(ALPHABET));
  A->numeric      = (uint8_t  *) Calloc(ALPHABET_MAX_SIZE, sizeof(uint8_t));
  A->toChars      = (uint8_t  *) Calloc(ALPHABET_MAX_SIZE, sizeof(uint8_t));
  A->revMap       = (uint8_t  *) Calloc(ALPHABET_MAX_SIZE, sizeof(uint8_t));
  A->alphabet     = (uint8_t  *) Calloc(ALPHABET_MAX_SIZE, sizeof(uint8_t));
  A->mask         = (uint8_t  *) Calloc(ALPHABET_MAX_SIZE, sizeof(uint8_t));
  A->counts       = (uint64_t *) Calloc(ALPHABET_MAX_SIZE, sizeof(uint64_t));
  A->length       = 0;
  A->cardinality  = 0;
  return A;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetAlphabet(ALPHABET *A)
  {
  uint32_t x;
  A->cardinality = 0;
  for(x = 0 ; x < ALPHABET_MAX_SIZE ; x++)
    {
    if(A->mask[x] == 1)
      {
      A->toChars[A->cardinality] = x;
      A->revMap[x] = A->cardinality++;
      }
    else
      A->revMap[x] = INVALID_SYMBOL;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LoadAlphabet(ALPHABET *A, char *fname)
  {
  uint64_t size = 0;
  uint32_t x;
  int32_t  k, header = 0, skip = 0;
  FILE     *F  = Fopen(fname, "r");
  uint8_t  *buffer, sym;
 
  buffer = (uint8_t *) Calloc(BUFFER_SIZE + 1, sizeof(uint8_t));
  
  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(x = 0 ; x < k ; ++x)
      {
      sym = buffer[x];
      switch(sym)
        {
        case '>':  header = 1; skip = 0; continue;
        case '\n': header = 0; continue;
        default: if(header==1) continue;
        }
      A->mask[sym] = 1;
      A->counts[sym]++;
      ++size;
      }

  Free(buffer);
  A->length = size;
  A->maxEntropy = (double) A->length * log2(A->cardinality);
  ResetAlphabet(A);
  fclose(F);
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LoadAlphabetWithExceptions(ALPHABET *A, char *name, char *alphabet)
  {
  uint64_t size = 0;
  uint32_t x, header = 1;
  int64_t  k;
  uint8_t  *buffer, sym;
  FILE     *F = fopen(name, "r");

  buffer = (uint8_t *) Calloc(BUFFER_SIZE+1, sizeof(uint8_t));
  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(x = 0 ; x < k ; ++x)
      {
      sym = buffer[x];
      if(sym == '>'){ header = 1; continue; }
      if(sym == '\n' && header == 1){ header = 0; continue; }
      if(sym == '\n') continue;
      if(header == 1) continue;
      if(strchr(alphabet, sym) == NULL) continue;
      A->mask[sym] = 1;
      A->counts[sym]++;
      ++size;
      }

  A->length = size;
  ResetAlphabet(A);
  fclose(F);
  Free(buffer);
  
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LoadAlphabet2Files(ALPHABET *A, char *fname1, char *fname2)
  {
  uint64_t size = 0;
  uint32_t x;
  int32_t  k, header = 0, skip = 0;
  uint8_t  *buffer, *buffer2;
  FILE     *F  = Fopen(fname1, "r");
  FILE     *F2 = Fopen(fname2, "r");

  buffer = (uint8_t *) Calloc(BUFFER_SIZE + 1, sizeof(uint8_t));
  while((k = fread(buffer, 1, BUFFER_SIZE, F)))
    for(x = 0 ; x < k ; ++x)
      {
      switch(buffer[x])
        {
        case '>':  header = 1; skip = 0; continue;
        case '\n': header = 0; continue;
        default: if(header==1) continue;
        }
      A->mask[buffer[x]] = 1;
      A->counts[buffer[x]]++;
      ++size;
      }

  skip = 0;
  header = 0;
  buffer2 = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  while((k = fread(buffer2, 1, BUFFER_SIZE, F2)))
    for(x = 0 ; x < k ; ++x)
      {
      switch(buffer2[x])
        {
        case '>':  header = 1; skip = 0; continue;
        case '\n': header = 0; continue;
        default: if(header==1) continue;
        }
      A->mask[buffer2[x]] = 1;
      A->counts[buffer2[x]]++;
      ++size;
      }

  Free(buffer);
  Free(buffer2);

  A->length = size;
  ResetAlphabet(A);
  
  fclose(F);
  fclose(F2);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintAlphabet(ALPHABET *A)
  {
  int x;
  fprintf(stderr, "[>] FASTA sequence size: %"PRIu64"\n", A->length);
  fprintf(stderr, "[>] Alphabet size: %u\n", A->cardinality);
  fprintf(stderr, "[>] Alphabet distribution: \n");
  for(x = 0 ; x < A->cardinality ; ++x)
    PrintID(A, (int) A->toChars[x]);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveAlphabet(ALPHABET *A)
  {
  Free(A->numeric);
  Free(A->toChars);
  Free(A->revMap);
  Free(A->alphabet);
  Free(A->mask);
  Free(A->counts);
  Free(A);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
