#ifndef CORE_H
#define CORE_H

#define PORT 8080
#define MAX_CLIENT_QUEUE 10
#define CLIENT_BUFFER_SIZE 1024

int init_server_socket();
int accept_client_connection(int server_fd);
int wait_client_data(int client_fd, char *requestBuffer);
int send_response(int client_fd, char *responseBuffer);

#endif
