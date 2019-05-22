/* 
 * Cliente de eco iterativo sobre TCP
 */
 
#include "iTCP_cliente.h"

/*------------------------------------------------------------------------
 * main() - conectarse, enviar un mensaje y esperar la respuesta.
 *------------------------------------------------------------------------*/
main ( int argc, char *argv[] ) {

	int descriptor;
	struct sockaddr_in dir;

	/*---------------------------------------------------------------------
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if ( argc < 3 ) {
		printf( "Uso: cliente <direccion>\n" );
		printf( "     Donde: <direccion> = <ip> <puerto>\n" );
		exit( -1 );
	}

	printf( "\n\tCliente de eco iterativo sobre TCP. Pulse <Ctrl>-D para salir.\n" );
	
	/*-------------------------------------------------------------------- 
	 * Establecer la dirección del servidor y conectarse
	 *--------------------------------------------------------------------*/
	bzero( (char *) &dir, sizeof( dir ) );
	dir.sin_family = AF_INET;
	if ( inet_pton( AF_INET, argv[1], &dir.sin_addr ) <= 0 ) {
	    perror( "inet_pton" );
	    exit( -1 );
	}    
	dir.sin_port = htons( atoi( argv[2] ) );

	if ( ( descriptor = conectar( dir ) ) < 0 ) {
		perror( "ERROR CONECTAR:" );
		exit( -1 );
	}
	/*---------------------------------------------------------------------
	 * Realizar la función del cliente 
	 *---------------------------------------------------------------------*/
	principal( stdin, descriptor );
	
	/*---------------------------------------------------------------------
	 * Cerrar la conexión y terminar
	 *---------------------------------------------------------------------*/
	cerrar( descriptor );
	printf( "Proceso cliente finalizado.\n" );
	exit( 0 );
}

/*-------------------------------------------------------------------------
 * conectar() 
 *-------------------------------------------------------------------------*/ 	    
int conectar( struct sockaddr_in dir ) {
	int sockfd;
	
	/* abrir el socket */
	if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		return ( -1 );
	
	/* conectarse al servidor */
	if ( connect( sockfd, (struct sockaddr *) &dir, sizeof( dir ) ) < 0 ) 
		return ( -2 );
	
	return ( sockfd );
}

/*-------------------------------------------------------------------------*
 * enviar() 
 *-------------------------------------------------------------------------*/ 	    
int enviar( int desc_con, char *msg, int len ) {
	int resultado;
	
	if ( ( resultado = write( desc_con, msg, len ) ) < 0 )
	    return (-1);
	
	return ( resultado );
}

/*-------------------------------------------------------------------------*
 * recibir() 
 *-------------------------------------------------------------------------*/ 	    
int recibir( int desc_con, char *linea, int len ) {
	int recibido;
	if ( ( recibido = read( desc_con, linea, len ) ) < 0 )
		return ( -1 ); 
	
	return ( recibido );
}

/*-------------------------------------------------------------------------*
 * cerrar() 
 *-------------------------------------------------------------------------*/ 			    
void cerrar ( int desc_con ) {
	close( desc_con );
}

/*-------------------------------------------------------------------------*
 * principal()
 *-------------------------------------------------------------------------*/ 			    
int principal( FILE *fp, int sockfd ) {
	int resultado, total;
	char msg[ MAXLINEA ], msg2[ MAXLINEA + 1 ];

	while( fgets( msg, MAXLINEA, fp ) != NULL ) {
		msg[ strlen( msg ) -1 ] = '\0';
		/*---------------------------------------------------------------------
		 * Enviar el mensaje 
		 *---------------------------------------------------------------------*/
		if ( ( resultado = enviar( sockfd, msg, strlen( msg ) ) ) < 0 ) {
			perror("ERROR ENVIAR: ");
			exit(-1);
		}
		printf( "\tTexto enviado: %s\n", msg );
	
		/*---------------------------------------------------------------------
		 * Esperar respuesta del servidor
		 *---------------------------------------------------------------------*/
		if ( ( total = recibir( sockfd, msg2, MAXLINEA ) ) < 0 ) {
			perror( "ERROR RECIBIR: " );
			exit( -1 );
		}

		/*---------------------------------------------------------------------
		 * Mostrar el resultado
		 *---------------------------------------------------------------------*/
		msg2[ total ]='\0';
		printf( "\tTexto recibido: %s\n", msg2 );  
	}
}
