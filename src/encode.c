#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "wave_reader.h"

int saveFile(char *filename, int difference, int runlength, int huffman, wav_hdr* header, int8_t *data, int data_size);

int obtainSamples(wav_hdr* header, int8_t *data, int32_t *samples);

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("\nPoucos argumentos\n");
        return -1;
    }

    if(argc > 6) {
        printf("\nMuitos argumentos\n");
        return -1;
    }

    // Flags que indicam quais codificações serão usadas
    int bDifference, bRunlength, bHuffman;
    bDifference = bRunlength = bHuffman = 0;

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

    // Lê o arquivo wave que será codificado
    int8_t *wav_data;
    wav_hdr* header;

    header = readWave(filenames[0], wav_data);
    if(header == 0) {
        return -1;
    }

    int32_t *samples;
    int samples_size;

    samples_size = obtainSamples(header, wav_data, samples);

    // Salva o arquivo codificado
    if(saveFile(filenames[1], bDifference, bRunlength, bHuffman, header, wav_data, (int)(header->Subchunk2Size) * (int)(header->NumOfChan) ) != 0) {
        printf("\nNão foi possível salvar o arquivo codificado\n");

        free(header);
        free(wav_data);

        return -1;
    }

    free(header);
    free(wav_data);

    return 0;
}

int saveFile(char *filename, int difference, int runlength, int huffman, wav_hdr* header, int8_t *data, int data_size) {
    FILE *f;

    f = fopen(filename, "w");
	if(f == NULL) {
		printf("\nNão foi possivel abrir o arquivo %s\n", filename);
		return -1;
	}

    int8_t codHeader = 0;
    codHeader = (int8_t)(( (difference<<2) + (runlength<<1) + (huffman<<0) ) << 5);

    fwrite((void*)&codHeader, sizeof(int8_t), 1, f);

    fwrite((void*)header, sizeof(wav_hdr), 1, f);

    fwrite((void*)data, data_size*sizeof(int8_t), 1, f);

    fclose(f);
}

int obtainSamples(wav_hdr *header, int8_t *data, int32_t *samples) {
    int samples_size = (header->Subchunk2Size * 8)/header->bitsPerSample;

    samples = (int32_t*)malloc(samples_size * sizeof(int32_t));

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
}
