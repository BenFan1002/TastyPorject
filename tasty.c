#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "C:/Users/Wheel of Fortune/Documents/GitHub/TastyPorject/cJSON.h"
#ifndef GDK_INCLUDE_PATH
#define GDK_INCLUDE_PATH "C:\msys64\mingw64\include"
#endif
#include "C:/msys64/mingw64/include/gtk-4.0/gtk/gtk.h"

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
static void on_activate (GtkApplication *app) {
    // Create a new window
    GtkWidget *window = gtk_application_window_new (app);
    // Create a new button
    GtkWidget *button = gtk_button_new_with_label ("Hello, World!");
    // When the button is clicked, close the window passed as an argument
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_window_close), window);
    gtk_window_set_child (GTK_WINDOW (window), button);
    gtk_window_present (GTK_WINDOW (window));
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
    // Create a new application
    GtkApplication *app = gtk_application_new ("com.example.GtkApplication",
                                               G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
    CURLcode ret = curl_easy_perform(hnd);
    cJSON* root = cJSON_Parse(chunk.memory);
    const cJSON *results = cJSON_GetObjectItemCaseSensitive(root, "results");
    if (cJSON_IsArray(results)) {
        cJSON *first_result = cJSON_GetArrayItem(results, 0);
        if (first_result != NULL) {
            const cJSON *name = cJSON_GetObjectItemCaseSensitive(first_result, "name");
            if (cJSON_IsString(name)) {
                printf("Name of first element: %s\n", name->valuestring);
            }
        }
    }


    curl_easy_cleanup(hnd);
    curl_slist_free_all(headers);
    free(chunk.memory);

    return 0;
}