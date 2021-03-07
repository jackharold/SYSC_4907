#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// Define the number of sensors starting at the chosen PORTC pin 
#define STARTING_US_POS (12)
#define US_SENSOR_NUM (1)

#define TRIGGER_INTERVAL (10)
#define CYCLE_INTERVAL (2 * 1000000)


#define PIN_TRIG						( 5 )
#define SET_TRIG(x)         if (x) {PTA->PSOR |= MASK( PIN_TRIG );} else {PTA->PCOR |= MASK( PIN_TRIG );}

extern float us_detections[US_SENSOR_NUM];

extern void init_ultrasonic_sensors(void);

extern void get_distance(void);

#endif