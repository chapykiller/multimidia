#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("\nPoucos argumentos\n");
        return -1;
    }

    if(argc > 3) {
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
        int j;
        for(j = 0; j < 2; j++) {
            if(filenames[j][0] == '\0') {
                strcpy(filenames[j], argv[i]);
            }
        }
    }

    return 0;
}
