#include "commons.h"

struct socket_data{
    int server_socket_id;
    struct sockaddr_in server;
    struct sockaddr_in client;
};

int initialize_sockets(struct socket_data *prepared_socket, int port);

void handle_message(int *read_size, int *client_socket_id, int *client_active, int message_buffer_capacity);