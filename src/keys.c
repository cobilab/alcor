#include <stdlib.h>
#include <string.h>

#include "keys.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int KeyString(char *key)
  {
  for(int x = 0 ; x < NKEYS ; ++x)
    {
    K_STRUCT SK = LT_KEYS[x];
    if(strcmp(SK.key, key) == 0)
      return SK.val;
    }
  return K0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
