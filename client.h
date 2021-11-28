#include "commons.h"

int send_message_to_server(int *socket_id, char *buffer, int *buffer_length);

//
// return codes:
// 0 code is OK
// -1 broken connection 
// 1 graceful shutdown
//
int read_message_from_server(int *socket_id, char *buffer, int *buffer_length, int *server_reply_number);