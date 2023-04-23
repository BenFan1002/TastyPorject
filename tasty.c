#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "C:/Users/Wheel of Fortune/Documents/GitHub/TastyPorject/cJSON.h"
//#include "E:/glib-main/glib/glib.h"
#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#include "E:/nuklear/nuklear.h"

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
/* nuklear - v1.00 - public domain */
static void
calculator(struct nk_context *ctx)
{
    if (nk_begin(ctx, "Calculator", nk_rect(10, 10, 180, 250),
                 NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE))
    {
        static int set = 0, prev = 0, op = 0;
        static const char numbers[] = "789456123";
        static const char ops[] = "+-*/";
        static double a = 0, b = 0;
        static double *current = &a;

        size_t i = 0;
        int solve = 0;
        {int len; char buffer[256];
            nk_layout_row_dynamic(ctx, 35, 1);
            len = snprintf(buffer, 256, "%.2f", *current);
            nk_edit_string(ctx, NK_EDIT_SIMPLE, buffer, &len, 255, nk_filter_float);
            buffer[len] = 0;
            *current = atof(buffer);}

        nk_layout_row_dynamic(ctx, 35, 4);
        for (i = 0; i < 16; ++i) {
            if (i >= 12 && i < 15) {
                if (i > 12) continue;
                if (nk_button_label(ctx, "C")) {
                    a = b = op = 0; current = &a; set = 0;
                } if (nk_button_label(ctx, "0")) {
                    *current = *current*10.0f; set = 0;
                } if (nk_button_label(ctx, "=")) {
                    solve = 1; prev = op; op = 0;
                }
            } else if (((i+1) % 4)) {
                if (nk_button_text(ctx, &numbers[(i/4)*3+i%4], 1)) {
                    *current = *current * 10.0f + numbers[(i/4)*3+i%4] - '0';
                    set = 0;
                }
            } else if (nk_button_text(ctx, &ops[i/4], 1)) {
                if (!set) {
                    if (current != &b) {
                        current = &b;
                    } else {
                        prev = op;
                        solve = 1;
                    }
                }
                op = ops[i/4];
                set = 1;
            }
        }
        if (solve) {
            if (prev == '+') a = a + b;
            if (prev == '-') a = a - b;
            if (prev == '*') a = a * b;
            if (prev == '/') a = a / b;
            current = &a;
            if (set) current = &b;
            b = 0; set = 0;
        }
    }
    nk_end(ctx);
}
int main1() {


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
    cJSON* root = cJSON_Parse(chunk.memory);
    const cJSON *results = cJSON_GetObjectItemCaseSensitive(root, "results");
    int size = cJSON_GetArraySize(results);
    for (int i = 0; i < size; i++) {
    if (cJSON_IsArray(results)) {
        cJSON *first_result = cJSON_GetArrayItem(results, i);
        if (first_result != NULL) {
            const cJSON *name = cJSON_GetObjectItemCaseSensitive(first_result, "name");
            if (cJSON_IsString(name)) {
                printf("Name of the dish: %s\n", name->valuestring);
            }
        }
    }
    }


    curl_easy_cleanup(hnd);
    curl_slist_free_all(headers);
    free(chunk.memory);

    return 0;
}