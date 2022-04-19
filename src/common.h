#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "defs.h"

#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

typedef struct{
  uint64_t size;
  uint64_t bytes;
  uint64_t header;
  }
INF;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FILE       *Fopen                (const char *, const char *);
uint64_t   NBytesInFile          (FILE *);
uint64_t   FopenBytesInFile      (const char *);
void       TestReadFile          (char *);
uint8_t    CmpCheckSum           (uint32_t, uint32_t);
void       FAccessWPerm          (char *);
void       CalcProgress          (uint64_t , uint64_t);
void       CheckStdinEmpty       (void);
void       CheckFileEmpty        (char *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
