/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include "gpio_defs.h"
#include "LEDs.h"
#include "infared.h"
#include "ultrasonic.h"
#include "serial.h"
//#include "logger.h"
#include "motor_control.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	
	init_infared_sensors();
	init_motor_control();
	init_RGB_LEDs();
	bsp_init();
	__enable_irq();
	
	//transmit_data("InitComplete\r\n");
	
	while (1) {
		control_RGB_LEDs(ir_detections[0], ir_detections[1], 0);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
