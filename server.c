#include "server.h"

int start(){
  
  struct socket_data socket_info;

  if (prepare_server_socket(&socket_info) == 0){
    return 1;
  }

  int server_socket_structure_size = sizeof(struct sockaddr_in);
  pid_t child_pid;

  while (1) {
    puts("Waiting for incoming connections...");
    int client_socket_id = accept(socket_info.server_socket_id, (struct sockaddr *)&socket_info.client, (socklen_t *)&server_socket_structure_size);

    if (client_socket_id < 0) {
      perror("Accept failed");
      return 1;
    }

    printf("Connection accepted new client socket id: %d \n", client_socket_id);  
    if ((child_pid = fork()) == 0) {
      int read_size;
      int sum = 0;
      int client_active = 1;
      char message_buffer[MESSAGE_BUFFER_CAPACITY];
      while ((read_size = recv(client_socket_id, message_buffer, MESSAGE_BUFFER_CAPACITY, 0)) > 0){
          int client_number;
          sscanf(message_buffer, "%d", &client_number);          
          if (client_number != STOP_NUMBER) {
              printf("Сlient with id %d sent number: %d \n", client_socket_id, client_number);  
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
              break;
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
  close(socket_info.server_socket_id); 
  exit(0);
}

int prepare_server_socket(struct socket_data *prepared_socket){
  (*prepared_socket).server_socket_id = socket(AF_INET, SOCK_STREAM, 0);
  if ((*prepared_socket).server_socket_id == -1) {
    printf("Could not create a socket");
  }
  puts("Socket created");

  //(*prepared_socket).client;
  (*prepared_socket).server.sin_family = AF_INET;
  (*prepared_socket).server.sin_addr.s_addr = INADDR_ANY;
  (*prepared_socket).server.sin_port = htons(PORT);

  if (bind((*prepared_socket).server_socket_id, (struct sockaddr *)&(*prepared_socket).server, sizeof((*prepared_socket).server)) < 0) {
    perror("Bind failed. Error");
    return 0;
  }
  puts("Bind succeded");

  listen((*prepared_socket).server_socket_id, 3);
  
  return 1;
}


int main(int argc, char *argv[])
{
  // // prepare server socket
  // int server_socket_id = socket(AF_INET, SOCK_STREAM, 0);
  // if (server_socket_id == -1) {
  //   printf("Could not create a socket");
  // }
  // puts("Socket created");

  // struct sockaddr_in server, client;
  // server.sin_family = AF_INET;
  // server.sin_addr.s_addr = INADDR_ANY;
  // server.sin_port = htons(PORT);

  // if (bind(server_socket_id, (struct sockaddr *)&server, sizeof(server)) < 0) {
  //   perror("Bind failed. Error");
  //   return 1;
  // }
  // puts("Bind succeded");

  // listen(server_socket_id, 3);
  
  // int server_socket_structure_size = sizeof(struct sockaddr_in);
  // pid_t child_pid;
  // while (1) {
  //   puts("Waiting for incoming connections...");
  //   int client_socket_id = accept(server_socket_id, (struct sockaddr *)&client, (socklen_t *)&server_socket_structure_size);
  //   if (client_socket_id < 0) {
  //     perror("Accept failed");
  //     return 1;
  //   }
  //   printf("Connection accepted new client socket id: %d \n", client_socket_id);  
  //   if ((child_pid = fork()) == 0) {
  //     int read_size;
  //     int sum = 0;
  //     int client_active = 1;
  //     char message_buffer[MESSAGE_BUFFER_CAPACITY];
  //     while ((read_size = recv(client_socket_id, message_buffer, MESSAGE_BUFFER_CAPACITY, 0)) > 0){
  //         int client_number;
  //         sscanf(message_buffer, "%d", &client_number);          
  //         if (client_number != STOP_NUMBER) {
  //             printf("Сlient with id %d sent number: %d \n", client_socket_id, client_number);  
  //             sum = sum + client_number;
  //             char string_buffer[100]; // output string buffer
  //             sprintf(string_buffer, "%d", sum);
  //             write(client_socket_id, string_buffer, strlen(string_buffer));
  //             memset(message_buffer, 0, MESSAGE_BUFFER_CAPACITY);
  //         } else {
  //             // finish session
  //             memset(message_buffer, 0, MESSAGE_BUFFER_CAPACITY);
  //             puts("Finish work");
  //             write(client_socket_id, TERMINATE_MESSAGE, strlen(TERMINATE_MESSAGE));
  //             client_active = 0;
  //             break;
  //         }
  //     }
  //     if (client_active){
  //         puts("Client finished session");
  //         fflush(stdout);
  //     } else if (read_size == 0){
  //         puts("Client disconnected");
  //         fflush(stdout);
  //     }
  //     else if (read_size == -1) {
  //         perror("recv command failed");
  //     }
  //     close(client_socket_id);
  //   }
  // }
  // close(server_socket_id); 
  // exit(0);
  start();
}