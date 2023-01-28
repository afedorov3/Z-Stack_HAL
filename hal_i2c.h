/**************************************************************************************************
  Filename:       hal_i2c.h

  Revision:       20230128

  Description:    Software I2C master interface driver

**************************************************************************************************/

#ifndef HAL_I2C_H
#define HAL_I2C_H

enum {
    I2C_SUCCESS = 0,
    I2C_E_ARB,        // Arbitration error
    I2C_E_NODEV,      // No ACK on sending address
    I2C_E_INCOMPLETE, // NAK while sending data
    I2C_E_REG,        // NAK on sending register address
    I2C_E_INVAL       // Invalid argument
};

/*********************************************************************
 * @fn      HalI2CInit
 * @brief   Initializes two-wire serial I/O bus
 * @param   void
 * @return  void
 */
void  HalI2CInit( void );

/*********************************************************************
 * @fn      HALI2CReceive
 * @brief   Receives data into a buffer from an I2C slave device
 * @param   address - address of the slave device
 * @param   buffer - target array for received data
 * @param   len - max number of bytes to read
 * @return  I2C_SUCCESS when successful.
 */
int8_t HalI2CReceive( uint8_t address, uint8_t *buffer, uint16_t len );

/*********************************************************************
 * @fn      HALI2CSend
 * @brief   Sends buffer contents to an I2C slave device
 * @param   address - address of the slave device
 * @param   buffer - ptr to buffered data to send
 * @param   len - number of bytes in the buffer
 * @return  I2C_SUCCESS when successful.
 */
int8_t HalI2CSend( uint8_t address, uint8_t *buf, uint16_t len );

/*********************************************************************
 * @fn      HalI2CReadRegisters
 * @brief   Read I2C slave registers, starting with specified one
 * @param   address - address of the slave device
 * @param   reg - register address to start read from
 * @param   buffer - target array for received data
 * @param   len - max number of bytes to read
 * @return  I2C_SUCCESS when successful.
 */
int8_t HalI2CReadRegisters( uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t len );

/*********************************************************************
 * @fn      HalI2CWriteRegisters
 * @brief   Write to I2C slave registers, starting with specified one
 * @param   address - address of the slave device
 * @param   reg - register address to start writing to
 * @param   buffer - ptr to buffered data to send
 * @param   len - number of bytes in the buffer
 * @return  I2C_SUCCESS when successful.
 */
int8_t HalI2CWriteRegisters( uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t len );

#endif /* HAL_I2C_H */
