#ifndef ALPHABET_H_INCLUDED
#define ALPHABET_H_INCLUDED

#include "defs.h"

#define ALPHABET_MAX_SIZE 256
#define INVALID_SYMBOL    255

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct 
  {
  uint8_t    *numeric;        // Symbols in numbers (20213413...) from file
  uint8_t    *toChars;        // chars: { 'A', 'C', 'G', 'T', 'N', ...}
  uint8_t    *revMap;         // Reverse symbols to numbers
  uint8_t    *alphabet;
  uint8_t    *mask;           // binary vector with symbol existing or not
  uint64_t   *counts;         // counts for symbol distribution
  uint64_t   length;          // total size of the symbols
  int        cardinality;
  double     maxEntropy;
  }
ALPHABET;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ALPHABET  *CreateAlphabet             (void);
void      ResetAlphabet               (ALPHABET *);
void      LoadAlphabet                (ALPHABET *, char *);
void      LoadAlphabetWithExceptions  (ALPHABET *, char *, char *);
void      LoadAlphabet2Files          (ALPHABET *, char *, char *);
void      PrintAlphabet               (ALPHABET *);
void      RemoveAlphabet              (ALPHABET *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
