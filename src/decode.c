#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "wave_reader.h"
#include "difference.h"
#include "runlength.h"
#include "huffman.h"

wav_hdr* readFile(char *filename, int *difference, int *runlength, int *huffman, int8_t **data, int *data_size);
int saveWave(char *filename, wav_hdr* header, int8_t *data);

int8_t* obtainBytes(wav_hdr *header, int32_t *samples);

int main(int argc, char *argv[]) {
    // Precisa ter 2 argumentos junto com o nome do programa
    if(argc < 3) {
        printf("\nPoucos argumentos\n");
        return -1;
    }

    // Se houver mais do que 2 argumentos além do nome do programa
    if(argc > 3) {
        printf("\nMuitos argumentos\n");
        return -1;
    }

    // Flags que indicam quais codificações foram usadas
    int bDifference, bRunlength, bHuffman;
    bDifference = bRunlength = bHuffman = 0;

    // Filenames[0] é o nome do arquivo de origem e filenames[1] é o nome do arquivo de destino
    char filenames[2][50];
    filenames[0][0] = filenames[1][0] = '\0';

    int i;
    for(i = 1; i < argc; i++) {
        // Salva os nomes de origem e destino
        int j;
        for(j = 0; j < 2; j++) {
            if(filenames[j][0] == '\0') {
                strcpy(filenames[j], argv[i]);
            }
        }
    }

    int8_t *shortenedSamples;
    int samples_size;
    wav_hdr* header;

    header = readFile(filenames[0], &bDifference, &bRunlength, &bHuffman, &shortenedSamples, &samples_size);
    if(header == 0) {
        return -1;
    }

    int32_t *samples;
    int newSize;

    samples = unshortenBytes(shortenedSamples, samples_size, &newSize);
    if(samples == 0) {
        free(shortenedSamples);
        free(header);

        return -1;
    }

    free(shortenedSamples);
    samples_size = newSize;

    if(bHuffman == 1) {
        int newSize;
        int32_t *newSamples = readHuffmanData(samples, samples_size, &newSize);

        free(samples);

        samples = newSamples;
        samples_size = newSize;
    }

    if(bRunlength == 1) {
        int newSize;
        int32_t *newSamples = readRunlength(samples, samples_size, &newSize);

        free(samples);

        samples = newSamples;
        samples_size = newSize;
    }

    if(bDifference == 1) {
        int newSize;
        int32_t *newSamples = differenceDecode(samples, samples_size, &newSize);

        free(samples);

        samples = newSamples;
        samples_size = newSize;
    }

    // Separa as amostras em bytes
    int8_t *wav_data = obtainBytes(header, samples);

    if(wav_data == 0) {
        free(samples);
        free(header);

        return -1;
    }

    // Salva o arquivo decodificado
    if(saveWave(filenames[1], header, wav_data) != 0) {
        printf("\nNão foi possível salvar o arquivo decodificado\n");

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
wav_hdr* readFile(char *filename, int *difference, int *runlength, int *huffman, int8_t **data, int *data_size) {
    FILE *f;

    f = fopen(filename, "r");
	if(f == NULL) {
		printf("\nNão foi possivel abrir o arquivo %s\n", filename);
		return 0;
	}

    wav_hdr *header = (wav_hdr*)malloc(sizeof(wav_hdr));
    if(header == 0) {
        fclose(f);
        return 0;
    }

    // Cabeçalho
    int8_t codHeader = 0;

    // Lê o nosso cabeçalho
    fread((void*)&codHeader, sizeof(int8_t), 1, f);
    fread((void*)data_size, sizeof(int), 1, f);

    (*data) = (int8_t*)malloc(*data_size * sizeof(int8_t));
    if(*data == 0) {
        free(header);
        fclose(f);
        return 0;
    }

    // Lê o cabeçalho do wave
    fread((void*)header, sizeof(wav_hdr), 1, f);

    // Salva os dados codificados
    fread((void*)data, *data_size*sizeof(int8_t), 1, f);

    *difference = (int)(codHeader & 0b10000000);
    *runlength = (int)(codHeader & 0b01000000);
    *huffman = (int)(codHeader & 0b00100000);

    fclose(f);

    return header;
}

int saveWave(char *filename, wav_hdr* header, int8_t *data) {
    FILE *f;

	f = fopen(filename, "w");
	if(f == NULL) {
		printf("\nNão foi possivel abrir o arquivo wave %s\n", filename);
		return -1;
	}

	fwrite(header, sizeof(wav_hdr), 1, f);

    fwrite(data, (int)(header->Subchunk2Size)*sizeof(int8_t), (int)(header->NumOfChan), f);

	fclose(f);

	return 0;
}

// Separa as amostras em bytes
int8_t* obtainBytes(wav_hdr *header, int32_t *samples) {
    int samples_size = (header->Subchunk2Size * 8)/header->bitsPerSample;

    int8_t *data = (int8_t*)malloc(header->Subchunk2Size * sizeof(int8_t));

    if(data == 0) {
        return 0;
    }

    int i;
    int j;
    int count = 0;
    for(i = 0; i < samples_size; i++ ) {
        for(j = 0; j < header->bitsPerSample/8; j++) {
            data[count] = (int8_t)(samples[i] >> ( (header->bitsPerSample/8) - j - 1) * 8 );
            count ++;
        }
    }

    return data;
}
