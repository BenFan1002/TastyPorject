#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
//#include <json-c/json.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void format_output(const char *input) {
    int indent = 0;
    int in_string = 0;

    while (*input) {
        switch (*input) {
            case '{':
            case '[':
                if (!in_string) {
                    putchar(*input);
                    putchar('\n');
                    indent += 2;
                    for (int i = 0; i < indent; i++) putchar(' ');
                } else {
                    putchar(*input);
                }
                break;

            case '}':
            case ']':
                if (!in_string) {
                    putchar('\n');
                    indent -= 2;
                    for (int i = 0; i < indent; i++) putchar(' ');
                    putchar(*input);
                } else {
                    putchar(*input);
                }
                break;

            case ',':
                if (!in_string) {
                    putchar(*input);
                    putchar('\n');
                    for (int i = 0; i < indent; i++) putchar(' ');
                } else {
                    putchar(*input);
                }
                break;

            case '"':
                in_string = !in_string;
                putchar(*input);
                break;

            default:
                putchar(*input);
                break;
        }
        input++;
    }
}
int main() {
    CURL *hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://tasty.p.rapidapi.com/recipes/list?from=0&size=20&tags=under_30_minutes&q=chicken");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "X-RapidAPI-Key: 25af54c4ecmsh4c292592ba14e97p1ed281jsn7d6876b49586");
    headers = curl_slist_append(headers, "X-RapidAPI-Host: tasty.p.rapidapi.com");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L); // Disable SSL peer verification

    CURLcode ret = curl_easy_perform(hnd);

    if (ret != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
    } else {
        printf("Output:\n");
        format_output(chunk.memory);
        printf("\n");
    }

    curl_easy_cleanup(hnd);
    curl_slist_free_all(headers);
    free(chunk.memory);

    return 0;
}