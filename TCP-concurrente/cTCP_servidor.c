/*
 * Servidor de eco concurrente sobre TCP
 */

#include "cTCP_servidor.h"
#include <string.h>	

main ( int argc, char *argv[] ) {
	char *msg;
	int descriptor, ndescriptor, total, pid_hijo;
	
	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if ( argc < 2 ) {
		printf( "Uso: servidor <puerto>\n" );
		exit( -1 );
	}
	printf( "\tServidor de eco concurrente sobre TCP. Para salir presione <Ctrl>-C\n" );
	
	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor
	 *--------------------------------------------------------------------*/
	/* Tomar el puerto donde va a atender
	*/ 
	if ( ( descriptor = inicializar( atoi( argv[1] ) ) ) < 0 ) {
		perror( "ERROR INICIALIZAR: " );
		exit(-1);
	}	
	printf( "Servidor inicializado.\n" );

	msg = (char*)malloc( sizeof( MAXLINEA + 1 ) );

	/*
	 * Comenzar tarea del servidor
	 */
	while ( 1 ) { 
		/*----------------------------------------------------------------* 
	 	 * Esperar una petición
	 	 *----------------------------------------------------------------*/
		printf( "Esperando petición (Padre).\n" );
		if ( ( ndescriptor = esperar( descriptor ) ) < 0 ) {
			perror("ERROR ESPERANDO: ");
			exit(-1);
		}
		
		/*-------------------------------------------------------*  
		 * Realizar fork del proceso para atender una petición 
		 * MODIFICAR PARA RECIBIR SEGUN EL COMANDO QUE LLEGUE
		 *-------------------------------------------------------*/
		if ( ( pid_hijo = fork() ) < 0 ) {
			perror("ERROR FORK:");
			exit(-1);
   		} else if ( pid_hijo == 0 ) { /* soy el hijo */
			printf( "(%5d) Atendiendo petición.\n", getpid() );
			
   			while ( total = recibir( ndescriptor, msg ) > 0 ) {
				printf( "(%5d) Recibido: %s\n", getpid(), msg );
		
				/*-------------------------------------------------------* 
		 	 	 * Realizar la tarea específica del servicio
				 * MODIFICAR PARA PROCESAR SEGUN EL COMANDO QUE LLEGUE
		 	 	 *-------------------------------------------------------*/
				procesar( msg );
	
				/*-------------------------------------------------------* 
	 			 * Responder petición	
				 * MODIFICAR ENVIAR PARA RESPONDER SEGUN EL COMANDO QUE LLEGUE	      					
				 *-------------------------------------------------------*/
				if ( ( total = enviar( ndescriptor, msg ) ) < 0 ) { 
					perror("ERROR ENVIAR: ");
					exit(-1);
				}
				printf("(%5d) Respuesta enviada: %s.\n", getpid(), msg );
			}
			
			/*-------------------------------------------------------* 
			 * Cerrar conexión y terminar
			 *-------------------------------------------------------*/
			close( ndescriptor );
			printf("(%5d) Conexión cerrada.\n", getpid() );
			exit( 0 );
		} else /* soy el padre */
			printf( "Solicitud atendida por el proceso hijo nro. %d\n", pid_hijo );
	}
}

/*-------------------------------------------------------------------------*
 * inicializar() - inicializar el servidor
 *-------------------------------------------------------------------------*/ 	    
int inicializar( int puerto ) {
	int sockfd;
	struct sockaddr_in dir_srv;
	
	if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		return ( -1 );
	
	/* bind de la dirección local */
	bzero( (char *) &dir_srv, sizeof( dir_srv ) );
	dir_srv.sin_family = AF_INET;	/* utilizará familia de protocolos de Internet */
	dir_srv.sin_addr.s_addr = htonl( INADDR_ANY );		/* sobre la dirección IP local */
	dir_srv.sin_port = htons( puerto );

	if ( bind( sockfd, (struct sockaddr *) &dir_srv, sizeof( dir_srv ) ) < 0 )
		return ( -2 );
	
	/* armar una lista de espera para MAXCLI clientes */	
	listen( sockfd, MAXCLI );
	
	/* mostrar un mensaje con los datos del socket de escucha.
	 * Sólo para ver qué está haciendo. Esta función no debería mostrar salida por pantalla.
	 */
	printf ( "\n\tEscuchando en puerto: %d, dirección %s. \n\tPodría tener hasta %d clientes esperando.\n", 
		ntohs( dir_srv.sin_port ), ntohl( dir_srv.sin_addr.s_addr ), MAXCLI );
	
	return ( sockfd );
}

/*-------------------------------------------------------------------------*
 * esperar()
 *-------------------------------------------------------------------------*/ 	    
int esperar( int sockfd ) {
	int nsockfd;

	/* aceptar una conexión */
	if ( ( nsockfd = accept( sockfd, 0, 0 ) ) < 0 )
		return ( -1 );
	
	return nsockfd;
}
		
/*-------------------------------------------------------------------------*
 * enviar() - enviar la respuesta al cliente 
 * retornar la longitud de la cadena enviada
 * MODIFICAR ENVIAR PARA RESPONDER SEGUN EL COMANDO EJECUTADO
 *-------------------------------------------------------------------------*/ 	    
int enviar( int nsockfd, char *msg ) {
	int longitud;
	
	if ( ( longitud = write( nsockfd, msg, strlen( msg ) ) ) < 0 ) {
		close( nsockfd );
		return ( -1 );
	}

	return longitud;
}

/*------------------------------------------------------------------------*
 * recibir() - recibir una petición
 * retornar la longitud de la cadena leída
 * MODIFICAR RECIBIR PARA RECIBIR UN NUEVO COMANDO
 *------------------------------------------------------------------------*/ 	    
int recibir( int nsockfd, char *msg ) {
	int longitud;

	bzero( msg, MAXLINEA );

	if ( ( longitud =  read( nsockfd, msg, (int)MAXLINEA ) ) < 0 ) {
		close( nsockfd );
		return ( -2 );
	}

	//char str[80] = "This is - www.tutorialspoint.com - website";
   	const char s[2] = " ";
   	char *token;
   
   	/* get the first token */
 	token = strtok(msg, s);
   
   	/* walk through other tokens */
 	while( token != NULL ) {
		printf( " %s\n", token );
		token = strtok(NULL, s);
   }
   
   //return(0);

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
	
 /* 	if ( argc < 2 ) {
		printf( "Uso: servidor <puerto>\n" );
		exit( -1 );
	} */

	return ( longitud );
}

/*-----------------------------------------------------------------------* 
 * procesar() - atender una petición
 * MODIFICAR PARA PROCESAR LA ACCION CORRESPONDIENTE A UN COMANDO
 *-----------------------------------------------------------------------*/
void procesar( char *mensaje ) {
	printf ( "(%5d) Procesando: %s \n", getpid(), mensaje );
}




