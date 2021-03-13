#include "mkl25z4.h"
#include "interrupts.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>


void init_serial(void) {
	
	/* Modify System Clock */
	
	// MCG_C1: CLKS (bit 7-6) = 00
	MCG->C1 |= MCG_C1_CLKS(0);  // Select PLL/FLL as clock source
	
	// MCG_C1: IREFS (bit 2)  = 1
	MCG->C1 |= MCG_C1_IREFS(1); // Select Inernal Reference clock
	                            // source for FLL
	
	// MCG_C4: DRST_DRS (bit 6-5) = 01
	MCG->C4 |= MCG_C4_DRST_DRS(1); // Select DCO range as Mid range
	                               // DCO generates the FLL Clock
	
	// MCG_C4: DMX32 (bit 7) = 1
	MCG->C4 |= MCG_C4_DMX32(1);    // Select DCO frequency as 48Mhz
	
	
	/* Configure UART0 */
	
	// Select MCGFLLCLK as UART0 clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	
	// Enable UART0 Clock
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	
	// Enable PORTA clock
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	// PTA1 is Rx, PTA2 is Tx
	PORTA->PCR[1] |= PORT_PCR_MUX(2); 
	PORTA->PCR[2] |= PORT_PCR_MUX(2); 
	
	// Configure Baud Rate as 9600
	UART0->BDL = 0x38;
	UART0->BDH = 0x1;
	
	// Configure Serial Port as 8-N-1
	// (8 data bits, No parity and 1 stop bit)
	UART0->C1 = 0x00;
	
	// Configure Tx/Rx Interrupts
	UART0->C2 |= UART_C2_TIE(0) | UART_C2_TCIE(0) | UART_C2_RIE(1);
	// Tx Interrupts disabled, Rx Interrupt enabled
	
	// Tx/Rx Enabled
	UART0->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;	 
	
	// Enable UART0 Interrupt
	__NVIC_EnableIRQ(UART0_IRQn);
}

void transmit_data(char *pdata) {
	// Wait until complete string is transmitted on serial port
	// and every byte is shifted out of Transmit buffer before
	// loading new byte
	while (*pdata) {
		UART0->D = *pdata;
		
		// Wait until byte is transmitted from Tx Buffer
		while (!(UART0->S1 & UART_S1_TDRE_MASK)) {}

		pdata++;
	}
	
	char* endChar = "\n";
	
	UART0->D = *endChar;
	
	// Wait until end byte is transmitted from Tx Buffer
	while (!(UART0->S1 & UART_S1_TDRE_MASK)) {}
	
}

void UART0_IRQHandler(void) {
	// Send New Data once value is recieved
	if ((UART0->S1 & UART_S1_RDRF_MASK)) {
		send_updated_data();
	}
}

