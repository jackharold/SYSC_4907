/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include "gpio_defs.h"
#include "LEDs.h"
#include "infared.h"
#include "ultrasonic.h"
#include "serial.h"
#include "interrupts.h"
#include "motor_control.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	
	init_serial();
	init_motor_control();
	init_infared_sensors();
	init_ultrasonic_sensors();
	init_RGB_LEDs();
	__enable_irq();
	
	transmit_data("InitComplete");
	//get_distance();
	
	while (1) {
		control_RGB_LEDs(ir_detections[0], ir_detections[1], 0);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
