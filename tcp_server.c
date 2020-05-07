#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>


// Globals
unsigned short g_usPort;


// Function Prototypes
void parse_args(int argc, char **argv);


// Function Implementations

int main(int argc, char **argv)
{
  parse_args(argc, argv);
  printf("Starting TCP server on port: %hu\n", g_usPort);

  //set up connection and check for error
  int server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(server_sock < 0)
  {
    printf("ERROR: server socket creation failed\n");
    exit(-1);
  }
  else
  {
    printf("SUCCESS: server socket created\n");
  }

  int client_sock;
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(g_usPort);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind the socket and check for error
  int bound_socket = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if(bound_socket < 0)
  {
    printf("ERROR: server socket binding failed\n");
    close(client_sock);
    exit(-1);
  }
  else
  {
    printf("SUCCESS: server socket binding complete\n");
  }

  //listen for connection request and check for error
  int listening = listen(server_sock, 10);
  if(listening < 0)
  {
    printf("ERROR: no socket found\n");
    close(client_sock);
    exit(-1);
  }
  else
  {
    printf("SUCCESS: socket found\n");
  }

  //reveive message
  char* buff = malloc(1024);
  int received_message;

  while(1)
  {
    client_sock = accept(server_sock, (struct sockaddr*)NULL, NULL);

    //get message
    received_message = recv(client_sock, buff, 1024, 0);
    while(received_message < 1024)
    {
      received_message += recv(client_sock, buff + received_message, 1024, 0);
    }

    //check for error in received_message
    if(received_message < 0)
    {
      printf("ERROR: no message received\n");
      exit(-1);
    }

    //print string
    printf("%s\n", buff);
  }

  //close connection
  close(server_sock);
  close(client_sock);
  return 0;
}


void parse_args(int argc, char **argv)
{
  if (argc < 2)
  {
    fprintf(stderr,
            "tcp_server: missing port operand\nUsage: tcp_server PORT\n");
    exit(1);
  }

  errno = 0;
  char *endptr = NULL;
  unsigned long ulPort = strtoul(argv[1], &endptr, 10);

  if (0 == errno)
  {
    // If no other error, check for invalid input and range
    if ('\0' != endptr[0])
      errno = EINVAL;
    else if (ulPort > USHRT_MAX)
      errno = ERANGE;
  }
  if (0 != errno)
  {
    // Report any errors and abort
    fprintf(stderr, "Failed to parse port number \"%s\": %s\n",
            argv[1], strerror(errno));
    abort();
  }
  g_usPort = ulPort;
}
