#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_CHAR 256

// Estrutura para representar um nó da árvore de Huffman
typedef struct Node {
    char character;
    int frequency;
    struct Node *left, *right;
} Node;

// Estrutura para armazenar as frequências dos caracteres
typedef struct {
    char character;
    int frequency;
} FrequencyTable;

// Estrutura para armazenar os códigos de Huffman
typedef struct {
    char character;
    char code[MAX_CHAR];
} CodeTable;

// Tabelas de frequência e de códigos
FrequencyTable freqTable[MAX_CHAR];
CodeTable codeTable[MAX_CHAR];
int freqCount = 0;
int codeCount = 0;

// Função para criar um novo nó da árvore de Huffman
Node* createNode(char character, int frequency, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(EXIT_FAILURE);
    }
    node->character = character;
    node->frequency = frequency;
    node->left = left;
    node->right = right;
    return node;
}

// Função para adicionar ou atualizar a frequência de um caractere
void addFrequency(char character, int frequency) {
    for (int i = 0; i < freqCount; i++) {
        if (freqTable[i].character == character) {
            freqTable[i].frequency = frequency;
            return;
        }
    }
    if (freqCount < MAX_CHAR) {
        freqTable[freqCount].character = character;
        freqTable[freqCount].frequency = frequency;
        freqCount++;
    } else {
        fprintf(stderr, "Erro: número máximo de caracteres excedido\n");
    }
}

// Função para construir a árvore de Huffman
Node* buildHuffmanTree() {
    Node* nodes[MAX_CHAR];
    int nodeCount = freqCount;

    // Cria nós para cada caractere na tabela de frequências
    for (int i = 0; i < freqCount; i++) {
        nodes[i] = createNode(freqTable[i].character, freqTable[i].frequency, NULL, NULL);
    }

    // Combina os nós até restar apenas um (a raiz da árvore)
    while (nodeCount > 1) {
        // Encontra os dois nós com menor frequência
        int minIndex1 = -1, minIndex2 = -1;
        for (int i = 0; i < nodeCount; i++) {
            if (minIndex1 == -1 || nodes[i]->frequency < nodes[minIndex1]->frequency) {
                minIndex2 = minIndex1;
                minIndex1 = i;
            } else if (minIndex2 == -1 || nodes[i]->frequency < nodes[minIndex2]->frequency) {
                minIndex2 = i;
            }
        }

        // Cria um novo nó combinando os dois nós de menor frequência
        Node* newNode = createNode('\0', nodes[minIndex1]->frequency + nodes[minIndex2]->frequency, nodes[minIndex1], nodes[minIndex2]);
        nodes[minIndex1] = newNode;
        nodes[minIndex2] = nodes[nodeCount - 1];
        nodeCount--;
    }

    return nodes[0];
}

// Função para gerar os códigos de Huffman a partir da árvore
void generateCodes(Node* root, char* code, int depth) {
    if (!root) return;

    if (root->left == NULL && root->right == NULL) {
        code[depth] = '\0';
        codeTable[codeCount].character = root->character;
        strcpy(codeTable[codeCount].code, code);
        codeCount++;
        return;
    }

    if (root->left) {
        code[depth] = '0';
        generateCodes(root->left, code, depth + 1);
    }
    if (root->right) {
        code[depth] = '1';
        generateCodes(root->right, code, depth + 1);
    }
}

// Função para calcular os códigos de Huffman
void calculateHuffmanCodes() {
    Node* root = buildHuffmanTree();
    char code[MAX_CHAR];
    codeCount = 0;
    generateCodes(root, code, 0);
}

// Função para imprimir a tabela de códigos de Huffman
void printHuffmanCodes() {
    for (int i = 0; i < codeCount; i++) {
        printf("%c com frequencia %d codificado como %s\n", codeTable[i].character, freqTable[i].frequency, codeTable[i].code);
    }
}

// Função para codificar uma string com base nos códigos de Huffman
void encodeString(char* str) {
    char encoded[MAX_CHAR * 8] = "";
    for (int i = 0; i < strlen(str); i++) {
        for (int j = 0; j < codeCount; j++) {
            if (codeTable[j].character == str[i]) {
                strcat(encoded, codeTable[j].code);
                break;
            }
        }
    }

    // Calcula a taxa de compressão
    int originalBits = strlen(str) * 8;
    int compressedBits = strlen(encoded);
    double compressionRate = (double)compressedBits / originalBits * 100;
    double minimumBits = (double)compressedBits / strlen(str) * 100;

    printf("%s %.2f%% %.2f%%\n", encoded, compressionRate, minimumBits);
}

// Função para decodificar uma sequência de bits
void decodeBits(char* bits) {
    Node* root = buildHuffmanTree();
    Node* current = root;
    for (int i = 0; i < strlen(bits); i++) {
        if (bits[i] == '0') {
            current = current->left;
        } else if (bits[i] == '1') {
            current = current->right;
        } else {
            fprintf(stderr, "Erro: sequência de bits inválida\n");
            return;
        }

        if (current->left == NULL && current->right == NULL) {
            printf("%c", current->character);
            current = root;
        }
    }
    printf("\n");
}

// Função para resetar a tabela de frequência
void clearFrequencyTable() {
    freqCount = 0;
    codeCount = 0;
}

// Função principal do programa
int main() {
    char command[MAX_CHAR];
    char character;
    int frequency;
    char str[MAX_CHAR];
    char bits[MAX_CHAR];

    while (1) {
        printf("->");
        if (scanf("%s", command) != 1) {
            fprintf(stderr, "Erro ao ler o comando\n");
            continue;
        }

        if (strcmp(command, "add") == 0) {
            if (scanf(" %c %d", &character, &frequency) != 2) {
                fprintf(stderr, "Erro ao ler os parâmetros para add\n");
                continue;
            }
            addFrequency(character, frequency);
        } else if (strcmp(command, "calc") == 0) {
            calculateHuffmanCodes();
        } else if (strcmp(command, "print") == 0) {
            printHuffmanCodes();
        } else if (strcmp(command, "cod") == 0) {
            if (scanf("%s", str) != 1) {
                fprintf(stderr, "Erro ao ler a string para cod\n");
                continue;
            }
            encodeString(str);
        } else if (strcmp(command, "dec") == 0) {
            if (scanf("%s", bits) != 1) {
                fprintf(stderr, "Erro ao ler a sequência de bits para dec\n");
                continue;
            }
            decodeBits(bits);
        } else if (strcmp(command, "clear") == 0) {
            clearFrequencyTable();
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            fprintf(stderr, "Comando inválido\n");
        }
    }

    return 0;
}
