#include <stdint.h>
#include "MKL25Z4.h"

static uint8_t is_menu_displayed; /* Flag indicating menu status */
static uint8_t logging_started;   /* Flag indicating log start/stop status */
volatile char user_input_key;     /* User input key read from serial port*/
static uint8_t do_log_count;      /* 5sec periodic logging counter */

// Local function prototypes
static void display_menu(void);
static void transmit_data(char *pdata);

void log_serial_data(void)
{
	transmit_data("Test Message\r\n");
	
	// Display menu on power-up
	if (!is_menu_displayed)
	{
		display_menu();
		is_menu_displayed = 1;
	}
	
	// Start/Stop logging based on user input
	if (user_input_key == '1')
	{
		if (!logging_started)
		{
			transmit_data("Logging Started\r\n");
			logging_started = 1;
		}
	}
	else if (user_input_key == '2')
	{
		if (logging_started)
		{
			transmit_data("Logging Stopped\r\n");
			logging_started = 0;
		}
	}
	else
	{
		// Don't respond for invalid entry
	}
	
	// Do periodic logging (every 5sec)
	if (logging_started)
	{
		do_log_count++;
		if (do_log_count >= 5)
		{
			do_log_count = 0;
			transmit_data("My UART\r\n");
		}
	}
}

static void display_menu(void)
{
	transmit_data("1. Start Logging\r\n");
	transmit_data("2. Stop  Logging\r\n");
	transmit_data("Press 1 or 2 to start/stop logging at any time...\r\n");
}

static void transmit_data(char *pdata)
{
	// Wait until complete string is transmitted on serial port
	// and every byte is shifted out of Transmit buffer before
	// loading new byte
	while (*pdata)
	{
		__disable_irq();
		UART0->D = *pdata;
		
		// Wait until byte is transmitted from Tx Buffer
		while (!(UART0->S1 & UART_S1_TDRE_MASK)) { 
		}
		__enable_irq();

		pdata++;
	}
}
