# Z-Stack HAL drivers for CC2530
## Introduction
This package contains additional HAL drivers for use in Z-Stack product development

## GPIO helper macros
Provides helper macros for GPIO ports configuration / manipulation
Includes file hal_gpio_defs.h

Based on zstack-lib I2C driver code
https://github.com/diyruz/zstack-lib

## Software I2C master driver
Driver proves software (Bit-banging) implementation of Philips I2C master interface.
Includes hal_i2c.c, hal_i2c.h files.

With default settings and 32MHz core clock SCL frequency reaches 70kHz.

By default SCL pin is P0.5, SDA pin is P0.6.
Can be reassigned by defining global preprocessor symbols
  OCM_SCL_PORT
  OCM_SCL_PIN
  OCM_SDA_PORT
  OCM_SDA_PIN
