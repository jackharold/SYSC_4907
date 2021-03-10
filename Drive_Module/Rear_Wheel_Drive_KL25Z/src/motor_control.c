#include <MKL25Z4.H>
#include "gpio_defs.h"
#include "infared.h"
#include "motor_control.h"

volatile int motor_state = 0;

void control_motor(int, int);


void init_motor_control(){
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK; /* enable clock for ports A and C */
	
	for(int i=0; i < IR_SENSOR_NUM; i++){
		// Enable motor out Pins on Port C as output
		PORTC->PCR[MOTOR_OUT_START + i] &= ~PORT_PCR_MUX_MASK;          
		PORTC->PCR[MOTOR_OUT_START + i] |= PORT_PCR_MUX(1);   
		PTC->PDDR |= MASK(MOTOR_OUT_START + i);
	}
	
	for (int i=0; i < IR_SENSOR_NUM; i++){
			control_motor(i, 0);
	}
	
	
	PORTA->PCR[MOTOR_CONTROL] &= ~PORT_PCR_MUX_MASK;          
	PORTA->PCR[MOTOR_CONTROL] |= (PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x09) ); 
		
	PTA->PDDR &= ~MASK(MOTOR_CONTROL);
	
	/* Enable Interrupts */
	NVIC_SetPriority(PORTA_IRQn, 64); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTA_IRQn); 
	NVIC_EnableIRQ(PORTA_IRQn);
}

// Change Motor state to On or Off
void control_motor(int control, int state){
	
	if (!!state){
		PTC->PSOR |= MASK(MOTOR_OUT_START + control);
	} else {
		PTC->PCOR |= MASK(MOTOR_OUT_START + control);
	}
	
}

