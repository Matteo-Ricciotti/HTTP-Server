#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/core.h"
#include "../include/request.h"

// Define routes
const Route routes[] = {
    {"GET", "/", "200 OK", "Hello World"},
    {"GET", "/about", "200 OK", "About"},
    {"GET", "/search", "200 OK", "Searching: ${q}"},
};

int main()
{
    // Initialize server socket
    int server_fd = init_server_socket();

    if (-1 == server_fd)
    {
        return 1;
    }

    // Infinite loop to accept connections
    while (1)
    {
        // Accept a client connection
        int client_fd = accept_client_connection(server_fd);

        if (-1 == client_fd)
        {
            continue;
        }

        // Read client request
        char requestBuffer[CLIENT_BUFFER_SIZE] = "";
        int bytes_read = wait_client_data(client_fd, requestBuffer);

        if (0 >= bytes_read)
        {
            continue;
        }

        // Parse the request
        char method[REQUEST_METHOD_SIZE] = "";
        char path[REQUEST_PATH_SIZE] = "";
        char queryString[REQUEST_QUERY_STRING_SIZE] = "";
        char version[REQUEST_VERSION_SIZE] = "";
        QueryParam queryParams[REQUEST_MAX_QUERY_PARAMS] = {0};

        parse_request(requestBuffer, method, path, queryString, version, queryParams);

        // Create the response
        char responseBuffer[CLIENT_BUFFER_SIZE];

        int routes_len = sizeof(routes) / sizeof(Route);
        const Route *found_route = find_route(routes, routes_len, method, path);

        build_response(found_route, responseBuffer);

        // Send the response
        int bytes_sent = send_response(client_fd, responseBuffer);

        if (-1 == bytes_sent)
        {
            continue;
        }

        // Close client socket
        close(client_fd);

        printf("Connection to the client %d closed\n\n", client_fd);
    }

    return 0;
}