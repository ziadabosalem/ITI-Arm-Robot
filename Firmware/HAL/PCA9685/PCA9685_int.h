#ifndef PCA9685_INT_H_
#define PCA9685_INT_H_


#include "../../LIB/STD_TYPES.h"


/* Initialize PCA9685 at 50 Hz */
void HPCA9685_vInit(void);


/* Raw PWM control */
void HPCA9685_vSetPWM(
        u8 Copy_u8Channel,
        u16 Copy_u16On,
        u16 Copy_u16Off
);


/* Set pulse directly in microseconds */
void HPCA9685_vSetPulseUs(
        u8 Copy_u8Channel,
        u16 Copy_u16PulseUs
);


/*
 * Convenience function for positional servos
 *
 * 0 -> 180 degree
 *
 * Do NOT use with Wrist1 continuous servo.
 */
void HPCA9685_vSetServoAngle(
        u8 Copy_u8Channel,
        u8 Copy_u8Angle
);


#endif /* PCA9685_INT_H_ */
