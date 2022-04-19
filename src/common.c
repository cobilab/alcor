#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include "mem.h"
#include "common.h"
#include "msg.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t NBytesInFile(FILE *file){
  uint64_t size = 0;
  fseeko(file, 0, SEEK_END);
  size = ftello(file);
  rewind(file);
  return size;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t FopenBytesInFile(const char *fn)
  {
  uint64_t size = 0;
  FILE *file = Fopen(fn, "r");

  size = NBytesInFile(file);
  fclose(file);

  return size;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FILE *Fopen(const char *path, const char *mode)
  {
  FILE *file = fopen(path, mode);

  if(file == NULL)
    {
    fprintf(stderr, "Error opening: %s (mode %s). Does the file exist?\n",
    path, mode);
    exit(1);
    }

  return file;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FAccessWPerm(char *fn)
  {
  if(access(fn, F_OK) != -1)
    {
    fprintf(stderr, "Error: file %s already exists!\n", fn);
    if(access(fn, W_OK) != -1)
      fprintf(stderr, "Note: file %s has write permission.\nTip: to force "
      "writing rerun with \"-f\" option.\nWarning: on forcing, the old (%s) "
      "file will be deleted permanently.\n", fn, fn);
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void TestReadFile(char *fn)
  {
  FILE *f = NULL;
  f = Fopen(fn, "r");
  fclose(f);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CalcProgress(uint64_t size, uint64_t i)
  {
  if(i % (size / 100) == 0 && size > PROGRESS_MIN)
    fprintf(stderr, "Progress:%3d %% \r", (uint8_t) (i / (size / 100)));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t CmpCheckSum(uint32_t cs, uint32_t checksum)
  {
  if(checksum != cs)
    {
    fprintf(stderr, "Error: invalid reference file!\n"
    "Compression reference checksum ................. %u\n"
    "Decompression reference checksum ............... %u\n",
    cs, checksum);
    fprintf(stderr, "Tip: rerun with correct reference file\n");
    return 1;
    }
  return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CheckStdinEmpty(void)
  {
  if(isatty(STDIN_FILENO))
    {
    PrintWarning("stdin is empty!");
    exit(1);
    }
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CheckFileEmpty(char *s)
  {
  FILE *F = Fopen(s, "r");
  int c = fgetc(F);
  if(c == EOF)
    {
    PrintWarning("file is empty!");
    exit(1);
    }
  else 
    ungetc(c, F);
  fclose(F);  
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

