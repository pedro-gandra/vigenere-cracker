#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define MAX_LEN 30
#define ALF_LEN 26
#define MAX_TXT 10000
#define TOTAL_BI 393
#define TOTAL_WORDS 245366
#define MAX_PRECISAO 5
#define MIN_COBERTURA 0.9
#define BI_ALVO 0.1

char allPalavras[TOTAL_WORDS][MAX_LEN];
int qtdePalavras[26];
char bigramasRaros[TOTAL_BI][2];
int precisaoAlvo = -1;
int maxKey = -1;
unsigned long long cTotal, cAtual = 0, cUltimo = 0;

double frequencias[ALF_LEN] = {0};
int cVogais = 0, cBigramas = 0, cCobertura = 0;
double tTotal = 0, tVogais = 0, tBigramas = 0, tCobertura = 0;

typedef struct {
    char l;
    double valor;   
} probLetra;

void formatarTempo(double segundos, char* buffer, size_t tam) {
    if (segundos < 60.0) {
        snprintf(buffer, tam, "%.1fs", segundos);
    } else if (segundos < 3600.0) {
        int min = (int)(segundos / 60);
        int sec = (int)(segundos) % 60;
        snprintf(buffer, tam, "%dm %ds", min, sec);
    } else if (segundos < 86400.0) {
        int h = (int)(segundos / 3600);
        int m = ((int)segundos % 3600) / 60;
        snprintf(buffer, tam, "%dh %dm", h, m);
    } else {
        int d = (int)(segundos / 86400);
        int h = ((int)segundos % 86400) / 3600;
        snprintf(buffer, tam, "%dd %dh", d, h);
    }
}

void atualizarProgresso() {
    static clock_t inicio = 0;
    static clock_t ultimaAtualizacao = 0;
    cUltimo = cAtual;

    if (inicio == 0) {
        inicio = clock();
        ultimaAtualizacao = inicio;
        return;
    }

    clock_t agora = clock();
    double decorrido = (double)(agora - inicio) / CLOCKS_PER_SEC;
    double desdeUltima = (double)(agora - ultimaAtualizacao) / CLOCKS_PER_SEC;

    if (desdeUltima < 1.0 || cAtual == 0) return;

    double progresso = (double)cAtual / (double)cTotal;
    if (progresso <= 0.0) return;

    double tempoEstimadoTotal = decorrido / progresso;
    double restante = tempoEstimadoTotal - decorrido;

    if (tempoEstimadoTotal < 5.0) return;

    char bufDecorrido[32];
    char bufRestante[32];

    formatarTempo(decorrido, bufDecorrido, sizeof(bufDecorrido));
    formatarTempo(restante, bufRestante, sizeof(bufRestante));

    fprintf(stderr, "\rProgresso: %5.1f%% | Decorrido: %s | Tempo maximo restante: %s",
            progresso * 100.0, bufDecorrido, bufRestante);
    fflush(stderr);

    ultimaAtualizacao = agora;
}

void copyWordsAndFrequency(FILE* palavras, char w[][MAX_LEN]) {
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

    int firstLetter = temp[0] - 'a';

    int start = 0;
    for (int i = 0; i < firstLetter; i++) {
        start += qtdePalavras[i];
    }
    int end = start + qtdePalavras[firstLetter] - 1;

    while (start <= end) {
        int mid = (start + end) / 2;
        int cmp = strcmp(temp, allPalavras[mid]);
        if (cmp == 0) {
            return true;
        } else if (cmp < 0) {
            end = mid - 1;
        } else {
            start = mid + 1;
        }
    }

    return false;
}

float coberturaPalavras(char* texto) {
    clock_t inicio, fim;
    inicio = clock();
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
    fim = clock();
    tCobertura += (double)(fim - inicio) / CLOCKS_PER_SEC;
    return (float) acertos/tam;
}

bool bigramaInList(char bi[2]) {
    int inicio = 0;
    int fim = TOTAL_BI - 1;
    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        char a = bigramasRaros[meio][0];
        char b = bigramasRaros[meio][1];

        if (bi[0] == a && bi[1] == b) return true;
        else if (bi[0] < a || (bi[0] == a && bi[1] < b)) fim = meio - 1;
        else inicio = meio + 1;
    }
    return false;
}

bool validarBigramas(char str[]) {
    clock_t inicio, fim;
    inicio = clock();
    int cont = 0;
    int size = strlen(str);
    for (int i = 1; i < size; i++) {
        char bi[] = {str[i-1], str[i]};
        if(bigramaInList(bi)) {
            cont++;
            if(cont >= (size-1)*BI_ALVO) {
                fim = clock();
                tBigramas += (double)(fim - inicio) / CLOCKS_PER_SEC;
                return false;
            }
        }
    }
    fim = clock();
    tBigramas += (double)(fim - inicio) / CLOCKS_PER_SEC;
    return true;
}

bool validarVogais(const char str[]) {
    clock_t inicio = clock();
    int vogais = 0;
    int size = 0;
    int janelaVogais = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        size++;
        bool atualEhVogal = (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
        if (atualEhVogal) vogais++;
        if (i < 7) {
            if (atualEhVogal) janelaVogais++;
        } else {
            char anterior = str[i - 7];
            if (anterior == 'a' || anterior == 'e' || anterior == 'i' || anterior == 'o' || anterior == 'u') {
                janelaVogais--;
            }
            if (atualEhVogal) janelaVogais++;
            if (janelaVogais < 2) {
                tVogais += (double)(clock() - inicio) / CLOCKS_PER_SEC;
                return false;
            }
        }
    }

    tVogais += (double)(clock() - inicio) / CLOCKS_PER_SEC;

    if (size == 0 || (float)vogais / size <= 0.35f) {
        return false;
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

void decode(char new[], char txt[], char key[]) {
    int pos = 0;
    int keySize = strlen(key);
    for(int i = 0; txt[i] != '\0'; i++) {
        new[i] = deslocar(txt[i], key[pos]);
        pos++;
        if(pos == keySize)
            pos = 0;
    }
}

void inserir(int precisao, probLetra prov[], double sim, int l) {
    probLetra novo;
    novo.l = l + 'a';
    novo.valor = sim;

    for (int i = 0; i < precisao; i++) {
        if (prov[i].valor == 0 || sim > prov[i].valor) {
            for (int j = precisao - 1; j > i; j--) {
                prov[j] = prov[j - 1];
            }
            prov[i] = novo;
            break;
        }
    }
}

bool gerarCombinacoes(int precisao, int sizeKey, char letras[][precisao], char chave[], char new[], char txt[]) {
    int indices[100] = {0}; // Posição atual em cada coluna de letras
    int pos = 0;
    while (pos >= 0) {
        if (indices[pos] < precisao) {
            chave[pos] = letras[pos][indices[pos]];
            indices[pos]++;
            if (pos == sizeKey - 1) {
                chave[sizeKey] = '\0';
                decode(new, txt, chave);
                if (validarVogais(new)) {
                    if (validarBigramas(new)) {
                        if (coberturaPalavras(new) >= MIN_COBERTURA) {
                            fprintf(stderr, "\r%*s\r", 80, "");
                            printf("**************************************************************\n\nTEXTO DECODIFICADO\n");
                            printf("Texto original: %s\nChave: %s\n", new, chave);
                            printf("\n**************************************************************\n\n");
                            return true;
                        } else cCobertura++;
                    } else cBigramas++;
                } else cVogais++;
                cAtual++;
                if (cAtual - cUltimo > 100000)
                    atualizarProgresso();
            } else {
                pos++;
                indices[pos] = 0;
            }
        } else {
            pos--;
        }
    }
    return false;
}

void letrasProvaveis(int precisao, char txt[], char letras[][precisao], int sizeKey) {
    for(int i = 0; i < sizeKey; i++) {
        char coluna[MAX_TXT];
        int cont = 0;
        int len = strlen(txt);
        for(int j = i; j < len; j+=sizeKey) {
            coluna[cont++] = txt[j];
        }
        coluna[cont] = '\0';
        char novo[cont+1];
        probLetra provaveis[MAX_PRECISAO] = {0};
        for(int j = 0; j < ALF_LEN; j++) {
            strcpy(novo, coluna);
            for(int k = 0; k < cont; k++) {
                novo[k] = deslocar(novo[k], j+'a');
            }
            double sim = calcularSimilaridade(novo);
            inserir(precisao, provaveis, sim, j);
        }
        for(int j = 0; j < precisao; j++) {
            letras[i][j] = provaveis[j].l;
        }
    }
}

bool descriptografar(char txt[]) {
    int sizeKey;
    char new[MAX_TXT];
    strcpy(new, txt);
    int prec = 1;
    while(prec <= precisaoAlvo) {
        for(sizeKey = 1; sizeKey <= maxKey; sizeKey++) {
            char letras[sizeKey][prec];
            char key[sizeKey+1];
            letrasProvaveis(prec, txt, letras, sizeKey);
            if(gerarCombinacoes(prec, sizeKey, letras, key, new, txt)) {
                printf("Precisao necessaria: %d\n\n", prec);
                return true;
            }
        }
        prec++;
    }
    return false;
}

int calcularPrecisao(int maxKey) {
    if(maxKey > 22)
        return 1;
    if(maxKey > 14)
        return 2;
    if(maxKey > 11)
        return 3;
    if(maxKey > 9)
        return 4;
    return 5;
}

int main() {
    FILE* listaMinusculas = fopen("resources/lista-minusculas.txt", "r");
    FILE* textoCifrado = fopen("resources/texto-cifrado.txt", "r");
    copyWordsAndFrequency(listaMinusculas, allPalavras);
    copyBigramas(bigramasRaros);
    char txt[MAX_TXT];
    limparTexto(textoCifrado, txt);

    while(maxKey <= 0) {
        printf("Qual o tamanho maximo da chave a ser testada: ");
        scanf("%d", &maxKey);
    }
    while(precisaoAlvo < 0 || precisaoAlvo > 5) {
        printf("Nivel maximo de precisao (1 a 5)? Escolha 0 caso queira que o programa escolha para voce (recomendado): ");
        scanf("%d", &precisaoAlvo);
    }
    if(precisaoAlvo == 0) {
        precisaoAlvo = calcularPrecisao(maxKey);
        printf("Precisao maxima escolhida: %d\n", precisaoAlvo);
    }
    cTotal = pow(precisaoAlvo, maxKey);

    clock_t inicio, fim;
    inicio = clock();
    printf("\nDESCRIPTOGRAFIA INICIADA\n\n");
    printf("Texto referencia:\n");
    printf("%s\n\n", txt);
    bool sucesso = false;
    if(!descriptografar(txt))
        printf("Nao foi possivel decodificar o texto, a chave usada eh maior do que o maximo informado ou a proporcao texto/chave nao eh grande o suficiente para a precisao utilizada!\n\n");
    fclose(listaMinusculas);
    fclose(textoCifrado);
    fim = clock();
    tTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;
    return 0;
}