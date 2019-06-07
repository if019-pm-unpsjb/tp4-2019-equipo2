/*
 * Servidor de eco concurrente sobre TCP
 */

#include "cTCP_servidor.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_info {    /* Used as argument to thread_start() */
	pthread_t thread_id;        /* ID returned by pthread_create() */
	int       thread_num;       /* Application-defined thread # */
	char     *argv_string;      /* From command-line argument */
};

struct thread_args {	/* Structure for thread arguments */
       int    sock;
       struct sockaddr_in dir_srv;
};

/* Thread start function: display address near top of our stack,
	and return upper-cased copy of argv_string */

static void *
udp_sub_thread(void *arg)
{
	// Thread encargado de resolver una solicitud UDP

	return 1;
}

static void *
udp_thread_start(void *arg)
{
	struct thread_info *tinfo = arg;
	char *uargv, *p;

	// For de threads para escuchar por udp

	// printf("Thread %d: top of stack near %p; argv_string=%s\n",
	// 		tinfo->thread_num, &p, tinfo->argv_string);

	// uargv = strdup(tinfo->argv_string);
	// if (uargv == NULL)
	// 	handle_error("strdup");

	// for (p = uargv; *p != '\0'; p++)
	// 	*p = toupper(*p);

	return uargv;
}

static void *
tcp_sub_thread(void *arg)
{
	int ndescriptor = (int) arg;
	char *msg;
	int total;

	printf("me activaron con ndescriptor %d \n\n", ndescriptor);

	// Thread encargado de resolver una solicitud TCP
	printf( "(%5d) Atendiendo petición.\n", getpid() );

	msg = (char*)malloc( sizeof( MAXLINEA + 1 ) );
			
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

	return ndescriptor;
}

static void *
tcp_thread_start(void *_args)
{
	struct thread_args *args = (struct thread_args *) _args;
	char *msg;
	int descriptor = args->sock;
	int ndescriptor, s;

	printf("me iniciaron con sock %d", descriptor);

	/* armar una lista de espera para MAXCLI clientes */	
	listen( descriptor, MAXCLI );
	
	/* mostrar un mensaje con los datos del socket de escucha.
	 * Sólo para ver qué está haciendo. Esta función no debería mostrar salida por pantalla.
	 */
	printf ( "\n\tEscuchando en puerto: %d, dirección %d. \n\tPodría tener hasta %d clientes esperando.\n", 
	 	ntohs( args->dir_srv.sin_port ), ntohl( args->dir_srv.sin_addr.s_addr ), MAXCLI );
		
	printf( "Servidor TCP inicializado.\n" );

	/*
	 * Comenzar tarea del servidor
	 */
	while ( 1 ) { 
		/*----------------------------------------------------------------* 
	 	 * Esperar una petición
	 	 *----------------------------------------------------------------*/
		printf( "Esperando petición (Padre).\n" );
		if ( ( ndescriptor = esperar( args->sock ) ) < 0 ) {
			perror("ERROR ESPERANDO: ");
			exit(-1);
		}
		
		/*-------------------------------------------------------*  
		 * Realizar fork del proceso para atender una petición 
		 * MODIFICAR PARA RECIBIR SEGUN EL COMANDO QUE LLEGUE
		 *-------------------------------------------------------*/

		// Implementar un thread con un for que espere conexiones UTP y les asigne un tread
		pthread_t id;
		s = pthread_create(&id, NULL,
							tcp_sub_thread, ndescriptor);
		if (s != 0)
			handle_error_en(s, "pthread_create");
		
	}

	printf("termine con descriptor %d\n\n", descriptor);

	return descriptor;
}

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
	pthread_exit(NULL);	
}

/*-------------------------------------------------------------------------*
 * inicializar() - inicializar el servidor
 *-------------------------------------------------------------------------*/ 	    
int inicializar( int puerto ) {
	int lsock = NULL;
	int lsock_udp;
	// int opt = 1;
	struct sockaddr_in dir_srv;
	int s, tnum, opt, num_threads;
	struct thread_info *tinfo;
	struct thread_args *tudpargs = malloc (sizeof (struct thread_args));
	struct thread_args *ttcpargs = malloc (sizeof (struct thread_args));
	pthread_attr_t attr;
	int stack_size;
	void *res;
	
	if ( ( lsock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		return ( -1 );
	
	if ( ( lsock_udp = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
		return ( -1 );
	
	/* bind de la dirección local */
	bzero( (char *) &ttcpargs->dir_srv, sizeof( struct sockaddr_in ) );
	ttcpargs->dir_srv.sin_family = AF_INET;	/* utilizará familia de protocolos de Internet */
	ttcpargs->dir_srv.sin_addr.s_addr = htonl( INADDR_ANY );		/* sobre la dirección IP local */
	ttcpargs->dir_srv.sin_port = htons( puerto );

	if ( setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) )
		return ( -2 );

	if ( setsockopt(lsock_udp, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) )
		return ( -2 );

	if ( bind( lsock, (struct sockaddr *) &ttcpargs->dir_srv, sizeof( struct sockaddr_in ) ) < 0 )
		return ( -2 );

	if ( bind( lsock_udp, (struct sockaddr *) &ttcpargs->dir_srv, sizeof( struct sockaddr_in ) ) < 0 )
		return ( -2 );
	
	/* Initialize thread creation attributes */
	// pthread_attr_init(&attr);
	// if (s != 0)
	// 	handle_error_en(s, "pthread_attr_init");

	//tudpargs->sock=lsock_udp;
	//tudpargs->dir_srv=dir_srv;
	// Implementar un thread con un for que espere conexiones UTP y les asigne un tread
	// s = pthread_create(NULL, &attr,
	// 					udp_thread_start, tudpargs);
	// if (s != 0)
	// 	handle_error_en(s, "pthread_create");

	ttcpargs->sock=lsock;
	// Implementar un thread con un for que espere conexiones UDP y les asigne un tread
	pthread_t id;
	s = pthread_create(&id, NULL,
						tcp_thread_start, ttcpargs);
	if (s != 0)
		handle_error_en(s, "pthread_create");

	return 1;
}

/*-------------------------------------------------------------------------*
 * esperar()
 *-------------------------------------------------------------------------*/ 	    
int esperar( int sockfd ) {
	int nsockfd;

	/* aceptar una conexión */
	if ( ( nsockfd = accept( sockfd, 0, 0 ) ) < 0 ){
		return ( -1 );
	}
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




