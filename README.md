# Z-Stack HAL drivers for CC2530
## Introduction
This package contains additional HAL drivers for use in Z-Stack product development

## GPIO helper macros
Provides helper macros for GPIO ports configuration / manipulation  
Includes file hal_gpio_defs.h

Based on zstack-lib code  
https://github.com/diyruz/zstack-lib

## Software I2C master driver
Provides software (Bit-banging) implementation of Philips I2C master interface.  
Includes hal_i2c.c, hal_i2c.h files.

With default settings and 32MHz core clock SCL frequency reaches 70kHz.  
No additional pull-up required on short / light bus.

By default SCL pin is P0.5, SDA pin is P0.6.  
Can be reassigned by defining global preprocessor symbols  
* OCM_SCL_PORT  
* OCM_SCL_PIN  
* OCM_SDA_PORT  
* OCM_SDA_PIN  

## Key ISR driver
Replacement for the stock Z-Stack hal_key driver  
Includes hal_key.c, hal_key.h files.

Direct fork of hal_key driver from zstack-lib

Listens for events on pins defined in global symbols  
* HAL_KEY_P0_INPUT_PINS
* HAL_KEY_P1_INPUT_PINS
* HAL_KEY_P2_INPUT_PINS

and triggers KEY_CHANGE event with action, port and pin on which
action was registered:  
* keyChange_t::state - bit field of port (2:0) and action (6:5)  
> |       6:5        |       2:0      |
> |------------------|----------------|
> | HAL_KEY_PRESS    |  HAL_KEY_PORT0 |
> | HAL_KEY_RELEASE  |  HAL_KEY_PORT1 |
> |                  |  HAL_KEY_PORT2 |
* keyChange_t::key - pin
