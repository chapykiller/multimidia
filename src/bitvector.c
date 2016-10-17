#include "bitvector.h"

bitvector * bv_new(){
	bitvector * newbv = (bitvector *)malloc(1*sizeof(bitvector));

	newbv->content = NULL;

	newbv->bitamount = 0;
	newbv->byteamount = 0;

	return newbv;
}

void bv_push(bitvector * bv, uint8_t bit){
	if(bv == NULL){
		printf("ERROR: bitvector is null pointer (bv_push)\n");
		exit(EXIT_FAILURE);
	}

	if(bv->bitamount/8 == bv->byteamount){
		if(bv->content == NULL){
			bv->byteamount = 1;
			bv->content = (uint8_t *)malloc(1*sizeof(uint8_t));
		}else{
			bv->byteamount *= 2;
			bv->content = (uint8_t *)realloc(bv->content, bv->byteamount*sizeof(uint8_t));
		}
	}

	// Calculando o byte no qual o novo bit deve ser inserido
	int currentbyte = bv->bitamount/8;

	// Limpando o bit antigo
	bv->content[currentbyte] = bv->content[currentbyte] & (~(((uint8_t)1)<<(bv->bitamount%8)));

	// Inserindo o novo bit
	bv->content[currentbyte] = bv->content[currentbyte] | (bit << (bv->bitamount%8));

	// Incrementando o contador de bits
	bv->bitamount++;

	return;
}

int bv_get(bitvector * bv, int pos){
	if(bv == NULL){
		printf("ERROR: bitvector is null pointer (bv_get)\n");
		exit(EXIT_FAILURE);
	}

	if(pos >= bv->bitamount || pos < 0){
		printf("ERROR: invalid position access [%d] (bv_get)\n", pos);
		exit(EXIT_FAILURE);
	}

	int currentbyte = pos/8;

	// printf("Retrieving: %2x\n", bv->content[currentbyte]);

	return (bv->content[currentbyte] >> (pos%8)) & 0x1;
}

void bv_free(bitvector * bv){
	if(bv->content != NULL)
		free(bv->content);
	free(bv);

	return;
}

bitvector * bv_copy(bitvector * bv){
	int i;

	bitvector * newbv = (bitvector *)malloc(1*sizeof(bitvector));

	newbv->bitamount = bv->bitamount;
	newbv->byteamount = bv->byteamount;

	if(bv->content != NULL){
		newbv->content = (uint8_t *)malloc(newbv->byteamount*sizeof(uint8_t));
	
		for(i=0; i<bv->byteamount; i++){
			newbv->content[i] = bv->content[i];
		}
	}else{
		newbv->content = NULL;
	}

	return newbv;
}

void bv_print(bitvector * bv){
	int i;

	for(i=0; i<bv->bitamount; i++){
		printf("%d", bv_get(bv, i));
	}

	return;
}