#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MESSAGE_BUFFER_CAPACITY 2000
#define STOP_NUMBER 0
#define TERMINATE_MESSAGE "0"
#define PORT 8888
#define ADDRESS "127.0.0.1"