#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    // 1. Create the server socket and return a file descriptor
    // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = default protocol
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == server_fd)
    {
        perror("Error while creating the socket");
        return 1;
    }

    // 2. Create the address structure for the binding
    struct sockaddr_in serverAddress;
    // Start filling the memory at address <param1>, with value, <param2>, for <param3> bytes
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // 3. Bind the socket to the IP + port
    if (-1 == bind(server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))
    {
        perror("Error while binding the socket to the IP + port");
        return 1;
    }

    // 4. Make the server listen to the socket
    // 10 is the max connections queue size
    if (-1 == listen(server_fd, 10))
    {
        perror("Error while listening the socket");
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    // 5. Infinte loop to accept connections
    while (1)
    {
        // 6. Create the socket of a client connection
        int client_fd = accept(server_fd, NULL, NULL);

        if (-1 == client_fd)
        {
            perror("Error while accepting the client");
            continue;
        }

        printf("Client %d connected!\n", client_fd);

        // 7. Read client data
        char buffer[BUFFER_SIZE] = "";

        // recv() blocks the execution until some data get received
        // -1 is for '\0'
        int bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

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

        printf("Received: %s\n", buffer);

        // 8. Echo
        int bytes_sent = send(client_fd, buffer, bytes_read, 0);

        if (-1 == bytes_sent)
        {
            perror("Error while sending data to the client");
            close(client_fd);
            continue;
        }

        // 9. Close client socket
        close(client_fd);

        printf("Connection to the client %d closed\n\n", client_fd);
    }

    // 10. close the server socket (will not be reached)
    close(server_fd);

    return 0;
}