/*
 * Servidor de eco concurrente sobre TCP
 */

#include <string.h>
#include <stdlib.h>
#include "eco.h"

void procesar( char *mensaje );
int inicializar( int puerto );
int esperar( int desc_con );
int enviar( int desc_con, char *msg );
int recibir( int desc_con, char *msg );


