/**
 * @file main.c
 * @author jonathan cagua Ordoñez
 * @brief Este servicio creará un cliente TCP que se conectará al servicio SerialService. 
 * Se encargará de leer y escribir el los archivos mencionados previamente para que la 
 * información se vea reflejada en la página web.
 * @version 0.1
 * @date 2021-11-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "SerialManager.h"
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// Definiciones para puerto serial
#define UART_BAUDRATE	115200
#define UART_PORT		0	 
void bloquearSign(void);
void desbloquearSign(void);

int main(void)
{
	socklen_t addr_len;
	int newfd;
	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	char ipClient[32];
	// Creamos socket
	int s = socket(AF_INET,SOCK_STREAM, 0);

	// Cargamos datos de IP:PORT del server
	bzero((char*) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(10000);
	printf("Inicio Serial Service\r\n");

	if(inet_pton(AF_INET, "127.0.0.1", &(serveraddr.sin_addr))<=0)
	{
		fprintf(stderr,"ERROR invalid server IP\r\n");
		return 1;
	}
		// Abrimos puerto con bind()
	if (bind(s, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
		close(s);
		perror("listener: bind");
		return 1;
	}

	// Seteamos socket en modo Listening
	if (listen (s, 10) == -1) // backlog=10
  	{
		close(s);
		perror("error en listen");
		exit(1);
  	}

	while(1){
		// Ejecutamos accept() para recibir conexiones entrantes
		addr_len = sizeof(struct sockaddr_in);
		if ( (newfd = accept(s, (struct sockaddr *)&clientaddr,&addr_len)) == -1)
		{
			perror("error en accept");
			exit(1);
		}
	 	inet_ntop(AF_INET, &(clientaddr.sin_addr), ipClient, sizeof(ipClient));
		printf("server:  conexion desde:  %s\n",ipClient);

		printf("main: Conectando Kit de NXP \n");
		printf("main: Se intenta abrir puerto serial \n");
		if (serial_open(UART_PORT, UART_BAUDRATE) != 0)
		{
			perror("main: error abriendo serial_open()");
			break;
		}
		printf("Bloqueo signal\n");
		bloquearSign();

		printf("Desbloqueo signal\n");
		desbloquearSign();
	}
	exit(EXIT_SUCCESS);
	return 0;
}

void bloquearSign(void)
{
    sigset_t set;
    int s;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
}

void desbloquearSign(void)
{
    sigset_t set;
    int s;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_UNBLOCK, &set, NULL);
}