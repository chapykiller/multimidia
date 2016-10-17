#include "stdlib.h"
#include "bitvector.h"


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

hftree * readTree();
hftree * buildTree(int * data, int datalength);

int * readHuffmanData(hftree * tree, int * data, int datasize);
int * writeHuffmanData(hftree * tree);

void sortNodes(hfnode ** array, int arraysize);

hfnode ** buildHuffmanMap(hfnode * node, int * returnsize, bitvector * prefix);
