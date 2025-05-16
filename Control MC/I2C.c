#include "I2C.h"
#include "common_macros.h"
#include <avr/io.h>

void TWI_init(const TWI_ConfigType *Config_Ptr)
{
    /* SCL Calculation using the user-defined Prescaler and Bit Rate
     *
     * SCL = (F_CPU / (16 + 2 * TWBR * Prescaler))
     *
     * Common SCL Speeds:
     *     Normal Mode       100 Kb/s
     *     Fast Mode         400 Kb/s
     *     Fast Mode Plus    1 Mb/s
     *     High-Speed Mode   3.4 Mb/s (rarely used)
     */
    TWBR = Config_Ptr->bit_rate;
    TWSR = Config_Ptr->twi_pre;

    /* Set the Two Wire Bus address for this device (used when it's a slave).
       General Call Recognition: Off */
    TWAR = ((Config_Ptr->address & 0x7F) << 1);

    /* Enable TWI module */
    TWCR = (1<<TWEN);
}

void TWI_start(void)
{
    /*
     * Clear the TWINT flag before sending the start bit (TWINT=1).
     * Set the TWSTA bit to generate a start condition (TWSTA=1).
     * Enable the TWI module (TWEN=1).
     */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    /* Wait for the TWINT flag to be set, indicating the start bit was sent */
    while(BIT_IS_CLEAR(TWCR,TWINT));
}

void TWI_stop(void)
{
    /*
     * Clear the TWINT flag before sending the stop bit (TWINT=1).
     * Set the TWSTO bit to generate a stop condition (TWSTO=1).
     * Enable the TWI module (TWEN=1).
     */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void TWI_writeByte(uint8 data)
{
    /* Load the data byte into the TWI data register */
    TWDR = data;

    /*
     * Clear the TWINT flag before sending the data byte (TWINT=1).
     * Enable the TWI module (TWEN=1).
     */
    TWCR = (1 << TWINT) | (1 << TWEN);

    /* Wait for the TWINT flag to be set, indicating the data byte was sent */
    while(BIT_IS_CLEAR(TWCR,TWINT));
}

uint8 TWI_readByteWithACK(void)
{
    /*
     * Clear the TWINT flag before reading data (TWINT=1).
     * Enable sending ACK after receiving data (TWEA=1).
     * Enable the TWI module (TWEN=1).
     */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    /* Wait for the TWINT flag to be set, indicating data was received */
    while(BIT_IS_CLEAR(TWCR,TWINT));

    /* Return the received data byte */
    return TWDR;
}

uint8 TWI_readByteWithNACK(void)
{
    /*
     * Clear the TWINT flag before reading data (TWINT=1).
     * Enable the TWI module (TWEN=1).
     */
    TWCR = (1 << TWINT) | (1 << TWEN);

    /* Wait for the TWINT flag to be set, indicating data was received */
    while(BIT_IS_CLEAR(TWCR,TWINT));

    /* Return the received data byte */
    return TWDR;
}

uint8 TWI_getStatus(void)
{
    uint8 status;

    /* Mask the TWSR register to get the last 5 status bits (eliminate the first 3) */
    status = TWSR & 0xF8;

    /* Return the status value */
    return status;
}
