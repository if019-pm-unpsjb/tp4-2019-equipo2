/*
 * Cliente luces apagar-encender
 * 02-05-2018
 */

#include "cliente.h"

/*------------------------------------------------------------------------*
 * main() - Enviar comandos para encender o apagar las luces.
 *------------------------------------------------------------------------*/
void main (int argc,char *argv[]) {

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
	char linea_env[ MAXLINEA ], linea_rcb[ MAXLINEA + 1 ];
	printf("ingrese servicio\n");
	while( fgets( linea_env, MAXLINEA, stdin ) != NULL ) {
		if(strcmp(linea_env, "riego\n")==0){
			printf("Recibido: %s\n", linea_env);
			principal_riego(stdin, descriptor, (struct sockaddr *)&dir, sizeof(dir));
		}
		if(strcmp(linea_env, "luces\n")==0){
			printf("Recibido: %s\n", linea_env);
			principal_luces(stdin, descriptor, (struct sockaddr *)&dir, sizeof(dir));
		}
	}
}

/*-------------------------------------------------------------------------*
 * principal()
 *-------------------------------------------------------------------------*/
int principal_riego( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa ) {
	int n;
	char *palabra1, *palabra2, *palabra3, *palabra4;
	char *linea_env_config = (char*)malloc(sizeof(MAXLINEA));//NO OLVIDARSE DEL MALLOC
	char linea_env[ MAXLINEA ], linea_rcb[ MAXLINEA + 1 ];
	char command[MAXLINEA] = "R";
	printf("Enviar 1 para encender el riego, 0 para apagar el riego de forma manual\n");
	printf("Enviar un mensaje para forma automatica: encender xx y apagar yy\n");
	printf("donde xx la hora esta en formato 24 hs\n");
	while( fgets( linea_env, MAXLINEA, fp ) != NULL ) {
		//printf("command: %s linea_env: %s\n", command, linea_env);
		if( linea_env[0] == PRENDER || linea_env[0] == APAGAR ){
					strcat(command, linea_env);
        	sendto( sockfd, command, strlen( command ), 0, dir, sa);
        	n = recvfrom( sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL );
			linea_rcb[ n ] = '\0';
			printf( "Recibido: %s\n", linea_rcb );
    	}
    	else{
			//	printf("else linea_env: %s\n", linea_env);
			palabra1 = strtok(linea_env, " ");
			printf("palabra1: %s\n", palabra1 );
			if(strcmp(palabra1,"encender") != 0 )
				perror("Error opcion incorrecta palabra 1");
			palabra2 = strtok(NULL, " ");
			printf("palabra2: %s\n", palabra2 );
			if(atoi(palabra2) < 0 || atoi(palabra2) > 24)
				perror("Error opcion incorrecta palabra 2");
			palabra3 = strtok(NULL, " ");
			printf("palabra3: %s\n", palabra3 );
			if(strcmp(palabra3, "apagar") != 0 ){
				perror("Error opcion incorrecta palabra 3");
			}
			palabra4 = strtok(NULL, " ");
			if(atoi(palabra4) < 0 || atoi(palabra4) > 24)
				perror("Error opcion incorrecta palabra 4");
			if(strtok(NULL, " ") != NULL)
				perror("Error opcion incorrecta palabra 5");
			strcpy(linea_env_config, "3 ");
			strcat(linea_env_config, palabra2);
			strcat(linea_env_config, " ");
			strcat(linea_env_config, palabra4);
			printf("envio: %s\n", linea_env_config);
			strcat(command, linea_env_config);
			sendto( sockfd, command, strlen( command ), 0, dir, sa);
        	n = recvfrom( sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL );
			linea_rcb[ n ] = '\0';
			printf( "Recibido: %s\n", linea_rcb );
		}
		/*setea command para poder enviar varias veces*/
		memset(command, 0, sizeof command);
		memset(command, 'R', 1);
	}
}

int principal_luces( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa ) {
	int n;
	char linea_env[ MAXLINEA ], linea_rcb[ MAXLINEA + 1 ];
	char command[MAXLINEA] = "L";

	printf("Enviar 1 para prender luces, 0 para apagar luces\n");
	while( fgets( linea_env, MAXLINEA, fp ) != NULL ) {
	//	printf("command: %s linea_env: %s\n", command, linea_env);
		strcat(command, linea_env);
		//printf("enviado: %s \n", command);
		if( linea_env[0] == PRENDER ){
        	sendto( sockfd, command, strlen( command ), 0, dir, sa);
        	n = recvfrom( sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL );
			linea_rcb[ n ] = '\0';
			printf( "Recibido: %s\n", linea_rcb );
    	}
    	else if(linea_env[0] == APAGAR ){
        	sendto( sockfd, command, strlen( command ), 0, dir, sa);
        	n = recvfrom( sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL );
			linea_rcb[ n ] = '\0';
			printf( "Recibido: %s\n", linea_rcb );
    	}
    	else{
        	perror("Error opcion incorrecta");
    	}
			/*setea command para poder enviar varias veces*/
			memset(command, 0, sizeof command);
			memset(command, 'L', 1);
	}
}
