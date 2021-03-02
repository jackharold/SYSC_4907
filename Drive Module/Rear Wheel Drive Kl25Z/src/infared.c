#include <MKL25Z4.H>
#include "LEDs.h"
#include "gpio_defs.h"
#include "motor_control.h"
#include "infared.h"

volatile int ir_detections[SENSOR_NUM];
int ir_detected_flag = 0;
	
void init_infared_sensors(){
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */
	
	for(int i=0; i < SENSOR_NUM; i++){
		// For each IR sensor, set up the GPIO input configurations
		// Current Config: GPIO Input, Pull up
		
		PORTD->PCR[STARTING_IR_POS + i] &= ~PORT_PCR_MUX_MASK;          
		PORTD->PCR[STARTING_IR_POS + i] |= (PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x0b) ); 
		
		PTD->PDDR &= ~MASK(STARTING_IR_POS + i);
		
	}
	
	/* Enable Interrupts */
	NVIC_SetPriority(PORTD_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTD_IRQn); 
	NVIC_EnableIRQ(PORTD_IRQn);
	
	for(int i=0; i < SENSOR_NUM; i++){
		ir_detections[i] = 0;
	}
	
}

void emergency_maneuver(volatile int values[SENSOR_NUM]) {
	//toggle_RGB_LEDs(0,0, values[0] & values[1]);
	
	if(values[0] & values[1]){
		stop_motor();
	}
}


void PORTD_IRQHandler(void) { 
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	
	for(int i=0; i < SENSOR_NUM; i++){
		
		if ((PORTD->ISFR & MASK(STARTING_IR_POS + i))) {
			
			//Set flag
			ir_detected_flag = 1;
			
			// Flip on an Edge Detection
			ir_detections[i] = !ir_detections[i];
			
			PORTD->ISFR |= MASK(STARTING_IR_POS + i);
		}
	}
	if (ir_detected_flag){
		
		emergency_maneuver(ir_detections);
		
		ir_detected_flag = 0;
	}
	
	
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
}*/