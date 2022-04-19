#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "levels.h"
#include "msg.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *GetLevelsLR(uint8_t l)
  {
  switch(l)
    {
    case 1:  return LR_LEVEL_1;
    case 2:  return LR_LEVEL_2;
    case 3:  return LR_LEVEL_3;
    case 4:  return LR_LEVEL_4;
    case 5:  return LR_LEVEL_5;
    case 6:  return LR_LEVEL_6;
    case 7:  return LR_LEVEL_7;
    case 8:  return LR_LEVEL_8;
    case 9:  return LR_LEVEL_9;
    case 10: return LR_LEVEL_10;
    case 11: return LR_LEVEL_11;
    case 12: return LR_LEVEL_12;
    case 13: return LR_LEVEL_13;
    case 14: return LR_LEVEL_14;
    case 15: return LR_LEVEL_15;
    default: PrintWarning("Unknown level!");
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *GetLevelsNC(uint8_t l)
  {
  switch(l)
    {
    case 1:  return NC_LEVEL_1;
    case 2:  return NC_LEVEL_2;
    case 3:  return NC_LEVEL_3;
    case 4:  return NC_LEVEL_4;
    case 5:  return NC_LEVEL_5;
    case 6:  return NC_LEVEL_6;
    case 7:  return NC_LEVEL_7;
    case 8:  return NC_LEVEL_8;
    case 9:  return NC_LEVEL_9;
    case 10: return NC_LEVEL_10;
    case 11: return NC_LEVEL_11;
    case 12: return NC_LEVEL_12;
    case 13: return NC_LEVEL_13;
    case 14: return NC_LEVEL_14;
    case 15: return NC_LEVEL_15;
    default: PrintWarning("Unknown level!");
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *GetLevelsNCD(uint8_t l)
  {
  switch(l)
    {
    case 1:  return NCD_LEVEL_1;
    case 2:  return NCD_LEVEL_2;
    case 3:  return NCD_LEVEL_3;
    case 4:  return NCD_LEVEL_4;
    case 5:  return NCD_LEVEL_5;
    case 6:  return NCD_LEVEL_6;
    case 7:  return NCD_LEVEL_7;
    case 8:  return NCD_LEVEL_8;
    case 9:  return NCD_LEVEL_9;
    case 10: return NCD_LEVEL_10;
    case 11: return NCD_LEVEL_11;
    case 12: return NCD_LEVEL_12;
    case 13: return NCD_LEVEL_13;
    case 14: return NCD_LEVEL_14;
    case 15: return NCD_LEVEL_15;
    default: PrintWarning("Unknown level!");
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintLevelsLR(void)
  {
  fprintf(stderr, "Level%s\n", LR_LEVEL_1);
  fprintf(stderr, "Level%s\n", LR_LEVEL_2);
  fprintf(stderr, "Level%s\n", LR_LEVEL_3);
  fprintf(stderr, "Level%s\n", LR_LEVEL_4);
  fprintf(stderr, "Level%s\n", LR_LEVEL_5);
  fprintf(stderr, "Level%s\n", LR_LEVEL_6);
  fprintf(stderr, "Level%s\n", LR_LEVEL_7);
  fprintf(stderr, "Level%s\n", LR_LEVEL_8);
  fprintf(stderr, "Level%s\n", LR_LEVEL_9);
  fprintf(stderr, "Level%s\n", LR_LEVEL_10);
  fprintf(stderr, "Level%s\n", LR_LEVEL_11);
  fprintf(stderr, "Level%s\n", LR_LEVEL_12);
  fprintf(stderr, "Level%s\n", LR_LEVEL_13);
  fprintf(stderr, "Level%s\n", LR_LEVEL_14);
  fprintf(stderr, "Level%s\n", LR_LEVEL_15);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintLevelsNC(void)
  {
  fprintf(stderr, "Level%s\n", NC_LEVEL_1);
  fprintf(stderr, "Level%s\n", NC_LEVEL_2);
  fprintf(stderr, "Level%s\n", NC_LEVEL_3);
  fprintf(stderr, "Level%s\n", NC_LEVEL_4);
  fprintf(stderr, "Level%s\n", NC_LEVEL_5);
  fprintf(stderr, "Level%s\n", NC_LEVEL_6);
  fprintf(stderr, "Level%s\n", NC_LEVEL_7);
  fprintf(stderr, "Level%s\n", NC_LEVEL_8);
  fprintf(stderr, "Level%s\n", NC_LEVEL_9);
  fprintf(stderr, "Level%s\n", NC_LEVEL_10);
  fprintf(stderr, "Level%s\n", NC_LEVEL_11);
  fprintf(stderr, "Level%s\n", NC_LEVEL_12);
  fprintf(stderr, "Level%s\n", NC_LEVEL_13);
  fprintf(stderr, "Level%s\n", NC_LEVEL_14);
  fprintf(stderr, "Level%s\n", NC_LEVEL_15);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void PrintLevelsNCD(void)
  {
  fprintf(stderr, "Level%s\n", NCD_LEVEL_1);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_2);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_3);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_4);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_5);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_6);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_7);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_8);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_9);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_10);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_11);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_12);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_13);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_14);
  fprintf(stderr, "Level%s\n", NCD_LEVEL_15);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

