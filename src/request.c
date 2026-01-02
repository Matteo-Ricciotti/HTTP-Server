#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/request.h"

void parse_request(char *requestBuffer, char *method, char *path, char *queryString, char *version, QueryParam *queryParams)
{
    char url[REQUEST_PATH_SIZE + REQUEST_QUERY_STRING_SIZE] = {0};

    sscanf(requestBuffer, "%s %s %s", method, url, version);

    char *pStartQueryString = strchr(url, '?');

    if (NULL == pStartQueryString)
    {
        strncpy(path, url, REQUEST_PATH_SIZE);
        path[REQUEST_PATH_SIZE - 1] = '\0';
    }
    else
    {
        int pathLen = pStartQueryString - url;
        if (pathLen >= REQUEST_PATH_SIZE)
            pathLen = REQUEST_PATH_SIZE - 1;

        strncpy(path, url, pathLen);
        path[pathLen] = '\0';

        strncpy(queryString, pStartQueryString + 1, REQUEST_QUERY_STRING_SIZE);
        queryString[REQUEST_QUERY_STRING_SIZE - 1] = '\0';

        parse_query_params(queryParams, queryString);
    }

    printf("Method: %s, Path: %s, Version: %s\n", method, path, version);
    printf("Query Params:\n");
    for (int i = 0; i < REQUEST_MAX_QUERY_PARAMS; i++)
    {
        if (queryParams[i].key[0] != '\0')
        {
            printf("  [%d] %s = %s\n", i, queryParams[i].key, queryParams[i].value);
        }
    }
}

void parse_query_params(QueryParam *queryParams, char *queryString)
{
    for (int i = 0; i < REQUEST_MAX_QUERY_PARAMS; ++i)
    {
        queryParams[i].key[0] = '\0';
        queryParams[i].value[0] = '\0';
    }

    char *entry = strtok(queryString, "&");

    int paramIndex = 0;

    while (entry != NULL && paramIndex < REQUEST_MAX_QUERY_PARAMS)
    {
        char *pEqualSign = strchr(entry, '=');

        if (NULL != pEqualSign)
        {
            QueryParam *curr = &queryParams[paramIndex];

            size_t keyLen = pEqualSign - entry;
            if (keyLen >= sizeof(curr->key))
                keyLen = sizeof(curr->key) - 1;

            memcpy(curr->key, entry, keyLen);
            curr->key[keyLen] = '\0';

            size_t valLen = strlen(pEqualSign + 1);
            if (valLen >= sizeof(curr->value))
                valLen = sizeof(curr->value) - 1;

            memcpy(curr->value, pEqualSign + 1, valLen);
            curr->value[valLen] = '\0';

            ++paramIndex;
        }

        // NULL = continue on the previous string
        entry = strtok(NULL, "&");
    }
}

void build_response(const Route *found_route, char *responseBuffer)
{
    const char *status;
    const char *body;

    if (NULL == found_route)
    {
        status = "404 Not Found";
        body = "Not Found";
    }
    else
    {
        status = found_route->status;
        body = found_route->body;
    }

    sprintf(responseBuffer,
            "HTTP/1.1 %s\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %ld\r\n"
            "\r\n"
            "%s\n",
            status,
            strlen(body) + 1,
            body);

    printf("Response:\n%s", responseBuffer);
}

const Route *find_route(const Route *routes, int routes_len, char *method, char *path)
{
    for (int i = 0; i < routes_len; ++i)
    {
        if (NULL == method || NULL == path)
        {
            continue;
        }

        int matchMethod = 0 == strcmp(routes[i].method, method);
        int matchPath = 0 == strcmp(routes[i].path, path);

        if (matchMethod && matchPath)
        {
            return &routes[i];
        }
    }

    return NULL;
}
