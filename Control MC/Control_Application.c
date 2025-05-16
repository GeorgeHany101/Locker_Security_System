#include "UART.h"
#include "I2C.h"
#include "EEPROM.h"
#include "Motor.h"
#include "Buzzer.h"
#include "PIR_Sensor.h"
#include "Timer.h"
#include "std_types.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define PASS_LOAD        0xA0    // Command: Load new password
#define PASS_IN          0xF1    // Command: Verify existing password
#define PASS_UPDATE      0xE0    // Command: Update existing password
#define PASS_CORRECT     0xC0    // Response: Password verified successfully
#define PASS_FAIL        0xF0    // Response: Password verification failed
#define PEOPLE_IN        0xB0    // Response: People detected entering
#define PEOPLE_NO        0xD0    // Response: No people detected
#define ALARM_ON         0xF2    // Command: Activate alarm
#define START_ADDRESS    0x000   // Starting EEPROM address for password storage
#define DOOR_CLOSED      0xF3    // Response: Door closed

uint8 timerCount = 0;
uint8 progState = 0;
boolean status = FALSE;
boolean peopleIN = FALSE;
uint8 password[10] = { 0 };
uint8 i = 0;

void Timer_Callback(void);

int main() {

    // UART Configuration and Initialization
    UART_ConfigType uart_cfg = { UART_8_BIT,
    		UART_PARITY_DISABLED,
            UART_1_STOP_BIT,
			9600 };
    UART_init(&uart_cfg);

    // Buzzer Initialization
    Buzzer_init();

    // I2C Configuration and Initialization
    TWI_ConfigType i2c_cfg = { 0x01,
    		2,
			TWI_PRE_1 };
    TWI_init(&i2c_cfg);

    // Motor Initialization
    DcMotor_Init();

    // PIR Sensor Initialization
    PIR_init();

    // Timer1 Configuration and Initialization for timeout tracking
    SREG |= (1<<7);  // Enable global interrupts
    Timer_ConfigType timer1 = { 0,
    		31250,
			TIMER1_ID,
			TIMER_PRESCALE_256,
            TIMER_COMPARE_MODE };
    Timer_init(&timer1);
    Timer_setCallBack(Timer_Callback, TIMER1_ID);


    while(1) {
        // Listen for program command from UART
        progState = UART_receiveByte();

        // Handling password creation and verification process
        if (progState == PASS_LOAD) {

            // Receive new password (10 bytes: 5 for password and 5 for confirmation)
            for (i = 0; i < 10; ++i) {
                password[i] = UART_receiveByte();
            }

            // Verify if the new password matches the confirmation input
            for (i = 0; i < 5; ++i) {
                if (password[i] != password[i + 5]) {
                    // Notify HMI of mismatch in password confirmation
                    UART_sendByte(PASS_FAIL);
                    break;
                }
            }

            if (i == 5) {  // Password successfully matched
                UART_sendByte(PASS_CORRECT);  // Notify HMI of successful match

                // Save new password to EEPROM
                for (i = 0; i < 5; ++i) {
                    status = EEPROM_writeByte(START_ADDRESS + i, password[i]);
                    _delay_ms(10); // Delay for EEPROM write
                }
            }
        }
        else if (progState == PASS_IN) {
            // Receive and validate user-entered password

            for (i = 0; i < 5; ++i) {
                password[i] = UART_receiveByte();
            }

            // Retrieve stored password from EEPROM for comparison
            for (i = 5; i < 10; ++i) {
                status = EEPROM_readByte(START_ADDRESS + i - 5, &password[i]);
                _delay_ms(10); // Delay for EEPROM read
            }

            // Compare entered password with stored password
            for (i = 0; i < 5; ++i) {
                if (password[i] != password[i + 5]) {
                    UART_sendByte(PASS_FAIL);  // Notify HMI of failure
                    break;
                }
            }

            if (i == 5) {
                UART_sendByte(PASS_CORRECT);  // Password verification success

                // Open door for 15 seconds
                DcMotor_Rotate(CW, 100);
                timerCount = 0;
                while (timerCount < 5);

                // Stop the motor (door closed)
                DcMotor_Rotate(STOP, 0);

                // Check for any further people entering
                UART_sendByte(PEOPLE_IN);
                _delay_ms(500);
                do {
                    peopleIN = PIR_getState();
                } while (peopleIN);

                // Begin door closure sequence
                UART_sendByte(PEOPLE_NO);
                timerCount = 0;
                DcMotor_Rotate(A_CW, 100);
                while (timerCount < 5);
                DcMotor_Rotate(STOP, 0);
                UART_sendByte(DOOR_CLOSED);
            }
        }
        else if (progState == PASS_UPDATE) {
            // Handle password update

            for (i = 0; i < 5; ++i) {
                password[i] = UART_receiveByte();
            }

            // Retrieve stored password for verification
            for (i = 5; i < 10; ++i) {
                status = EEPROM_readByte(START_ADDRESS + i - 5, &password[i]);
                _delay_ms(10);
            }

            // Verify current password before updating
            for (i = 0; i < 5; ++i) {
                if (password[i] != password[i + 5]) {
                    UART_sendByte(PASS_FAIL);  // Current password mismatch
                    break;
                }
            }

            if (i == 5) {  // Password verified for update
                UART_sendByte(PASS_CORRECT);

                // Receive new password
                for (i = 0; i < 10; ++i) {
                    password[i] = UART_receiveByte();
                }

                // Verify new password confirmation
                for (i = 0; i < 5; ++i) {
                    if (password[i] != password[i + 5]) {
                        UART_sendByte(PASS_FAIL);
                        break;
                    }
                }

                if (i == 5) {  // New password confirmed
                    UART_sendByte(PASS_CORRECT);

                    // Save new password to EEPROM
                    for (i = 0; i < 5; ++i) {
                        status = EEPROM_writeByte(START_ADDRESS + i, password[i]);
                        _delay_ms(10);
                    }
                }
            }
        }
        else if (progState == ALARM_ON) {
            // Activate alarm for a duration of 60 seconds
            Buzzer_on();
            timerCount = 0;
            while (timerCount < 20);
            Buzzer_off();
        }
    }
}

void Timer_Callback(void) {
    ++timerCount;
}
