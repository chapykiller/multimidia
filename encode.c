#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Poucos argumentos");
        return -1;
    }

    // Flags que indicam quais codificações serão usadas
    int bDifference, bRunlength, bHuffman;
    bDifference = bRunlength = bHuffman = 0;

    char filenames[2][50];
    filenames[0] = filenames[1] = '\0';

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
}
