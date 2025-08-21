/*
 * Copyright (c) 2025, Michal Chvatal
 * All rights reserved.
 *
 * This source code is licensed under the BSD 3-Clause License found in the
 * LICENSE file in the root directory of this source tree.
 */ 

// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include "functions.h"
	
/// Global ADC result value
volatile int ADCResult = 0;

/// Current state of the controller
volatile controller_states state = WAITING_FOR_TRIGGER;

/**
 * @brief Main program loop.
 * 
 * Initializes peripherals, enables interrupts, and starts ADC.  
 * Then remains in an infinite loop (logic runs in ISRs).
 */
int main(void)
{
	// Initialization functions
	PinsInit();
	// Enable interrupts
	sei(); 
	// Initial start of ADC (also necessary when running in free running mode)
	// (in free running mode ADC then starts automatically)
	ADCStart();
	
    while (1) 
    {
	}
}

/**
 * @brief ISR for zero-cross detection (INT0).
 * 
 * Interrupt service routine executed on the rising edge of pin PB1 (i.e., at zero crossing of the mains voltage)
 *
 * - Turns OFF triac.  
 * - Sets timer based on ADC value.  
 * - Starts next ADC conversion.  
 */
ISR (INT0_vect) 
{
	OPTOTRIAC_OFF;
	// Start the timer (based on the ADC setting), then output a 250 µs trigger pulse on pin PB1
	SetWaitingPulse(CalculateADCValue((unsigned)ADCResult));
	state = WAITING_FOR_TRIGGER;
    // Change the detection variable – signals zero crossing in the main loop
    // Get a new value from the ADC (free running mode is not used to avoid continuous ADC ISR calls)
	ADCStart();
}

/**
 * @brief ISR for ADC conversion complete.
 * 
 * Interrupt service routine executed every time an ADC conversion completes on pin PB3 (ADC3)
 * 
 */
ISR (ADC_vect)
{
	// Read necessary values from ADCL and ADCH registers (note that the read order is important!! ADCL must be read first!)
    // ADCResult ranges from 0 to 1023 (0-5V)
	volatile int ADCTempResult = ADCL;
	ADCResult = (ADCH << 8) + ADCTempResult;
}

/**
 * @brief ISR for Timer0 Compare Match A.
 * 
 * - Generates triac trigger pulse.  
 * - Turns triac OFF after pulse duration.  
 */
ISR (TIM0_COMPA_vect){
	if(state == WAITING_FOR_TRIGGER)
	{
		// Here the the trigger pulse is set and state is changed  to SWITCHING
		OPTOTRIAC_ON;
		state = SWITCHING;
		// Set trigger pulse timing
		SetTimer(8, 149);
	}
	else if (state == SWITCHING)
	{
		// Here the trigger pulse is turned off
		OPTOTRIAC_OFF;
	}
}