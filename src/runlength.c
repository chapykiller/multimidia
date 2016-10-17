#include "runlength.h"

int * readRunlength(int * data, int datasize, int * retsizeparam){
	int * ret = NULL;

	int i, j;
	int bitcount = 0;

	{
		for(i=0; i<datasize; i++){
			bitcount += data[i];
		}

		ret = (int *)malloc(bitcount/32*sizeof(int));
	}

	int currentbit = 0;
	int bitoffset = 0;

	for(i=0; i<datasize; i++){
		for(j=0; j<data[i]; j++){
			if(bitoffset%32==0)
				ret[bitoffset/32]=0;

			ret[bitoffset/32] = ret[bitoffset/32] + ( currentbit<<(31-bitoffset%32) );
			bitoffset++;
		}

		currentbit = !currentbit;
	}

	(*retsizeparam) = bitcount/32;
	return ret;
}

int * writeRunlength(int * data, int datasize, int * retsize){
	int i, j;

	int * ret = NULL;
	(*retsize) = 0;

	int currentbit = 0;
	int bitfreq = 0;

	for(i=0; i<datasize; i++){
		for(j=31; j>=0; j--){
			int bit = (data[i]>>j) & 0x1;

			if(bit!=currentbit){
				ret = (int *)realloc(ret, (*retsize+1)*sizeof(int));

				ret[*retsize] = bitfreq;
				(*retsize)++;

				currentbit = !currentbit;
				bitfreq = 1;
			}else{
				bitfreq++;
			}
		}
	}


	ret = (int *)realloc(ret, (*retsize+1)*sizeof(int));

	ret[*retsize] = bitfreq;
	(*retsize)++;

	return ret;
}