#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")
#define PORT 8080
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "C:/Users/Wheel of Fortune/Documents/GitHub/TastyPorject/cJSON.h"
//#include "E:/glib-main/glib/glib.h"

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

char * getRecipe(char* ingredients) {


    CURL *hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    char *base_url = "https://tasty.p.rapidapi.com/recipes/list?from=0&size=5&q=";
    char *url = malloc(strlen(base_url) + strlen(ingredients) + 1);
    // Copy the base URL to the new array
    strcpy(url, base_url);

    // Concatenate the ingredients onto the new array
    strcat(url, ingredients);

    printf("This is requesting %s", url);
    curl_easy_setopt(hnd, CURLOPT_URL, url);

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
    return chunk.memory;
//    cJSON* root = cJSON_Parse(chunk.memory);
//    cJSON *results = cJSON_GetObjectItemCaseSensitive(root, "results");
//    int size = cJSON_GetArraySize(results);
//
//
//
//    curl_easy_cleanup(hnd);
//    curl_slist_free_all(headers);
//    free(chunk.memory);
//    return results;
}
int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024] = {0};
    char *respnse;
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

// Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("socket failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

// Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("setsockopt failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("bind failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        printf("listen failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    int addrlen = sizeof(address);
//    if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (int *) &addrlen)) == INVALID_SOCKET) {
//        printf("accept failed. Error Code : %d", WSAGetLastError());
//        return 1;
//    }
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *) &address, (int *) &addrlen);
        const char *response = NULL;
        char input[1024];
        int valread = recv(new_socket, input, 1024, 0);
        if (valread == SOCKET_ERROR) {
            printf("recv failed. Error Code : %d", WSAGetLastError());
            return 1;
        }
        input[valread] = '\0'; // null terminate the string
        // Find the start and end positions of the search-input parameter
        const char* search_param = "search-input=";
        const char* search_start = strstr(input, search_param);
        if (search_start == NULL) {
            printf("search-input parameter not found\n");
            continue;
        }
        search_start += strlen(search_param);
        const char* search_end = strchr(search_start, ' ');
        if (search_end == NULL) {
            printf("Invalid user input format\n");
            return 1;
        }
        // Extract the search-input value
        char search_value[256];
        int search_len = search_end - search_start;
        memcpy(search_value, search_start, search_len);
        search_value[search_len] = '\0';
        printf("Search input value: %s\n", search_value);
        // Send the search request to the Tasty API
        if (search_value[0] != '\0') {
            // Send search request to Tasty API and receive response
            printf("Sending search request to Tasty API\n");
            char* response_str = getRecipe(search_value);

            // Construct HTTP response
            char response[strlen(response_str) + 1024];
            if (response_str == NULL) {
                sprintf(response, "HTTP/1.1 500 Internal Server Error\r\n"
                                  "Content-Type: text/plain\r\n"
                                  "Access-Control-Allow-Origin: *\r\n"
                                  "Content-Length: %llu\r\n\r\n%s",
                        strlen("Internal Server Error"), "Internal Server Error");
            } else {
                sprintf(response, "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: application/json\r\n"
                                  "Access-Control-Allow-Origin: *\r\n"
                                  "Content-Length: %llu\r\n\r\n%s",
                        strlen(response_str), response_str);
            }
            // Send response to client
            if (send(new_socket, response, strlen(response), 0) < 0) {
                printf("Failed to send response to client\n");
            } else {
                printf("\nResponse sent to client\n");
            }
            free(response_str);
        } else {
            printf("No search input value\n");
            send(new_socket, "No search input value", strlen("No search input value"), 0);
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
    }
