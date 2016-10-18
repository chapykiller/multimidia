#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include "stdlib.h"

#include "bitvector.h"
#include "auxiliary.h"

typedef struct huffmannode{
	struct huffmannode ** children;

	int isLeaf;
	int frequency;
	int data;

	bitvector * prefix;
} hfnode;

typedef struct huffmantree{
	hfnode * root;
} hftree;

// Decodifica uma árvore de Huffman
hfnode * readTree(bitvector * bv, int * current, int bitsperword);
// Codifica uma árvore de huffman
hftree * buildTree(int * data, int datalength);

// Codifica usando compressão de Huffman
int * readHuffmanData(int * data, int datasize, int * retsize);
// Decodifica usando compressão de Huffman
int * writeHuffmanData(int * data, int datasize, int * retsize);

// Ordena os nós de uma árvore de Huffman que estão num vetor
// de acordo com seus códigos
void sortNodes(hfnode ** array, int l, int r);

// Constrói um vetor com os nós de huffman ordenado de acordo
// com seus códigos
hfnode ** buildHuffmanMap(hfnode * node, int * returnsize, bitvector * prefix);

// Libera a memória de uma árvore de Huffman
void freeHuffmanTree(hfnode * node);

// Encontra um determinado código de Huffman num vetor ordenado
int findHuffmanCode(hfnode ** vector, int min, int max, int value);

#endif