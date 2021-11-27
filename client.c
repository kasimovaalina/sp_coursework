#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int client_scoket_id;
  struct sockaddr_in server_socket;
  char message[1000], server_reply[2000];
  int stopSignalCheck;

  client_scoket_id = socket(AF_INET, SOCK_STREAM, 0);
  if (client_scoket_id == -1)  {
    printf("Could not create socket");
  }
  puts("Socket created");

  server_socket.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_socket.sin_family = AF_INET;
  server_socket.sin_port = htons(8888);

  if (connect(client_scoket_id, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0) {
    perror("connect failed. Error");
    return 1;
  }

  puts("Connected\n");

  while (1) {
    memset(server_reply, 0, 2000);
    printf("Enter message : ");
    scanf("%s", message);

    if (send(client_scoket_id, message, strlen(message), 0) < 0) {
      puts("Send failed");
      return 1;
    }

    if (recv(client_scoket_id, server_reply, 2000, 0) < 0) {
      puts("recv failed");
      break;
    }
    
    sscanf(server_reply, "%d", &stopSignalCheck);
    if (stopSignalCheck == 0){
      puts("End");
      close(client_scoket_id);
      return 0;
    }

    puts("Server reply :");
    puts(server_reply);
  }

  close(client_scoket_id);
  return 0;
}