#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/request.h"

void parse_request(char *requestBuffer, char *method, char *path, char *version, QueryParam *queryParams)
{

    char url[REQUEST_PATH_SIZE + REQUEST_QUERY_STRING_SIZE] = {0};
    char queryString[REQUEST_QUERY_STRING_SIZE] = {0};

    sscanf(requestBuffer, "%s %s %s", method, url, version);

    char *pStartQueryString = strchr(url, '?');

    if (NULL == pStartQueryString)
    {
        strncpy(path, url, REQUEST_PATH_SIZE + 1);
    }
    else
    {
        int index = pStartQueryString - url;

        strncpy(path, url, index);
        strncpy(queryString, pStartQueryString + 1, sizeof(queryString));

        parse_query_params(queryParams, queryString);
    }

    printf("Method: %s, Path: %s, QueryString: %s, Version: %s\n", method, path, queryString, version);
}

void parse_query_params(QueryParam *queryParams, char *queryString)
{
    char *entry = strtok(queryString, "&");

    int paramIndex = 0;

    while (entry != NULL)
    {
        if (paramIndex >= REQUEST_MAX_QUERY_PARAMS)
        {
            break;
        }

        char *pEqualSign = strchr(entry, '=');

        if (NULL == pEqualSign)
        {
            entry = strtok(NULL, "&");
            continue;
        }

        queryParams[paramIndex].key = entry; // key=value
        *pEqualSign = '\0';                  // key\0value
        queryParams[paramIndex].value = pEqualSign + 1;
        ++paramIndex;

        // NULL = continue on the previous string
        entry = strtok(NULL, "&");
    }
}

void build_response(Route *found_route, char *responseBuffer)
{
    char *status;
    char *body;

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
            "%s",
            status,
            strlen(body),
            body);

    printf("Response:\n%s\n", responseBuffer);
}

Route *find_route(Route *routes, int routes_len, char *method, char *path)
{
    for (int i = 0; i < routes_len; ++i)
    {
        Route *currentRoute = &routes[i];

        if (NULL == currentRoute->method ||
            NULL == currentRoute->path ||
            NULL == method ||
            NULL == path)
            continue;

        int matchMethod = 0 == strcmp(currentRoute->method, method);
        int matchPath = 0 == strcmp(currentRoute->path, path);

        if (matchMethod && matchPath)
        {
            return currentRoute;
        }
    }

    return NULL;
}
