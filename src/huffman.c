#include "huffman.h"

hftree * readTree(){
	hftree localTree;

	
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
		datalength = bufferlength;

		hfnode * newNode = (hfnode *)malloc(1*sizeof(hfnode));
		newNode->isLeaf = 1;
		newNode->data = currentcode;
		newNode->frequency = codefrequency;
		newNode->children = NULL;

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

		hfnodes[0] = newNode;
		hfnodes[1] = hfnodes[nodeamount-1];

		nodeamount--;

		sortNodes(hfnodes, nodeamount);
	}

	localTree->root = hfnodes[0];

	return localTree;
}

int * readHuffmanData(hftree * tree, int * data, int datasize){
	// Vetor de retorno
	int * ret=malloc(1*sizeof(int));

	// wordi se refere a word (int) do input sobre a qual estamos
	// iterando atualmente. biti se refere ao bit da palavra. 
	int wordi, biti;

	// retpos se refere a ultima posição não preenchida no vetor de
	// retorno. retsize se refere ao tamanho atual do vetor de retorno.
	// O vetor de retorno tem crescimento amortizado.
	int retpos = 0;
	int retsize = 1;

	// Nó atual no qual estamos
	hfnode * currentNode = tree->root;

	for(wordi=0; wordi<datasize; wordi++){
		for(biti=31; biti>=0; biti--){
			int bit = (data[wordi] >> biti) & 1;

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
				currentNode = tree->root;
			}
		}
	}

	return ret;
}

int * writeHuffmanData(hftree * tree){
	// Vetor de retorno
	int * ret = (int *)malloc(1*sizeof(int));

	// wordo representa a word atual na qual estamos
	// armazenando bits, enquanto bito representa o bit
	// atual
	int wordo=0, bito=31;


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
			free(prefix);

		free(prefixA);
		free(prefixB);

		*returnsize = mapCsize;
		return returnMap;
	}
}