#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define REQUEST_METHOD_SIZE 8
#define REQUEST_PATH_SIZE 256
#define REQUEST_VERSION_SIZE 16
#define REQUEST_BODY_SIZE 256
#define MAX_CLIENT_QUEUE 10

int main()
{
    // Create the server socket and return a file descriptor
    // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = default protocol
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == server_fd)
    {
        perror("Error while creating the socket");
        return 1;
    }

    // Create the address structure for the binding
    struct sockaddr_in serverAddress;
    // Start filling the memory at address <param1>, with value, <param2>, for <param3> bytes
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the socket to the IP + port
    if (-1 == bind(server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))
    {
        perror("Error while binding the socket to the IP + port");
        return 1;
    }

    // Make the server listen to the socket
    // 10 is the max connections queue size
    if (-1 == listen(server_fd, MAX_CLIENT_QUEUE))
    {
        perror("Error while listening the socket");
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    // Infinte loop to accept connections
    while (1)
    {
        // Create the socket of a client connection
        int client_fd = accept(server_fd, NULL, NULL);

        if (-1 == client_fd)
        {
            perror("Error while accepting the client");
            continue;
        }

        printf("Client %d connected!\n", client_fd);

        // Read client data
        char requestBuffer[BUFFER_SIZE] = "";

        // recv() blocks the execution until some data is received
        // -1 is for '\0'
        int bytes_read = recv(client_fd, requestBuffer, BUFFER_SIZE - 1, 0);

        if (-1 == bytes_read)
        {
            perror("Error while reading data from client");
            close(client_fd);
            continue;
        }

        if (0 == bytes_read)
        {
            printf("The Client %d closed the connection\n\n", client_fd);
            close(client_fd);
            continue;
        }

        printf("Received HTTP request:\n%s\n", requestBuffer);

        // Parse the request elements
        char method[REQUEST_METHOD_SIZE] = "";
        char path[REQUEST_PATH_SIZE] = "";
        char version[REQUEST_VERSION_SIZE] = "";

        sscanf(requestBuffer, "%s %s %s", method, path, version);

        printf("Method: %s, Path: %s, Version: %s\n", method, path, version);

        // Create the formatted response
        char responseBuffer[BUFFER_SIZE];

        char *responseMessage = "200 OK";
        char *body;

        if (0 == strcmp("/", path))
        {
            body = "Hello World";
        }
        else if (0 == strcmp("/about", path))
        {
            body = "About";
        }
        else
        {
            responseMessage = "404 Not Found";
            body = "Not Found";
        }

        sprintf(responseBuffer,
                "HTTP/1.1 %s\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: %ld\r\n"
                "\r\n"
                "%s",
                responseMessage,
                strlen(body),
                body);

        printf("Response:\n%s\n", responseBuffer);

        // Send the response
        int bytes_sent = send(client_fd, responseBuffer, strlen(responseBuffer), 0);

        if (-1 == bytes_sent)
        {
            perror("Error while sending data to the client");
            close(client_fd);
            continue;
        }

        // Close client socket
        close(client_fd);

        printf("Connection to the client %d closed\n\n", client_fd);
    }

    // Close the server socket (will not be reached)
    close(server_fd);

    return 0;
}