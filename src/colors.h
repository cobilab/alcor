#ifndef COLORS_H_INCLUDED
#define COLORS_H_INCLUDED

#include <stdio.h>

#define MAX_COLOR 21

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char colors[22][16] = 
          { "\x1B[41m",  "\x1B[42m",  "\x1B[43m",  "\x1B[44m",
	    "\x1B[45m",  "\x1B[46m",  "\x1B[47m",  "\x1B[31m", 
	    "\x1B[32m",  "\x1B[33m",  "\x1B[34m",  "\x1B[35m",
	    "\x1B[36m",  "\x1B[37m",  "\x1B[100m", "\x1B[101m", 
	    "\x1B[102m", "\x1B[103m", "\x1B[104m", "\x1B[105m", 
	    "\x1B[106m", "\x1B[107m"}; 

char normal_color[10] = "\x1B[0m";
char error_color[10]  = "\x1B[41m";
char info_color[10]   = "\x1B[44m";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

