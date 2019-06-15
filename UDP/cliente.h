/*
 * Cliente luces apgar-encender
 * 02-05-2018
 */

#include <string.h>
#include <stdlib.h>
#include "eco.h"

#define PRENDER '1'
#define APAGAR '0'

int principal_riego( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa );
int principal_luces( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa );
