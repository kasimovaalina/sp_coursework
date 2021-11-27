#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
  struct sockaddr_in server, client;
  int MESSAGE_BUFFER_CAPACITY = 2000;
  char message_buffer[MESSAGE_BUFFER_CAPACITY];
  pid_t child_pid;
  char TERMINATE_MESSAGE[1] = "0";
  int STOP_NUMBER = 0;
  int server_socket_id = socket(AF_INET, SOCK_STREAM, 0);

  if (server_socket_id == -1)
  {
    printf("Could not create socket");
  }
  puts("Socket created");

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(8888);

  if (bind(server_socket_id, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("Bind failed. Error");
    return 1;
  }
  puts("Bind succeded");

  listen(server_socket_id, 3);
  puts("Waiting for incoming connections...");
  int server_socket_structure_size = sizeof(struct sockaddr_in);
  
  while (1) {
    int client_socket_id = accept(server_socket_id, (struct sockaddr *)&client, (socklen_t *)&server_socket_structure_size);
    if (client_socket_id < 0) {
      perror("Accept failed");
      return 1;
    }
    puts("Connection accepted");
    close(server_socket_id);
    if ((child_pid = fork()) == 0) {
      int read_size;
      int sum = 0;
      int client_active = 1;
      while (client_active && (read_size = recv(client_socket_id, message_buffer, MESSAGE_BUFFER_CAPACITY, 0)) > 0){
          int client_number;
          sscanf(message_buffer, "%d", &client_number);          
          if (client_number != STOP_NUMBER) {
              sum = sum + client_number;
              char string_buffer[100]; // output string buffer
              sprintf(string_buffer, "%d", sum);
              write(client_socket_id, string_buffer, strlen(string_buffer));
              memset(message_buffer, 0, MESSAGE_BUFFER_CAPACITY);
          } else {
              // finish session
              memset(message_buffer, 0, MESSAGE_BUFFER_CAPACITY);
              puts("Finish work");
              write(client_socket_id, TERMINATE_MESSAGE, strlen(TERMINATE_MESSAGE));
              client_active = 0;
          }
      }
      if (client_active){
          puts("Client finished session");
          fflush(stdout);
      } else if (read_size == 0){
          puts("Client disconnected");
          fflush(stdout);
      }
      else if (read_size == -1) {
          perror("recv command failed");
      }
      close(client_socket_id);
    }
  } 
  exit(0);
}