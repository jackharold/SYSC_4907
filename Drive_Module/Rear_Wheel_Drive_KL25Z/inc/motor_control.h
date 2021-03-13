#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

// Pin number on port A to watch
#define MOTOR_CONTROL (4)

// Pin Number of the test motors state on port C
#define MOTOR_OUT_START (10)

extern volatile int motor_state;

extern void init_motor_control(void);

extern void control_motor(int, int);

#endif