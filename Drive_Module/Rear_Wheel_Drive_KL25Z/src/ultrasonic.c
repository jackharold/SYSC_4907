#include <MKL25Z4.H>
#include "gpio_defs.h"
#include "serial.h"
#include "motor_control.h"
#include "ultrasonic.h" 

volatile unsigned state = 0;
volatile float us_detections[US_SENSOR_NUM];
int US_SENSOR_POSITIONS[] = {12, 13, 16, 17};

int triggerTimerActive = 0;
int cycleTimerActive = 0;



void Init_PIT(unsigned period_us_0, unsigned period_us_1) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period_us_0*24); // 24 MHz clock frequency

	// Initialize PIT1 to count down from argument 
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(period_us_1*24); // 24 MHz clock frequency

	
	// No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
	PIT->CHANNEL[1].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	

	/* Enable Interrupts */ 
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
}


void Start_PIT(int timer) {
// Enable counter
	PIT->CHANNEL[timer].LDVAL = PIT_LDVAL_TSV( ( (timer)?(TRIGGER_INTERVAL):((CYCLE_INTERVAL) ) *24 )); 
	(timer) ? (triggerTimerActive = 1): (cycleTimerActive = 1);
	
	PIT->CHANNEL[timer].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void Stop_PIT(int timer) {
// Disable counter
	PIT->CHANNEL[timer].TCTRL &= ~PIT_TCTRL_TEN_MASK;
	
	PIT->CHANNEL[timer].LDVAL = PIT_LDVAL_TSV( ( (timer)?(TRIGGER_INTERVAL):((CYCLE_INTERVAL) ) *24 ));
	(timer) ? (triggerTimerActive = 0): (cycleTimerActive = 0);
}


void PIT_IRQHandler() {

	//clear pending IRQ
	NVIC_ClearPendingIRQ(PIT_IRQn);
	
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
		
		Stop_PIT(0);
		//PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(CYCLE_INTERVAL*24);
		
	}
	if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
		
		//PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(CYCLE_INTERVAL*24);
		//Stop_PIT(1);
		
		//SET_TRIG(1);
		Start_PIT(0);
		//get_distance();
	} 
}


void init_ultrasonic_sensors(void) {
	
	// TPM Init
	MCG->C1 |= 0x1;
	MCG->C2 |= MCG_C2_IRCS_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x01);
	SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL(0x1);
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	TPM0->SC &= ~TPM_SC_CMOD(0x3);
	TPM0->MOD = 65535;
	TPM0->CNT = 0;
	TPM0->SC &= ~TPM_SC_PS(0x1);
	
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK; /* enable clock for port A */
	
	for(int i=0; i < US_SENSOR_NUM; i++){
		// For each US sensor, set up the GPIO input configurations
		
		
		PORTA->PCR[ US_SENSOR_POSITIONS[i] ] |= PORT_PCR_MUX(1);
		PORTA->PCR[ US_SENSOR_POSITIONS[i] ] |= PORT_PCR_IRQC(0xb);
		
		PTD->PDDR &= ~MASK( US_SENSOR_POSITIONS[i] );
		
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
	
	//Init_PIT(TRIGGER_INTERVAL, CYCLE_INTERVAL);
	
}

void Delay (int dly) {
  volatile uint32_t t;

	for (t=(uint32_t)dly*10000; t>0; t--)
		;
}

void get_distance (void) {
  SET_TRIG(1);
	Delay(100);
	//Start_PIT(0);
	//while(triggerTimerActive);
	SET_TRIG(0);
}



