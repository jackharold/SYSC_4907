#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// Define the number of sensors starting at the chosen PORTC pin 
#define STARTING_US_POS (12)
#define US_SENSOR_NUM (1)

#define TRIGGER_INTERVAL (10* 10UL)
#define CYCLE_INTERVAL (1 * 1000000UL)

#define SYSTEM_CLOCK_FREQ      48000000UL  // 48 Mhz
#define SYTICK_TIME_INTERVAL   1           // 1 usec
#define SYTICK_TIME_FREQ       (SYTICK_TIME_INTERVAL * 1000000UL)
#define SYSTICK_TMR_RELOAD_VAL (SYSTEM_CLOCK_FREQ / SYTICK_TIME_FREQ - 1UL) // 48 - 1



#define PIN_TRIG						( 5 )
#define SET_TRIG(x)         if (x) {PTA->PSOR |= MASK( PIN_TRIG );} else {PTA->PCOR |= MASK( PIN_TRIG );}

extern float us_detections[US_SENSOR_NUM];

extern void init_ultrasonic_sensors(void);

extern void start_timer();

extern void stop_timer();

extern int get_current_time(void);

#endif