#ifndef INFARED_H
#define INFARED_H

// Define the number of sensors starting at the chosen PORTD pin 
#define STARTING_IR_POS (2)
#define IR_SENSOR_NUM (3)

extern volatile int ir_detections[IR_SENSOR_NUM];

extern void init_infared_sensors(void);

extern int ir_detected_flag;

#endif