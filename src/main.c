#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/core.h"
#include "../include/request.h"

int main()
{
    // Initialize server socket
    int server_fd = init_server_socket();

    // Define routes
    Route routes[] = {
        {"GET", "/", "200 OK", "Hello World"},
        {"GET", "/about", "200 OK", "About"},
        {"GET", "/search", "200 OK", "Searching {q}..."},
    };

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
            close(client_fd);
            continue;
        }

        // Parse the request
        char method[REQUEST_METHOD_SIZE] = "";
        char path[REQUEST_PATH_SIZE] = "";
        char queryString[REQUEST_QUERY_STRING_SIZE] = "";
        char version[REQUEST_VERSION_SIZE] = "";
        QueryParam queryParams[REQUEST_MAX_QUERY_PARAMS] = {0};

        int result = parse_request(requestBuffer, method, path, queryString, version, queryParams);

        if (-1 == result)
        {
            close(client_fd);
            continue;
        }

        // Create the response
        char responseBuffer[CLIENT_BUFFER_SIZE];

        int routes_len = sizeof(routes) / sizeof(Route);
        Route *found_route = find_route((Route *)routes, routes_len, method, path);

        build_response(found_route, queryParams, responseBuffer);

        // Send the response
        int bytes_sent = send_response(client_fd, responseBuffer);

        if (-1 == bytes_sent)
        {
            close(client_fd);
            continue;
        }

        // Close client socket
        close(client_fd);

        printf("Connection to the client %d closed\n\n", client_fd);
    }

    return 0;
}