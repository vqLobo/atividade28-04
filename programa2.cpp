
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MAX_SENSORES 5 //TEMP, PRES, VIBR, UMID, FLUX

struct Leitura {
    long timestamp;
    double valor;
};

long converter_para_timestamp(const char *data_hora_str) {
    struct tm tm = {0};
    int ano, mes, dia, hora, min, seg;

    if (sscanf(data_hora_str, "%d-%d-%d %d:%d:%d", &ano, &mes, &dia, &hora, &min, &seg) != 6) {
        return -1;
    }

    tm.tm_year = ano - 1900;
    tm.tm_mon = mes - 1;
    tm.tm_mday = dia;
    tm.tm_hour = hora;
    tm.tm_min = min;
    tm.tm_sec = seg;

    return mktime(&tm);
}

int busca_binaria(struct Leitura *v, int n, long alvo) {
    int esq = 0, dir = n - 1, melhor = 0;
    while (esq <= dir) {
        int meio = (esq + dir) / 2;

        if (llabs(v[meio].timestamp - alvo) < llabs(v[melhor].timestamp - alvo)) {
            melhor = meio;
        }
        if (v[meio].timestamp < alvo) {
            esq = meio + 1;
        } else if (v[meio].timestamp > alvo) {
            dir = meio - 1;
        } else {
            return meio;
        }
    }
    return melhor;
}

int main() {
    const char *sensores[MAX_SENSORES] = {
        "TEMP",
        "PRES",
        "VIBR",
        "UMID",
        "FLUX"
    };

    char escolha[16];
    int achou = 0;
    char data_hora[32];

    printf("Qual sensor quer verificar?\n");
    for (int i = 0; i < MAX_SENSORES; i++) {
        printf(" %d. %s\n", i +1, sensores[i]);
    }

    printf("Digite sua escolha: ");
    if (scanf("%15s", escolha) != 1) {
        fprintf(stderr, "Erro ao ler a escolha\n");
        return 1;
    }

    for (int i = 0; i < MAX_SENSORES; i++) {
        if (strcmp(escolha, sensores[i]) == 0) {
            printf("Escolha do sensor: %s\n", sensores[i]);
            achou = 1;
            break;
        }
    }

    if (!achou) {
        printf("Sensor %s nao foi encontrado\n");
        return 1;
    }

    printf("Digite a data e a hora (modelo: XXXX(ano)-YY(mes)-ZZ(dia) HH(hora):MM(minutos):SS(segundos)): ");
    getchar();
    if (!fgets(data_hora, sizeof(data_hora), stdin)) {
        fprintf(stderr, "Erro ao ler a data/hora\n");
        return 1;
    }
    data_hora[strcspn(data_hora, "\n")] = 0;

    long ts_alvo = converter_para_timestamp(data_hora);
    if (ts_alvo < 0) {
        fprintf(stderr, "Erro ao ler a data/hora\n");
        return 1;
    }

    char nome_arquivo[32];
    sprintf(nome_arquivo, "%s.txt", escolha);
    FILE *f = fopen(nome_arquivo, "r");
    if (f == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo\n");
        return 1;
    }

    int cap = 128, n = 0;

    struct Leitura *v = (struct Leitura *) malloc(cap * sizeof *v);

    if (v == NULL) {
        fprintf(stderr, "Erro ao alocar memoria\n");
        fclose(f);
        return 1;
    }

    while (fscanf(f, "%ld %lf", &v[n].timestamp, &v[n].valor) == 2) {
        n++;
        if (n == cap) {
            cap *= 2;
            v = (struct Leitura *) realloc(v, cap * sizeof *v);
            if (v == NULL) {
                fprintf(stderr, "Erro ao alocar memoria\n");
                fclose(f);
                return 1;
            }
        }
    }
    fclose(f);

    if (n == 0) {
        printf("O arquivo esta vazio.");
        free(v);
        return 1;
    }

    int idx = busca_binaria(v, n, ts_alvo);
    printf("\nLeitura mais proxima: \n");
    printf("    Timestamp: %ld\n", v[idx].timestamp);
    printf("    Valor: %.2f\n", v[idx].valor);

    free(v);

    return 0;
}
