/* 
 * Cliente de eco iterativo sobre TCP
 */

#include "eco.h"

#define CLIENT_PORT_ID 6086
/* gaia.cs.umass.edu */
#define STARTTRANSFER 500
#define MAXLINE 256


int conectar(struct sockaddr_in dir);
int enviar(int desc_con, char *msg, int len);
int recibir(int desc_con, char *linea, int len);
void cerrar(int desc_con);
void recibirFoto();
int enviarLlamada();
void transferirArchivo(int newsd, char *nombreArchivo);
void recibirArchivo(int newsd, char *nombreArchivo);