#include "PWM.h"
#include "GPIO.h"
#include <avr/io.h>
#include "common_macros.h"

void PWM_Timer0_Start(uint8_t duty_cycle) {
	TCNT0 = 0; /* Initialize Timer0 counter register */

	/* Set Output Compare Register (OCR0) based on duty cycle percentage */
	OCR0 = (duty_cycle * 255) / 100;

	/* Configure Timer0:
	 * - Fast PWM Mode (WGM00 & WGM01 = 1)
	 * - Non-inverting mode (COM01 = 1, COM00 = 0)
	 * - Prescaler 64 (CS02 = 0, CS01 = 1, CS00 = 1)
	 */
	TCCR0 |= 0x6B;

	/* Set PB3 (OC0) as output for PWM signal */
	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);
}
