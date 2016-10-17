#include "huffman.h"

hfnode * readTree(bitvector * bv, int * current, int bitsperword){
	hfnode * newnode = (hfnode *)malloc(1*sizeof(hfnode));

	if(bv_get(bv, *current)==0){
		// Nó não-folha
		(*current)++;

		newnode->isLeaf = false;
		newnode->children = (hfnode **)malloc(2*sizeof(hfnode *));

		newnode->frequency = 0;
		newnode->data = 0;
		newnode->prefix = NULL;

		newnode->children[0] = readTree(bv, current, bitsperword);
		newnode->children[1] = readTree(bv, current, bitsperword);

		return newnode;
	}else{
		// Nó folha
		(*current)++;

		newnode->isLeaf = true;
		newnode->children = NULL;

		newnode->frequency = 0;
		newnode->data = 0;
		newnode->prefix = NULL;

		int i;

		for(i=0; i<bitsperword; i++){
			newnode->data = (newnode->data << 1) + bv_get(bv, (*current));
			(*current)++;
		}

		return newnode;
	}
}

void writeTree(bitvector * bv, hfnode * node, int bitsperword){
	if(node->isLeaf){
		bv_push(bv, 1);

		int i;
		for(i=bitsperword-1; i>=0; i--)
			bv_push(bv, (node->data>>i)&1);

		return;
	}else{
		bv_push(bv, 0);

		writeTree(bv, node->children[0], bitsperword);
		writeTree(bv, node->children[1], bitsperword);

		return;
	}
}

hftree * buildTree(int * data, int datalength){
	hftree * localTree = (hftree *)malloc(1*sizeof(hftree));

	int i, j;

	// Buffers para armazenar os códigos temporariamente
	int * localdata = (int *)malloc(datalength*sizeof(int));
	int * bufferdata = (int *)malloc(datalength*sizeof(int));

	// Queremos preservar o datalength original,
	// por isso o salvamos
	int locallength = datalength;

	// Vetor de nós da árvore de huffman, usados posteriormente
	// para construir a árvore
	hfnode ** hfnodes = NULL;

	// Número de nós atualmente na árvore
	int nodeamount = 0;

	// Copiando os dados originais para nosso buffer
	for(i=0; i<datalength; i++){
		localdata[i] = data[i];
	}

	// Enquanto nosso buffer de códigos não estiver vazio, pegamos o primeiro código
	// no vetor, contamos sua frequencia, geramos um nó para a árvore de huffman
	// com este código e sua devida frequencia, e então copiamos todos os caracteres
	// que não são o que selecionamos para um novo vetor (bufferdata). Portanto, enquanto
	// nosso vetor não estiver vazio, repetimos esse processo.

	while(locallength!=0){
		// Armazenando o código do primeiro caractere no vetor
		int currentcode = localdata[0];
		int codefrequency = 0;

 		// bufferlength corresponde ao tamanho do vetor bufferdata
		int bufferlength = 0;

		// Percorremos o vetor inteiro de códigos. Se o i-ésimo caractere
		// for o código cuja frequência estamos contando, então
		// incrementamos sua frequência. Caso contrário, simplesmente
		// copiamos o caractere para um vetor buffer.
		for(i=0; i<locallength; i++){
			if(localdata[i]!=currentcode){
				bufferdata[bufferlength++] = localdata[i];
			}else{
				codefrequency++;
			}
		}

		// Já podemos copiar o vetor buffer para o vetor original
		for(i=0; i<bufferlength; i++){
			localdata[i] = bufferdata[i];
		}

 		// E podemos atualizar seu tamanho
		locallength = bufferlength;

		hfnode * newNode = (hfnode *)malloc(1*sizeof(hfnode));
		newNode->isLeaf = 1;
		newNode->data = currentcode;
		newNode->frequency = codefrequency;
		newNode->children = NULL;
		newNode->prefix = NULL;

		nodeamount++;

		if(hfnodes == NULL){
			hfnodes = (hfnode **)malloc(1*sizeof(hfnode *));
			hfnodes[0] = newNode;
		}else{
			hfnodes = (hfnode **)realloc(hfnodes, nodeamount*sizeof(hfnode *));
			hfnodes[nodeamount-1] = newNode;

			sortNodes(hfnodes, nodeamount);
		}
	}

	free(localdata);
	free(bufferdata);

	while(nodeamount>1){
		hfnode * newNode = (hfnode *)malloc(1*sizeof(hfnode));
		newNode->isLeaf = 0;
		newNode->children = (hfnode **)malloc(2*sizeof(hfnode *));

		newNode->children[0] = hfnodes[0];
		newNode->children[1] = hfnodes[1];

		newNode->prefix = NULL;
		newNode->frequency = 0;
		newNode->data = 0;

		hfnodes[0] = newNode;
		hfnodes[1] = hfnodes[nodeamount-1];

		nodeamount--;

		sortNodes(hfnodes, nodeamount);
	}

	localTree->root = hfnodes[0];

	free(hfnodes);

	return localTree;
}

int * readHuffmanData(int * data, int datasize, int * retsizeparam){
	// Vetor de retorno
	int * ret=malloc(1*sizeof(int));

	// Colocamos os dados no formato de um vetor de bits para
	// facilitar a lógica do programa
	bitvector * bitdata = bv_new();

	int i, j;

	// for(i=0; i<datasize; i++)
	// 	for(j=31; j>=0; j--)
	// 		bv_push(bitdata, (data[i]>>j) & 1);
	{
		int bitcount = datasize*32 - 64 + data[datasize-1];

		for(i=0; i<bitcount; i++)
			bv_push(bitdata, (data[i/32]>>(31-i%32)) & 1);
	}

	int currentbit;

	// Primeiro devemos ler o número de bits per word para poder
	// ler a árvore de huffman corretamente
	int bitsperword = 0;

	for(currentbit=0; currentbit<6; currentbit++){
		bitsperword = (bitsperword << 1) + bv_get(bitdata, currentbit);
	}

	// Logo a seguir lemos a árvore de huffman
	hfnode * root = readTree(bitdata, &currentbit, bitsperword);

	// retpos se refere a ultima posição não preenchida no vetor de
	// retorno. retsize se refere ao tamanho atual do vetor de retorno.
	// O vetor de retorno tem crescimento amortizado.
	int retpos = 0;
	int retsize = 1;

	// Nó atual no qual estamos
	hfnode * currentNode = root;

	for(; currentbit<bitdata->bitamount; currentbit++){
		int bit = bv_get(bitdata, currentbit);

		currentNode = currentNode->children[bit];

		if(currentNode->isLeaf){
			if(retpos==retsize){
				// Caso não haja mais espaço no vetor, então
				// dobramos sua capacidade

				retsize *= 2;
				ret = realloc(ret, retsize*sizeof(int));
			}

			// Adicionando o novo elemento no vetor de retorno
			ret[retpos] = currentNode->data;
			retpos++;

			// Ao chegar a um nó folha, devemos retornar ao nó raiz
			currentNode = root;
		}
	}

	bv_free(bitdata);
	freeHuffmanTree(root);

	(*retsizeparam) = retpos;

	return ret;
}

int * writeHuffmanData(int * data, int datasize, int * retsize){
	int i, j;

	// Máximo numero de bits necessários para representar
	// qualquer dado
	int maxbits = 0;

	// Encontrando o bit mais significativo não-zero entre todos os códigos dos dados
	for(i=0; i<datasize; i++){
		int bitreq = getBitAmount(data[i]);

		if(bitreq > maxbits)
			maxbits = bitreq;
	}

	// Vector de bits para guardar o output
	bitvector * output = bv_new();

	// No passo de compressão de huffman, a primeira coisa a ser
	// guardada é mínimo número de bits necessários para gravar
	// todos os códigos
	for(i=5; i>=0; i--)
		bv_push(output, (maxbits>>i)&1);

	// O segundo passo é construir e armazenar a árvore de huffman
	hftree * tree = buildTree(data, datasize);
	writeTree(output, tree->root, maxbits);

	// O terceiro passo é construir os prefixos de cada código
	int nodespacelength;
	hfnode ** nodespace = buildHuffmanMap(tree->root, &nodespacelength, NULL);

	// E então colocamos os códigos no bitvector de retorno
	for(i=0; i<datasize; i++){
		int codepos = findHuffmanCode(nodespace, 0, nodespacelength-1, data[i]);

		for(j=0; j<nodespace[codepos]->prefix->bitamount; j++)
			bv_push(output, bv_get(nodespace[codepos]->prefix, j) );
	}

	{
		int intamount = (output->bitamount/32 + 2);
		int * ret = (int *)malloc(intamount*sizeof(int));

		for(i=0; i<output->bitamount; i++){
			if(i%32==0)
				ret[i/32] = 0;

			ret[i/32] = ret[i/32] + (bv_get(output, i)<<(31-i%32));
		}

		ret[intamount-1] = output->bitamount % 32;

		bv_free(output);
		freeHuffmanTree(tree->root);

		free(tree);
		free(nodespace);

		(*retsize) = intamount;
		return ret;
	}
}

void sortNodes(hfnode ** array, int arraysize){
	int i;

	int correct=0;

	while(!correct){
		correct = 1;
		for(i=0; i<arraysize-1; i++){
			if(array[i]->frequency > array[i+1]->frequency){
				hfnode * tmp = array[i];
				array[i] = array[i+1];
				array[i+1] = tmp;

				correct=0;
			}
		}
	}

	return;
}

int findHuffmanCode(hfnode ** vector, int min, int max, int value){
	if(min>=max)
		if(vector[min]->data == value)
			return min;
		else{
			printf("ERROR: unable to find correct code (findHuffmanCode)\n");
			exit(EXIT_FAILURE);
		}

	int middle = (min+max)/2;

	if( value < vector[middle]->data ){
		return findHuffmanCode(vector, min, middle-1, value);
	}else if( value > vector[middle]->data ){
		return findHuffmanCode(vector, middle+1, max, value);
	}else{
		return middle;
	}
}

hfnode ** buildHuffmanMap(hfnode * node, int * returnsize, bitvector * prefix){
	if(node->isLeaf){
		if(prefix == NULL){
			printf("ERROR: prefix is null at leaf level (buildHuffmanMap)\n");
			exit(EXIT_FAILURE);
		}

		hfnode ** retmap = (hfnode **)malloc(1*sizeof(hfnode *));
		retmap[0] = node;

		if(node->prefix == NULL){
			node->prefix = bv_copy(prefix);
		}else{
			printf("WARNING: leaf nodes are supposed to have null prefix at init (buildHuffmanMap)\n");
		}

		*returnsize = 1;
		return retmap;
	}else{
		int isRoot = 0;
		if(prefix == NULL){
			prefix = bv_new();
			isRoot = 1;
		}

		bitvector * prefixA = bv_copy(prefix);
		bv_push(prefixA, 0);

		int mapAsize;
		hfnode ** mapA = buildHuffmanMap(node->children[0], &mapAsize, prefixA);

		bitvector * prefixB = bv_copy(prefix);
		bv_push(prefixB, 1);

		int mapBsize;
		hfnode ** mapB = buildHuffmanMap(node->children[1], &mapBsize, prefixB);

		int i=0, j=0;

		int mapCsize=0;
		hfnode ** returnMap = (hfnode **)malloc( (mapAsize+mapBsize)*sizeof(hfnode *) );

		while(i<mapAsize || j<mapBsize){
			if(i<mapAsize && j<mapBsize){
				if(mapA[i]->data < mapB[j]->data){
					returnMap[mapCsize++] = mapA[i++];
				}else{
					returnMap[mapCsize++] = mapB[j++];
				}
			}else{
				if(i<mapAsize)
					returnMap[mapCsize++] = mapA[i++];
				else
					returnMap[mapCsize++] = mapB[j++];
			}
		}

		free(mapA);
		free(mapB);

		if(isRoot)
			bv_free(prefix);

		bv_free(prefixA);
		bv_free(prefixB);

		*returnsize = mapCsize;
		return returnMap;
	}
}

void freeHuffmanTree(hfnode * node){
	if(node->isLeaf){
		if(node->children != NULL){
			printf("ERROR: Leaf node's children is not null pointer (freeHuffmanTree)\n");
			exit(EXIT_FAILURE);
		}

		if(node->prefix != NULL)
			bv_free(node->prefix);
		free(node);

		return;
	}else{
		if(node->children == NULL){
			printf("ERROR: Non leaf node's children is null pointer (freeHuffmanTree)\n");
			exit(EXIT_FAILURE);
		}

		if(node->prefix != NULL)
			bv_free(node->prefix);

		freeHuffmanTree(node->children[0]);
		freeHuffmanTree(node->children[1]);

		free(node->children);
		free(node);

		return;
	}
}