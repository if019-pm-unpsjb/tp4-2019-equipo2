/*
 * Servidor de eco iterativo sobre UDP
 * 17-09-2010
 */

#include <string.h>
#include <stdlib.h>
#include "eco.h"

void procesar( int descriptor, struct sockaddr *dir_cli_p, socklen_t longcli );

