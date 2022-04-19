#ifndef STRINGS_H_INCLUDED
#define STRINGS_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void     StringHasUniqueCharacters  (char *);
void     ShiftBuffer                (uint8_t *, int, uint8_t);
char     *ReplaceSubStr             (char *, char *, char *);
uint8_t  *ReverseStr                (uint8_t *, uint32_t);
void     ReverseArrayStack          (char [], uint32_t);
void     Reverse                    (char *);
char     *Fgets_backwards           (char *, int, FILE *);
char     *CloneString               (char *);
char     *Cat                       (char *, char *);
int32_t  StrToArgv                  (char *, char ***);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

