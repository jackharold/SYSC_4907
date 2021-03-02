#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

// Pin number on port A to watch
#define MOTOR_CONTROL (4)

// Pin Number of the test motor state on port C
#define MOTOR_OUT (13)

extern volatile int motor_state;

extern void init_motor_control(void);

extern void stop_motor(void);

#endif