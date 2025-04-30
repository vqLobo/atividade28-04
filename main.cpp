//incluir os "includes" necessários para o bom funcionamento do programa

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#define MAX_SENSORES 5 //TEMP, PRES, VIBR, UMID, FLUX

struct Leitura {
    long timestamp;
    double valor;
};

struct SensorDados {
    char nome[10];
    struct Leitura *leituras;
    int num_leituras;
};

int comparar_timestamp(const void *a, const void *b) {
    const struct Leitura *l1 = (const struct Leitura*) a;
    const struct Leitura *l2 = (const struct Leitura*) b;
    return (l1->timestamp - l2->timestamp);
}

int main() {
    //inicializa os sensores
    struct SensorDados sensores[MAX_SENSORES] = {
        {"TEMP", NULL, 0},
        {"PRES", NULL, 0},
        {"VIBR", NULL, 0},
        {"UMID", NULL, 0},
        {"FLUX", NULL, 0}
    };

    char caminho[256];
    getcwd(caminho, sizeof(caminho));
    printf("Diretorio atual: %s\n", caminho);
    //abre o arquivo para leitura e escrita
    FILE *arquivo_entrada = fopen("dados_brutos.txt", "r"); //esse comando abre o arquivo
    if (arquivo_entrada == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    } //o if informa que o programa não conseguiu abrir o arquivo.

    char linha[256]; //buffer para cada linha do arquivo.
    while (fgets(linha, sizeof(linha), arquivo_entrada)){
        long timestamp;
        char id_sensor[50];
        double valor;

        if (sscanf(linha, "%ld %s %lf", &timestamp, id_sensor, &valor) == 3){
            printf("Lido: %ld %s %.2f\n", timestamp, id_sensor, valor);

            //verifica a qual sensor pertence
            for (int i = 0; i < MAX_SENSORES; i++) {
              if (strcmp(id_sensor, sensores[i].nome) == 0) {

                  //calcula o novo tamanho
                  int novo_tamanho = sensores[i].num_leituras + 1;

                  struct Leitura *temp = (struct Leitura*) realloc (sensores[i].leituras, novo_tamanho * sizeof(struct Leitura));

                  if (temp == NULL) {
                      printf("Erro na alocação da memória\n");
                      exit(1);
                  }

                  //armazena a nova leitura
                  sensores[i].leituras = temp;
                  sensores[i].leituras[sensores[i].num_leituras].timestamp = timestamp;
                  sensores[i].leituras[sensores[i].num_leituras].valor = valor;

                  //incrementa
                  sensores[i].num_leituras++;
                  break;
              }
            }
        } else {
            printf("Formato inválido: %s", linha);
        }
    }

    fclose(arquivo_entrada);

    //salvar arquivos
    for (int i = 0; i < MAX_SENSORES; i++) {
        if (sensores[i].num_leituras > 0) {
            qsort(sensores[i].leituras, sensores[i].num_leituras, sizeof(struct Leitura), comparar_timestamp);

            char nome_arquivo[20];
            sprintf(nome_arquivo, "%s.txt", sensores[i].nome);
            FILE *arquivo_saida = fopen(nome_arquivo, "w");

            if (arquivo_saida == NULL) {
                printf("Erro ao criar arquivo %s\n", nome_arquivo);
                continue;
            }

            //dados são ordenados

            for (int j =0; j < sensores[i].num_leituras; j++) {
                fprintf(arquivo_saida, "%ld %.2f\n", sensores[i].leituras[j].timestamp, sensores[i].leituras[j].valor);

            }
            fclose(arquivo_saida);
        }
    }

    //libera a memória alocada.
    for (int i = 0; i < MAX_SENSORES; i++) {
        free(sensores[i].leituras);
    }

    return 0;
}
