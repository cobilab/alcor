#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "msg.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void StringHasUniqueCharacters(char *str)
  {
  int x, y, length = strlen(str);
  for(int x = 0; x < length - 1; ++x)
    for(int y = x + 1; y < length; ++y)
      if(str[x] == str[y])
        {
        PrintWarning("alphabet string contains repetitions");
        exit(1);
        }
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ShiftBuffer(uint8_t *buf, int size, uint8_t new){
//  memmove(buf, buf + 1, size * sizeof(uint8_t));
  uint32_t x;
  uint32_t tmp[size];

  for(x = 1 ; x < size ; ++x)
    tmp[x-1] = buf[x];
  tmp[size-1] = new;

  for(x = 0 ; x < size ; ++x)
    buf[x] = tmp[x];
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *ReplaceSubStr(char *str, char *a, char *b){
  char *buf = (char *) Calloc(MAX_STR, sizeof(char));
  char *p;
  if(strlen(str) > MAX_STR){
    PrintWarning("string too long!");
    exit(1);
    }
  if(!(p = strstr(str, a)))
    return str;
  strncpy(buf, str, p-str);
  buf[p-str] = '\0';
  sprintf(buf+(p-str), "%s%s", b, p+strlen(a));
  return buf;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t *ReverseStr(uint8_t *str, uint32_t end){
  uint32_t start = 0;
  while(start < end)
    {
    str[start] ^= str[end];
    str[end]   ^= str[start];
    str[start] ^= str[end];
    ++start;
    --end;
    }
  return str;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Reverse(char *start)
  {
  size_t len = strlen(start);
  for(char *end = &start[len-1] ; start < end ; start++, end--)
    {
    char tmp = start[0];
    start[0] = end[0];
    end[0]   = tmp;
    }
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ReverseArrayStack(char array[], uint32_t size)
  {
  for (uint32_t low = 0, high = size - 1 ; low < high ; low++, high--)
    {
    uint32_t tmp = array[low];
    array[low]   = array[high];
    array[high]  = tmp;
    }
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void SortString(char str[]){
  char     tmp;
  int32_t  i, j, length = strlen(str);
  for(i = 0 ; i != length-1 ; ++i)
    for (j = i+1 ; j != length ; ++j)
      if(str[i] > str[j])
        {
        tmp    = str[i];
        str[i] = str[j];
        str[j] = tmp;
        }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *Fgets_backwards(char *str, int size, FILE *F)
  {
  int i;
  if(ftell(F) == 0) return NULL;
  for(i = 0; ftell(F) != 0 && i < size; i++)
    {
    fseek(F, -1, SEEK_CUR);
    str[i] = (char)fgetc(F);
    if(str[i] == '\n' && i != 0) break;
    fseek(F, -1, SEEK_CUR);
    }
  str[i] = '\0';
  return str;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *CloneString(char *str){
  char *clone;
  if(str == NULL)
    return NULL;
  strcpy((clone = (char*) Malloc((strlen(str) + 1) * sizeof(char))), str);
  return clone;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char *Cat(char *a, char *b){
  char *base = (char *) Malloc(strlen(a) + strlen(b) + 1);
  strcpy(base, a);
  strcat(base, b);
  return base;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET STRING TO ARGV
//
int32_t StrToArgv(char *s, char ***v){
  int32_t n = 0, c = 2;
  char *d = strdup(s);   // STRDUP <=> CLONE STR
  while(d[n]) if(d[n++] == ' ') ++c;
  *v = (char **) Calloc(c, sizeof(char *));
  n = 0; (*v)[0] = d; c = 1;
  do if(d[n] == ' '){
       d[n] = '\0';
       (*v)[c++] = d+n+1;
       }
  while(d[++n]);
  return c;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
