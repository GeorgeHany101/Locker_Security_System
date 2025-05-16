#ifndef MOTOR_H_
#define MOTOR_H_

#include "std_types.h"

#define MOTOR_PORT_ID PORTD_ID

#define MOTOR_IN1_PIN_ID PIN6_ID
#define MOTOR_IN2_PIN_ID PIN7_ID
#define MOTOR_EN1_PIN_ID PIN3_ID

typedef enum{
	CW, A_CW, STOP
}DcMotor_State;

void DcMotor_Init(void);

void DcMotor_Rotate(DcMotor_State state, uint8 speed);

#endif /* MOTOR_H_ */
