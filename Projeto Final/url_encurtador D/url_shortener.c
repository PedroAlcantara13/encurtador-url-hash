#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DB_FILE "urls.db"
#define MAX_LINE 1024
#define BASE_URL "http://localhost/cgi-bin/url_shortener.cgi?code="
#define HASH_SIZE 7
#define MAX_LEN 256 

typedef struct {
    char chave[HASH_SIZE];
    char urlOriginal[MAX_LEN];
} URLMap;

// --- Funções de hash e manipulação de URL D---

void gerarHash(const char *url, char *saida, int tamanho) {
    unsigned int hash = 5381;
    for (int i = 0; url[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + url[i]; // hash * 33 + c
    }

    const char *base62 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < tamanho - 1; i++) {
        saida[i] = base62[hash % 62];
        hash /= 62;
    }
    saida[tamanho - 1] = '\0';
}

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

// --- Funções para manipulação do banco (arquivo) ---

int url_exists(const char *url) {
    FILE *fp = fopen(DB_FILE, "r");
    if (!fp) return 0;

    char code_buf[64], url_buf[MAX_LINE];
    while (fscanf(fp, "%s %s", code_buf, url_buf) == 2) {
        if (strcmp(url_buf, url) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

char* find_url(const char *code) {
    static char line[MAX_LINE], code_buf[64], url[MAX_LINE];
    FILE *fp = fopen(DB_FILE, "r");
    if (!fp) return NULL;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%s %[^\n]", code_buf, url) == 2) {
            if (strcmp(code_buf, code) == 0) {
                fclose(fp);
                return url;
            }
        }
    }
    fclose(fp);
    return NULL;
}

void save_url(const char *code, const char *url) {
    FILE *fp = fopen(DB_FILE, "a");
    if (!fp) {
        printf("Content-Type: text/html\r\n\r\n");
        printf("<html><body><p>Erro ao abrir arquivo para salvar URL.</p></body></html>");
        return;
    }
    fprintf(fp, "%s %s\n", code, url);
    fclose(fp);
}

// --- Decodifica URL codificada em form-urlencoded ---

void urldecode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10); else a -= '0';
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b -= ('A' - 10); else b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

// --- Extrai valor do parâmetro 'url' do corpo POST ---

int extrair_valor_url(const char *buffer, char *saida, int max_len) {
    const char *url_start = strstr(buffer, "url=");
    if (!url_start) return 0;
    url_start += 4; // pula "url="

    int i = 0;
    while (url_start[i] && url_start[i] != '&' && i < max_len - 1) {
        saida[i] = url_start[i];
        i++;
    }
    saida[i] = '\0';
    return 1;
}

// --- Função principal para encurtar URL e gerar resposta HTML ---

void encurtarURL_CGI(const char *urlCompleta) {
    char prefixo[MAX_LEN];
    char parteLonga[MAX_LEN];
    extrairPrefixo(urlCompleta, prefixo, parteLonga);

    char code[HASH_SIZE];
    char tentativa[MAX_LEN];
    char *urlExistente;
    int tentativas = 0;

    do {
        if (tentativas == 0) {
            strncpy(tentativa, urlCompleta, MAX_LEN);
        } else {
            snprintf(tentativa, MAX_LEN, "%s%d", urlCompleta, tentativas); // Adiciona sufixo para gerar nova hash
        }

        gerarHash(tentativa, code, sizeof(code));
        urlExistente = find_url(code);

        // Se já existe com a MESMA URL → reutiliza
        if (urlExistente && strcmp(urlExistente, urlCompleta) == 0) {
            printf("Content-Type: text/html\r\n\r\n");
            printf("<html><body>");
            printf("<p>URL já encurtada: <a href=\"%s%s\">%s%s</a></p>", prefixo, code, prefixo, code);
            printf("</body></html>");
            return;
        }

        tentativas++;
    } while (urlExistente); // repete se o código já está usado por outra URL

    // Salva nova URL com código exclusivo
    save_url(code, urlCompleta);

    printf("Content-Type: text/html\r\n\r\n");
    printf("<html><body>");
    printf("<p>URL encurtada: <a href=\"%s%s\">%s%s</a></p>", prefixo, code, prefixo, code);
    printf("</body></html>");
}


// --- Redireciona para a URL original a partir do código ---

void redirect_to_original(const char *code) {
    char *url = find_url(code);

    if (url) {
        printf("Status: 302 Found\r\n");
        printf("Location: %s\r\n\r\n", url);
    } else {
        printf("Content-Type: text/html\r\n\r\n");
        printf("<html><body><p>URL não encontrada para o código: %s</p></body></html>", code);
    }
}

// --- Handle CGI request ---

void handle_request() {
    char *method = getenv("REQUEST_METHOD");
    if (method == NULL) {
        printf("Content-Type: text/html\r\n\r\n");
        printf("<html><body><p>REQUEST_METHOD não encontrado.</p></body></html>");
        return;
    }

    if (strcmp(method, "GET") == 0) {
        char *query = getenv("QUERY_STRING");
        if (query && strncmp(query, "code=", 5) == 0) {
            const char *code = query + 5;
            redirect_to_original(code);
        } else {
            // Página inicial ou formulário
            printf("Content-Type: text/html\r\n\r\n");
            printf("<html><body>");
            printf("<h2>Encurtador de URLs</h2>");
            printf("<form method=\"POST\" action=\"\">");
            printf("URL: <input type=\"text\" name=\"url\" size=\"60\">");
            printf("<input type=\"submit\" value=\"Encurtar\">");
            printf("</form>");
            printf("</body></html>");
        }
    } else if (strcmp(method, "POST") == 0) {
        int len = atoi(getenv("CONTENT_LENGTH"));
        if (len > 0 && len < MAX_LINE) {
            char post_data[MAX_LINE];
            fread(post_data, 1, len, stdin);
            post_data[len] = '\0';

            char url_raw[MAX_LINE];
            char url_decoded[MAX_LINE];

            if (extrair_valor_url(post_data, url_raw, sizeof(url_raw))) {
                urldecode(url_decoded, url_raw);
                encurtarURL_CGI(url_decoded);
            } else {
                printf("Content-Type: text/html\r\n\r\n");
                printf("<html><body><p>Parâmetro URL não encontrado no POST.</p></body></html>");
            }
        } else {
            printf("Content-Type: text/html\r\n\r\n");
            printf("<html><body><p>Dados POST inválidos ou vazios.</p></body></html>");
        }
    } else {
        printf("Content-Type: text/html\r\n\r\n");
        printf("<html><body><p>Método HTTP não suportado.</p></body></html>");
    }
}

int main() {
    handle_request();
    return 0;
}
