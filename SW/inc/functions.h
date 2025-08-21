/*
 * Copyright (c) 2025, Michal Chvatal
 * All rights reserved.
 *
 * This source code is licensed under the BSD 3-Clause License found in the
 * LICENSE file in the root directory of this source tree.
 */ 

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
	#include <avr/io.h>
	#include <avr/interrupt.h>

	// Definitions for digital output (used for optocoupler / optotriac)
	#define OPTOTRIAC_OFF PORTB   &= ~(1 << PB0)
	#define OPTOTRIAC_ON PORTB    |= (1 << PB0)
	#define OPTOTRIAK_TOGGLE PINB |= (1 << PINB0)

	#define TIMER_STOP TCCR0B    &= ~((1 << CS00) | (1 << CS01) | (1 << CS02))
	#define TIMER_INT_ON TIMSK0  |= (1 << OCIE0A)
	#define TIMER_INT_OFF TIMSK0 &= ~(1 << OCIE0A)

	// Definitions for CalculateADCValue() function
	#define UPPER_THRESHOLD_VALUE 941	// 941 corresponds to 4.6 V
	#define LOWER_THRESHOLD_VALUE 220 	// Can be adjusted based on oscilloscope measurement
	#define MAX_ADC_VALUE         1023	// Maximum value of a 10-bit ADC (at 5V)
	#define MIN_ADC_VALUE         205	// Expected minimum voltage value with 100 kOhm potentiometer = 1 V
	#define ADC_RANGE_VALUE (MAX_ADC_VALUE - MIN_ADC_VALUE)

	// Defines for the SetWaitingPulse() function
	#define ZERO_CROSS_DELAY_US             1000  // Time delay from the zero-detect pulse to the actual zero crossing (1000 µs)
	#define ZERO_CROSS_DELAY_OCR0A_PRESC_64 74	  // Time delay from pulse for OCR0A with prescaler 64 (corresponds to 1000 µs)
	#define TRIGGER_PULSE_DURATION_PRESC_8  149	  // Duration of the trigger pulse with prescaler 8 (250 µs)
	#define HALF_PERIOD_DURATION_US         10000 // Duration of half the AC period (in µs) 

	// Initialization functions
	void PinsInit(void);
	void ZeroDetectorInputInit(void);
	void OptotriacOutputInit(void);
	void ADCInit(void);
	void TimerInit (void);
	
	// Run-time funkce
	void ADCStart(void);
	void SetWaitingPulse (unsigned percent);
	unsigned CalculateADCValue(unsigned ADCValue);
	char CalculateRegisterValue(unsigned prescaler, unsigned time);
	void SetTimer (unsigned prescaler, char OCValue);

	typedef enum {
		WAITING_FOR_TRIGGER, // Indicates the state where the timer is running, waiting based on the ADC value
		SWITCHING,			 // Indicates the state where the timer is running, generating the trigger pulse for the optotriac
	}controller_states;
		
#endif /* FUNCTIONS_H_ */