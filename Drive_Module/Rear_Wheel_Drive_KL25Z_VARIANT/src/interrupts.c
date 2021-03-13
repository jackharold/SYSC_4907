#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>
#include "gpio_defs.h"
#include "serial.h"
#include "motor_control.h"
#include "ultrasonic.h" 
#include "infared.h"
#include "interrupts.h"


int us_active = 0;
int us_detected_count = 0;


void emergency_maneuver(volatile int values[IR_SENSOR_NUM]) {
	
	if(values[0] & values[1]){
		control_motor(0);
	}
}

void send_updated_data() {
	
	char data[2+IR_SENSOR_NUM+US_SENSOR_NUM];
		
		// Add Infared Data to the sent datastream 
		data[0] = 'I';
	
		for(int i=0; i < IR_SENSOR_NUM; i++){ 
			sprintf(&data[i + 1], "%d", ir_detections[i]);
		}
		
		// Add Ultrasonic Data after the IR data 
		data[IR_SENSOR_NUM + 1] = 'U';
		
		int lastDataSize = 1;
		
		for(int i=0; i < (US_SENSOR_NUM); i++) {//(sizeof(roundf(us_detections[i]*100)/100)) ){ 
			sprintf(&data[i + IR_SENSOR_NUM + 2], "%.2f", us_detections[i]);
			
		}

		transmit_data(data);

}

// Port A Handler used by the US Sensor and the Motor Control
void PORTA_IRQHandler(void) {
	
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	
	/* Motor Control Segment */
	if (PORTA->ISFR & MASK(MOTOR_CONTROL)) {
		
		control_motor(1);
		
		PORTA->ISFR |= MASK(MOTOR_CONTROL);
	}
	
	
	/* Ultrasonic Segment */
	for(int i=0; i < US_SENSOR_NUM; i++){
	
		if ((PORTA->ISFR & MASK(STARTING_US_POS + i))) {
			float timeInterval = get_current_time();
			// Update time for this sensor
			us_detected_count += 1;
			
			if (us_active) {
				us_detections[i] = timeInterval/(1);
				//us_detections[i] = (float) ((uint16_t)TPM0->CNT / (12*58)) ;
				//us_detections[i] = (float) ((uint16_t)TPM0->CNT / (29 / 2)) ;
			}
			
			PORTA->ISFR |= MASK(STARTING_US_POS + i);
			
			
			if (us_detected_count >= US_SENSOR_NUM){
				// Reset timer when all sensors close
				if (us_active) {
					
					stop_timer();
					
					us_detected_count = 0;
					us_active = 0;
					
					send_updated_data();
					
				} else {
					// Initialize the time once all sensors have started
					
					start_timer();
					
					us_detected_count = 0;
					us_active = 1;
				}	
			}
			
		}
	}

}

// Port D Handler used by the IR Sensor
void PORTD_IRQHandler(void) { 
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	
	for(int i=0; i < IR_SENSOR_NUM; i++){
		
		if ((PORTD->ISFR & MASK(STARTING_IR_POS + i))) {
			
			//Set flag
			ir_detected_flag = 1;
			
			// Flip on an Edge Detection (Inverted signal is due to the Sensor configuration)
			ir_detections[i] = !(PTD->PDIR & MASK(STARTING_IR_POS + i));
			
			PORTD->ISFR |= MASK(STARTING_IR_POS + i);
		}
	}
	if (ir_detected_flag){
		
		emergency_maneuver(ir_detections);
		
	  send_updated_data();
		
		// Reset Flag
		ir_detected_flag = 0;
	}
	
}

