/*
 * Copyright (c) 2025, Michal Chvatal
 * All rights reserved.
 *
 * This source code is licensed under the BSD 3-Clause License found in the
 * LICENSE file in the root directory of this source tree.
 */ 

#include "functions.h"

/**
 * @brief Configure pin PB0 for optotriac output.
 * 
 * Sets pin 5 (PB0) as output and initializes the optotriac to OFF state.
 */
void OptotriacOutputInit(void)
{
	DDRB |= (1 << DDB0);
	OPTOTRIAC_OFF;
}

/**
 * @brief Initialize ADC for potentiometer on pin PB3 (ADC3).
 * 
 * - Disables digital input buffer for power saving.  
 * - Configures ADC3 channel.  
 * - Enables ADC and ADC interrupts.  
 */
void ADCInit(void) 
{
	// PIN PB3 is set as input after reset => DDR bit = 0 and pull-up resistor is disabled => PORT bit = 0
    // This is suitable, so leave it as is.
    // Disable digital input buffer on this pin (better for power consumption)
	DIDR0 |= (1 << ADC3D);
	// Select pin ADC3 (PB3) => MUX0 = 1 and MUX1 = 1, select Vcc as reference => REFS0 = 0, left adjust result disabled => ADLAR = 0
	ADMUX |= (1 << MUX0) | (1 << MUX1);
	// Enable ADC, enable auto-triggering, and enable interrupt for this peripheral
    // ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE);
    // Enable ADC and enable interrupt for this peripheral
	ADCSRA |= (1 << ADEN) | (1 << ADIE);
	// ADCSRB - mode selection (free running mode — We leave it because 0 0 0 means free running mode)
			
	// If free running mode is enabled, the first conversion cycle must be started by setting ADSC = 1, which performs initialization, then we don't do it anymore
}

/**
 * @brief Configure pin for zero-cross detection of mains voltage.
 * 
 * Enables external interrupt on rising edge of PB1.
 */
void ZeroDetectorInputInit(void)
{
    // PIN PB1 is set as input after reset => DDR bit = 0 and pull-up resistor is disabled => PORT bit = 0
    // This is suitable, so leave it as is.
    // Configure interrupt for pin PB1 (INT0) on rising edge and enable it:
	MCUCR |= (1 << ISC00) | (1 << ISC01);  // ISC00 = 1 and ISC01 = 1 - sets interrupt on rising edge
	GIMSK |= (1 << INT0);				   // INT0 = 1 - enables external interrupt on pin PB1
}

/**
 * @brief Start the first ADC conversion.
 */
void ADCStart(void)
{
	// If free running mode is enabled, the first conversion cycle must be started by setting ADSC = 1,
    // as this performs initialization; after that, it runs automatically
	ADCSRA |= (1 << ADSC);
}

/**
 * @brief Initialize all necessary pins and peripherals.
 */
void PinsInit(void)
{
	OptotriacOutputInit();
	ZeroDetectorInputInit();
	ADCInit();
}

void TimerInit(void)
{
    // Set normal mode (i.e., OC0A pin is disconnected — this is the default setting in register TCCR0A, so no changes needed)

    // Selection of prescaler (CS00, CS01, CS02) and writing to the Output Compare Register (OCR0A) is done at run time!

    /*
     * Calculations:
     * Note - The timer is 8-bit, so the maximum value that can be written to OCR0A must be considered!! (i.e., 0xFF)
     * Desired timer time:
     * Timer clock frequency [Hz] = Input clock frequency [Hz] / Prescaler;
     * OCR0A = Timer clock frequency [Hz] * Desired time [s] - 1;
     * Desired time [s] = 1 / Desired frequency [Hz] = (OCR0A + 1) / Timer clock frequency [Hz];
     *
     * Values for 250 µs pulse (the duration of the trigger pulse sent to the optotriac)
     * I/O Clock frequency will be 4.8 MHz
     * Timer clock frequency [Hz] = 4,800,000 [Hz] / 8 = 600,000 [Hz];
     * OCR0A = 600,000 [Hz] * 0.000250 [s] - 1 = 149;
     * => For a 250 µs pulse, a prescaler of 8 and an OCR0A register value of 149 are required
     *
     * Notes on 4.8 MHz as clock source:
     * With prescaler 8, the maximum measurable time is 425 µs
     * With prescaler 64, the maximum measurable time is 3.4 ms
     * With prescaler 256, the maximum measurable time is 13.6 ms
	 */
	return;
}

/**
 * @brief Configure Timer0 with given prescaler and OCR0A value.
 * 
 * @param prescaler Timer prescaler (8, 64, 256).
 * @param OCValue OCR0A register value.
 */
void SetTimer(unsigned prescaler, char OCValue)
{
	// Disable interrupts of the running timer
	TIMER_INT_OFF;
	// Stop the running timer (reset the clock signal)
	TIMER_STOP;
	switch(prescaler){
		case 8:
			// Set the input clock (prescaler) to 8 (0 1 0) - this enables the timer function
            // With prescaler 8, the maximum measurable time is 425 µs
			TCCR0B |= (1 << CS01);
			break;
		case 64:
			// With prescaler 64, the maximum measurable time is 3.4 ms
            // Set the input clock (prescaler) to 64 (0 1 1) - this enables the timer function
			TCCR0B |= (1 << CS00) | (1 << CS01);
			break;
		case 256:
			// With prescaler 256, the maximum measurable time is 13.6 ms
            // Set the input clock (prescaler) to 256 (1 0 0) - this enables the timer function
			TCCR0B |= (1 << CS02);
			break;
	}
    // Clear interrupt flags
	TIFR0 |= (1 << OCF0A);
	// Set Output Compare Register (current timer value + desired value (e.g., 1ms) => 149)
	OCR0A = TCNT0 + OCValue;
	// Enable interrupt on output compare match
	TIMSK0 |= (1 << OCIE0A);
}

/**
 * @brief Configure delay and trigger pulse after zero-cross event.
 * 
 * @param percent Power level (0–100%). 0% = always off, 100% = always on.
 */
void SetWaitingPulse(unsigned percent)
{
	/* Calculations:
     * Maximum voltage 5 V
     * Minimum voltage 1 V
     * Range 4 V
     * 10% of range is 1.4 V
     * 90% of range is 4.6 V
     */
	
	if (percent == 100)
	{
		SetTimer(64, (char)ZERO_CROSS_DELAY_OCR0A_PRESC_64);
	}
	else if(percent == 0)
	{
        // Disable interrupts of the running timer
		TIMER_INT_OFF;
        // Stop the running timer (reset the clock signal)
		TIMER_STOP;
	}
	else
	{
        // Calculate delay time in microseconds
		// volatile unsigned timeDelay = HALF_PERIOD_DURATION_US - (((HALF_PERIOD_DURATION_US / 100) * percent) + ZERO_CROSS_DELAY_US);
		volatile unsigned timeDelay = ((unsigned)HALF_PERIOD_DURATION_US - ((((unsigned)HALF_PERIOD_DURATION_US / 100) * percent) + (unsigned)ZERO_CROSS_DELAY_US));
		if (timeDelay < 425)
		{
			SetTimer(8, CalculateRegisterValue(8, timeDelay));
		}
		else if(timeDelay < 3400)
		{
			SetTimer(64, CalculateRegisterValue(64, timeDelay));
		}
		else {
			SetTimer(256, CalculateRegisterValue(256, timeDelay));
		}
	}
}

/**
 * @brief Map ADC value to percentage (0–100%).
 * 
 * @param ADCValue Raw 10-bit ADC value (0–1023).
 * @return unsigned Calculated percentage (0–100%).
 */
unsigned CalculateADCValue(unsigned ADCValue)
{
	/* ADC Range:
     * Max 5 V = 1023 = 100%
     * Min 0 V = 0
     * Actual min 1 V = 205 = 0%
     * 10% of range is 1.4 V => 287     (5 - 1 = 4 V => 4 * 0.1 = 0.4 => 0.4 + 1 = 1.4)
     * 90% of range is 4.6 V => 941
     */
	
	if (ADCValue > ((unsigned)UPPER_THRESHOLD_VALUE))
	{
		// If value is greater than 90%, automatically set to 100% (Always ON)
		return 100;
	 }
	else if(ADCValue < ((unsigned)LOWER_THRESHOLD_VALUE))
	{
		// If value is less than 10%, automatically set to 0% (Always OFF)
		return 0;
	}
	else
	{
		// If value is between 10% and 90%, calculate the corresponding percentage
		volatile unsigned long a = ((unsigned long)ADCValue - (unsigned long)MIN_ADC_VALUE) * 100 / ((unsigned long)ADC_RANGE_VALUE);
		return((unsigned)a);
	}
}


/**
 * @brief Compute the value to be written into the OCR0A register   register value.
 * 
 * @param prescaler Timer prescaler.
 * @param time Desired delay in microseconds.
 * @return char OCR0A value.
 */
 char CalculateRegisterValue(unsigned prescaler, unsigned time)
{
	// (ClockFrequency * DesiredTime) / (Prescaler * Conversion from µs to s) - 1;
	volatile unsigned long a = (48 * (unsigned long)time / 10 / (unsigned long)prescaler) - 1;
	return ((char)a);
}