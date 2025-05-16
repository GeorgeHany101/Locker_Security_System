#ifndef I2C_H_
#define I2C_H_

#include"std_types.h"

/* I2C Status codes for the TWSR Register */
#define TWI_START         0x08 /* Start condition transmitted successfully */
#define TWI_REP_START     0x10 /* Repeated start condition transmitted successfully */
#define TWI_MT_SLA_W_ACK  0x18 /* Master transmitted slave address (write request) and received ACK */
#define TWI_MT_SLA_R_ACK  0x40 /* Master transmitted slave address (read request) and received ACK */
#define TWI_MT_DATA_ACK   0x28 /* Master transmitted data byte and received ACK */
#define TWI_MR_DATA_ACK   0x50 /* Master received data byte and sent ACK */
#define TWI_MR_DATA_NACK  0x58 /* Master received data byte but did not send ACK */

/*******************************************************************************
 *                             Type Definitions                                *
 *******************************************************************************/

/* Data type for TWI (I2C) Address */
typedef uint8     TWI_AddressType;

/* Data type for TWI (I2C) Baud Rate */
typedef uint32    TWI_BaudRateType;

/* Enumeration for TWI (I2C) Prescaler options */
typedef enum{
	TWI_PRE_1, TWI_PRE_4, TWI_PRE_16, TWI_PRE_64
}TWI_PRESCALER;

/* Structure to hold TWI (I2C) Configuration settings */
typedef struct{
	TWI_AddressType  address;   /* Device address for TWI communication */
	TWI_BaudRateType bit_rate;  /* Baud rate for communication */
	TWI_PRESCALER    twi_pre;   /* Prescaler for clock generation */
}TWI_ConfigType;

/*******************************************************************************
 *                      Function Prototypes                                   *
 *******************************************************************************/

/*
 * Function to initialize the TWI module with the provided configuration settings.
 * Config_Ptr: Pointer to the configuration structure containing settings.
 * Returns: void
 */
void TWI_init(const TWI_ConfigType *Config_Ptr);

/*
 * Function to generate a start condition on the TWI bus to begin communication.
 * Returns: void
 */
void TWI_start(void);

/*
 * Function to generate a stop condition on the TWI bus to end communication.
 * Returns: void
 */
void TWI_stop(void);

/*
 * Function to write a byte of data to the TWI bus.
 * data: The byte of data to write.
 * Returns: void
 */
void TWI_writeByte(uint8 data);

/*
 * Function to read a byte of data from the TWI bus and send ACK (acknowledge).
 * Returns: The byte of data read from the TWI bus with ACK.
 */
uint8 TWI_readByteWithACK(void);

/*
 * Function to read a byte of data from the TWI bus and send NACK (no acknowledge).
 * Returns: The byte of data read from the TWI bus with NACK.
 */
uint8 TWI_readByteWithNACK(void);

/*
 * Function to retrieve the current status of the TWI module.
 * Returns: The status code of the TWI module.
 */
uint8 TWI_getStatus(void);


#endif /* I2C_H_ */
