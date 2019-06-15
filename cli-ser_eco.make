#
# Ejemplo de archivo makefile para compilar varios ejecutables
# Compilar los ejemplos de servidor de eco iterativo sobre UDP y concurrente sobre TCP, con sus respectivos clientes
# 13/10/10
#
# Para compilar, desde línea de comandos:
# 	make -f cli-ser_eco.make
#
# Tener en cuenta:
# 	- las etiquetas (nombre seguido de :) indican el nombre de lo que se quiere construir (ej. el ejecutable).
#	- si se repiten persiste la última aparición (y muestra un warning)
#	- el espacio en blanco antes de cada comando es un tabulador
#	- puede utilizarse cualquier comando del shell
#	- las variables deben definirse e inicializarse al comienzo
#	- toma un sólo objetivo (la primer etiqueta)(ejemplo-cli-ser-eco)
#	- en la línea de la etiqueta van los archivos objeto que deben linkearse para obtener el ejecutable
#	- luego habrá una etiqueta por cada objeto, donde se indica:
#		- las dependencias de archivos (en la misma línea a continuación de la etiqueta)
#		- los comandos necesarios para compilar
#

# ubicación servidores
VIA_SRV_1 = UDP
VIA_SRV_2 = TCP
VIA_SRV_3 = Test-combinado

OBJS_SRV_1 = $(VIA_SRV_1)/servidor.o 
OBJS_SRV_2 = $(VIA_SRV_2)/servidor.o 
OBJS_SRV_3 = $(VIA_SRV_3)/Test_servidor.o

# ubicación clientes
VIA_CLI_1 = UDP
VIA_CLI_2 = TCP

OBJS_CLI_1 = $(VIA_CLI_1)/cliente.o 
OBJS_CLI_2 = $(VIA_CLI_2)/cliente.o 

# paquete completo de clientes y servidores
ejemplo-cli-ser-eco: $(OBJS_SRV_1) $(OBJS_SRV_2) $(OBJS_SRV_3) $(OBJS_CLI_1) $(OBJS_CLI_2)
	cc $(OBJS_SRV_1) -o $(VIA_SRV_1)/servidor
	cc $(OBJS_SRV_2) -o $(VIA_SRV_2)/servidor
	cc $(OBJS_SRV_3) -o $(VIA_SRV_3)/Test_servidor
	cc $(OBJS_CLI_1) -o $(VIA_CLI_1)/cliente
	cc $(OBJS_CLI_2) -o $(VIA_CLI_2)/cliente
	
# 1.- Servidor de eco iterativo sobre UDP
servidor.o:	$(VIA_SRV_1)/servidor.c $(VIA_SRV_1)/servidor.h $(VIA_SRV_1)/eco.h
	cc -c $(VIA_SRV_1)/servidor.c

# 2.- Servidor de eco concurrente sobre TCP
servidor.o:	$(VIA_SRV_2)/servidor.c $(VIA_SRV_2)/servidor.h $(VIA_SRV_2)/eco.h
	cc -c $(VIA_SRV_2)/servidor.c

# 3.- Servidor combinado de prueba
Test_servidor.o:	$(VIA_SRV_3)/Test_servidor.c $(VIA_SRV_3)/cTCP_servidor.h $(VIA_SRV_3)/eco.h
	cc -c $(VIA_SRV_3)/Test_servidor.c

# 1.- Cliente de eco iterativo sobre UDP
cliente.o: 	$(VIA_CLI_1)/cliente.c $(VIA_CLI_1)/cliente.h $(VIA_CLI_1)/eco.h
	cc -c $(VIA_CLI_1)/cliente.c

# 2.- Cliente de eco iterativo sobre TCP
cliente.o:	$(VIA_CLI_2)/cliente.c $(VIA_CLI_2)/cliente.h $(VIA_CLI_2)/eco.h
	cc -c $(VIA_CLI_2)/cliente.c

