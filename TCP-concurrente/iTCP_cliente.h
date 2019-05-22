/* 
 * Cliente de eco iterativo sobre TCP
 */
 

#include <string.h>
#include <stdlib.h>
#include "eco.h"

int conectar( struct sockaddr_in dir );
int enviar( int desc_con, char *msg, int len );
int recibir( int desc_con, char *linea, int len );
void cerrar ( int desc_con );

