#ifndef PCA9685_CFG_H_
#define PCA9685_CFG_H_


/* Servo PWM Frequency */
#define PCA9685_FREQUENCY          50UL


/* PCA9685 Internal Oscillator */
#define PCA9685_OSCILLATOR         25000000UL


/* PCA9685 is 12-bit */
#define PCA9685_RESOLUTION         4096UL


/*
 * Calibration we measured before
 *
 * 0 degree   -> 450 us
 * 90 degree  -> 1500 us
 * 180 degree -> 2550 us
 */
#define PCA9685_MIN_PULSE_US       450U
#define PCA9685_MAX_PULSE_US       2550U


#endif /* PCA9685_CFG_H_ */
