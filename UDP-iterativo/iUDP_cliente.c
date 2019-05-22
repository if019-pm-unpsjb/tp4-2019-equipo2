/* 
 * cliente de eco iterativo sobre UDP
 * 17-07-2010
 */

#include "iUDP_cliente.h"

/*------------------------------------------------------------------------*
 * main() - conectarse al servidor, enviar un mensaje y esperar la respuesta
 *------------------------------------------------------------------------*/
main (int argc,char *argv[]) {

	int descriptor;
	struct sockaddr_in dir;

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if ( argc < 3 ) {
		printf("Uso: cliente <direccion>\n");
		printf("     Donde: <direccion> = <ip> <puerto>\n");
		exit( -1 );
	}
	
	/*--------------------------------------------------------------------* 
	 * Establecer la dirección del servidor y conectarse
	 *--------------------------------------------------------------------*/
	bzero( (char *) &dir, sizeof( dir ) );
	dir.sin_family = AF_INET;
	if ( inet_pton( AF_INET, argv[1], &dir.sin_addr ) <= 0 ) {
	    perror("Error en la función inet_pton:");
	    exit( -1 );
	}    
	dir.sin_port = htons(atoi(argv[2]));

	if ( ( descriptor = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror( "ERROR SOCKET:" );
		exit( -1 );
	}
	
	/*---------------------------------------------------------------------*
	 * Realizar la función del cliente
	 *---------------------------------------------------------------------*/
	principal( stdin, descriptor, (struct sockaddr *)&dir, sizeof(dir) );
	return( 0 );
}

/*-------------------------------------------------------------------------*
 * principal()
 *-------------------------------------------------------------------------*/ 			    
int principal( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa ) {
	int n;
	char linea_env[ MAXLINEA ], linea_rcb[ MAXLINEA + 1 ];

	while( fgets( linea_env, MAXLINEA, fp ) != NULL ) {
		sendto( sockfd, linea_env, strlen( linea_env ), 0, dir, sa);
		n = recvfrom( sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL );
		linea_rcb[ n ] = '\0';
		printf( "Recibido: %s\n", linea_rcb );
	}
}
