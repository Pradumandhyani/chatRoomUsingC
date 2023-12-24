#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

// Callback function to write HTTP response to a buffer
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    char** buffer = (char**)userp;

    *buffer = realloc(*buffer, realsize + 1);
    if (*buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }

    memcpy(*buffer, contents, realsize);
    (*buffer)[realsize] = 0;

    return realsize;
}

int main() {
    CURL* curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL
        const char* url = "https://api.dictionaryapi.dev/api/v2/entries/en/apple";
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Perform the HTTP request
        char* response_buffer = NULL;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse JSON using cJSON library
            cJSON* root = cJSON_Parse(response_buffer);
            cJSON* meanings = cJSON_GetObjectItemCaseSensitive(root, "meanings");

            if (cJSON_IsArray(meanings)) {
                for (int i = 0; i < cJSON_GetArraySize(meanings); i++) {
                    cJSON* meaning = cJSON_GetArrayItem(meanings, i);
                    cJSON* partOfSpeech = cJSON_GetObjectItemCaseSensitive(meaning, "partOfSpeech");
                    cJSON* definition = cJSON_GetObjectItemCaseSensitive(meaning, "definition");

                    printf("Part of Speech: %s\n", cJSON_GetStringValue(partOfSpeech));
                    printf("Definition: %s\n", cJSON_GetStringValue(definition));
                }
            }

            cJSON_Delete(root);
        }

        // Clean up
        curl_easy_cleanup(curl);
        free(response_buffer);
    }

    // Cleanup libcurl
    curl_global_cleanup();

    return 0;
}

