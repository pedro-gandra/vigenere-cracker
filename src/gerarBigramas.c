#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main() {
    FILE *bigramas = fopen("resources/all-bigramas.txt", "w");
    for (int i = 'a'; i <= 'z'; i++) {
        for (int j = 'a'; j <= 'z'; j++) {
            fprintf(bigramas, "%c%c\n", i, j);
        }
    }
    fclose(bigramas);
    return 0;
}