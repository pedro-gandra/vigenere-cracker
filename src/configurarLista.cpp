#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

void toLowerStr(string& str) {
    for (char& c : str) {
        c = tolower((unsigned char)c);
    }
}

int main() {
    ifstream lista("resources/lista-palavras.txt");
    ofstream novaLista("resources/lista-minusculas.txt");
    string palavra;
    while (lista >> palavra) {
        toLowerStr(palavra);
        novaLista << palavra << '\n';
    }
    return 0;
}