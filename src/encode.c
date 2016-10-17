#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "wave_reader.h"
#include "difference.h"
#include "runlength.h"
#include "huffman.h"
#include "auxiliary.h"

int saveFile(char *filename, int difference, int runlength, int huffman, int blockSize, wav_hdr* header, int32_t *data, int data_size);

int32_t* obtainSamples(wav_hdr *header, int8_t *data, int *samples_size);

int main(int argc, char *argv[]) {

    // Precisa ter pelo menos 3 argumentos junto com o nome do programa
    if(argc < 4) {
        printf("\nPoucos argumentos\n");
        return -1;
    }

    // Se houver mais do que 5 argumentos além do nome do programa
    if(argc > 6) {
        printf("\nMuitos argumentos\n");
        return -1;
    }

    // Flags que indicam quais codificações serão usadas
    int bDifference, bRunlength, bHuffman;
    bDifference = bRunlength = bHuffman = 0;

    // Filenames[0] é o nome do arquivo de origem e filenames[1] é o nome do arquivo de destino
    char filenames[2][50];
    filenames[0][0] = filenames[1][0] = '\0';

    int i;
    for(i = 1; i < argc; i++) {
        // Verifica quais codificações serão usadas
        if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-D") == 0) {
            bDifference = 1;
        }
        else if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "-C") == 0) {
            bRunlength = 1;
        }
        else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-H") == 0) {
            bHuffman = 1;
        }
        // Salva os nomes de origem e destino
        else {
            int j;
            for(j = 0; j < 2; j++) {
                if(filenames[j][0] == '\0') {
                    strcpy(filenames[j], argv[i]);
                }
            }
        }
    }

    if(bHuffman == 0 && bDifference == 0 && bRunlength == 0) {
        printf("\nPelo menos uma das opções de codificação deve ser passada.\n");
        return -1;
    }

    // Lê o arquivo wave que será codificado
    int8_t *wav_data;
    wav_hdr* header;

    header = readWave(filenames[0], &wav_data);
    if(header == 0) {
        return -1;
    }

    int32_t *samples;
    int samples_size;

    // Separa os bytes lidos em amostras de no máximo 32 bits
    samples = obtainSamples(header, wav_data, &samples_size);

    if(samples_size < 0) {
        free(header);
        free(wav_data);
        return -1;
    }

    if(bDifference == 1) {
        int newSize;
        int32_t *newSamples = differenceEncode(samples, samples_size, &newSize);

        free(samples);

        samples = newSamples;
        samples_size = newSize;
    }

    if(bRunlength == 1) {
        int newSize;
        int32_t *newSamples = writeRunlength(samples, samples_size, &newSize);

        free(samples);

        samples = newSamples;
        samples_size = newSize;
    }

    if(bHuffman == 1) {
        int newSize;
        int32_t *newSamples = writeHuffmanData(samples, samples_size, &newSize);

        free(samples);

        samples = newSamples;
        samples_size = newSize;
    }

    // TODO shortenBytes();

    // Salva o arquivo codificado
    if(saveFile(filenames[1], bDifference, bRunlength, bHuffman, /* TODO */0, header, samples, (int)(header->Subchunk2Size) * (int)(header->NumOfChan) ) != 0) {
        printf("\nNão foi possível salvar o arquivo codificado\n");

        free(header);
        free(wav_data);
        free(samples);

        return -1;
    }

    free(header);
    free(wav_data);
    free(samples);

    return 0;
}

// Salva o arquivo codificado
int saveFile(char *filename, int difference, int runlength, int huffman, int blockSize, wav_hdr* header, int32_t *data, int data_size) {
    FILE *f;

    f = fopen(filename, "w");
	if(f == NULL) {
		printf("\nNão foi possivel abrir o arquivo %s\n", filename);
		return -1;
	}

    // Gerando o cabeçalho
    int8_t codHeader = 0;
    codHeader = (int8_t)(( (difference<<2) + (runlength<<1) + (huffman<<0) ) << 5);
    codHeader += (int8_t)blockSize;

    // Salva o nosso cabeçalho
    fwrite((void*)&codHeader, sizeof(int8_t), 1, f);

    // Salva o cabeçalho do wave
    fwrite((void*)header, sizeof(wav_hdr), 1, f);

    // Salva os dados codificados
    fwrite((void*)data, data_size*sizeof(int32_t), 1, f);

    fclose(f);

    return 0;
}

// Junta os bytes em um único int32_t com base na quantidade de bits por amostra
int32_t* obtainSamples(wav_hdr *header, int8_t *data, int *samples_size) {
    *samples_size = (header->Subchunk2Size * 8)/header->bitsPerSample;

    int32_t *samples = (int32_t*)malloc(*samples_size * sizeof(int32_t));

    if(samples == 0) {
        return 0;
    }

    int i;
    int j;
    int count = 0;
    for(i = 0; i < header->Subchunk2Size; i += (header->bitsPerSample/8) ) {
        samples[count] = 0;

        for(j = 0; j < header->bitsPerSample/8; j++) {
            samples[count] += (data[i+j] << ( (header->bitsPerSample/8) - j - 1) * 8 );
        }

        count += 1;
    }

    return samples;
}
