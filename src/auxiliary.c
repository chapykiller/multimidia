#include "auxiliary.h"

int getBitAmount(int32_t number){
	int i;

	// Analisa todos os bits, começando do mais significativo.
	// Quando encontra o primeiro bit mais significativo
	// diferente de 0, retorna sua posição+1
	for(i=31; i>=0; i--){
		if( (number>>i)&1 )
			return i+1;
	}

	return 0;
}

int8_t * shortenBytes(int * data, int datasize, int * retsize){
	// Número de bits necessários para representar o mais alto
	// número de vetor
	int maxbits = getBitAmount(data[0]);

	int i, j;
	for(i=0; i<datasize; i++){
		int bitsreq = getBitAmount(data[i]);

		if(bitsreq>maxbits){
			maxbits = bitsreq;
		}
	}

	// Calculando o número total de bits necessários para representar
	// todos os dados no vetor
	int totalbits = maxbits*datasize;

	// Cria o header, que contém o offset do último bit significativo
	// no último byte, e o número de bits por palavra
	int8_t headerbyte = ((totalbits%8)<<5) | (maxbits-1);
	totalbits += 8;

	// Calcula o total de bytes para armazenar todos os bits. Note
	// que (x + y - 1)/y = ceil(x/y)
	int totalbytes = (totalbits + 8 - 1)/8;

	// Aloca o vetor de retorno e atribui o header
	int8_t * ret = (int8_t *)calloc(totalbytes, sizeof(int8_t));
	ret[0] = headerbyte;

	// Começando do bit 8, começa a transcrever os dados do vetor
	// no vetor de bytes de retorno
	int currentbit=8;
	for(i=0; i<datasize; i++){
		for(j=maxbits-1; j>=0; j--){
			ret[currentbit/8] = ret[currentbit/8] | (((data[i]>>j)&1)<<(7-currentbit%8));

			currentbit++;
		}
	}

	(*retsize) = totalbytes;
	return ret;
}

int32_t * unshortenBytes(int8_t * data, int datasize, int * retsize){
	// Separa o header e extrai seus dados
	int8_t headerbyte = data[0];

	int excessbits = ((headerbyte & 0xE0)>>5);
	if(excessbits==0)
		excessbits = 8;

	int bitsperword = (headerbyte & 0x1F)+1;
	int totalbits = datasize*8 - 8 + excessbits;

	// Calcula o tamanho do vetor de retorno
	int intcount = (totalbits-8)/bitsperword;
	int32_t * ret = (int32_t *)calloc(intcount, sizeof(int32_t));

	// currentbit guarda a posição do último bit
	// a qual não foi escrito nada no vetor de saída
	int currentbit;

	int buffer = 0;
	int buffersize = 0;
	int currentint = 0;

	// Transcreve e separa os bits em ints
	for(currentbit=8; currentbit<totalbits; currentbit++){
		int bit = (data[currentbit/8]>>(7-currentbit%8)) & 1;

		// Reconstruindo o dado original
		buffer = (buffer<<1) | bit;
		buffersize++;

		// Quando o buffer atingir o tamanho de uma palavra, então
		// colocamos seu conteúdo na posição apropriada do vetor de
		// saída
		if(buffersize==bitsperword){
			ret[currentint++] = buffer;

			buffer = 0;
			buffersize = 0;
		}
	}

	(*retsize) = intcount;
	return ret;
}

void printBits(int * v, int size){
	int i, j;

	for(i=0; i<size; i++){
		printf("%8x ", v[i]);

		if(i%8==0)
			printf("\n");
	}
}