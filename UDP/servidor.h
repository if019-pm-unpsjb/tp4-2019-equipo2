/*
 * Servidor de eco iterativo sobre UDP
 * 03/05/2018
 */

#include <string.h>
#include <stdlib.h>
#include "eco.h"

#define PRENDER '1'
#define APAGAR '0'
#define CONFIG '3'

void procesar( int descriptor, struct sockaddr *dir_cli_p, socklen_t longcli );