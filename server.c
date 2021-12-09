#include "server.h"

int start(){
  
  struct socket_data socket_info;

  if (initialize_sockets(&socket_info, PORT) == 0){
    return 1;
  }

  int server_socket_structure_size = sizeof(struct sockaddr_in);
  pid_t child_pid;

  void* shmem = create_shared_memory(64);
  int initial_value = 0;
  memcpy(shmem, &initial_value, sizeof(int));
  while (1) {
    puts("Waiting for incoming connections...");
    int client_socket_id = accept(socket_info.server_socket_id, 
                                  (struct sockaddr *)&socket_info.client, 
                                  (socklen_t *)&server_socket_structure_size);

    if (client_socket_id < 0) {
      perror("Accept failed");
      return 1;
    }

    printf("Connection accepted new client socket id: %d \n", client_socket_id);  

    if ((child_pid = fork()) == 0) {
      
      int read_size;
      int client_active = 1;

      handle_message(&read_size, &client_socket_id, &client_active, MESSAGE_BUFFER_CAPACITY, shmem);

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

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable
  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANONYMOUS;
  return mmap(NULL, size, protection, visibility, -1, 0);
}

int initialize_sockets(struct socket_data *prepared_socket, int port){
  (*prepared_socket).server_socket_id = socket(AF_INET, SOCK_STREAM, 0);
  if ((*prepared_socket).server_socket_id == -1) {
    printf("Could not create a socket");
  }
  puts("Socket created");

  (*prepared_socket).server.sin_family = AF_INET;
  (*prepared_socket).server.sin_addr.s_addr = INADDR_ANY;
  (*prepared_socket).server.sin_port = htons(port);

  if (bind((*prepared_socket).server_socket_id, (struct sockaddr *)&(*prepared_socket).server, sizeof((*prepared_socket).server)) < 0) {
    perror("Bind failed. Error");
    return 0;
  }
  puts("Bind succeded");

  listen((*prepared_socket).server_socket_id, 3);
  
  return 1;
}

void handle_message(int *read_size, int *client_socket_id, int *client_active, int message_buffer_capacity, void* shared_memory){
      char message_buffer[message_buffer_capacity];
      while (((*read_size) = recv((*client_socket_id), message_buffer, message_buffer_capacity, 0)) > 0){
        int client_number;
        sscanf(message_buffer, "%d", &client_number);          
        if (client_number != STOP_NUMBER) {
            int total = -1;
            puts("Try to read total value from shared mem");
            memcpy(&total, shared_memory, sizeof(int));
            printf("Сlient with id %d sent number: %d \n", (*client_socket_id), client_number);  
            total += client_number;
            memset(message_buffer, 0, message_buffer_capacity);
            sprintf(message_buffer, "%d", total);
            write((*client_socket_id), message_buffer, strlen(message_buffer));
            printf("Update total value with %d\n", total);
            memcpy(shared_memory, &total, sizeof(int));
            puts("Clean buffer");
            memset(message_buffer, 0, message_buffer_capacity);
        } else {
            // finish session
            memset(message_buffer, 0, message_buffer_capacity);
            printf("Client %d finished session\n", (*client_socket_id));
            write((*client_socket_id), TERMINATE_MESSAGE, strlen(TERMINATE_MESSAGE));
            (*client_active) = 0;
            break;
        }
    }
}


int main(int argc, char *argv[]){
  start();
}