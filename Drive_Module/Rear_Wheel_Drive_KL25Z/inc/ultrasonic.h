#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// Define the number of sensors starting at the chosen PORTA pin 
#define US_SENSOR_NUM (0)

#define TRIGGER_INTERVAL (100)
#define CYCLE_INTERVAL (0.5 * 1000000)

#define PIN_TRIG						( 5 )
#define SET_TRIG(x)         if (x) {PTA->PSOR |= MASK( PIN_TRIG );} else {PTA->PCOR |= MASK( PIN_TRIG );}

extern int US_SENSOR_POSITIONS[];

extern volatile float us_detections[US_SENSOR_NUM];

extern void init_ultrasonic_sensors(void);

extern void get_distance(void);

extern void Delay(int);

#endif