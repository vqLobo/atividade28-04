
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N_SENSORES 5
#define N_LEITURAS 1000
#define TOTAL_LEITURAS (N_SENSORES * N_LEITURAS)

struct Leitura {
    time_t timestamp;
    char sensor[10];
    double valor;
};

void embaralhar(struct Leitura *v, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        struct Leitura tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}


int main() {
    const char *sensores[N_SENSORES] = {"TEMP", "PRES", "VIBR", "UMID", "FLUX"};
    int dia, mes, ano;
    char nome_arquivo[] = "dados_brutos.txt";

    printf("Digite o dia, mes e ano (modelo: 30 04 2025): ");
    if (scanf("%d %d %d", &dia, &mes, &ano) != 3) {
        fprintf(stderr, "Formato invalido.\n");
        return 1;
    }

    struct tm tm_inicio = {0};

    tm_inicio.tm_mday = dia;
    tm_inicio.tm_mon = mes - 1;
    tm_inicio.tm_year = ano - 1900;

    time_t t_inicio = mktime(&tm_inicio);
    if (t_inicio == (time_t) - 1) {
        fprintf(stderr, "Erro ao converter a data.\n");
        return 1;
    }

    struct Leitura *leituras = (struct Leitura *)malloc(TOTAL_LEITURAS * sizeof(struct Leitura));

    if (leituras == NULL) {
        printf("Erro ao alocar memoria");
        return 1;
    }

    srand((unsigned) time(NULL));

    int k = 0;
    for (int s = 0; s < N_SENSORES; s++) {
        for (int i = 0; i < N_LEITURAS; i++) {
            leituras[k].timestamp = t_inicio + (rand() % 86400);
            strncpy(leituras[k].sensor, sensores[s], sizeof(leituras[k].sensor) - 1);
            leituras[k].sensor[sizeof(leituras[k].sensor) - 1] = '\0';
            leituras[k].valor = ((double) rand() / RAND_MAX * 100.0);
            k++;
        }
    }

    embaralhar(leituras, TOTAL_LEITURAS);

    FILE *f = fopen(nome_arquivo, "w");
    if (f == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    for (int i = 0; i < TOTAL_LEITURAS; i++) {
        fprintf(f, "%ld %s %.2f\n", leituras[i].timestamp, leituras[i].sensor, leituras[i].valor);
    }

    fclose(f);
    free(leituras);
    printf("Arquivo gerado com sucesso. \n");

    return 0;
}
