#ifndef PCA9685_PRV_H_
#define PCA9685_PRV_H_


/*
 * PCA9685 7-bit I2C Address
 *
 * A0-A5 = 0
 */
#define PCA9685_ADDRESS            0x40


/* ================================
 * PCA9685 Registers
 * ================================ */

#define PCA9685_MODE1              0x00
#define PCA9685_MODE2              0x01

#define PCA9685_LED0_ON_L          0x06

#define PCA9685_PRESCALE           0xFE


/* ================================
 * MODE1 Bits
 * ================================ */

#define PCA9685_RESTART            7
#define PCA9685_AI                 5
#define PCA9685_SLEEP              4


/* ================================
 * MODE2 Bits
 * ================================ */

#define PCA9685_OUTDRV             2


#endif /* PCA9685_PRV_H_ */
