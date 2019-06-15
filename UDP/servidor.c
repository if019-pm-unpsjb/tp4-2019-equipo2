/*
 * Servidor de eco iterativo sobre UDP
 */

#include "servidor.h"

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

	//comun
	int recibido;
	socklen_t longitud;
	char msg[ MAXLINEA ];
	char *linea_env = (char*)malloc(sizeof(MAXLINEA));//NO OLVIDARSE DEL MALLOC
	
	//riego
	char *s_hora_inic = (char*)malloc(sizeof(MAXLINEA));//NO OLVIDARSE DEL MALLOC
	char *s_hora_fin = (char*)malloc(sizeof(MAXLINEA));//NO OLVIDARSE DEL MALLOC
	int riego = 0;
	int hora_inic, hora_fin;

	//luz
	int luz = 0;

	for(;;) {
		longitud = longcli;
		recibido = recvfrom( descriptor, msg, MAXLINEA, 0, dir_cli_p, &longitud );
   //printf("mensaje recibido: %s", msg);
		if(msg[0]=='R'){
     		if(msg[1] == APAGAR){
		 		if(riego == 0){
					strcpy(linea_env, "El riego ya estaba apagado");
	    			sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
		 		}else{
					strcpy(linea_env, "El riego fue apagado");
	    			sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
					riego = 0;
		 		}
	 		}
	 		else if(msg[1] == PRENDER){
		 		if(riego == 1){
					strcpy(linea_env, "El riego estaba encendido");
	    			sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
		 		}else{
					strcpy(linea_env, "El riego fue encendido");
	    			sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
					riego = 1;
		 		}
	 		}
		 	else if(msg[1] == CONFIG){
				strtok(msg," ");
				s_hora_inic = strtok(NULL," ");
			 	hora_inic = atoi(s_hora_inic);
				s_hora_fin = strtok(NULL," ");
			 	hora_fin = atoi(s_hora_fin);
				// printf("ENTRE A CONFIG \n");
				strcpy(linea_env, "La nueva configuracion es: ");
				strcat(linea_env,s_hora_inic);
				strcat(linea_env, " de encendido, y de apagado: ");
				strcat(linea_env, s_hora_fin);
				strcat(linea_env, "\0");
				sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
		 	}
		 	else{
				strcpy(linea_env, "mensaje invalido!");
				sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
		 	}
		} 
	 	else if(msg[0]=='L'){
			if(msg[1] == APAGAR){
				if(luz == 0){
					strcpy(linea_env, "Las luces ya estaban apagadas");
					sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
				}else{
					strcpy(linea_env, "Las luces fueron apagadas");
					sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
					luz = 0;
				}
			}
			else if(msg[1] == PRENDER){
				if(luz == 1){
					strcpy(linea_env, "Las luces ya estaban encendidas");
					sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
				}else{
					strcpy(linea_env, "Las luces fueron encendidas");
					sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
					luz = 1;
				}
			}
			else{
				strcpy(linea_env, "mensaje invalido1!");
				sendto( descriptor, linea_env, strlen(linea_env), 0, dir_cli_p, longitud );
			}
		}
	}
}
