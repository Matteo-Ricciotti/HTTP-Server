#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/request.h"

void parse_request(char *requestBuffer, char *method, char *path, char *version, QueryParam *queryParams)
{
    sscanf(requestBuffer, "%s %s %s", method, path, version);

    printf("Method: %s, Path: %s, Version: %s\n", method, path, version);
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

QueryParam *parse_query_params(QueryParam *queryParams, char *queryString)
{
    return NULL;
}