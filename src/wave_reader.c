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


wav_hdr* readWave(char *filename) {
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
	fileLength = getFileSize(f);
	fclose(f);

	return header;
}
