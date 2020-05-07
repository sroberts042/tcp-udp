#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>


// Globals
const char *g_szMessage;
const char *g_szServer;
unsigned short g_usPort;


// Function Prototypes
void parse_args(int argc, char **argv);


// Function Implementations

int main(int argc, char **argv)
{
  parse_args(argc, argv);
  printf("Sending message \"%s\" to %s:%hu\n",
         g_szMessage, g_szServer, g_usPort);

  //create socket and check for error
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) 
  {
    printf("ERROR: client socket creation failed\n");
    exit(-1);
  }
  else
  {
    printf("SUCCESS: client socket created\n");
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  inet_aton(g_szServer, &addr.sin_addr);
  addr.sin_port = htons(g_usPort);

  //create connection and check for error
  int connection = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
  if(connection < 0)
  {
    printf("ERROR: connection failed\n");
    printf("%d\n", errno);
    close(sock);
    exit(-1);
  }
  else 
  {
    printf("SUCCESS: connection established\n");
  }

  //send message and check for error
  int message, i;
  for(i = 0; i < 1024; i++)
  {
    message = send(sock, g_szMessage + i, sizeof(*g_szMessage), 0); 
  }
  if(message < 0)
  {
    printf("ERROR: message not sent\n");
    close(sock);
    exit(-1);
  }
  else 
  {
    printf("SUCCESS: message sent\n");
  }

  //close connection
  close(sock);
  return 0;
}


void parse_args(int argc, char **argv)
{
  if (argc < 4)
  {
    fprintf(stderr,
            "Usage: tcp_client SERVER PORT \"MESSAGE\"\n");
    exit(1);
  }

  errno = 0;
  char *endptr = NULL;
  unsigned long ulPort = strtoul(argv[2], &endptr, 10);

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
            argv[2], strerror(errno));
    abort();
  }
  g_usPort = ulPort;

  g_szServer = argv[1];
  g_szMessage = argv[3];
}
