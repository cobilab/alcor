#include <string.h>

#include "buffer.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATE CONTEXT BUFFER
//
CBUF *CreateCBuffer(uint32_t s, uint32_t g)
  {
  CBUF *B  = (CBUF *) Calloc(1, sizeof(CBUF));
  B->size  = s;
  B->guard = g;
  B->buf   = (uint8_t *) Calloc(B->size+B->guard, sizeof(uint8_t));
  B->buf  += B->guard;
  B->idx   = 0;
  return B;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATE ETERNAL BUFFER (UNTIL RAM GOES AWAY)
//
EBUF *CreateEBuffer(uint64_t s, uint64_t i)
  {
  EBUF *B      = (EBUF *) Calloc(1, sizeof(EBUF));
  B->size      = s;
  B->increment = i;
  B->buf       = (uint8_t *) Calloc(B->size, sizeof(uint8_t));
  B->idx       = 0;
  return B;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATE SIMPLE BUFFER
//
BUF *CreateBuffer(uint32_t s)
  {
  BUF *B  = (BUF *) Calloc(1, sizeof(BUF));
  B->size = s;
  B->buf  = (uint8_t *) Calloc(B->size, sizeof(uint8_t));
  B->idx  = 0;
  return B;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE CONTEXT BUFFER
//
void UpdateCBuffer(CBUF *B)
  {
  if(++B->idx == B->size)
    {
    memcpy(B->buf-B->guard, B->buf+B->idx-B->guard, B->guard);
    B->idx = 0;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE ETERNAL BUFFER
//
void UpdateEBuffer(EBUF *B)
  {
  if(++B->idx == B->size)
    {
    B->size += B->increment;
    B->buf = (char *) Realloc(B->buf, B->size, B->increment);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE SIMPLE BUFFER
//
void UpdateBuffer(BUF *B)
  {
  if(++B->idx == B->size)
    B->idx = 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// RESET ETERNAL BUFFER
//
void ResetEBuffer(EBUF *B)
  {
  memset(B->buf, '\0', B->size * sizeof(char));
  B->idx = 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REMOVE CONTEXT BUFFER
//
void RemoveCBuffer(CBUF *B)
  {
  Free(B->buf-B->guard);
  Free(B);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REMOVE ETERNAL BUFFER
//
void RemoveEBuffer(EBUF *B)
  {
  Free(B->buf);
  Free(B);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REMOVE SIMPLE BUFFER
//
void RemoveBuffer(BUF *B)
  {
  Free(B->buf);
  Free(B);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
