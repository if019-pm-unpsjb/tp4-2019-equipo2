/*
 * Servidor de eco concurrente sobre TCP
 */

#include "servidor.h"

char *ipHost;

void main(int argc, char *argv[])
{
	char *msg, *msg2;
	int descriptor, ndescriptor, total, pid_hijo;

	ipHost = (char *)malloc(MAXLINEA);
	strcpy(ipHost, argv[1]);

	/*--------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
	if (argc < 2)
	{
		printf("Uso: servidor <puerto>\n");
		exit(-1);
	}
	printf("\tServidor de eco concurrente sobre TCP. Para salir presione <Ctrl>-C\n");

	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor
	 *--------------------------------------------------------------------*/
	/* Tomar el puerto donde va a atender
	*/
	int resultado, sockfd;
	struct sockaddr_in dir;

	if (((descriptor = inicializar(SERVER_PORT_ID)) < 0))
	{
		perror("ERROR INICIALIZAR: ");
		exit(-1);
	}
	printf("Servidor inicializado.\n");

	/*--------------------------------------------------------------------* 
	 * Establecer la dirección del servidor y conectarse al timbre
	 *--------------------------------------------------------------------*/
	bzero((char *)&dir, sizeof(dir));
	dir.sin_family = AF_INET;
	if (inet_pton(AF_INET, ipHost, &dir.sin_addr) <= 0)
	{
		perror("Error en la función inet_pton:");
		exit(-1);
	}
	dir.sin_port = htons(SERVER_UDP_PORT_ID);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("ERROR SOCKET:");
		exit(-1);
	}

	msg = (char *)malloc(MAXLINEA + 1);
	msg2 = (char *)malloc(MAXLINEA + 1);
	int n;
	while (1 == 1)
	{
		printf("\tIngrese cualquier tecla si desea tocar el timbre\n");
		while (fgets(msg, MAXLINEA, stdin) != NULL)
		{
			//printf("\tQV\n");
			msg[strlen(msg) - 1] = '\0';
			/*---------------------------------------------------------------------
		 * Enviar el mensaje por consola
		 *---------------------------------------------------------------------*/
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&dir, sizeof(dir));
			n = recvfrom(sockfd, msg2, MAXLINEA, 0, NULL, NULL);
			msg2[n] = '\0';
			printf("\tTocando el timbre, la respuesta fue: %s\n", msg2);
			switch (msg2[0])
			{
			case '1':
				printf("El usuario rechazo la llamada\n");
				break;
			case '2':
				mandarFoto(descriptor);
				printf("se envio la foto todo bien\n");
				n = recvfrom(sockfd, msg2, MAXLINEA, 0, NULL, NULL);
				msg2[n] = '\0';
				if (msg2[0] == '3')
				{
					printf("El usuario solicito la llamada\n");
					enviarLlamada(descriptor);
					printf("Finalizo la llamada\n");
				}
				else
				{
					printf("El usuario rechazo la llamada\n");
				}
				break;
			case '3':
				printf("El usuario solicito la llamada\n");
				enviarLlamada(descriptor);
				printf("Finalizo la llamada\n");
				break;
			default:
				printf("server: solicitud invalida :%d\n", errno);
				exit(0);
			}
			break;
		}
	}
	printf("\tPASO MAL POR ACA\n");
}

/*-------------------------------------------------------------------------*
 * inicializar() - inicializar el servidor
 *-------------------------------------------------------------------------*/
int inicializar(int puerto)
{
	int sockfd;
	struct sockaddr_in dir_srv;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return (-1);

	/* bind de la dirección local */
	bzero((char *)&dir_srv, sizeof(dir_srv));
	dir_srv.sin_family = AF_INET;				 /* utilizará familia de protocolos de Internet */
	dir_srv.sin_addr.s_addr = htons(INADDR_ANY); /* sobre la dirección IP local */
	dir_srv.sin_port = htons(puerto);

	if (bind(sockfd, (struct sockaddr *)&dir_srv, sizeof(dir_srv)) < 0)
		return (-2);

	/* armar una lista de espera para MAXCLI clientes */
	//listen(sockfd, MAXCLI);
	if (listen(sockfd, 5) < 0)
	{
		printf("server: listen error :%d\n", errno);
		exit(0);
	}

	/* mostrar un mensaje con los datos del socket de escucha.
	 * Sólo para ver qué está haciendo. Esta función no debería mostrar salida por pantalla.
	 */
	//printf("\n\tEscuchando en puerto: %d, dirección %s. \n\tPodría tener hasta %d clientes esperando.\n",
	//   ntohs(dir_srv.sin_port), ntohl(dir_srv.sin_addr.s_addr), MAXCLI);

	//printf("EL SOCKET ES :%d\n", sockfd);
	return (sockfd);
}

/*-------------------------------------------------------------------------*
 * esperar()
 *-------------------------------------------------------------------------*/
int esperar(int sockfd)
{
	int nsockfd;

	/* aceptar una conexión */
	if ((nsockfd = accept(sockfd, 0, 0)) < 0)
		return (-1);

	return nsockfd;
}

/*-------------------------------------------------------------------------*
 * enviar() - enviar la respuesta al cliente 
 * retornar la longitud de la cadena enviada
 *-------------------------------------------------------------------------*/
int enviar(int nsockfd, char *msg)
{
	int longitud;

	if ((longitud = write(nsockfd, msg, strlen(msg))) < 0)
	{
		close(nsockfd);
		return (-1);
	}

	return longitud;
}

int mandarFoto(int descriptor)
{
	int sockid, newsd, newsockid, i, getfile, ack, msg, msg_2, c, len, clilen;
	int no_writen, start_xfer, num_blks, num_last_blk;
	struct sockaddr_in my_addr, server_addr, client_addr;
	FILE *fp;
	char in_buf[MAXSIZE];

	int fsize, fd, msg_ok, fail, fail1, req, line;
	int no_read, num_blks1, num_last_blk1, tmp;
	char fname[MAXLINE];
	char out_buf[MAXSIZE];

	no_writen = 0;
	no_read = 0;
	num_blks = 0;
	num_last_blk = 0;

	sockid = descriptor;

	printf("EL SOCKET ES :%d\n", sockid);
	if ((newsd = accept(sockid, (struct sockaddr *)&client_addr,
						&clilen)) < 0)
	{
		printf("server: accept  error :%d\n", errno);
		exit(0);
	}
	printf("server: return from accept, socket for this ftp: %d\n",
		   newsd);

	if ((fp = fopen("foto.jpg", "r")) == NULL) /*cant open file*/
		fail = BADFILENAME;

	if (fail == BADFILENAME)
	{
		printf("server cant open file\n");
		close(newsd);
		return (0);
	}

	/*SERVER GETS FILESIZE AND CALCULATES THE NUMBER OF BLOCKS OF 
     SIZE = MAXSIZE IT WILL TAKE TO TRANSFER THE FILE. ALSO CALCULATE
     NUMBER OF BYTES IN THE LAST PARTIALLY FILLED BLOCK IF ANY. 
     SEND THIS INFO TO CLIENT, RECEIVING ACKS */
	printf("server: starting transfer\n");
	fsize = 0;
	ack = 0;
	while ((c = getc(fp)) != EOF)
	{
		fsize++;
	}
	num_blks = fsize / MAXSIZE;
	num_blks1 = htons(num_blks);
	num_last_blk = fsize % MAXSIZE;
	num_last_blk1 = htons(num_last_blk);
	if ((writen(newsd, (char *)&num_blks1, sizeof(num_blks1))) < 0)
	{
		printf("server: write error :%d\n", errno);
		exit(0);
	}
	printf("server: told client there are %d blocks\n", num_blks);
	if ((readn(newsd, (char *)&ack, sizeof(ack))) < 0)
	{
		printf("server: ack read error :%d\n", errno);
		exit(0);
	}
	if (ntohs(ack) != ACK)
	{
		printf("client: ACK not received on file size\n");
		exit(0);
	}
	if ((writen(newsd, (char *)&num_last_blk1, sizeof(num_last_blk1))) < 0)
	{
		printf("server: write error :%d\n", errno);
		exit(0);
	}
	printf("server: told client %d bytes in last block\n", num_last_blk);
	if ((readn(newsd, (char *)&ack, sizeof(ack))) < 0)
	{
		printf("server: ack read error :%d\n", errno);
		exit(0);
	}
	if (ntohs(ack) != ACK)
	{
		printf("server: ACK not received on file size\n");
		exit(0);
	}
	rewind(fp);

	/* ACTUAL FILE TRANSFER STARTS  BLOCK BY BLOCK*/

	for (i = 0; i < num_blks; i++)
	{
		no_read = fread(out_buf, sizeof(char), MAXSIZE, fp);
		if (no_read == 0)
		{
			printf("server: file read error\n");
			return (0);
		}
		if (no_read != MAXSIZE)
		{
			printf("server: file read error : no_read is less\n");
			return (0);
		}
		if ((writen(newsd, out_buf, MAXSIZE)) < 0)
		{
			printf("server: error sending block:%d\n", errno);
			return (0);
		}
		if ((readn(newsd, (char *)&ack, sizeof(ack))) < 0)
		{
			printf("server: ack read  error :%d\n", errno);
			return (0);
		}
		if (ntohs(ack) != ACK)
		{
			printf("server: ACK not received for block %d\n", i);
			return (0);
		}
		printf(" %d...", i);
	}

	if (num_last_blk > 0)
	{
		printf("%d\n", num_blks);
		no_read = fread(out_buf, sizeof(char), num_last_blk, fp);
		if (no_read == 0)
		{
			printf("server: file read error\n");
			return (0);
		}
		if (no_read != num_last_blk)
		{
			printf("server: file read error : no_read is less 2\n");
			return (0);
		}
		if ((writen(newsd, out_buf, num_last_blk)) < 0)
		{
			printf("server: file transfer error %d\n", errno);
			return (0);
		}
		if ((readn(newsd, (char *)&ack, sizeof(ack))) < 0)
		{
			printf("server: ack read  error %d\n", errno);
			return (0);
		}
		if (ntohs(ack) != ACK)
		{
			printf("server: ACK not received last block\n");
			return (0);
		}
	}
	else
		printf("\n");

	/* FILE TRANSFER ENDS */
	printf("server: FILE TRANSFER COMPLETE on socket %d\n", newsd);
	fclose(fp);
	close(newsd);
	return 0;
}

int enviarSenal(int desc_con, char *msg, int len)
{
	int resultado;

	if ((resultado = write(desc_con, msg, len)) < 0)
		return (-1);

	return (resultado);
}

/*------------------------------------------------------------------------*
 * recibir() - recibir una petición
 * retornar la longitud de la cadena leída
 *------------------------------------------------------------------------*/
int recibir(int nsockfd, char *msg)
{
	int longitud;

	bzero(msg, MAXLINEA);

	if ((longitud = read(nsockfd, msg, (int)MAXLINEA)) < 0)
	{
		close(nsockfd);
		return (-2);
	}

	return (longitud);
}

/*-----------------------------------------------------------------------* 
 * procesar() - atender una petición
 *-----------------------------------------------------------------------*/
void procesar(char *mensaje)
{
	printf("(%5d) Procesando: %s \n", getpid(), mensaje);
}

/*
  TO TAKE CARE OF THE POSSIBILITY OF BUFFER LIMMITS IN THE KERNEL FOR THE
 SOCKET BEING REACHED (WHICH MAY CAUSE READ OR WRITE TO RETURN FEWER CHARACTERS
  THAN REQUESTED), WE USE THE FOLLOWING TWO FUNCTIONS */

int readn(int sd, char *ptr, int size)
{
	int no_left, no_read;
	no_left = size;
	while (no_left > 0)
	{
		no_read = read(sd, ptr, no_left);
		if (no_read < 0)
			return (no_read);
		if (no_read == 0)
			break;
		no_left -= no_read;
		ptr += no_read;
	}
	return (size - no_left);
}

int writen(int sd, char *ptr, int size)
{
	int no_left, no_written;
	no_left = size;
	while (no_left > 0)
	{
		no_written = write(sd, ptr, no_left);
		if (no_written <= 0)
			return (no_written);
		no_left -= no_written;
		ptr += no_written;
	}
	return (size - no_left);
}

int enviarLlamada(int descriptor)
{
	int sockid, newsd, clilen;
	struct sockaddr_in client_addr;

	sockid = descriptor;

	printf("EL SOCKET ES :%d\n", sockid);
	if ((newsd = accept(sockid, (struct sockaddr *)&client_addr,
						&clilen)) < 0)
	{
		printf("server: accept  error :%d\n", errno);
		exit(0);
	}
	printf("server: return from accept, socket for this ftp: %d\n",
		   newsd);

	recibirArchivo(newsd, "clienteA-0.m4a");
	printf("Se recibio el audio 1\n");
	transferirArchivo(newsd, "servidorA-1.m4a");
	printf("Se envio el audio 1\n");
	recibirArchivo(newsd, "clienteA-2.m4a");
	printf("Se recibio el audio 2\n");
	transferirArchivo(newsd, "servidorA-3.m4a");
	printf("Se envio el audio 2\n");
	recibirArchivo(newsd, "clienteA-4.m4a");
	printf("Se recibio el audio 3\n");
	close(newsd);
	printf("Se cerro la conexion de llamada telefonica\n");
}

int transferirArchivo(int newsd, char *nombreArchivo)
{
	int sockid, newsockid, i, getfile, ack, msg, msg_2, c, len, clilen;
	int no_writen, start_xfer, num_blks, num_last_blk;
	struct sockaddr_in my_addr, server_addr, client_addr;
	FILE *fp;
	char in_buf[MAXSIZE];

	int fsize, fd, msg_ok, fail, fail1, req, line;
	int no_read, num_blks1, num_last_blk1, tmp;
	char fname[MAXLINE];
	char out_buf[MAXSIZE];

	no_writen = 0;
	no_read = 0;
	num_blks = 0;
	num_last_blk = 0;

	if ((fp = fopen(nombreArchivo, "r")) == NULL) /*cant open file*/
		fail = BADFILENAME;

	if (fail == BADFILENAME)
	{
		printf("server cant open file\n");
		close(newsd);
		return (0);
	}

	/*SERVER GETS FILESIZE AND CALCULATES THE NUMBER OF BLOCKS OF 
     SIZE = MAXSIZE IT WILL TAKE TO TRANSFER THE FILE. ALSO CALCULATE
     NUMBER OF BYTES IN THE LAST PARTIALLY FILLED BLOCK IF ANY. 
     SEND THIS INFO TO CLIENT, RECEIVING ACKS */
	printf("server: starting transfer\n");
	fsize = 0;
	ack = 0;
	while ((c = getc(fp)) != EOF)
	{
		fsize++;
	}
	num_blks = fsize / MAXSIZE;
	num_blks1 = htons(num_blks);
	num_last_blk = fsize % MAXSIZE;
	num_last_blk1 = htons(num_last_blk);
	if ((writen(newsd, (char *)&num_blks1, sizeof(num_blks1))) < 0)
	{
		printf("server: write error :%d\n", errno);
		exit(0);
	}
	printf("server: told client there are %d blocks\n", num_blks);
	if ((readn(newsd, (char *)&ack, sizeof(ack))) < 0)
	{
		printf("server: ack read error :%d\n", errno);
		exit(0);
	}
	if (ntohs(ack) != ACK)
	{
		printf("client: ACK not received on file size\n");
		exit(0);
	}
	if ((writen(newsd, (char *)&num_last_blk1, sizeof(num_last_blk1))) < 0)
	{
		printf("server: write error :%d\n", errno);
		exit(0);
	}
	printf("server: told client %d bytes in last block\n", num_last_blk);
	if ((readn(newsd, (char *)&ack, sizeof(ack))) < 0)
	{
		printf("server: ack read error :%d\n", errno);
		exit(0);
	}
	if (ntohs(ack) != ACK)
	{
		printf("server: ACK not received on file size\n");
		exit(0);
	}
	rewind(fp);

	/* ACTUAL FILE TRANSFER STARTS  BLOCK BY BLOCK*/

	for (i = 0; i < num_blks; i++)
	{
		no_read = fread(out_buf, sizeof(char), MAXSIZE, fp);
		if (no_read == 0)
		{
			printf("server: file read error\n");
			return (0);
		}
		if (no_read != MAXSIZE)
		{
			printf("server: file read error : no_read is less\n");
			return (0);
		}
		if ((writen(newsd, out_buf, MAXSIZE)) < 0)
		{
			printf("server: error sending block:%d\n", errno);
			return (0);
		}
		if ((readn(newsd, (char *)&ack, sizeof(ack))) < 0)
		{
			printf("server: ack read  error :%d\n", errno);
			return (0);
		}
		if (ntohs(ack) != ACK)
		{
			printf("server: ACK not received for block %d\n", i);
			return (0);
		}
		printf(" %d...", i);
	}

	if (num_last_blk > 0)
	{
		printf("%d\n", num_blks);
		no_read = fread(out_buf, sizeof(char), num_last_blk, fp);
		if (no_read == 0)
		{
			printf("server: file read error\n");
			return (0);
		}
		if (no_read != num_last_blk)
		{
			printf("server: file read error : no_read is less 2\n");
			return (0);
		}
		if ((writen(newsd, out_buf, num_last_blk)) < 0)
		{
			printf("server: file transfer error %d\n", errno);
			return (0);
		}
		if ((readn(newsd, (char *)&ack, sizeof(ack))) < 0)
		{
			printf("server: ack read  error %d\n", errno);
			return (0);
		}
		if (ntohs(ack) != ACK)
		{
			printf("server: ACK not received last block\n");
			return (0);
		}
	}
	else
		printf("\n");

	/* FILE TRANSFER ENDS */
	printf("server: FILE TRANSFER COMPLETE on socket %d\n", newsd);
	fclose(fp);
	//close(newsd);
}

void recibirArchivo(int newsd, char *nombreArchivo)
{
	struct sockaddr_in dir;
	int sockfd;
	int sockid, newsockid, i, getfile, ack, msg, msg_2, c, len;
	int no_writen, start_xfer, num_blks, num_last_blk;
	struct sockaddr_in my_addr, server_addr;
	FILE *fp;
	char in_buf[MAXSIZE];

	no_writen = 0;
	num_blks = 0;
	num_last_blk = 0;

	sockid = newsd;

	if ((fp = fopen(nombreArchivo, "w")) == NULL)
	{
		printf(" client: local open file error \n");
		exit(0);
	}

	/*NOW THE CLIENT IS READING INFORMATION FROM THE SERVER REGARDING HOW MANY
    FULL BLOCKS OF SIZE MAXSIZE IT CAN EXPECT. IT ALSO RECEIVES THE NUMBER
   OF BYTES REMAINING IN THE LAST PARTIALLY FILLED BLOCK, IF ANY */

	if ((readn(sockid, (char *)&num_blks, sizeof(num_blks))) < 0)
	{
		printf("client: read error on nblocks :%d\n", errno);
		printf("client: read error on nblocks :%d\n", ntohs(num_blks));
		exit(0);
	}
	num_blks = ntohs(num_blks);
	printf("client: server responded: %d blocks in file\n", num_blks);
	ack = ACK;
	ack = htons(ack);
	if ((writen(sockid, (char *)&ack, sizeof(ack))) < 0)
	{
		printf("client: ack write error :%d\n", errno);
		exit(0);
	}

	if ((readn(sockid, (char *)&num_last_blk, sizeof(num_last_blk))) < 0)
	{
		printf("client: read error :%d on nbytes\n", errno);
		exit(0);
	}
	num_last_blk = ntohs(num_last_blk);
	printf("client: server responded: %d bytes last blk\n", num_last_blk);
	if ((writen(sockid, (char *)&ack, sizeof(ack))) < 0)
	{
		printf("client: ack write error :%d\n", errno);
		exit(0);
	}

	/* BEGIN READING BLOCKS BEING SENT BY SERVER */
	printf("client: starting to get file contents\n");
	for (i = 0; i < num_blks; i++)
	{
		if ((readn(sockid, in_buf, MAXSIZE)) < 0)
		{
			printf("client: block error read: %d\n", errno);
			exit(0);
		}
		no_writen = fwrite(in_buf, sizeof(char), MAXSIZE, fp);
		if (no_writen == 0)
		{
			printf("client: file write error\n");
			exit(0);
		}
		if (no_writen != MAXSIZE)
		{
			printf("client: file write  error : no_writen is less\n");
			exit(0);
		}
		/* send an ACK for this block */
		if ((writen(sockid, (char *)&ack, sizeof(ack))) < 0)
		{
			printf("client: ack write  error :%d\n", errno);
			exit(0);
		}
		printf(" %d...", i);
	}

	/*IF THERE IS A LAST PARTIALLY FILLED BLOCK, READ IT */

	//printf("POR ACA TODO BIEN -2\n");
	if (num_last_blk > 0)
	{
		//printf("POR ACA TODO BIEN -1\n");
		printf("%d\n", num_blks);
		if ((readn(sockid, in_buf, num_last_blk)) < 0)
		{
			printf("client: last block error read :%d\n", errno);
			exit(0);
		}
		//printf("POR ACA TODO BIEN 0\n");
		no_writen = fwrite(in_buf, sizeof(char), num_last_blk, fp);
		if (no_writen == 0)
		{
			printf("client: last block file write err :%d\n", errno);
			exit(0);
		}
		//printf("POR ACA TODO BIEN 1\n");
		if (no_writen != num_last_blk)
		{
			printf("client: file write error : no_writen is less 2\n");
			exit(0);
		}
		//printf("POR ACA TODO BIEN 2\n");
		if ((writen(sockid, (char *)&ack, sizeof(ack))) < 0)
		{
			printf("client :ack write  error  :%d\n", errno);
			exit(0);
		}
	}
	else
		printf("\n");

	/*FILE TRANSFER ENDS. CLIENT TERMINATES AFTER  CLOSING ALL ITS FILES
    AND SOCKETS*/
	fclose(fp);
	printf("client: FILE TRANSFER COMPLETE\n");
	//close(sockid);
	return;
}
