#include"PIR_Sensor.h"


void PIR_init(void)
{
	GPIO_setupPinDirection(PIR_SENSOR_PORT_ID, PIR_SENSOR_PIN_ID, PIN_INPUT);
	GPIO_writePin(PIR_SENSOR_PORT_ID, PIR_SENSOR_PIN_ID, LOGIC_LOW);
}

uint8 PIR_getState(void)
{
//	return GPIO_readPin(PORTC_ID, PIN7_ID);
	uint8 state;
	state=GPIO_readPin(PIR_SENSOR_PORT_ID, PIR_SENSOR_PIN_ID);
    return state;
}
