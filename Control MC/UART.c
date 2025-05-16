#include "UART.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */

void UART_init(const UART_ConfigType * Config_Ptr)
{
    uint16 ubrr_value = 0;

    /* U2X = 1 for double transmission speed */
    UCSRA = (1 << U2X);

    /* Enable RX and TX, configure UCSZ2 for data size */
    UCSRB = (1 << RXEN) | (1 << TXEN);
    if (Config_Ptr->bit_data == UART_9_BIT)
    {
        UCSRB |= (1 << UCSZ2); /* Enable 9-bit mode */
    }

    /* Configure frame format: data bits, parity, stop bits */
    UCSRC = (1 << URSEL) | (Config_Ptr->parity << UPM0) | (Config_Ptr->stop_bit << USBS);
    if (Config_Ptr->bit_data == UART_5_BIT) {
        /* No bits needed, already default */
    } else if (Config_Ptr->bit_data <= UART_8_BIT) {
        UCSRC |= ((Config_Ptr->bit_data - UART_5_BIT) << UCSZ0);
    }

    /* Calculate and set baud rate */
    ubrr_value = (uint16)(((F_CPU / (Config_Ptr->baud_rate * 8UL))) - 1);
    UBRRH = ubrr_value >> 8;
    UBRRL = ubrr_value;
}

/*
 * Description :
 * Send byte through UART.
 */
void UART_sendByte(const uint8 data)
{
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;

	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/
}
/*
 * Description :
 * Receive byte from UART.
 */
uint8 UART_receiveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
    return UDR;
}


/*
 * Description :
 * Send a string through UART.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}
	*******************************************************************/
}

/*
 * Description :
 * Receive a string through UART until '#' character.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_receiveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_receiveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}
