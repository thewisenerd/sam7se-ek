#include "AT91SAM7SE512.H"
#include "..\AT91SAM7SE-EK.H"

#define speed (AT91B_MAIN_OSC/1000)

/* define ledspeed because any good compiler will substitute value
 * at compile
 */
#define ledspeed (50 * speed)

/*
 * wait function not used in this program
 */
void wait()
{
	/* this saves a couple of instructions compared to an
	 * incrementing for.
	 *
	 * again, any decent compiler should've optimized this
	 * but keep code clean.
	 */
	unsigned int i = ledspeed;
	while(i--);
}

/*
 * UART0
 */
void init_uart0()//uart0 initialisation
{
	// Enable the Clock of the PIOA (UART, LED{1,2,POWER})
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;

	// Configure the PIO Lines for LEDs as Outputs
	AT91C_BASE_PIOA->PIO_PER  = AT91B_LED_MASK;
	AT91C_BASE_PIOA->PIO_OER  = AT91B_LED_MASK;

	//to enable the peripheral clock for the uart (usart0)
	AT91C_BASE_PMC->PMC_PCER  = (1 << AT91C_ID_US0);

	//configuring the i/o lines for uart

	//to disable the pins for gpio function
	AT91C_BASE_PIOA->PIO_PDR= AT91C_PIO_PA5 | AT91C_PIO_PA6;

	//to select peripheral A by A-Select-Register(pg-37)
	AT91C_BASE_PIOA->PIO_ASR= AT91C_PA5_RXD0 | AT91C_PA6_TXD0;

	//to disable peripheral B
	AT91C_BASE_PIOA->PIO_BSR=0;

	// Open drain
	AT91C_BASE_PIOA->PIO_MDDR=~(AT91C_PA5_RXD0 | AT91C_PA6_TXD0);
	AT91C_BASE_PIOA->PIO_MDER=(AT91C_PA5_RXD0 | AT91C_PA6_TXD0);

	//to reset the transmitter and receiver and then disable them (later we are enabling it)
	AT91C_BASE_US0->US_CR= AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;

	//to configure the mode register(pg-421)
	AT91C_BASE_US0->US_MR= AT91C_US_PAR_NONE | AT91C_US_CHRL_8_BITS;

	//to disable all interupts
	AT91C_BASE_US0->US_IDR= 0xFFFF;//(we need rxrdy and txempty interupts that will be enabled later)

	//to generate baud rate (divisor CD=313) (baud rate=9600)
	AT91C_BASE_US0->US_BRGR= 0x139;

	//to disable unused registers
	AT91C_BASE_US0->US_RTOR= 0;
	AT91C_BASE_US0->US_TTGR= 0;
	AT91C_BASE_US0->US_FIDI= 0;
	AT91C_BASE_US0->US_IF= 0;

	//to enable the transmitter and receiver
	AT91C_BASE_US0->US_CR= AT91C_US_TXEN | AT91C_US_RXEN;
	AT91C_BASE_US0->US_IER= AT91C_US_TXRDY;
}

char uart0_getchar()//Receiver function
{
	char x;
	AT91C_BASE_US0->US_IER = AT91C_US_RXRDY;//Enabling the Receiver Ready
	AT91C_BASE_US0->US_IDR = ~AT91C_US_RXRDY;//Disabling all other interrupts
	// y u no haz empty loops?
	while(!(AT91C_BASE_US0->US_CSR & AT91C_US_RXRDY)); //loop runs till rxrdy==1
	x=AT91C_BASE_US0->US_RHR;
	return(x);

}

void uart0_putchar(char x)//Transmitter function
{
	AT91C_BASE_US0->US_IER= AT91C_US_TXRDY;//Enabling the Tx ready
	AT91C_BASE_US0->US_IDR= ~AT91C_US_TXRDY;//Disabling all other interrups
	//no loops and condition actually its working fine without any conditions
	AT91C_BASE_US0->US_THR=x;
}

void uart0_gets(char x[])//String receiving function .It can receive a string till Enter('\r') is pressed
{
	int i=-1;
	AT91C_BASE_US0->US_IER= AT91C_US_RXRDY;//Enabling the Receiver Ready
	AT91C_BASE_US0->US_IDR= ~AT91C_US_RXRDY;//Disabling all other interrupts

	/* TODO: checking page size here? */
	do {
		while(!(AT91C_BASE_US0->US_CSR & AT91C_US_RXRDY));//loop runs till rxrdy==1
		i++;
		x[i]=AT91C_BASE_US0->US_RHR;//character received is stored in x[i]
		if(x[i]=='\r')//checks whether the last character is Enter or not
			x[i]='\0';//if yes then replace is by null character (standard string ends with a null character
	} while ( x[i]!='\0' );//this checks the condition
}

void uart0_puts(char x[])//String Transmitting function.
{
	// use a pointer??? ; char take 1 byte. uint take 4 byte?
	unsigned int i = 0;
	while(x[i]!='\0')//transmitts character by character till null character is reached
	{
		while((AT91C_BASE_US0->US_CSR & AT91C_US_TXRDY)==AT91C_US_TXRDY)//checks whether the Transmitter is ready
		{
			AT91C_BASE_US0->US_THR = x[i];//transmitts a character
			i++;
		}
	}
}
/*
 * UART0 (end)
 */

/*
 * UART1
 */
void init_uart1()//uart1 initialisation
{
	// Enable the Clock of the PIO
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOB;

	// Configure the PIO Lines for LEDs as Outputs
	AT91C_BASE_PIOA->PIO_PER  = AT91B_LED_MASK;
	AT91C_BASE_PIOA->PIO_OER  = AT91B_LED_MASK;

	//to enable the peripheral clock for the uart (usart1)
	AT91C_BASE_PMC->PMC_PCER= (1 << AT91C_ID_US1);

	//configuring the i/o lines for uart

	//to disable the pins for gpio function
	AT91C_BASE_PIOA->PIO_PDR= AT91C_PIO_PA21 | AT91C_PIO_PA22;

	//to select peripheral A by A-Select-Register(pg-37)

	AT91C_BASE_PIOA->PIO_ASR= AT91C_PA21_RXD1 | AT91C_PA22_TXD1;

	//to disable peripheral B

	AT91C_BASE_PIOA->PIO_BSR=0;

	// Open drain
	AT91C_BASE_PIOA->PIO_MDDR=~(AT91C_PA21_RXD1 | AT91C_PA22_TXD1);
	AT91C_BASE_PIOA->PIO_MDER=(AT91C_PA21_RXD1 | AT91C_PA22_TXD1);

	//to reset the transmitter and receiver and then disable them (later we are enabling it)
	AT91C_BASE_US1->US_CR= AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;

	//to configure the mode register(pg-421)
	AT91C_BASE_US1->US_MR= AT91C_US_PAR_NONE | AT91C_US_CHRL_8_BITS;

	//to disable all interupts
	AT91C_BASE_US1->US_IDR= 0xFFFF;//(we need rxrdy and txempty interupts that will be enabled later)

	//to generate baud rate (divisor CD=313) (baud rate=9600)
	AT91C_BASE_US1->US_BRGR= 0x139;

	//to disable unused registers
	AT91C_BASE_US1->US_RTOR= 0;
	AT91C_BASE_US1->US_TTGR= 0;
	AT91C_BASE_US1->US_FIDI= 0;
	AT91C_BASE_US1->US_IF= 0;

	//to enable the transmitter and receiver
	AT91C_BASE_US1->US_CR= AT91C_US_TXEN | AT91C_US_RXEN;
	AT91C_BASE_US1->US_IER= AT91C_US_TXRDY;
}

char uart1_getchar()//Receiver function
{
	char x;
	AT91C_BASE_US1->US_IER= AT91C_US_RXRDY;//Enabling the Receiver Ready
	AT91C_BASE_US1->US_IDR= ~AT91C_US_RXRDY;//Disabling all other interrupts
	while(!(AT91C_BASE_US1->US_CSR & AT91C_US_RXRDY)); //loop runs till rxrdy==1
	x=AT91C_BASE_US1->US_RHR;
	return(x);
}

void uart1_putchar(char x)//Transmitter function
{
	AT91C_BASE_US1->US_IER= AT91C_US_TXRDY;//Enabling the Tx ready
	AT91C_BASE_US1->US_IDR= ~AT91C_US_TXRDY;//Disabling all other interrups
	//no loops and condition actually its working fine without any conditions
	AT91C_BASE_US1->US_THR=x;
}

void uart1_gets(char x[])//String receiving function .It can receive a string till Enter('\r') is pressed
{
	int i=-1;
	AT91C_BASE_US1->US_IER= AT91C_US_RXRDY;//Enabling the Receiver Ready
	AT91C_BASE_US1->US_IDR= ~AT91C_US_RXRDY;//Disabling all other interrupts

	do {
		while(!(AT91C_BASE_US1->US_CSR & AT91C_US_RXRDY)); //loop runs till rxrdy==1
		i++;
		x[i]=AT91C_BASE_US1->US_RHR;//character received is stored in x[i]
		if(x[i]=='\r')//checks whether the last character is Enter or not
			x[i]='\0';//if yes then replace is by null character (standard string ends with a null character
	} while(x[i]!='\0');//this checks the condition
}

void uart1_puts(char x[])//String Transmitting function.
{
	unsigned int i = 0;
	while(x[i]!='\0')//transmitts character by character till null character is reached
	{
		while((AT91C_BASE_US1->US_CSR & AT91C_US_TXRDY)==AT91C_US_TXRDY)//checks whether the Transmitter is ready
		{
			AT91C_BASE_US1->US_THR = x[i];//transmitts a character
			i++;
		}
	}
}
