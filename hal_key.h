/**************************************************************************************************
  Filename:       hal_key.h

  Revision:       20230128

  Description:    Key driver, replaces the stock driver
                  fork of zstack-lib driver
                  https://github.com/diyruz/zstack-lib

**************************************************************************************************/

#ifndef HAL_KEY_H
#define HAL_KEY_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**************************************************************************************************
 *                                             INCLUDES
 **************************************************************************************************/
#include "hal_board.h"
  
/**************************************************************************************************
 * MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/
#define HAL_KEY_BIT0   0x01
#define HAL_KEY_BIT1   0x02
#define HAL_KEY_BIT2   0x04
#define HAL_KEY_BIT3   0x08
#define HAL_KEY_BIT4   0x10
#define HAL_KEY_BIT5   0x20
#define HAL_KEY_BIT6   0x40
#define HAL_KEY_BIT7   0x80

/* Interrupt option - Enable or disable */
#define HAL_KEY_INTERRUPT_DISABLE    0x00
#define HAL_KEY_INTERRUPT_ENABLE     0x01

/* Key state - shift or nornal */
#define HAL_KEY_STATE_NORMAL          0x00
#define HAL_KEY_STATE_SHIFT           0x01

#define HAL_KEY_RISING_EDGE 0
#define HAL_KEY_FALLING_EDGE 1

#define HAL_KEY_PORT0 0x01
#define HAL_KEY_PORT1 0x02
#define HAL_KEY_PORT2 0x04

#define HAL_KEY_PRESS 0x20
#define HAL_KEY_RELEASE 0x40

#define HAL_KEY_SW_1 0x01  // Joystick up
#define HAL_KEY_SW_2 0x02  // Joystick right
#define HAL_KEY_SW_5 0x04  // Joystick center
#define HAL_KEY_SW_4 0x08  // Joystick left
#define HAL_KEY_SW_3 0x10  // Joystick down

#define HAL_KEY_SW_6 0x20  // Button S1 if available
#define HAL_KEY_SW_7 0x40  // Button S2 if available

/**************************************************************************************************
 * TYPEDEFS
 **************************************************************************************************/
typedef void (*halKeyCBack_t) (uint8 keys, uint8 state);

/**************************************************************************************************
 *                                             GLOBAL VARIABLES
 **************************************************************************************************/
extern bool Hal_KeyIntEnable;

/**************************************************************************************************
 *                                             FUNCTIONS - API
 **************************************************************************************************/

/*
 * Initialize the Key Service
 */
extern void HalKeyInit( void );

/*
 * Configure the Key Service
 */
extern void HalKeyConfig( bool interruptEnable, const halKeyCBack_t cback);

/*
 * Read the Key status
 */
extern uint8 HalKeyRead( void);

/*
 * Enter sleep mode, store important values
 */
extern void HalKeyEnterSleep ( void );

/*
 * Exit sleep mode, retore values
 */
extern uint8 HalKeyExitSleep ( void );

/*
 * This is for internal used by hal_driver
 */
extern void HalKeyPoll ( void );

/*
 * This is for internal used by hal_sleep
 */
extern bool HalKeyPressed( void );

extern uint8 hal_key_keys( void );

extern uint8 hal_key_int_keys( void );

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAL_KEY_H */
