#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "difference.h"
#include "auxiliary.h"

// Codifica usando diferenças
int32_t* differenceEncode(int32_t *orig, int orig_size, int *dest_size) {
    // Menor valor (usado para tirar os valores negativos das diferenças)
    int32_t minimum;
    // Número de bits necessário para representar o maior valor
    int numBits;
    int32_t *dest;

    int32_t *aux = (int32_t*)malloc( orig_size * sizeof(int32_t) );

    if(aux == 0) {
        return 0;
    }

    // Faz as diferenças
    int i;
    for(i = 0; i < orig_size; i++) {
        aux[i] = (i == 0) ? orig[i] : orig[i] - orig[i-1];
    }

    // Encontra o menor valor
    minimum = aux[1];
    for(i = 2; i < orig_size; i++) {
        minimum = (aux[i] < minimum) ? aux[i] : minimum;
    }

    // Deixa todas as diferenças positivas
    int max = abs(minimum) << 1;
    for(i = 1; i < orig_size; i++) {
        aux[i] -= minimum;
        max = (aux[i] > max) ? aux[i] : max;
    }

    numBits = getBitAmount(max);
    int numBlocks = ceil((double)32.0/(double)numBits);

    *dest_size = orig_size - 1 + numBlocks + 2;
    dest = (int32_t*)malloc(*dest_size * sizeof(int32_t));

    if(dest == 0) {
        free(aux);
        return 0;
    }

    // Salva os dados necessários para decodificação
    dest[0] = numBits;
    dest[1] = abs(minimum) << 1;
    dest[1] = (minimum < 0) ? dest[1] + 1 : dest[1];

    // Divide a primeira amostra em numBlocks blocos de numBits bits
    int blockMask = ~(-1 << numBits);

    for(i = 0; i < numBlocks; i++) {
        int value = aux[0] >> ( (numBlocks - i -1) * numBits );
        dest[2 + i] = value & blockMask;
    }

    int count = numBlocks + 2;
    for(i = 1; i < orig_size; i++) {
        dest[count] = aux[i];
        count += 1;
    }

    free(aux);

    return dest;
}

// Decodifica por diferenças
int32_t* differenceDecode(int32_t *orig, int orig_size, int *dest_size) {
    int numBits, minimum;
    int32_t *dest;

    numBits = orig[0];
    minimum = orig[1] >> 1;
    if(orig[1] & 1 == 1) {
        minimum *= -1;
    }

    int numBlocks = ceil((double)32.0/(double)numBits);

    *dest_size = orig_size - numBlocks + 1 - 2;
    dest = (int32_t*)malloc(*dest_size * sizeof(int32_t));

    if(dest == 0) {
        return 0;
    }

    dest[0] = 0;

    int i;
    for(i = 0; i < numBlocks; i++) {
        dest[0] += orig[2 + i] << ( (numBlocks - i -1) * numBits );
    }

    int count = 1;
    for(i = numBlocks + 2; i < orig_size; i++) {
        dest[count] = orig[i] += minimum;
        dest[count] += dest[count - 1];

        count += 1;
    }

    return dest;
}
