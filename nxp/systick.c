
#include "sapi.h"
#include "main.h"

#define TICKRATE_HZ (1000)	/* 1000 ticks per second */
#define TiempoSize(timex) (sizeof(timex)/sizeof(uint16_t))-1

#define STATE_OFF	'0'
#define STATE_ON	'1'
#define STATE_BLINK	'2'
#define UART_CIAA UART_GPIO

#define BUFF_RX_SIZE	128
#define RX_TIMEOUT	500

static unsigned char bufferRx[BUFF_RX_SIZE];
static int indexBuffer=0;
static volatile int flagStartedNewPacket=0;
static volatile int timeout=0;
//static volatile int delayCounter;
static volatile int toggleFlag=0;
static volatile int blinkCounter=0;
static char outsStates[4] = {'0','0','0','0'};


char getPressedKey(void)
{
	if(!gpioRead( TEC1 ))
	{
		while(!gpioRead( TEC1 ));
		return '0';
	}
	if(!gpioRead( TEC2 ))
	{
		while(!gpioRead( TEC2 ));
		return '1';
	}
	if(!gpioRead( TEC3 ))
	{
		while(!gpioRead( TEC3 ));
		return '2';
	}
	if(!gpioRead( TEC4 ))
	{
		while(!gpioRead( TEC4 ));
		return '3';
	}

	return 0x00;
}
void sendToggle(char sw)
{
  	uartWriteString( UART_CIAA, ">TOGGLE STATE:");
	uartWriteByte( UART_CIAA, sw);
 	uartWriteByte( UART_CIAA, '\r' );
   	uartWriteByte( UART_CIAA, '\n' );
}
void sendOK(void)
{
  	uartWriteString( UART_CIAA, ">OK\r\n");
}

/*void delayMs(int t)
{
	delayCounter=0;
	while(delayCounter<t)
	{
		__WFI();
	}
}*/

void setOut(int outIndex,int value)
{
	switch(outIndex)
	{
		case 0: gpioWrite( LEDB,value);break;
		case 1: gpioWrite( LEDR,value);break;
		case 2: gpioWrite( LEDG,value);break;
		case 3: gpioWrite( LEDG,value);break;
	}
}





int receiveStringNonBlocking(void)
{
	unsigned char data;
	if(uartReadByte(UART_CIAA, &data))
	{
		bufferRx[indexBuffer] = data;
		indexBuffer++;
		timeout=RX_TIMEOUT;
		flagStartedNewPacket=1;
	}
	if(flagStartedNewPacket==1)
	{
		if(timeout==0 || indexBuffer>=BUFF_RX_SIZE)
		{
			return indexBuffer;
		}
	}
	return 0;
}

void systickEvent(void* args)
{
	if(flagStartedNewPacket==1)
	{
		if(timeout>0)
			timeout--;
	}

	//delayCounter++;

	blinkCounter++;
	if(blinkCounter>=500)
	{
		blinkCounter=0;
		if(toggleFlag==0)
			toggleFlag=1;
		else
			toggleFlag=0;
	}
}
void receiveReset(void)
{
	indexBuffer=0;
	flagStartedNewPacket=0;
	timeout=0;
}

void analizePacket(int lenRx)
{
	//Format: >OUTS:X,Y,W,Z\r\n
	if(lenRx>=13)
	{
		if(bufferRx[0]=='>' && bufferRx[1]=='O' && bufferRx[2]=='U' && bufferRx[3]=='T' && bufferRx[4]=='S' && bufferRx[5]==':')
		{
			// 6 8 10 12
			outsStates[0] = bufferRx[6];
			outsStates[1] = bufferRx[8];
			outsStates[2] = bufferRx[10];
			outsStates[3] = bufferRx[12];
			sendOK();
		}
	}
}


static void outsManager(void)
{
	int i;
	for(i=0; i<4;i++)
	{
		switch(outsStates[i])
		{
			case STATE_OFF:
				setOut(i,1); // off
				break;
			case STATE_ON:
				setOut(i,0); // on
				break;
			case STATE_BLINK:
				setOut(i,toggleFlag);
				break;
		}
	}
}

int main(void){
	delay_t myDelay;

	SystemCoreClockUpdate();
	//tickCallbackSet(systickEvent,NULL);
	Board_Init();
	boardInit();
	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);
	//printf("hola mundo");
	gpioWrite( LEDR,1);
	gpioWrite( LEDG,1);
	gpioWrite( LEDB,1);
// Inicializar UART_USB a 115200 baudios
   uartConfig( UART_CIAA, 115200 );

   delayConfig( &myDelay, 1);
	while(1){
		// Teclas
		char key = getPressedKey();
		if(key!=0x00)
		{
			//delayMs(100);
			delay( 100);
			sendToggle(key);
		}
		//_______

		// salidas
		outsManager();
		if(delayRead( &myDelay )){
			systickEvent(NULL);
		}
		//________

		// Recepcion
		int lenRx = receiveStringNonBlocking();
		if(lenRx>0)
		{
			analizePacket(lenRx);
			receiveReset();
		}
		//__________
	}
}


// No se implementa la atención del error, se deja colgado con el while(1)
void atenderError()
{
	while(1);
}





