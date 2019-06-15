#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>


#define SERVER_PORT_ID 50002
#define SERVER_UDP_PORT_ID 40002
#define SERVER_HOST_ADDR "127.0.0.1"
#define MAXLINEA 100
#define MAXCLI   10
#define MAXSIZE 512
#define ACK 2
#define NACK 3
#define REQUESTFILE 100
#define COMMANDNOTSUPPORTED 150
#define COMMANDSUPPORTED 160
#define BADFILENAME 200
#define FILENAMEOK 400

int readn(int sd, char *ptr, int size);
int writen(int sd, char *ptr, int size);