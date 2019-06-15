/*
 * Servidor de eco concurrente sobre TCP
 */

#include "eco.h"

#define MAXLINE 256

void procesar( char *mensaje );
int inicializar( int puerto );
int esperar( int desc_con );
int enviar( int desc_con, char *msg );
int recibir( int desc_con, char *msg );
int mandarFoto(int descriptor);
int enviarLlamada(int descriptor);
int transferirArchivo(int newsd, char *nombreArchivo);
void recibirArchivo(int newsd, char *nombreArchivo);