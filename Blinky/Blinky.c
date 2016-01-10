/******************************************************************************/
/* BLINKY.C: LED Flasher                                                      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
                  
#include <AT91SAM7SE512.H>              /* AT91SAM7SE512 definitions          */
#include "..\AT91SAM7SE-EK.h"           /* AT91SAM7SE-EK board definitions    */

#include "uart.h"

AT91S_USART  *_usart_in_device = AT91C_BASE_US0;
AT91S_USART *_usart_out_device = AT91C_BASE_US0;

//int rgb[1024][1024][3];
int x = 0;
int y = 0;
int i = 0;

char buf[24000];

void readRGBImage(void);
void writeRGBImageasYCbCr(void);

/*
 * Main Program
 */

int main (void) {
  // Enable the Clock of the PIO
  AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;

	ConfigureUsart0();
	ConfigureUsart1();
	
	printf("READY TO RECEIVE\n");

	set_printf_us(AT91C_BASE_US1);
	set_scanf_us(AT91C_BASE_US1);
	// Loop forever

	printf("READY TO RECEIVE\n");

	while(1) {
		// ignore first two 255's
		USART_read(AT91C_BASE_US1);
		USART_read(AT91C_BASE_US1);

		for (i = 0;i < 10000; i++) {
			buf[i] = USART_read(AT91C_BASE_US1);
		}

		set_printf_us(AT91C_BASE_US0);
		for (y = 0; y < 10000; y++) {
			printf("%u ", buf[y]);
		}
		/*
			if (scanf("%c", &c) == 1) {
				if (c == 'r') {
					readRGBImage();
				} else if (c == 'w') {
					writeRGBImageasYCbCr();
				}
			} else {
				; // wait for cmd
			}
		}
		*/
	} // while
} //main

/*
 * rgb[1024][1024][3]
 * x
 * y
 */
void readRGBImage(void) {
	//rgb[x][y][0]
}

void writeRGBImageasYCbCr() {
	
}
