#include "runlength.h"

int * readRunlength(int * data, int datasize, int * retsizeparam){
	int * ret = NULL;

	int i, j;
	int bitcount = 0;

	{
		// Primeiro contamos o número total de bits que havia
		// no código original, e alocamos um vetor que possa
		// contê-los
		for(i=0; i<datasize; i++){
			bitcount += data[i];
		}

		ret = (int *)calloc(bitcount/32, sizeof(int));
	}

	// currentbit indica se estamos escrevendo 0's ou 1's
	int currentbit = 0;
	// bitoffset indica em qual bit estamos escrevendo na saída
	// atualmente
	int bitoffset = 0;

	// Percorrendo todo o vetor codificado, enquanto decodificamos
	for(i=0; i<datasize; i++){
		for(j=0; j<data[i]; j++){
			// Escrevendo o bit apropriado
			ret[bitoffset/32] = ret[bitoffset/32] + ( currentbit<<(31-bitoffset%32) );
			bitoffset++;
		}

		// Ao terminar de escrever todos os bits, invertemos o bit que
		// estamos escrevendo
		currentbit = !currentbit;
	}

	// Retorno por referência do tamanho do vetor de retorno
	(*retsizeparam) = bitcount/32;
	return ret;
}

int * writeRunlength(int * data, int datasize, int * retsize){
	int i, j;

	int * ret = NULL;
	(*retsize) = 0;

	// currentbit indica o bit sendo lido atualmente (0 ou 1),
	// enquanto bitfreq vai contando a quantidade de bits do
	// tipo apropriado lidos até o momento
	int currentbit = 0;
	int bitfreq = 0;

	for(i=0; i<datasize; i++){
		for(j=31; j>=0; j--){
			// Extraindo o bit atual
			int bit = (data[i]>>j) & 0x1;

			if(bit!=currentbit){
				// Se encontramos um bit diferente do qual estamos lendo
				// atualmente, guardamos sua frequência e invertemos
				// o bit que estamos lendo

				ret = (int *)realloc(ret, (*retsize+1)*sizeof(int));

				ret[*retsize] = bitfreq;
				(*retsize)++;

				currentbit = !currentbit;
				bitfreq = 1;
			}else{
				// Caso contrário, apenas incrementamos sua frequência
				bitfreq++;
			}
		}
	}

	// Salva a frequencia do último tipo de bit lido
	ret = (int *)realloc(ret, (*retsize+1)*sizeof(int));

	ret[*retsize] = bitfreq;
	(*retsize)++;

	return ret;
}