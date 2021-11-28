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
// Definiciones para puerto serial
#define UART_BAUDRATE	115200
#define UART_PORT		0	 
void bloquearSign(void);
void desbloquearSign(void);

int main(void)
{
	printf("Inicio Serial Service\r\n");
	

	while(1){
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