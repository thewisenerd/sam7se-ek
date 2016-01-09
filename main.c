#include <RTL.h>

#include "inc/AT91SAM7SE512.h"
#include "inc/AT91SAM7SE-EK.h"

int i = 0;
int j = 0;

OS_TID t_phaseA, t_phaseB, t_phaseC;

__task void phaseA (void) {
	while(1) {
		for (j = 0;j < 5; j++) {
			*AT91C_PIOA_CODR = AT91B_LED1;     /* Turn off LED's ("1")             */
			for (i = 0; i < 1000000; i++);
			*AT91C_PIOA_SODR = AT91B_LED1;     /* Turn off LED's ("1")             */
			for (i = 0; i < 1000000; i++);
		}
		os_tsk_delete_self();
	}
}

__task void phaseB (void) {
	while(1){
		for (j=0;j < 5; j++) {
			*AT91C_PIOA_CODR = AT91B_LED2;     /* Turn off LED's ("1")             */
			for (i = 0; i < 1000000; i++);
			*AT91C_PIOA_SODR = AT91B_LED2;     /* Turn off LED's ("1")             */
			for (i = 0; i < 1000000; i++);
		}
		os_tsk_delete_self();
	}
}

__task void phaseC (void) {
	while (1) {
		for (j = 0; j < 5; j++) {
			*AT91C_PIOA_CODR = AT91B_POWERLED;     /* Turn off LED's ("1")             */
			for (i = 0; i < 1000000; i++);
			*AT91C_PIOA_SODR = AT91B_POWERLED;     /* Turn off LED's ("1")             */
			for (i = 0; i < 1000000; i++);
		}
		os_tsk_delete_self();
	}
}

__task void init (void) {
	*AT91C_PMC_PCER = (1 << AT91C_ID_PIOA);/* Enable Clock for PIO             */

	*AT91C_PIOA_PER  = AT91B_LED_MASK;     /* Enable PIO for LED1..2           */
	*AT91C_PIOA_OER  = AT91B_LED_MASK;     /* LED1..2 are Outputs              */
	*AT91C_PIOA_SODR = AT91B_LED_MASK;     /* Turn off LED's ("1")             */

	*AT91C_PIOA_SODR = AT91B_POWERLED;

	t_phaseA = os_tsk_create (phaseA, 4);  /* start task phaseA                */
	t_phaseB = os_tsk_create (phaseB, 3);  /* start task phaseB                */
	t_phaseC = os_tsk_create (phaseC, 2);  /* start task phaseC                */

	os_tsk_delete_self ();
}

int main() {
	os_sys_init (init);
	return 0;
}
