#include <MKL25Z4.H>
#include "LEDs.h"
#include "gpio_defs.h"
#include "serial.h"
#include "motor_control.h"
#include "infared.h"

volatile int ir_detections[IR_SENSOR_NUM];
int ir_detected_flag = 0;
	
void init_infared_sensors(){
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */
	
	for(int i=0; i < IR_SENSOR_NUM; i++){
		// For each IR sensor, set up the GPIO input configurations
		// Current Config: GPIO Input, Pull up
		
		PORTD->PCR[STARTING_IR_POS + i] &= ~PORT_PCR_MUX_MASK;          
		PORTD->PCR[STARTING_IR_POS + i] |= (PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x0b) ); 
		
		PTD->PDDR &= ~MASK(STARTING_IR_POS + i);
		
	}
	
	for(int i=0; i < IR_SENSOR_NUM; i++){
		ir_detections[i] = 0;
	}
	
	
	/* Enable Interrupts */
	NVIC_SetPriority(PORTD_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTD_IRQn); 
	NVIC_EnableIRQ(PORTD_IRQn);
	
}

