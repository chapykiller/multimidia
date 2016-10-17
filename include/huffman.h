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

hfnode * readTree(bitvector * bv, int * current, int bitsperword);
hftree * buildTree(int * data, int datalength);

int * readHuffmanData(int * data, int datasize, int * retsize);
int * writeHuffmanData(int * data, int datasize, int * retsize);

void sortNodes(hfnode ** array, int arraysize);

hfnode ** buildHuffmanMap(hfnode * node, int * returnsize, bitvector * prefix);

void freeHuffmanTree(hfnode * node);

int findHuffmanCode(hfnode ** vector, int min, int max, int value);

#endif