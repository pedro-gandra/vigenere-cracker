#include <stdio.h>
#include <stdlib.h>

#define nBigramas 676

void pasteBigramas(FILE *arq, char bi[nBigramas][2]) {
    for (int i = 0; i < nBigramas; i++) {
        if(bi[i][0] != '*')
            fprintf(arq, "%c%c\n", bi[i][0], bi[i][1]);
    }
}

void copyBigramas(FILE* arq, char bi[nBigramas][2], int freq[nBigramas]) {
    char c, previousChar;
    int curSize = 0, total = 0;
    while(fscanf(arq, "%c", &c) != EOF) {
        curSize++;
        if(c=='\n') {
            curSize = 0;
        } else if(curSize == 2) {
            bi[total][0] = previousChar;
            bi[total][1] = c;
            freq[total] = 0;
            total++;
        }
        previousChar = c;
    }
} 

void deletarBigrama(char bi[2], char allBigramas[nBigramas][2], int freq[nBigramas]) {
    for(int i = 0; i < nBigramas; i++) {
        if(allBigramas[i][0] != '*') {
            if(bi[0] == allBigramas[i][0] && bi[1] == allBigramas[i][1]) {
                freq[i]++;
                if(freq[i]>=100) {
                    allBigramas[i][0] = '*';
                    allBigramas[i][1] = '*';
                }
            }
        }
    }
}

int main() {
    FILE *palavras = fopen("resources/lista-minusculas.txt", "r");
    FILE *bigramas = fopen("resources/all-bigramas.txt", "r");
    FILE *bigramasFiltrados = fopen("resources/bigramas-raros.txt", "w");
    char allBigramas[nBigramas][2];
    int freqBigrama[nBigramas];
    copyBigramas(bigramas, allBigramas, freqBigrama);
    char c, previousChar;
    int curSize = 0;
    while(fscanf(palavras, "%c", &c) != EOF) {
        curSize++;
        if(c == '\n') {
            curSize = 0;
        } else if(curSize > 1) {
            char bi[2] = {previousChar, c};
            deletarBigrama(bi, allBigramas, freqBigrama);
        }
        previousChar = c;
    }
    pasteBigramas(bigramasFiltrados, allBigramas);
    fclose(palavras);
    fclose(bigramas);
    fclose(bigramasFiltrados);
    return 0;
}