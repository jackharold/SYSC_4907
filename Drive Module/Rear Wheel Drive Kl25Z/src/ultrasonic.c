#include <MKL25Z4.H>
#include "gpio_defs.h"


volatile unsigned state = 0;
volatile int us_detections[US_SENSOR_NUM];
int us_detected_flag = 0;


void init_ultrasonic (void) {
	
	// TPM Init
	MCG->C1 |= 0x3;
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x03);
	TPM0->SC = TPM_SC_CMOD(0x0);
	TPM0->MOD = 65535;
	TPM0->CNT = 0;
	TPM0->SC = TPM_SC_PS(2);
	
	
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK; /* enable clock for port A */
	
	for(int i=0; i < US_SENSOR_NUM; i++){
		// For each IR sensor, set up the GPIO input configurations
		// Current Config: GPIO Input, Pull up
		
		
		PORTA->PCR[PIN_ECHO + i] |= PORT_PCR_MUX(1);
		PORTA->PCR[PIN_ECHO + i] |= PORT_PCR_IRQC(0xa);
		
		
		PTD->PDDR &= ~MASK(STARTING_US_POS + i);
		
	}
	
	// Common Trigger
	PORTA->PCR[PIN_TRIG] |= PORT_PCR_MUX(1);
	PTA->PDDR = PTA->PDDR | MASK( PIN_TRIG );
	
	
	/* Enable Interrupts */
	NVIC_SetPriority(PORTA_IRQn, 128); 
	NVIC_ClearPendingIRQ(PORTA_IRQn); 
	NVIC_EnableIRQ(PORTA_IRQn);
	
	for(int i=0; i < US_SENSOR_NUM; i++){
		us_detections[i] = 0;
	}
	
}

void Delay (uint32_t dly) {
  volatile uint32_t t;

	for (t=dly*10000; t>0; t--)
		;
}

void get_distance (void) {
  SET_TRIG(1);
	Delay(1);
	SET_TRIG(0);
}

void PORTA_IRQHandler(void) {
	char str[50];
	uint16_t distance;
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	if (PORTA->ISFR & MASK( PIN_ECHO )){
		PORTA->ISFR &= ~(MASK( PIN_ECHO ));
		if (state == 0){
			TPM0->CNT = 0;
			TPM0->SC |= TPM_SC_CMOD(1);
			state = 1;
		}
		else {
			TPM0->SC |= TPM_SC_CMOD(0);
			state = 0;
			
			distance = (uint16_t)TPM0->CNT / 29 / 2 ;
		}
	}
	PORTA->ISFR = 0;
}
