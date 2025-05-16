#include "GPIO.h"
#include "UART.h"
#include "Timer.h"
#include "LCD.h"
#include "Keypad.h"
#include "util/delay.h"
#include "avr/io.h"

/* Status */
#define PASS_LOAD        0xA0
#define PASS_IN          0xF1
#define PASS_UPDATE      0xE0
#define PASS_CORRECT     0xC0
#define PASS_FAIL        0xF0
#define PEOPLE_IN        0xB0
#define PEOPLE_NO        0xD0
#define ALARM_ON         0xF2
#define DOOR_CLOSED      0xF3

/* Password */
uint8 password[10] = { 0 };
uint8 initialPass = 0;
uint8 keyPressed = 0;
uint8 timerCount = 0;
uint8 incorrect = 0;
uint8 updateFailCount = 0;
uint8 peopleState = 0;
uint8 i = 0;

void Enter_Pass(uint8 state);

void Timer_Callback(void);

int main() {
	/* Initialize LCD */
	LCD_init();

	/* Configure and initialize UART */
	UART_ConfigType uart_cfg = { UART_8_BIT,
			UART_PARITY_DISABLED,
			UART_1_STOP_BIT,
			9600 };
	UART_init(&uart_cfg);

	/* Initialize Timer1 */
	SREG |= (1<<7);  // Enable global interrupts
	Timer_ConfigType time1 = { 0,
			31250,
			TIMER1_ID,
			TIMER_PRESCALE_256,
			TIMER_COMPARE_MODE };

	Timer_init(&time1);
	Timer_setCallBack(Timer_Callback, TIMER1_ID);

	/* Prompt user to enter password for the first time */
	while (initialPass != PASS_CORRECT) {
		UART_sendByte(PASS_LOAD);
		LCD_clearScreen();
		LCD_displayString("Plz Enter Pass:");
		LCD_moveCursor(1, 0);

		/* Capture and mask password input */
		for (i = 0; i < 5; ++i) {
			password[i] = KEYPAD_getPressedKey();
			LCD_displayCharacter('*');
			_delay_ms(500);
		}

		while (KEYPAD_getPressedKey() != '=');  // Wait for enter keyPressed
		_delay_ms(500);

		/* Prompt user to re-enter password */
		LCD_clearScreen();
		LCD_displayString("Plz re-enter the");
		LCD_displayStringRowColumn(1, 0, "same pass: ");

		for (i = 5; i < 10; ++i) {
			password[i] = KEYPAD_getPressedKey();
			LCD_displayCharacter('*');
			_delay_ms(500);
		}
		while (KEYPAD_getPressedKey() != '=');  // Confirm entry
		_delay_ms(500);

		/* Transmit password via UART to Control ECU */
		for (i = 0; i < 10; ++i) {
			UART_sendByte(password[i]);
		}

		/* Verify password */
		initialPass = UART_receiveByte();
		if (initialPass == PASS_FAIL) {
			LCD_clearScreen();
			LCD_displayString("Mismatch!!");
			timerCount = 0;
			while (timerCount < 1);  // Delay before retry
		}
	}


	while (1) {
		/* Display main menu options */
		LCD_clearScreen();
		LCD_displayString("+ : OPEN DOOR");
		LCD_displayStringRowColumn(1, 0, "- : CHANGE PASS");

		while (keyPressed != '+' && keyPressed != '-'){
			keyPressed = KEYPAD_getPressedKey();
		}

		_delay_ms(500);

		/* Process user choice */
		if (keyPressed == '+') {   // open door
			Enter_Pass(PASS_IN);

			if (initialPass == PASS_CORRECT) {
				incorrect = 0;
				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 1, "Door Unlocking");
				LCD_displayStringRowColumn(1, 3, "Please Wait");
				timerCount = 0;
				while (timerCount < 5);  // Allow 15 seconds for entry
				_delay_ms(500);

				/* Check for people entering */
				peopleState = UART_receiveByte();
				LCD_clearScreen();
				if (peopleState == PEOPLE_IN) {
					LCD_displayStringRowColumn(0, 0, "Wait for People");
					LCD_displayStringRowColumn(1, 3, "to Enter");
				}
				peopleState = UART_receiveByte();
				if (peopleState == PEOPLE_NO) {
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 2, "Door Locking");
					timerCount = 0;
					while (timerCount < 5);  // Allow time for people to exit
				}
				peopleState = UART_receiveByte();
			}
			else if (initialPass == PASS_FAIL) {
				++incorrect;
				if (incorrect == 3) {
					/* Lock system for 1 minute */
					UART_sendByte(ALARM_ON);
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 1, "System LOCKED");
					LCD_displayStringRowColumn(1, 0, "Wait for 1 min.");
					timerCount = 0;
					while (timerCount < 20);  // 1-minute delay
					incorrect = 0;
				}
				else {
					/* Incorrect password */
					LCD_clearScreen();
					LCD_displayString("Incorrect..");
					timerCount = 0;
					while (timerCount < 1);
				}
			}
			keyPressed = 0;
		}
		else if (keyPressed == '-'){  // Change Password
			Enter_Pass(PASS_UPDATE);

			if (initialPass == PASS_CORRECT) {
				updateFailCount = 0;
				LCD_clearScreen();
				LCD_displayString("Plz Enter New ");
				LCD_moveCursor(1, 0);
				LCD_displayString("Pass: ");

				/* Enter new password */
				for (i = 0; i < 5; ++i) {
					password[i] = KEYPAD_getPressedKey();
					LCD_displayCharacter('*');
					_delay_ms(500);
				}
				while (KEYPAD_getPressedKey() != '=');
				_delay_ms(500);
				LCD_clearScreen();
				LCD_displayString("Plz re-enter the");
				LCD_displayStringRowColumn(1, 0, "same pass: ");

				/* Re-enter new password for confirmation */
				for (i = 5; i < 10; ++i) {
					password[i] = KEYPAD_getPressedKey();
					LCD_displayCharacter('*');
					_delay_ms(500);
				}
				while (KEYPAD_getPressedKey() != '=');
				_delay_ms(500);

				/* Transmit new password */
				for (i = 0; i < 10; ++i) {
					UART_sendByte(password[i]);
				}

				/* Confirm new password */
				initialPass = UART_receiveByte();
				if (initialPass == PASS_FAIL) {
					LCD_clearScreen();
					LCD_displayString("Mismatch!!");
					timerCount = 0;
					while (timerCount < 1);
				}
			}
			else if (initialPass == PASS_FAIL) {
				++updateFailCount;
				if (updateFailCount == 3) {
					/* Lock system for 1 minute */
					UART_sendByte(ALARM_ON);
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 1, "System LOCKED");
					LCD_displayStringRowColumn(1, 0, "Wait for 1 min.");
					timerCount = 0;
					while (timerCount < 20);
					updateFailCount = 0;
				}
				else {
					/* Incorrect password */
					LCD_clearScreen();
					LCD_displayString("Incorrect..");
					timerCount = 0;
					while (timerCount < 1);
				}
			}
			keyPressed = 0;
		}
	}
	return 0;
}


void Enter_Pass (uint8 state){
	UART_sendByte(state);
	LCD_clearScreen();
	LCD_displayString("Plz enter old");
	LCD_displayStringRowColumn(1, 0, "pass: ");

	for (i = 0; i < 5; ++i) {
		password[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(500);
	}
	while (KEYPAD_getPressedKey() != '=');
	_delay_ms(500);

	/* Transmit password */
	for (i = 0; i < 5; ++i) {
		UART_sendByte(password[i]);
		_delay_ms(10);
	}
	_delay_ms(100);

	initialPass = UART_receiveByte();

}

void Timer_Callback (void) {
	++timerCount;
}
