#ifndef LR_H_INCLUDED
#define LR_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#include "defs.h"
#include "param.h"
#include "threads.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void reverse           (char *);
void CompressTarget    (THREADS);
void *CompressThread   (void *);
void CompressAction    (THREADS *);
void LocalRedundancy   (LR_PARAMETERS *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

