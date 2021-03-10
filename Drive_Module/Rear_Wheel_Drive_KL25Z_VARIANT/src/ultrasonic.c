#include <MKL25Z4.H>
#include "gpio_defs.h"
#include "serial.h"
#include "motor_control.h"
#include "ultrasonic.h" 

volatile unsigned state = 0;
float us_detections[US_SENSOR_NUM];


// Time tick counters
uint32_t base_tick;
uint32_t timer_tick;

// Timer tick flag
volatile uint8_t triggerActive = 0;
volatile uint8_t timerActive = 0;

void SysTickConfig(void)
{
  SysTick->LOAD  = (uint32_t)(SYSTICK_TMR_RELOAD_VAL);              /* set reload register */
  NVIC_SetPriority (SysTick_IRQn, 64);//(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
}



void init_ultrasonic_sensors(void) {
	
		
	SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK; /* enable clock for port A */
	
	for(int i=0; i < US_SENSOR_NUM; i++){
		// For each US sensor, set up the GPIO input configurations
		
		
		PORTA->PCR[STARTING_US_POS + i] |= PORT_PCR_MUX(1);
		PORTA->PCR[STARTING_US_POS + i] |= PORT_PCR_IRQC(0xb);
		
		
		PTD->PDDR &= ~MASK(STARTING_US_POS + i);
		
	}
	
	// Common Trigger
	PORTA->PCR[PIN_TRIG] |= PORT_PCR_MUX(1);
	PTA->PDDR = PTA->PDDR | MASK( PIN_TRIG );
	
	
	for(int i=0; i < US_SENSOR_NUM; i++){
		us_detections[i] = 0.00;
	}
	
	/* Enable Interrupts */
	NVIC_SetPriority(PORTA_IRQn, 64); 
	NVIC_ClearPendingIRQ(PORTA_IRQn); 
	NVIC_EnableIRQ(PORTA_IRQn);
	
	SysTickConfig();
}



void SysTick_Handler(void) {	
	// 1usec base tick
	++base_tick;
	
	//Reset Cycle
	if (triggerActive && base_tick >= TRIGGER_INTERVAL) {
		SET_TRIG(0)
		triggerActive = 0;
	}
	
	// Start New Cycle
	if (base_tick >= CYCLE_INTERVAL) {
		base_tick = 0;
		SET_TRIG(1)
		triggerActive = 1;
	}
	
	// Count if timer is active
	if (timerActive) {
		++timer_tick;
	}
	
}

void start_timer() {
	timer_tick = 0;
	timerActive = 1;
}

void stop_timer(){
	timerActive = 0;
}

int get_current_time() {
	return timer_tick;
}
