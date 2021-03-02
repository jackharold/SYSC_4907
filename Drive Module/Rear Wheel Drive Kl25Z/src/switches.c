#include <MKL25Z4.H>
#include "switches.h"
#include "LEDs.h"

volatile unsigned count=0;

void init_switch(void) {
	SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */

	/* Select GPIO and enable pull-up resistors and interrupts 
		on falling edges for pins connected to switches */
	PORTD->PCR[SW_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);
	
	/* Set port D switch bit to inputs */
	PTD->PDDR &= ~MASK(SW_POS);

	/* Enable Interrupts */
	NVIC_SetPriority(PORTD_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTD_IRQn); 
	NVIC_EnableIRQ(PORTD_IRQn);
}

/*void PORTD_IRQHandler(void) {  
	DEBUG_PORT->PSOR = MASK(DBG_ISR_POS);
	// clear pending interrupts
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	if ((PORTD->ISFR & MASK(SW_POS))) {
		count++;
	}
	// clear status flags 
	PORTD->ISFR = 0xffffffff;
	DEBUG_PORT->PCOR = MASK(DBG_ISR_POS);
}*/
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
