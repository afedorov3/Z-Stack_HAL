/**************************************************************************************************
  Filename:       hal_i2c.c

  Revision:       20230128

  Description:    Software I2C master interface driver

**************************************************************************************************/

#include "hal_defs.h"
#include "hal_gpio_defs.h"
#include "hal_i2c.h"
#include "OnBoard.h" // MicroWait()

// *************************   MACROS   ************************************

#if !defined HAL_I2C_STARTSTOP_WAITS   // Maximum time to busy wait for HIGH SCL on START or STOP
#define HAL_I2C_STARTSTOP_WAITS 30     // Approx. 1ms units
#endif

#if !defined HAL_I2C_STRETCH_WAITS     // Maximum time to busy wait for HIGH SCL on clock STRETCH
#define HAL_I2C_STRETCH_WAITS 100      // Approx. 10us units
#endif

// the default cofiguration below uses P0.6 for SDA and P0.5 for SCL.
// change these as needed.
#ifndef OCM_SCL_PORT
#define OCM_SCL_PORT 0
#endif
#ifndef OCM_SCL_PIN
#define OCM_SCL_PIN 5
#endif

#ifndef OCM_SDA_PORT
#define OCM_SDA_PORT 0
#endif
#ifndef OCM_SDA_PIN
#define OCM_SDA_PIN 6
#endif

#define I2C_OP_READ  (0x01)
#define I2C_OP_WRITE (0x00)

#define I2C_ACK (0)
#define I2C_NAK (1)

// OCM port I/O and wait defintions
#define OCM_SCL_STATE  IO_PIN(OCM_SCL_PORT, OCM_SCL_PIN) // 0 for LOW, not 0 for HIGH
#define OCM_SDA_STATE  IO_PIN(OCM_SDA_PORT, OCM_SDA_PIN) // 0 for LOW, not 0 for HIGH
#define OCM_SCL_HIGH() IO_DIR_PORT_PIN(OCM_SCL_PORT, OCM_SCL_PIN, IO_IN)
#define OCM_SCL_LOW()  st( IO_DIR_PORT_PIN(OCM_SCL_PORT, OCM_SCL_PIN, IO_OUT); IO_PIN(OCM_SCL_PORT, OCM_SCL_PIN) = 0; )
#define OCM_SDA_HIGH() IO_DIR_PORT_PIN(OCM_SDA_PORT, OCM_SDA_PIN, IO_IN)
#define OCM_SDA_LOW()  st( IO_DIR_PORT_PIN(OCM_SDA_PORT, OCM_SDA_PIN, IO_OUT); IO_PIN(OCM_SDA_PORT, OCM_SDA_PIN) = 0; )
#define OCM_HPERIOD()  MicroWait(2)
#define OCM_STRETCH()  MicroWait(10)
#define OCM_SSWAIT()   MicroWait(1000)

// ************************* DECLARATIONS **********************************

static inline  int8_t HalI2CStart(void);
static inline  int8_t HalI2CStop(void);
static inline uint8_t HalI2CReceiveByte(int8_t ack);
static inline  int8_t HalI2CSendByte(uint8_t value);

/* PRIVATE */

/*********************************************************************
 * @fn      HalI2CStart
 * @brief   Initiates SM-Bus communication. Makes sure that both the
 *          clock and data lines of the SM-Bus are high. Then the data
 *          line is set high and clock line is set low to start I/O.
 * @param   none
 * @return  I2C_SUCCESS when START condition is set, otherwise I2C_E_*
 */
static inline int8_t HalI2CStart(void)
{
    uint8_t retry = HAL_I2C_STARTSTOP_WAITS;

    OCM_SDA_HIGH();
    OCM_HPERIOD();
    OCM_SCL_HIGH();
    OCM_HPERIOD();
    while(!(OCM_SCL_STATE))
    {
        if (!retry--)
        {
            return I2C_E_ARB; // START timeout
        }
        OCM_SSWAIT();
    }
    OCM_SDA_LOW();
    OCM_HPERIOD();
    OCM_SCL_LOW();

    return I2C_SUCCESS;
}

/*********************************************************************
 * @fn      HalI2CStop
 * @brief   Terminates SM-Bus communication. Waits unitl the data line
 *          is low and the clock line is high. Then sets the data line
 *          high, keeping the clock line high to stop I/O.
 * @param   none
 * @return  I2C_SUCCESS when STOP condition is set, otherwise I2C_E_*
 */
static inline int8_t HalI2CStop(void)
{
    uint8_t retry = HAL_I2C_STARTSTOP_WAITS;
    int8_t ret = I2C_SUCCESS;

    OCM_SDA_LOW();
    OCM_HPERIOD();
    OCM_SCL_HIGH();
    OCM_HPERIOD();
    while(!(OCM_SCL_STATE))
    {
        if (!retry--)
        {
            ret = I2C_E_ARB; // STOP timeout
            break;
        }
        OCM_SSWAIT();
    }
    OCM_HPERIOD();
    OCM_SDA_HIGH();
    OCM_HPERIOD();

    return ret;
}

/*********************************************************************
 * @fn      HalI2CReceiveByte
 * @brief   Read the 8 data bits and set ACK.
 * @param   ack - acknowledge bit to set
 * @return  byte read
 */
static inline uint8_t HalI2CReceiveByte(int8_t ack)
{
    uint8_t i, rval = 0;
    uint8_t stretch;

    for (i = 0; i < 8; ++i)
    {
        OCM_SDA_HIGH();
        OCM_HPERIOD();
        OCM_SCL_HIGH();
        OCM_HPERIOD();
        for(stretch = 0; !(OCM_SCL_STATE) && stretch < HAL_I2C_STRETCH_WAITS; stretch++)
            OCM_STRETCH();
        if (stretch)
            OCM_HPERIOD();
        rval = (rval << 1) | (OCM_SDA_STATE) != 0;
        OCM_SCL_LOW();
    }

    // ACK
    if (ack)
        OCM_SDA_HIGH();
    else
        OCM_SDA_LOW();
    OCM_HPERIOD();
    OCM_SCL_HIGH();
    OCM_HPERIOD();
    for(stretch = 0; !(OCM_SCL_STATE) && stretch < HAL_I2C_STRETCH_WAITS; stretch++)
        OCM_STRETCH();
    if (stretch)
        OCM_HPERIOD();
    OCM_SCL_LOW();    

    return rval;
}

/*********************************************************************
 * @fn      HalI2CSendByte
 * @brief   Serialize and send one byte to SM-Bus device, reading ACK bit
 * @param   value - data byte to send
 * @return  I2C_ACK or I2C_NAK
 */
static inline int8_t HalI2CSendByte(uint8_t value)
{
    int8_t ack;
    uint8_t i;
    uint8_t stretch;

    for (i = 0; i < 8; i++)
    {
        if (value & 0x80)
            OCM_SDA_HIGH();
        else
            OCM_SDA_LOW();
        OCM_HPERIOD();
        OCM_SCL_HIGH();
        OCM_HPERIOD();
        for(stretch = 0; !(OCM_SCL_STATE) && stretch < HAL_I2C_STRETCH_WAITS; stretch++)
            OCM_STRETCH();
        if (stretch)
            OCM_HPERIOD();
        OCM_SCL_LOW();
        value <<= 1;
    }

    // ACK
    OCM_SDA_HIGH();
    OCM_HPERIOD();
    OCM_SCL_HIGH();
    OCM_HPERIOD();
    for(stretch = 0; !(OCM_SCL_STATE) && stretch < HAL_I2C_STRETCH_WAITS; stretch++)
        OCM_STRETCH();
    if (stretch)
        OCM_HPERIOD();
    ack = ((OCM_SDA_STATE) != 0);
    OCM_SCL_LOW();

    return ack;
}

/*********************************************************************
**********************************************************************/

/* PUBLIC */

/*********************************************************************
 * @fn      HalI2CInit
 * @brief   Initializes two-wire serial I/O bus
 * @param   void
 * @return  void
 */
void HalI2CInit(void) {
    // Set port pins as inputs
    IO_DIR_PORT_PIN(OCM_SCL_PORT, OCM_SCL_PIN, IO_IN);
    IO_DIR_PORT_PIN(OCM_SDA_PORT, OCM_SDA_PIN, IO_IN);

    // Set for general I/O operation
    IO_FUNC_PORT_PIN(OCM_SCL_PORT, OCM_SCL_PIN, IO_GIO);
    IO_FUNC_PORT_PIN(OCM_SDA_PORT, OCM_SDA_PIN, IO_GIO);

    // Set I/O mode for pull-up/pull-down
    IO_IMODE_PORT_PIN(OCM_SCL_PORT, OCM_SCL_PIN, IO_PUD);
    IO_IMODE_PORT_PIN(OCM_SDA_PORT, OCM_SDA_PIN, IO_PUD);

    // Set pins to pull-up
    IO_PUD_PORT(OCM_SCL_PORT, IO_PUP);
    IO_PUD_PORT(OCM_SDA_PORT, IO_PUP);
}

/*********************************************************************
 * @fn      HALI2CReceive
 * @brief   Receives data into a buffer from an I2C slave device
 * @param   address - address of the slave device
 * @param   buffer - target array for received data
 * @param   len - max number of bytes to read
 * @return  I2C_SUCCESS when successful, otherwise I2C_E_*
 */
int8_t HalI2CReceive( uint8_t address, uint8_t *buffer, uint16_t len )
{
    uint16_t i;
    int8_t ret;

    if (buffer == NULL)
        return I2C_E_INVAL;

    ret = HalI2CStart();
    if (ret != I2C_SUCCESS)
        return ret;

    do
    {
        if (HalI2CSendByte(address << 1 | I2C_OP_READ) != I2C_ACK) // NAK
        {
            ret = I2C_E_NODEV;
            break;
        }

        for (i = 0; i < len - 1; i++)
            buffer[i] = HalI2CReceiveByte(I2C_ACK);
        buffer[i] = HalI2CReceiveByte(I2C_NAK);
    } while(0);

    if (HalI2CStop() != I2C_SUCCESS)
      return I2C_E_ARB;

    return ret;
}

/*********************************************************************
 * @fn      HALI2CSend
 * @brief   Sends buffer contents to an I2C slave device
 * @param   address - address of the slave device
 * @param   buffer - ptr to buffered data to send
 * @param   len - number of bytes in the buffer
 * @return  I2C_SUCCESS when successful, otherwise I2C_E_*
 */
int8_t HalI2CSend( uint8_t address, uint8_t *buffer, uint16_t len )
{
    uint16_t i;
    int8_t ret;

    if (buffer == NULL)
        return I2C_E_INVAL;

    ret = HalI2CStart();
    if (ret != I2C_SUCCESS)
        return ret;

    do
    {
        if (HalI2CSendByte(address << 1 | I2C_OP_WRITE) != I2C_ACK) // NAK
        {
            ret = I2C_E_NODEV;
            break;
        }

        for (i = 0; i < len; i++)
        {
            if (HalI2CSendByte(buffer[i]) != I2C_ACK) // NAK
            {
                ret = I2C_E_INCOMPLETE;
                break;
            }
        }
    } while(0);

    if (HalI2CStop() != I2C_SUCCESS)
      return I2C_E_ARB;

    return ret;
}

/*********************************************************************
 * @fn      HalI2CReadRegisters
 * @brief   Read I2C slave registers, starting with specified one
 * @param   address - address of the slave device
 * @param   reg - register address to start read from
 * @param   buffer - target array for received data
 * @param   len - max number of bytes to read
 * @return  I2C_SUCCESS when successful, otherwise I2C_E_*
 */
int8_t HalI2CReadRegisters( uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t len )
{
    int8_t ret;

    if (buffer == NULL)
        return I2C_E_INVAL;

    ret = HalI2CStart();
    if (ret != I2C_SUCCESS)
        return ret;

    do
    {
        if (HalI2CSendByte(address << 1 | I2C_OP_WRITE) != I2C_ACK) // NAK
        {
            ret = I2C_E_NODEV;
            break;
        }

        if (HalI2CSendByte(reg) != I2C_ACK) // NAK
            ret = I2C_E_REG;
    } while(0);
    
    if (ret != I2C_SUCCESS)
    {
        if (HalI2CStop() != I2C_SUCCESS)
            return I2C_E_ARB;
        return ret;
    }

    /* Restart with read */
    return HalI2CReceive(address, buffer, len);
}

/*********************************************************************
 * @fn      HalI2CWriteRegisters
 * @brief   Write to I2C slave registers, starting with specified one
 * @param   address - address of the slave device
 * @param   reg - register address to start writing to
 * @param   buffer - ptr to buffered data to send
 * @param   len - number of bytes in the buffer
 * @return  I2C_SUCCESS when successful, otherwise I2C_E_*
 */
int8_t HalI2CWriteRegisters( uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t len )
{
    uint16_t i = 0;
    int8_t ret;

    if (buffer == NULL)
        return I2C_E_INVAL;

    ret = HalI2CStart();
    if (ret != I2C_SUCCESS)
        return ret;

    do {
        if (HalI2CSendByte(address << 1 | I2C_OP_WRITE) != I2C_ACK) { // NAK
            ret = I2C_E_NODEV;
            break;
        }

        if (HalI2CSendByte(reg) != I2C_ACK) // NAK
        {
            ret = I2C_E_REG;
            break;
        }

        for (i = 0; i < len; i++) {
            if (!HalI2CSendByte(buffer[i])) // NAK
            {
                ret = I2C_E_INCOMPLETE;
                break;
            }
        }
    } while(0);

    if (HalI2CStop() != I2C_SUCCESS)
      return I2C_E_ARB;
    
    return ret;
}
