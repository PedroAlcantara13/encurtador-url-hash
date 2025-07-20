#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_URLS 100
#define HASH_SIZE 7
#define MAX_LEN 256

typedef struct {
    char chave[HASH_SIZE];
    char urlOriginal[MAX_LEN];
} URLMap;

URLMap banco[MAX_URLS];
int total = 0;

// Hash simples estilo DJB2
void gerarHash(const char *url, char *saida) {
    unsigned int hash = 5381;
    for (int i = 0; url[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + url[i]; // hash * 33 + c
    }

    const char *base62 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < HASH_SIZE - 1; i++) {
        saida[i] = base62[hash % 62];
        hash /= 62;
    }
    saida[HASH_SIZE - 1] = '\0';
}

// Separa o prefixo (até última '/')
void extrairPrefixo(const char *url, char *prefixo, char *restante) {
    const char *ultimaBarra = strrchr(url, '/');
    if (ultimaBarra != NULL && ultimaBarra > url + 7) { // depois de https://
        int pos = ultimaBarra - url + 1;
        strncpy(prefixo, url, pos);
        prefixo[pos] = '\0';
        strcpy(restante, ultimaBarra + 1);
    } else {
        strcpy(prefixo, url);
        restante[0] = '\0';
    }
}

void encurtarURL(const char *urlCompleta, char *saidaFinal) {
    if (total >= MAX_URLS) {
        printf("Limite de URLs atingido.\n");
        return;
    }

    char prefixo[MAX_LEN];
    char parteLonga[MAX_LEN];
    extrairPrefixo(urlCompleta, prefixo, parteLonga);

    char hash[HASH_SIZE];
    gerarHash(urlCompleta, hash);

    // Verifica se já existe
    for (int i = 0; i < total; i++) {
        if (strcmp(banco[i].urlOriginal, urlCompleta) == 0) {
            sprintf(saidaFinal, "%s%s", prefixo, banco[i].chave);
            return;
        }
    }

    strcpy(banco[total].urlOriginal, urlCompleta);
    strcpy(banco[total].chave, hash);
    sprintf(saidaFinal, "%s%s", prefixo, hash);
    total++;
}

const char* recuperarURL(const char *chave) {
    for (int i = 0; i < total; i++) {
        if (strcmp(banco[i].chave, chave) == 0) {
            return banco[i].urlOriginal;
        }
    }
    return NULL;
}

// Exemplo de uso
int main() {
    char url[MAX_LEN];
    char encurtada[MAX_LEN];
    char chaveBusca[HASH_SIZE];

    printf("Digite a URL para encurtar: ");
    fgets(url, sizeof(url), stdin);
    url[strcspn(url, "\n")] = 0; // remove '\n'

    encurtarURL(url, encurtada);
    printf("URL Encurtada: %s\n", encurtada);

    printf("Digite a chave curta para buscar (ex: a1b2c3): ");
    fgets(chaveBusca, sizeof(chaveBusca), stdin);
    chaveBusca[strcspn(chaveBusca, "\n")] = 0;

    const char *original = recuperarURL(chaveBusca);
    if (original) {
        printf("URL Original: %s\n", original);
    } else {
        printf("Chave não encontrada.\n");
    }

    return 0;
}
