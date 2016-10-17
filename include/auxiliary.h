#ifndef _AUXILIARY_H_
#define _AUXILIARY_H_

#include "stdlib.h"

// getBitAmount retorna o mínimo número de bits necessários para
// representar o número passado como argumento
int getBitAmount(int number);

// shortenBytes analisa o mínimo número de bits necessários para
// que seja possível representar todos os dados no vetor, e trans
// forma o vetor num vetor de bits, reduzindo o número de bits por
// palavra
int8_t * shortenBytes(int * data, int datasize, int * retsize);

// unshortenBytes faz o processo inverso de shortenBytes
int * unshortenBytes(int8_t * data, int datasize, int * retsize);

#endif 