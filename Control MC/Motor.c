#include "Motor.h"
#include "common_macros.h"
#include "GPIO.h"
#include "PWM.h"

void DcMotor_Init(void){
	GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_IN1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_IN2_PIN_ID, PIN_OUTPUT);
	//GPIO_setupPinDirection(MOTOR_PORT_ID, MOTOR_EN1_PIN_ID, PIN_OUTPUT);

	GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
}

void DcMotor_Rotate(DcMotor_State state, uint8 speed){
    /* Set Speed Using PWM */
	PWM_Timer0_Start(speed);  // Scale 0-100% to 8-bit

    /* Control Motor Direction */
    switch (state) {
        case CW:
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_HIGH);
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
            break;
        case A_CW:
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_HIGH);
            break;
        case STOP:
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
            break;
        default:
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
            GPIO_writePin(MOTOR_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
            break;
    }
}
