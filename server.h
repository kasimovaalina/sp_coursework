#include "commons.h"

struct socket_data{
    int server_socket_id;
    struct sockaddr_in server;
    struct sockaddr_in client;
};

int prepare_server_socket(struct socket_data *prepared_socket);