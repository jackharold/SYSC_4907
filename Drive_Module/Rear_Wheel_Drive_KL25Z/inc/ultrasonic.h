#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// Define the number of sensors starting at the chosen PORTC pin 
#define STARTING_US_POS (2)
#define US_SENSOR_NUM (1)

#define PIN_TRIG						( 4 )
#define PIN_ECHO						( 12 )
#define SET_TRIG(x)         if (x) {PTA->PSOR = MASK( PIN_TRIG );} else {PTA->PCOR = MASK( PIN_TRIG );}
#define GET_ECHO_IN         ((PTA->PDIR & MASK( PIN_ECHO )) >> PIN_ECHO)

extern volatile int us_detections[US_SENSOR_NUM];

extern void init_ultrasonic_sensors(void);

#endif