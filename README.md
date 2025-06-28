# vigenere-cracker
Projeto em C capaz de decodificar de forma eficiente e personalizável textos escritos em língua portuguesa (PT-BR) e criptografados com Cifra de Vigenere, adivinhando a chave utilizada.


## 📁 Estrutura dos Arquivos

- `/code/descriptografar.c` — Arquivo principal que decodifica o texto informado e apresenta o resultado.
- `/code/*.c` — Os demais arquivos .c (todos criados por mim) são usados para gerar os recursos necessários para o projeto, caso já faça o download do projeto com os recursos, não será necessário executá-los.
- `/code/resources/texto-cifrado.txt` — Texto que será decodificado, o repositório tem um exemplo para você testar, substitua pelo texto alvo.
- `/code/resources/*.txt` — Demais arquivos de texto utilizados pelo programa. Com a excessão do "lista-palavras" foram todos gerados pelos arquivos .c disponíveis no projeto.

## 💡 Saiba para Testar

1. **Texto**
   - Cole o texto cifrado (sem espaços) em "resources/texto-cifrado.txt"
   - Caso o texto tenha qualquer caracter fora do intervalo [A-Z] ele será removido na descriptografia

2. **Tamanho máximo da chave**
    - O programa irá pedir para você informar o tamanho máximo da chave a ser testada.
    - O ideal é usar o valor superior mais próximo possível, caso não saiba, pode colocar um valor alto, mas isso irá impedir de aumentar a precisão, explicada abaixo.

3. **Precisão máxima**
   - Você pode escolher a precisão (1 a 5) máxima a ser utilizada ou pode deixar que o programa escolha por você.
   - **Considerações importantes:**
     - A precisão 1 é capaz de testar qualquer tamanho de chave, mas depende de uma razão texto/chave grande.
     - A escolha automática da precisão é baseada no tamanho da chave e tem o objetivo de adivinhar o texto instantâneamente, só use uma precisão manual caso ela não seja suficiente.
     - Para chaves acima de 30 caracteres apenas a precisão 1 é viável, pois tem crescimento de complexidade linear, enquanto as demais tem crescimento exponencial.

## ⏱ Progresso e Desempenho
Na maior parte dos casos, a decodificação deve ser imediata, mas para os casos de razão texto/chave pequena em que o usuário escolha usar uma precisão mais alta que a recomendada, temos os seguintes recursos:
- A cada pelo menos 100.000 tentivas e 1 segundo decorrido, o progresso é exibido no terminal.
- Estimativas de tempo restante e tempo total decorrido são apresentadas para facilitar o acompanhamento.

## ⚙️​ Como o Algoritmo Funciona

Para reduzir possibilidades de teste o programa utiliza **análise de frequência** e para reconhecer textos válidos usa de **validação fonética** e **validação por cobertura do dicionário**. O passo a passo é o seguinte:

1. **Análise de frequência**  
   Para cada tamanho de chave **T** começando em 1 até o valor máximo informado na entrada, o programa quebra o texto cifrado em **N** colunas de tamanho **T** e aplica os seguintes passos:
   - Calcula a frequência de letras em cada coluna.
   - Compara com as frequências da língua portuguesa.
   - Seleciona as **P** (onde P é a precisão máxima escolhida) letras mais prováveis para cada posição da coluna baseado na convergência de frequência.
   - Após esse passo, temos as letras que vamos usar para gerar as combinações de chaves possíveis.


2. **Geração de combinações**
    - O programa vai gerar todas as combinações possíveis baseado nas letras prováveis e testar seguindo os passos descritos mais adiante.
    - O número de combinações a serem testadas é $P^T$.
    - Assim, quando a precisão é 1, há apenas uma combinação a se testar.

3. **Validação fonética**   
    Apesar de a análise de frequência contribuir para gerar combinações com fonética acima da média, são necessárias validações mais rígidas. O algoritmo estabelece as seguintes, se baseando em conhecimento estatístico da língua portuguesa: 
   1. **Vogais**   
       - O texto decodificado deve ser composto por pelo menos 35% de vogais;
       - Não pode haver nenhuma sequência de 7 caracteres com menos de 2 vogais;
   2. **Bigramas**
        - Usando uma lista dicionário como base, foi gerada uma lista contendo bigramas raros da língua portuguesa;
        - Para o texto ser válido, ele deve ter menos de 10% de bigramas raros;

4. **Cobertura do dicionário**   
    Com os passos anteriores agora sobram poucas combinações, que somente podem ser descartadas através de uma validação com palavras reais, que é feita da seguinte forma:   
    - Como o texto cifrado não possui espaços, é preciso inferir quais são as palavras;
    - Um algoritmo guloso busca, para cada sequência de caracteres do texto cifrado, a maior palavra de pelo menos 2 letras correspondente na lista dicionário, então passando para a próxima palavra. Caso não haja correspondência, simplesmente passa para o próximo caracter;
    - Para o texto ser válido, esse algoritmo deve encontrar correspondências para mais de 90% do seu comprimento;

Com esses passos é possível processar trilhões de combinações em **menos de 1s** e encontrar um **único resultado** que corresponde exatamente ao texto original sem espaços.

## ⚖️ Parâmetros e Ajustes   
- `MAX_PRECISAO (5)`: número máximo de letras prováveis por posição da chave, limita a escolha de precisão do usuário.
- `BI_ALVO (0.1)`: proporção máxima aceitável de bigramas raros.
- `MIN_COBERTURA (0.9)`: proporção mínima do texto que é composto por palavras conhecidas.

Esses valores podem ser ajustados no código para refinar a sensibilidade da análise.

## 📚 Créditos

Criado por **Pedro Augusto Gandra de Andrade**.  
Algoritmo e heurísticas baseados em conhecimento estatístico da língua portuguesa.