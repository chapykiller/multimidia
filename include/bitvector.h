#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

typedef struct bvector{
	uint8_t * content;

	int bitamount;
	int byteamount;	
} bitvector;

bitvector * bv_new();

void bv_push(bitvector * bv, uint8_t bit);

int bv_get(bitvector * bv, int pos);

void bv_free(bitvector * bv);

bitvector * bv_copy(bitvector * bv);