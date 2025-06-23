#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void toLowerStr(char str[], int size) {
    for(int i = 0; i < size; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

int main() {
    FILE *lista = fopen("resources/lista-palavras.txt", "r");
    FILE *novaLista = fopen("resources/lista-minusculas.txt", "w");
    char palavra[100];
    while(fscanf(lista, "%s", palavra) != EOF) {
        int size = strlen(palavra);
        toLowerStr(palavra, size);
        fprintf(novaLista, "%s\n", palavra);
    }
    fclose(lista);
    fclose(novaLista);
    return 0;
}