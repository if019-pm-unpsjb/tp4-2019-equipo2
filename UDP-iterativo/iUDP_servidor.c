/*
 * Servidor de eco iterativo sobre UDP
 */
 
#include "iUDP_servidor.h"

void main ( int argc, char *argv[] ) {
	int descriptor;
	struct sockaddr_in dir_srv, dir_cli;
  
	/*---------------------------------------------------------------------*
	 * Verificar los argumentos recibidos								   *
	 *---------------------------------------------------------------------*/
 	if ( argc < 2 ) {
		printf( "Uso: servidor <puerto>\n" );
		exit(-1);
	}

 	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor                               			  *
	 *--------------------------------------------------------------------*/
	if ( ( descriptor = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror("ERROR SOCKET: ");
		exit(-1);
	}	
	
	bzero( (char *) &dir_srv, sizeof( dir_srv ) );
	dir_srv.sin_family = AF_INET;
	dir_srv.sin_addr.s_addr = htonl( INADDR_ANY );
	dir_srv.sin_port = htons( atoi( argv[1] ) );

	if ( bind( descriptor, (struct sockaddr *) &dir_srv, sizeof( dir_srv ) ) < 0 ) {
		perror("ERROR BIND: ");
		exit (-1);
	}
	printf( "Servidor inicializado.\n\n" );
	
	procesar( descriptor, (struct sockaddr *) &dir_cli, sizeof( dir_cli ) );
	
}

/*-----------------------------------------------------------------------* 
 * procesar() - realizar la tarea específica del servidor
 *-----------------------------------------------------------------------*/
void procesar( int descriptor, struct sockaddr *dir_cli_p, socklen_t longcli ) {
	int recibido;
	socklen_t longitud;
	char msg[ MAXLINEA ];

	for(;;) {
	 longitud = longcli;
	 recibido = recvfrom( descriptor, msg, MAXLINEA, 0, dir_cli_p, &longitud );
	 sendto( descriptor, msg, recibido, 0, dir_cli_p, longitud );
	}
}
