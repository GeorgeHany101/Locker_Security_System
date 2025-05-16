#ifndef UART_H_
#define UART_H_

#include "std_types.h"


/* Define types for UART configuration */
typedef enum {
    UART_5_BIT = 0,
    UART_6_BIT,
    UART_7_BIT,
    UART_8_BIT,
    UART_9_BIT = 7
} UART_BitDataType;

typedef enum {
    UART_PARITY_DISABLED = 0,
    UART_PARITY_EVEN = 2,
    UART_PARITY_ODD = 3
} UART_ParityType;

typedef enum {
    UART_1_STOP_BIT = 0,
    UART_2_STOP_BITS = 1
} UART_StopBitType;

typedef uint32 UART_BaudRateType;

/* Configuration structure for UART */
typedef struct {
    UART_BitDataType bit_data;
    UART_ParityType parity;
    UART_StopBitType stop_bit;
    UART_BaudRateType baud_rate;
} UART_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #
#endif /* UART_H_ */
