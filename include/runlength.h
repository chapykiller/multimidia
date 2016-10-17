#ifndef _RUNLENGTH_H_
#define _RUNLENGTH_H_

#include "stdlib.h"
#include "stdio.h"
#include "auxiliary.h"

int * readRunlength(int * data, int datasize, int * retsize);
int * writeRunlength(int * data, int datasize, int * retsizeparam);

#endif