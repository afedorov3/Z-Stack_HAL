/**************************************************************************************************
  Filename:       hal_gpio_defs.h

  Revision:       20230128

  Description:    CC2530 GPIO helper macros

**************************************************************************************************/

#ifndef HAL_GPIO_DEFS_H
#define HAL_GPIO_DEFS_H

// General I/O definitions
#define IO_GIO 0 // General purpose I/O
#define IO_PER 1 // Peripheral function
#define IO_IN  0 // Input pin
#define IO_OUT 1 // Output pin
#define IO_PUD 0 // Pullup/pulldn input
#define IO_TRI 1 // Tri-state input
#define IO_PUP 0 // Pull-up input pin
#define IO_PDN 1 // Pull-down input pin

/* I/O PORT CONFIGURATION */
#define IO_REG1(port, reg) P##port##reg
#define IO_PIN1(port, pin) P##port##_##pin
#define IO_DIR(port) IO_REG1(port, DIR)
#define IO_INP(port) IO_REG1(port, INP)
#define IO_SEL(port) IO_REG1(port, SEL)
#define IO_PIN(port, pin) IO_PIN1(port, pin)

#define IO_DIR_PORT_PIN(port, pin, dir)                   \
    st(                                                   \
        if (dir == IO_OUT)                                \
            IO_DIR(port) |= BV(pin);                      \
        else                                              \
            IO_DIR(port) &= ~BV(pin);                     \
    )

#define IO_FUNC_PORT_PIN(port, pin, func)                 \
    st(                                                   \
        if (port < 2)                                     \
        {                                                 \
            if (func == IO_PER)                           \
                IO_SEL(port) |= BV(pin);                  \
            else                                          \
                IO_SEL(port) &= ~BV(pin);                 \
        }                                                 \
        else                                              \
        {                                                 \
            if (func == IO_PER)                           \
                P2SEL |= BV(pin >> 1);                    \
            else                                          \
                P2SEL &= ~BV(pin >> 1);                   \
        }                                                 \
    )

#define IO_IMODE_PORT_PIN(port, pin, mode)                \
    st(                                                   \
        if (mode == IO_TRI)                               \
            IO_INP(port) |= BV(pin);                      \
        else                                              \
            IO_INP(port) &= ~BV(pin);                     \
    )

#define IO_PUD_PORT(port, dir)                            \
    st(                                                   \
        if (dir == IO_PDN)                                \
            P2INP |= BV(port + 5);                        \
        else                                              \
            P2INP &= ~BV(port + 5);                       \
    )

#endif /* HAL_GPIO_DEFS_H */
