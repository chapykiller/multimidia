#ifndef _RUNLENGTH_H_
#define _RUNLENGTH_H_

#include "stdlib.h"
#include "stdio.h"
#include "auxiliary.h"

// Decodifica dados usando runlength
int * readRunlength(int * data, int datasize, int * retsize);

// Codifica usando runlength
int * writeRunlength(int * data, int datasize, int * retsizeparam);

#endif