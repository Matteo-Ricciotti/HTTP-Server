#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/core.h"

int init_server_socket()
{
    // Create the server socket and return a file descriptor
    // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = default protocol
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == server_fd)
    {
        perror("Error while creating the server socket");
        return -1;
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
        return -1;
    }

    // Make the server listen to the socket
    if (-1 == listen(server_fd, MAX_CLIENT_QUEUE))
    {
        perror("Error while listening the socket");
        return -1;
    }

    printf("Server listening on port %d\n", PORT);

    return server_fd;
}

int accept_client_connection(int server_fd)
{
    // Create the socket for a client connection
    int client_fd = accept(server_fd, NULL, NULL);

    if (-1 == client_fd)
    {
        perror("Error while accepting the client");
        return -1;
    }

    printf("Client %d connected!\n", client_fd);

    return client_fd;
}

int wait_client_data(int client_fd, char *requestBuffer)
{
    // recv() blocks the execution until some data is received
    // -1 is for '\0'
    int bytes_read = recv(client_fd, requestBuffer, CLIENT_BUFFER_SIZE, 0);

    if (-1 == bytes_read)
    {
        perror("Error while reading data from client");
        close(client_fd);
        return -1;
    }

    if (0 == bytes_read)
    {
        printf("The Client %d closed the connection\n\n", client_fd);
        close(client_fd);
        return 0;
    }

    printf("Received HTTP request:\n%s\n", requestBuffer);

    return bytes_read;
}

int send_response(int client_fd, char *responseBuffer)
{
    int bytes_sent = send(client_fd, responseBuffer, strlen(responseBuffer), 0);

    if (-1 == bytes_sent)
    {
        perror("Error while sending data to the client");
        close(client_fd);
        return -1;
    }

    return bytes_sent;
}