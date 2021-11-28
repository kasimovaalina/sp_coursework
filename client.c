#include "client.h"

int start(){
  int client_scoket_id = socket(AF_INET, SOCK_STREAM, 0);
  if (client_scoket_id == -1)  {
    puts("Could not create socket");
    return 1;
  }
  puts("Client socked successfully created");
  
  struct sockaddr_in server_socket;
  server_socket.sin_addr.s_addr = inet_addr(ADDRESS);
  server_socket.sin_family = AF_INET;
  server_socket.sin_port = htons(PORT);

  if (connect(client_scoket_id, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0) {
    perror("Connection failed. Error");
    return 1;
  }
  puts("Connection estabilished");

  int buffer_length = MESSAGE_BUFFER_CAPACITY;
  char buffer[buffer_length];

  int server_reply_number;
  int session_close_code = 0;
  int last_reply_code;
  while (1) {
    if (send_message_to_server(&client_scoket_id, buffer, &buffer_length) != 0){
       session_close_code = 1;
       break;
    }
    if ((last_reply_code = read_message_from_server(&client_scoket_id, buffer, &buffer_length, &server_reply_number)) != 0){
       if (last_reply_code < 0){
          session_close_code = 1;
       }
       break;
    }
  }

  close(client_scoket_id);
  return session_close_code;
}

int send_message_to_server(int *socket_id, char * buffer, int *buffer_length){
    memset(buffer, 0, *buffer_length);
    printf("Enter number : ");
    scanf("%s", buffer);

    if (send(*socket_id, buffer, strlen(buffer), 0) < 0) {
      puts("Failed to send message to server");
      return 1;
    }
    return 0;
} 

int read_message_from_server(int *socket_id, char *buffer, int *buffer_length, int *server_reply_number){
    memset(buffer, 0, *buffer_length);

   if (recv(*socket_id, buffer, *buffer_length, 0) < 0) {
      puts("Failed to get server reply");
      return -1;
    }
    
    sscanf(buffer, "%d", server_reply_number);
    if (*server_reply_number == 0){
      puts("Close session");
      return 1;
    }
    printf("Server reply : %d\n", *server_reply_number);
    return 0;
} 

int main(int argc, char *argv[]) { 
   return start();
}