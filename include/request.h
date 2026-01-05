#ifndef REQUEST_H
#define REQUEST_H

#include "core.h"

#define REQUEST_METHOD_SIZE 8
#define REQUEST_PATH_SIZE 128
#define REQUEST_QUERY_KEY_SIZE 16
#define REQUEST_QUERY_VALUE_SIZE 64
#define REQUEST_QUERY_STRING_SIZE 256
#define REQUEST_VERSION_SIZE 16
#define REQUEST_BODY_SIZE 256
#define REQUEST_MAX_QUERY_PARAMS 3

#define RESPONSE_STATUS_SIZE 24

struct Route
{
    char method[REQUEST_METHOD_SIZE];
    char path[REQUEST_PATH_SIZE];
    char status[RESPONSE_STATUS_SIZE];
    char body[REQUEST_BODY_SIZE];
} typedef Route;

struct QueryParam
{
    char key[REQUEST_QUERY_KEY_SIZE];
    char value[REQUEST_QUERY_VALUE_SIZE];
} typedef QueryParam;

int parse_request(char *requestBuffer, char *method, char *path, char *queryString, char *version, QueryParam *queryParams);
void parse_query_params(QueryParam *queryParams, char *queryString);
void build_response(Route *found_route, QueryParam *queryParams, char *responseBuffer);
Route *find_route(Route *routes, int routes_len, char *method, char *path);
QueryParam *find_param(QueryParam *QueryParams, char *body);

#endif