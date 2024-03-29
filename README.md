# SYSC_4907

## SYSC 4907 Self-Driving Vehicle Capstone Project

This repository was created to organize the programs necessary for Project Team 33 and 54 to create the *Perceptual Autonomous Vehicle Control System* capstone project.

### Contributors:
#### Team 33:
- Colin McKay, 4th Year Computer Systems Engineering
- Jack Harold, 4th Year Computer Systems Engineering
- Shan Rameshkanna, 4th Year Computer Systems Engineering
#### Team 54:
- Ragini Bakshi, 4th Year Electrical Engineering
- Ayokunle Ipinloju, 4th Year Electrical Engineering
- Mohammed Rabie, 4th Year Electrical Engineering

### Major catagories of the project repository:
---
#### Surface level programs
- central_code.py represents the main program executed on the central Raspberry Pi module used for navigation
- System_Command_Handler.py is used by the communication systems to parse incoming requests to the navigation module
---
#### /carleton_test_system
- This program is an old variant of the main program executed on the arduino module used for our tests in-person at Carleton
---
#### /Communication_Systems
- The Communication Client and Server and associated materials are present here
- This section also contains the navigational interface function for the infrared sensor data collection module
---
#### /Drive_Module
- The FRDM KL25Z IR sensor data collection program is contained here along with a variant option that was briefly tested in hope of integrating ultrasonic sensors
---
#### /full_arduino_system
- The main program included here (full_arduino_system.ino) is the latest version of the main program executed on the arduino module
- The other program (highNavDataRate_corrections.ino) is executed by the GPS navigation module
---
#### /tests
- This section contains python scripts used to test the vehicle
#### /Angle
-This code segment written for analog to digital conversion of phase angle readings from phase detector.
-the code outputs orientation angle of the vehicle in real-time
#### /steer_drive_us_lka-ir
-This is the updated test phase code for complete autonomous drive of the car with AoA self parking - direct arduino controlled.
-The code segments will be used to program the centralized Pi unit so the Arduino can receive commands from the Pi.
