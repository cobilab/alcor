#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// RUNNING COMPILATION PROPERTIES :: COMMENT OR UNCOMMENT 

#define PROGRESS // DISPLAY % OF PROGRESS
#define PROFILE  // DISPLAY A PROFILE WITH EXISTS OR NOT

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef uint64_t U64;
typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t  U8; 
typedef int64_t  I64;
typedef int32_t  I32;
typedef int16_t  I16;
typedef int8_t   I8;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define PNAME                  "AlcoR"
#define VERSION                1
#define RELEASE                1

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define MAX_STR                2048
#define BUFFER_SIZE            65535    
#define PROGRESS_MIN           200
#define DEF_HELP               0
#define DEF_FORCE              0
#define DEF_VERBOSE            0
#define DEF_VV                 0
#define DEF_IR                 1
#define DEF_AA                 1
#define DEF_OUT                0
#define DEF_PROF_G             1
#define DEF_PROF_R             1
#define DEF_SPLIT              1
#define DEF_PLOTS              0
#define DEF_MIN_CTX            11
#define DEF_MAX_CTX            15
#define DEF_THREADS            1
#define BGUARD                 32
#define ALPHABET_SIZE          4

#define MIN_NPARAM_FOR_PROGS   2

#define TARGET                 0
#define REFERENCE              1

#define DEF_LR_HELP            0
#define DEF_LR_FORCE           0
#define DEF_LR_VERBOSE         0
#define DEF_LR_THRESHOLD       0
#define DEF_LR_WINDOW          5
#define DEF_LR_MASK            0
#define DEF_LR_RENORMALIZE     0
#define DEF_LR_IGNORE          10
#define DEF_LR_REGION          500
#define DEF_LR_ALPHABET        "ACGT"
#define DEF_LR_DNA             0
#define DEF_LR_NOSIZE          0
#define DEF_LR_MIN_LEVEL       1
#define DEF_LR_MAX_LEVEL       15
#define DEF_LR_LEVEL           9
#define DEF_LR_COLOR           100
#define DEF_LR_HIDE            0

#define DEF_SI_HELP            0
#define DEF_SI_FORCE           0
#define DEF_SI_VERBOSE         0
#define DEF_SI_ALPHABET        "ACGT"
#define DEF_SI_DNA             1

#define DEF_EX_HELP            0
#define DEF_EX_VERBOSE         0
#define DEF_EX_FASTA           0
#define DEF_EX_INIT            1
#define DEF_EX_END             1

#define DEF_IF_HELP            0
#define DEF_IF_VERBOSE         0
#define DEF_IF_FASTA           0

#define DEF_VI_HELP            0
#define DEF_VI_VERBOSE         0
#define DEF_VI_SPACE           8
#define DEF_VI_WIDTH           20
#define DEF_VI_ENLARGE         0
#define DEF_VI_CORNER          0
#define DEF_VI_BACKCOLOR       "#FFFFFF"
#define DEF_VI_BORDERCOLOR     "#000000"
#define DEF_VI_COLOR           "#343434"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

