#ifndef INFARED_H
#define INFARED_H

// Define the number of sensors starting at the chosen PORTC pin 
#define STARTING_IR_POS (2)
#define SENSOR_NUM (2)

extern volatile int ir_detections[SENSOR_NUM];

extern void init_infared_sensors(void);

#endif