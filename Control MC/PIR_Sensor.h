#ifndef PIR_SENSOR_H_
#define PIR_SENSOR_H_

#include "std_types.h"
#include "GPIO.h"

//#define PIR_SENSOR_PIN_REG PINC /* Register to read the PIR sensor state */
#define PIR_SENSOR_PORT_ID PORTC_ID /* Port where the PIR sensor is connected */
#define PIR_SENSOR_PIN_ID PIN2_ID

void PIR_init(void);
uint8 PIR_getState(void);

#endif /* PIR_SENSOR_H_ */
