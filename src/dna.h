#ifndef DNA_H_INCLUDED
#define DNA_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct
  {
  uint64_t nReads;
  uint32_t max_nsym;
  }
FASTA_READS;

typedef struct
  {
  uint64_t idx;
  uint64_t *array;
  uint64_t size;
  }
SA;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FASTA_READS  *CreateFastaReads        (void);
void         CountFastaReadsAndMax    (FASTA_READS *, char *);
void         RemoveFastaReads         (FASTA_READS *);
void         GetCumulativeReadsLength (SA *, char *);
SA           *CreateSA                (void);
void         UpdateSA                 (SA *, uint64_t);
void         RemoveSA                 (SA *);
uint64_t     NDNASyminFile            (FILE *);
uint64_t     NDNASymInFasta           (FILE *);
uint64_t     NSymInFasta              (FILE *);
uint64_t     NDNASymInFastq           (FILE *);
uint8_t      DNASymToNum              (uint8_t);
uint8_t      NumToDNASym              (uint8_t);
uint8_t      GetCompNum               (uint8_t);
uint8_t      GetCompSym               (uint8_t);
void         ComplementArrayStack     (uint8_t [], uint32_t);
void         CheckInputIsFASTA        (void);
void         CheckFileIsFASTA         (char *);
void         ExtractSeqFromFASTA      (uint32_t, uint32_t, char *, char[], 
		                       char *, uint32_t);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
