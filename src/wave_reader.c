#include <stdio.h>
#include <stdlib.h>

#include "wave_reader.h"

// find the file size
int getFileSize(FILE *inFile){
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}


wav_hdr* readWave(char *filename, int8_t **data, int8_t **footer, int *footer_size) {
	FILE *f;
	int headerSize = sizeof(wav_hdr);
	int fileLength = 0;
	wav_hdr* header = (wav_hdr*)malloc(sizeof(wav_hdr));

	f = fopen(filename, "r");
	if(f == NULL) {
		printf("\nNão foi possivel abrir o arquivo wave %s\n", filename);
        free(header);
		return 0;
	}

	fread(header, headerSize, 1, f);

    *data = (int8_t*)malloc(( (int)(header->Subchunk2Size) ) * sizeof(int8_t));

    fread(*data, (int)(header->Subchunk2Size)*sizeof(int8_t), 1, f);

	fileLength = getFileSize(f);

    *footer_size = fileLength - (int)(header->Subchunk2Size) - (int)(sizeof(wav_hdr));

    *footer = (int8_t*)malloc( (*footer_size) * sizeof(int8_t));

    fread(*footer, ( fileLength - (int)(header->Subchunk2Size) - (int)(sizeof(wav_hdr)) ) * sizeof(int8_t), 1, f);

	fclose(f);

	return header;
}

int saveWave(char *filename, wav_hdr* header, int8_t *data, int8_t *footer, int footer_size) {
    FILE *f;

	f = fopen(filename, "w");
	if(f == NULL) {
		printf("\nNão foi possivel abrir o arquivo wave %s\n", filename);
		return -1;
	}

	fwrite(header, sizeof(wav_hdr), 1, f);

    fwrite(data, (int)(header->Subchunk2Size)*sizeof(int8_t), 1, f);

    fwrite(footer, footer_size*sizeof(int8_t), 1, f);

	fclose(f);

	return 0;
}
