#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

#define MAX_LEN 30
#define MAX_KEY 10
#define ALF_LEN 26
#define MAX_TXT 10000
#define TOTAL_BI 393
#define TOTAL_WORDS 245366
#define MAX_CESAR 7
#define MIN_COBERTURA 0.80

char **allPalavras;
int qtdePalavras[26];
char bigramasRaros[TOTAL_BI][2];
float freqAlvo;
int cesarAlvo = 1;
float bigramasAlvo;

double frequencias[ALF_LEN] = {0};

typedef struct {
    char l;
    double valor;   
} probLetra;

void copyWordsAndFrequency(FILE* palavras, char** w) {
    int i = 0, l = 0;
    char letra = 'a';
    int total = 0;
    while(fscanf(palavras, "%s", w[i]) == 1) {
        if(w[i][0] != letra) {
            letra = w[i][0];
            l++;
        }
        qtdePalavras[l]++;
        for(int j = 0; w[i][j] != '\0'; j++) {
            frequencias[w[i][j]-'a']++;
            total++;
        }
        i++;
    }
    for(int i = 0; i < ALF_LEN; i++) {
        frequencias[i]/=total;
    }
}

void copyBigramas(char bi[TOTAL_BI][2]) {
    FILE *bigramasFile = fopen("resources/bigramas-raros.txt", "r");
    char c, previousChar;
    int curSize = 0, total = 0;
    while(fscanf(bigramasFile, "%c", &c) != EOF) {
        curSize++;
        if(c=='\n') {
            curSize = 0;
        } else if(curSize == 2) {
            bi[total][0] = previousChar;
            bi[total][1] = c;
            total++;
        }
        previousChar = c;
    }
    fclose(bigramasFile);
}

bool palavraExiste(const char* str, int len) {
    char temp[MAX_LEN];
    strncpy(temp, str, len);
    temp[len] = '\0';
    int firstLetter = temp[0]-97;
    int totalSkip = 0;
    for(int i = 0; i < firstLetter; i++) {
        totalSkip+=qtdePalavras[i];
    }
    for (int i = totalSkip; i < TOTAL_WORDS; i++) {
        int cmp = strcmp(temp, allPalavras[i]);
        if (cmp == 0) {
            return true;
        } else if(cmp < 0) {
            return false;
        }
    }
    return false;
}

float coberturaPalavras(char* texto) {
    int tam = strlen(texto);
    int i = 0;
    int acertos = 0;
    while(i < tam) {
        bool achou = false;
        for (int len = MAX_LEN; len >= 2; len--) {
            if (i + len > tam) continue;
            if (palavraExiste(&texto[i], len)) {
                acertos += len;
                i += len;
                achou = true;
                break;
            }
        }
        if (!achou) {
            i++;
        }
    }
    return (float) acertos/tam;
}

bool bigramaInList(char bi[2]) {
    for(int i = 0; i < TOTAL_BI; i++) {
        if((bi[0] == bigramasRaros[i][0] && bi[1] < bigramasRaros[i][1]) || bi[0] < bigramasRaros[i][0])
            return false;
        if(bi[0] == bigramasRaros[i][0] && bi[1] == bigramasRaros[i][1]) {
            return true;
        }
    }
    return false;
}

bool validarBigramas(char str[]) {
    int cont = 0;
    int size = strlen(str);
    for (int i = 1; i < size; i++) {
        char bi[] = {str[i-1], str[i]};
        if(bigramaInList(bi)) {
            cont++;
            if(cont >= (size-1)*bigramasAlvo)
                return false;
        }
    }
    return true;
}

void limparTexto(FILE* textoFile, char txt[]) {
    char c;
    int i = 0;
    while(fscanf(textoFile, "%c", &c) != EOF) {
        if(isalpha(c)) {
            txt[i] = tolower(c);
            i++;
        }
    }
    txt[i] = '\0';
}

double calcularSimilaridade(char texto[]) {
    double freq[ALF_LEN] = {0};
    int total = 0;

    for (int i = 0; texto[i] != '\0'; i++) {
        char c = texto[i];
        freq[c - 'a']++;
        total++;
    }

    for (int i = 0; i < ALF_LEN; i++) {
        freq[i] /= total;
    }

    double numerador = 0.0, denomTexto = 0.0, denomRef = 0.0;
    for (int i = 0; i < ALF_LEN; i++) {
        numerador += freq[i] * frequencias[i];
        denomTexto += freq[i] * freq[i];
        denomRef += frequencias[i] * frequencias[i];
    }

    if (denomTexto == 0 || denomRef == 0) return 0.0;

    return numerador / (sqrt(denomTexto) * sqrt(denomRef));
}

char deslocar(char c, char k) {
    int d = k-'a';
    c -= 'a';
    c = c-d;
    if(c<0)
        c = ALF_LEN+c;
    return c+'a';
}

void decode(char txt[], char key[]) {
    int pos = 0;
    int keySize = strlen(key);
    for(int i = 0; txt[i] != '\0'; i++) {
        txt[i] = deslocar(txt[i], key[pos]);
        pos++;
        if(pos == keySize)
            pos = 0;
    }
}


void inserir(probLetra prov[], double sim, int l) {
    probLetra novo;
    novo.l = l + 'a';
    novo.valor = sim;

    for (int i = 0; i < cesarAlvo; i++) {
        if (prov[i].valor == 0 || sim > prov[i].valor) {
            for (int j = cesarAlvo - 1; j > i; j--) {
                prov[j] = prov[j - 1];
            }
            prov[i] = novo;
            break;
        }
    }
}

bool gerarCombinacoes(int pos, int sizeKey, char chave[], char letras[][cesarAlvo], char new[], char txt[]) {
    if (pos == sizeKey) {
        chave[pos] = '\0';
        decode(new, chave);
        if(calcularSimilaridade(new) > freqAlvo) {
            if(validarBigramas(new) && coberturaPalavras(new) >= MIN_COBERTURA) {
                printf("%s - %s\n", new, chave);
                return true;
            }
        }
        strcpy(new, txt);
        return false;
    }

    for (int i = 0; i < cesarAlvo; i++) {
        chave[pos] = letras[pos][i];
        if (gerarCombinacoes(pos + 1, sizeKey, chave, letras, new, txt)) {
            return true;
        }
    }

    return false;
}

void letrasProvaveis(char txt[], char letras[][cesarAlvo], int sizeKey) {
    for(int i = 0; i < sizeKey; i++) {
        char coluna[MAX_TXT];
        int cont = 0;
        int len = strlen(txt);
        for(int j = i; j < len; j+=sizeKey) {
            coluna[cont++] = txt[j];
        }
        coluna[cont] = '\0';
        char novo[cont+1];
        probLetra provaveis[MAX_CESAR] = {0};
        for(int j = 0; j < ALF_LEN; j++) {
            strcpy(novo, coluna);
            for(int k = 0; k < cont; k++) {
                novo[k] = deslocar(novo[k], j+'a');
            }
            double sim = calcularSimilaridade(novo);
            inserir(provaveis, sim, j);
        }
        for(int j = 0; j < cesarAlvo; j++) {
            letras[i][j] = provaveis[j].l;
        }
    }
}

void printProvaveis(char letras[][cesarAlvo], int sizeKey) {
    for(int i = 0; i < sizeKey; i++) {
        for(int j = 0; j < cesarAlvo; j++) {
            printf("%c, ", letras[i][j]);
        }
        printf("\n");
    }
}

bool descriptografar(char txt[]) {
    int sizeKey;
    char new[MAX_TXT];
    strcpy(new, txt);
    for(sizeKey = 1; sizeKey <= MAX_KEY; sizeKey++) {
        char letras[sizeKey][cesarAlvo];
        char key[sizeKey+1];
        letrasProvaveis(txt, letras, sizeKey);
        //printf("Chaves de tamanho %d:\n", sizeKey);
        //printProvaveis(letras, sizeKey);
        if(gerarCombinacoes(0, sizeKey, key, letras, new, txt))
            return true;
    }
    return false;
}

int main() {
    freqAlvo = 0.85;
    cesarAlvo = 1;
    bigramasAlvo = 0.10;
    FILE* listaMinusculas = fopen("resources/lista-minusculas.txt", "r");
    FILE* textoCifrado = fopen("resources/texto-cifrado.txt", "r");
    allPalavras = malloc(TOTAL_WORDS*sizeof(char*));
    for (int i = 0; i < TOTAL_WORDS; i++) {
        allPalavras[i] = malloc(MAX_LEN*sizeof(char));
    };
    copyWordsAndFrequency(listaMinusculas, allPalavras);
    copyBigramas(bigramasRaros);
    char txt[MAX_TXT];
    limparTexto(textoCifrado, txt);
    printf("\nDESCRIPTOGRAFIA INICIADA\n\n");
    printf("Texto referencia:\n");
    printf("%s\n", txt);
    while(cesarAlvo <= MAX_CESAR) {
        printf("\nTentando descriptografia com precisao de nivel %d:\n", cesarAlvo);
        if(descriptografar(txt))
            break;
        cesarAlvo++;
        printf("Nenhum texto possivel\n");
    }
    printf("\n");
    //printf("%.4f", calcularSimilaridade("hojeeuvoucomerarrozefeijaoedepoisapresentarabombadotrabalhodeti"));
    //printf("%.4f", coberturaPalavras("hojeeuvoucomerarrozefeijaoedepoisapresentarabombadotrabalhodeti"));
    fclose(listaMinusculas);
    fclose(textoCifrado);
    return 0;
}