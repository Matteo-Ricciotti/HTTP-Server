#ifndef REQUEST_H
#define REQUEST_H

#define REQUEST_METHOD_SIZE 8
#define REQUEST_PATH_SIZE 128
#define REQUEST_QUERY_STRING_SIZE 128
#define REQUEST_VERSION_SIZE 16
#define REQUEST_BODY_SIZE 256
#define REQUEST_MAX_QUERY_PARAMS 3

#define RESPONSE_STATUS_SIZE 24

struct Route
{
    char *method;
    char *path;
    char status[RESPONSE_STATUS_SIZE + 1];
    char body[REQUEST_BODY_SIZE + 1];
} typedef Route;

struct QueryParam
{
    char *key;
    char *value;
} typedef QueryParam;

void parse_request(char *requestBuffer, char *method, char *path, char *version, QueryParam *queryParams);
void parse_query_params(QueryParam *queryParams, char *queryString);
void build_response(Route *found_route, char *responseBuffer);
Route *find_route(Route *routes, int routes_len, char *method, char *path);

#endif