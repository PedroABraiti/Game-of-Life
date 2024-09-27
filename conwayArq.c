#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
#define CLEAR_COMMAND "cls"
#else
#define CLEAR_COMMAND "clear"
#endif

// Função para ler o arquivo e inicializar a matriz
int **lerArquivo(char *nomeArquivo, int *linhas, int *colunas) {
    FILE *pFile;
    char linhaArq[100];
    int **matriz = NULL;

    pFile = fopen(nomeArquivo, "r");
    if (pFile == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", nomeArquivo);
        return NULL;
    }

    // Lê as dimensões da matriz
    if (fgets(linhaArq, sizeof(linhaArq), pFile) == NULL) {
        printf("Erro ao ler as dimensões da matriz no arquivo: %s\n", nomeArquivo);
        fclose(pFile);
        return NULL;
    }

    if (sscanf(linhaArq, "%d %d", linhas, colunas) != 2) {
        printf("Formato de dimensões inválido no arquivo: %s\n", nomeArquivo);
        fclose(pFile);
        return NULL;
    }

    // Verifica se as dimensões são válidas
    if (*linhas <= 0 || *colunas <= 0) {
        printf("Dimensões inválidas da matriz lidas do arquivo: %s\n", nomeArquivo);
        fclose(pFile);
        return NULL;
    }

    // Aloca a matriz dinamicamente
    matriz = (int **)malloc((*linhas + 1) * sizeof(int *));
    if (matriz == NULL) {
        printf("Erro ao alocar memória para a matriz.\n");
        fclose(pFile);
        return NULL;
    }

    // Aloca a primeira linha da matriz para armazenar as dimensões
    matriz[0] = (int *)malloc(2 * sizeof(int));
    if (matriz[0] == NULL) {
        printf("Erro ao alocar memória para armazenar as dimensões da matriz.\n");
        fclose(pFile);
        free(matriz);
        return NULL;
    }

    // Armazena as dimensões na primeira linha da matriz
    matriz[0][0] = *linhas;
    matriz[0][1] = *colunas;

    // Lê o restante das linhas e preenche a matriz
    for (int i = 1; i <= *linhas; i++) {
        matriz[i] = (int *)malloc(*colunas * sizeof(int));
        if (matriz[i] == NULL) {
            printf("Erro ao alocar memória para a linha %d da matriz.\n", i);
            fclose(pFile);
            for (int j = 0; j < i; j++) {
                free(matriz[j]);
            }
            free(matriz);
            return NULL;
        }

        if (fgets(linhaArq, sizeof(linhaArq), pFile) == NULL) {
            printf("Erro ao ler a linha %d da matriz no arquivo: %s\n", i, nomeArquivo);
            fclose(pFile);
            for (int j = 0; j <= i; j++) {
                free(matriz[j]);
            }
            free(matriz);
            return NULL;
        }

        for (int j = 0; j < *colunas; j++) {
            matriz[i][j] = linhaArq[j] - '0';
        }
    }

    fclose(pFile);
    return matriz;
}

// Função para imprimir a matriz no console
void imprimeMatriz(int **matriz, int linhas, int colunas) {
    for (int i = 1; i <= linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (matriz[i][j] == 1) {
                printf("\033[0;32m"); // Cor verde para células vivas
                printf("*");
                printf("\033[0m"); // Reset para a cor padrão do terminal
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

// Função para contar o número de vizinhos vivos de uma célula
int contaVizinhos(int **matriz, int linha, int coluna, int linhas, int colunas) {
    int contador = 0;

    for (int i = linha - 1; i <= linha + 1; i++) {
        for (int j = coluna - 1; j <= coluna + 1; j++) {
            if (i == linha && j == coluna) {
                continue; // Ignora a própria célula
            }
            if (i >= 1 && i <= linhas && j >= 0 && j < colunas) {
                contador += matriz[i][j];
            }
        }
    }

    return contador;
}

// Função para simular uma iteração do Jogo da Vida de Conway
void simulaJogo(int **matriz, int linhas, int colunas) {
    int **novaMatriz = (int **)malloc((linhas + 1) * sizeof(int *));
    for (int i = 1; i <= linhas; i++) {
        novaMatriz[i] = (int *)malloc(colunas * sizeof(int));
        if (novaMatriz[i] == NULL) {
            printf("Erro ao alocar memória para a nova matriz.\n");
            for (int j = 1; j < i; j++) {
                free(novaMatriz[j]);
            }
            free(novaMatriz);
            return;
        }
    }

    // Copia a matriz atual para a nova matriz
    for (int i = 1; i <= linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            novaMatriz[i][j] = matriz[i][j];
        }
    }

    // Aplica as regras do Jogo da Vida
    for (int i = 1; i <= linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            int vizinhosVivos = contaVizinhos(matriz, i, j, linhas, colunas);

            if (matriz[i][j] == 1) {
                if (vizinhosVivos < 2 || vizinhosVivos > 3) {
                    novaMatriz[i][j] = 0; // Célula morre por solidão ou superpopulação
                }
            } else {
                if (vizinhosVivos == 3) {
                    novaMatriz[i][j] = 1; // Célula nasce por reprodução
                }
            }
        }
    }

    // Copia a nova matriz de volta para a matriz original
    for (int i = 1; i <= linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = novaMatriz[i][j];
        }
    }

    // Libera a memória da nova matriz
    for (int i = 1; i <= linhas; i++) {
        free(novaMatriz[i]);
    }
    free(novaMatriz);
}

int main() {
    char nomeArquivo[] = "estado3.vid";
    int **matriz;
    int linhas, colunas, qtdLoops;

    // Ler a matriz do arquivo
    matriz = lerArquivo(nomeArquivo, &linhas, &colunas);
    if (matriz == NULL) {
        printf("Erro ao carregar a matriz do arquivo.\n");
        return 1;
    }

    // Imprimir a matriz inicial
    imprimeMatriz(matriz, linhas, colunas);

    // Solicitar o número de iterações
    printf("Digite quantas iterações deseja simular: ");
    scanf("%d", &qtdLoops);

    // Executar o Jogo da Vida por um número de iterações especificado
    for (int i = 0; i < qtdLoops; i++) {
        sleep(1); // Espera 1 segundo antes de atualizar a tela
        system(CLEAR_COMMAND); // Limpa a tela do console

        simulaJogo(matriz, linhas, colunas);
        imprimeMatriz(matriz, linhas, colunas);
    }

    // Liberar a memória da matriz
    for (int i = 0; i <= linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}
