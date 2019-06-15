/*
 * Cliente luces apagar-encender
 * 02-05-2018
 */

#include "cliente.h"

/*------------------------------------------------------------------------*
 * main() - Enviar comandos para encender o apagar las luces.
 *------------------------------------------------------------------------*/
void main(int argc, char *argv[])
{
  int descriptor;
  struct sockaddr_in dir_srv, dir_cli;

  /*---------------------------------------------------------------------*
	 * Verificar los argumentos recibidos								   *
	 *---------------------------------------------------------------------*/
  if (argc < 2)
  {
    printf("Uso: servidor <puerto>\n");
    exit(-1);
  }

  /*--------------------------------------------------------------------* 
	 * Inicializar el servidor                               			  *
	 *--------------------------------------------------------------------*/
  if ((descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("ERROR SOCKET: ");
    exit(-1);
  }
  printf("LLLEGO ACA.\n\n");
  bzero((char *)&dir_srv, sizeof(dir_srv));
  dir_srv.sin_family = AF_INET;
  dir_srv.sin_addr.s_addr = htonl(INADDR_ANY);
  dir_srv.sin_port = htons(SERVER_UDP_PORT_ID);
  //printf("LLLEGO ACA2.\n\n");
  if (bind(descriptor, (struct sockaddr *)&dir_srv, sizeof(dir_srv)) < 0)
  {
    perror("ERROR BIND: ");
    exit(-1);
  }
  //printf("Servidor inicializado.\n\n");

  procesar(descriptor, (struct sockaddr *)&dir_cli, sizeof(dir_cli));
}

/*-----------------------------------------------------------------------* 
 * procesar() - realizar la tarea específica del servidor
 *-----------------------------------------------------------------------*/
void procesar(int descriptor, struct sockaddr *dir_cli_p, socklen_t longcli)
{
  int recibido;
  socklen_t longitud;
  char msg[MAXLINEA];
  char usrMsg[MAXLINEA];
  char *linea_env = (char *)malloc(sizeof(MAXLINEA)); //NO OLVIDARSE DEL MALLOC
  int luz = 0;

  for (;;)
  {
    longitud = longcli;
    recibido = recvfrom(descriptor, msg, MAXLINEA, 0, dir_cli_p, &longitud);
    printf("\tEsta sonando el timbre! Que desea hacer??\n");
    printf("\tIngrese su respuesta:\n");
    printf("\t1 - Para rechazar la llamada:\n");
    printf("\t2 - Para solicitar una imagen:\n");
    printf("\t3 - Para atender la llamada:\n");
    //LEER LO QUE DESEA EL USUARIO
    while (fgets(usrMsg, MAXLINEA, stdin) != NULL)
    {
      usrMsg[strlen(usrMsg) - 1] = '\0';
      printf("\tLa respuesta del usuario es: %s\n", usrMsg);
      //CASE SOBRE LO Q DESEA
      switch (usrMsg[0])
      {
      case '1':
        //NO HACER NADA
        sendto(descriptor, "1", strlen("1"), 0, dir_cli_p, longitud);
        printf("\tSe rechazo la llamada\n");
        break;
      case '2':
        //SOLICITAR UNA FOTO
        sendto(descriptor, "2", strlen("2"), 0, dir_cli_p, longitud);
        recibirFoto();
        printf("\tSe recibio la foto todo bien\n");
        printf("\tQue desea hacer con la llamada?\n");
        printf("\tIngrese 1 para rechazarla o 3 para aceptarla\n");
        while (fgets(usrMsg, MAXLINEA, stdin) != NULL)
        {
          if (usrMsg[0] == '3')
          {
            sendto(descriptor, "3", strlen("3"), 0, dir_cli_p, longitud);
            printf("\tSe acepta la llamada\n");
            enviarLlamada();
            printf("\tLlamada finalizada\n");
          }
          else
          {
            sendto(descriptor, "1", strlen("1"), 0, dir_cli_p, longitud);
            printf("\tSe rechazo la llamada\n");
          }
          break;
        }
        break;
      case '3':
        sendto(descriptor, "3", strlen("3"), 0, dir_cli_p, longitud);
        printf("\tSe acepta la llamada\n");
        enviarLlamada();
        printf("\tLlamada finalizada\n");
        break;
      default:
        sendto(descriptor, "1", strlen("1"), 0, dir_cli_p, longitud);
        printf("client: Opcion incorrecta : %d\n", errno);
      }
      break;
    }
  }
}

void recibirFoto()
{
  int descriptor;
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

  printf("client: creating socket\n");
  if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("client: socket error : %d\n", errno);
    exit(0);
  }

  //printf("client: binding my local socket\n");
  /*bzero((char *)&my_addr, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htons(INADDR_ANY);
  my_addr.sin_port = htons(CLIENT_PORT_ID);*/
  /*if (bind(sockid, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
  {
    printf("client: bind  error :%d\n", errno);
    exit(0);
  }*/

  printf("EL SOCKET ES :%d\n", sockid);
  printf("client: starting connect\n");
  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST_ADDR);
  server_addr.sin_port = htons(SERVER_PORT_ID);
  if (connect(sockid, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("client: connect  error :%d\n", errno);
    exit(0);
  }

  if ((fp = fopen("foto.jpg", "w")) == NULL)
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
  close(sockid);
  return;
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

int enviarLlamada()
{
  int newsd, clilen;
	struct sockaddr_in client_addr;
  struct sockaddr_in dir;
  int sockid, newsockid, i, getfile, ack, msg, msg_2, c, len;
  int no_writen, start_xfer, num_blks, num_last_blk;
  struct sockaddr_in my_addr, server_addr;

  printf("client: creating socket\n");
  if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("client: socket error : %d\n", errno);
    exit(0);
  }

 /* printf("client: binding my local socket\n");
  bzero((char *)&my_addr, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htons(INADDR_ANY);
  my_addr.sin_port = htons(CLIENT_PORT_ID);
  if (bind(sockid, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
  {
    printf("client: bind  error :%d\n", errno);
    exit(0);
  }*/

  printf("EL SOCKET ES :%d\n", sockid);
  printf("client: starting connect\n");
  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST_ADDR);
  server_addr.sin_port = htons(SERVER_PORT_ID);
  if ((newsd = connect(sockid, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
  {
    printf("client: connect  error :%d\n", errno);
    exit(0);
  }

	/*printf("EL SOCKET ES :%d\n", sockid);
	if ((newsd = connect(sockid, (struct sockaddr *)&client_addr,
						&clilen)) < 0)
	{
		printf("server: accept  error :%d\n", errno);
		exit(0);
	}*/
	printf("server: return from accept, socket for this ftp: %d\n",
		   sockid);
	
	transferirArchivo(sockid, "clienteA-0.m4a");
	printf("Se recibio el audio 1\n");
	recibirArchivo(sockid, "servidorA-1.m4a");
	printf("Se envio el audio 1\n");
	transferirArchivo(sockid, "clienteA-2.m4a");
	printf("Se recibio el audio 2\n");
	recibirArchivo(sockid, "servidorA-3.m4a");
	printf("Se envio el audio 2\n");
	transferirArchivo(sockid, "clienteA-4.m4a");
	printf("Se recibio el audio 3\n");
	close(sockid);
	printf("Se cerro la conexion de llamada telefonica\n");
}

void transferirArchivo(int newsd, char *nombreArchivo)
{
	int sockid,newsockid, i, getfile, ack, msg, msg_2, c, len, clilen;
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