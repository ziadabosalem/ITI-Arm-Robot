#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>


#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/TWI/TWI_int.h"

#include "PCA9685_int.h"
#include "PCA9685_cfg.h"
#include "PCA9685_prv.h"



/* =========================================================
 * Private Function:
 * Write one byte into one PCA9685 register
 * ========================================================= */

static void HPCA9685_vWriteRegister(
        u8 Copy_u8Register,
        u8 Copy_u8Data
)
{
    /* 1) Send START */
    MTWI_u8SendStartCondition();


    /* 2) Send PCA9685 address + WRITE */
    MTWI_u8SendSLA_RW(
            PCA9685_ADDRESS,
            TWI_WRITE
    );


    /* 3) Send register address */
    MTWI_u8SendByte(
            Copy_u8Register
    );


    /* 4) Send register data */
    MTWI_u8SendByte(
            Copy_u8Data
    );


    /* 5) Send STOP */
    MTWI_u8SentStopCondition();
}



/* =========================================================
 * Initialize PCA9685
 * ========================================================= */

void HPCA9685_vInit(void)
{
    u8 Local_u8Prescale;


    /*
     * Prescale =
     *
     * Oscillator
     * -------------------  - 1
     * 4096 * Frequency
     *
     *
     * 25 MHz / (4096 * 50) - 1
     *
     * approximately 121
     */

    Local_u8Prescale =
            (u8)(
                (
                    PCA9685_OSCILLATOR /
                    (
                        PCA9685_RESOLUTION *
                        PCA9685_FREQUENCY
                    )
                )
                - 1UL
            );


    /* =====================================================
     * Put PCA9685 into SLEEP
     *
     * AI = Auto Increment
     *
     * We need SLEEP before changing PRE_SCALE.
     * ===================================================== */

    HPCA9685_vWriteRegister(
            PCA9685_MODE1,
            (1U << PCA9685_SLEEP) |
            (1U << PCA9685_AI)
    );


    /* =====================================================
     * Set PWM Frequency
     * ===================================================== */

    HPCA9685_vWriteRegister(
            PCA9685_PRESCALE,
            Local_u8Prescale
    );


    /* =====================================================
     * MODE2
     *
     * OUTDRV = 1
     * Totem Pole Output
     * ===================================================== */

    HPCA9685_vWriteRegister(
            PCA9685_MODE2,
            (1U << PCA9685_OUTDRV)
    );


    /* =====================================================
     * Wake PCA9685
     *
     * SLEEP = 0
     * AI    = 1
     * ===================================================== */

    HPCA9685_vWriteRegister(
            PCA9685_MODE1,
            (1U << PCA9685_AI)
    );


    /* Wait for oscillator to stabilize */
    _delay_ms(1);
}



/* =========================================================
 * Set Raw PWM
 *
 * Channel = 0 -> 15
 * ON      = 0 -> 4095
 * OFF     = 0 -> 4095
 * ========================================================= */

void HPCA9685_vSetPWM(
        u8 Copy_u8Channel,
        u16 Copy_u16On,
        u16 Copy_u16Off
)
{
    u8 Local_u8Register;


    /* Check channel */
    if(Copy_u8Channel > 15)
    {
        return;
    }


    /* Limit ON value */
    if(Copy_u16On > 4095)
    {
        Copy_u16On = 4095;
    }


    /* Limit OFF value */
    if(Copy_u16Off > 4095)
    {
        Copy_u16Off = 4095;
    }


    /*
     * Each channel uses 4 registers:
     *
     * CH0 starts at 0x06
     * CH1 starts at 0x0A
     * CH2 starts at 0x0E
     * ...
     *
     * Register =
     * LED0_ON_L + (4 * Channel)
     */

    Local_u8Register =
            PCA9685_LED0_ON_L +
            (4U * Copy_u8Channel);


    /* Send START */
    MTWI_u8SendStartCondition();


    /* Address + WRITE */
    MTWI_u8SendSLA_RW(
            PCA9685_ADDRESS,
            TWI_WRITE
    );


    /*
     * Tell PCA9685 which register
     * we want to start writing from.
     */
    MTWI_u8SendByte(
            Local_u8Register
    );


    /*
     * Because Auto Increment is enabled,
     * the PCA will automatically go:
     *
     * LEDx_ON_L
     * LEDx_ON_H
     * LEDx_OFF_L
     * LEDx_OFF_H
     */


    /* ON Low Byte */
    MTWI_u8SendByte(
            (u8)(Copy_u16On & 0x00FF)
    );


    /* ON High Byte */
    MTWI_u8SendByte(
            (u8)((Copy_u16On >> 8) & 0x0F)
    );


    /* OFF Low Byte */
    MTWI_u8SendByte(
            (u8)(Copy_u16Off & 0x00FF)
    );


    /* OFF High Byte */
    MTWI_u8SendByte(
            (u8)((Copy_u16Off >> 8) & 0x0F)
    );


    /* STOP */
    MTWI_u8SentStopCondition();
}



/* =========================================================
 * Set PWM Pulse in Microseconds
 *
 * Example:
 *
 * 450 us
 * 1500 us
 * 2550 us
 *
 * ========================================================= */

void HPCA9685_vSetPulseUs(
        u8 Copy_u8Channel,
        u16 Copy_u16PulseUs
)
{
    u16 Local_u16Off;


    /* Check channel */
    if(Copy_u8Channel > 15)
    {
        return;
    }


    /* Limit pulse */
    if(Copy_u16PulseUs < PCA9685_MIN_PULSE_US)
    {
        Copy_u16PulseUs =
                PCA9685_MIN_PULSE_US;
    }


    if(Copy_u16PulseUs > PCA9685_MAX_PULSE_US)
    {
        Copy_u16PulseUs =
                PCA9685_MAX_PULSE_US;
    }


    /*
     * Convert microseconds to PCA9685 ticks
     *
     * At 50 Hz:
     *
     * Period = 20,000 us
     *
     * ticks =
     *
     * Pulse_us * 4096
     * ----------------
     *     20000
     *
     *
     * General Formula:
     *
     * Pulse_us * 4096 * Frequency
     * --------------------------------
     *             1,000,000
     */

    Local_u16Off =
            (u16)(
                (
                    (u32)Copy_u16PulseUs *
                    PCA9685_RESOLUTION *
                    PCA9685_FREQUENCY
                )
                /
                1000000UL
            );


    HPCA9685_vSetPWM(
            Copy_u8Channel,
            0,
            Local_u16Off
    );
}



/* =========================================================
 * Set Positional Servo Angle
 *
 * Calibration:
 *
 * 0 degree    = 450 us
 *
 * 90 degree   = 1500 us
 *
 * 180 degree  = 2550 us
 *
 * ========================================================= */

void HPCA9685_vSetServoAngle(
        u8 Copy_u8Channel,
        u8 Copy_u8Angle
)
{
    u16 Local_u16PulseUs;


    /* Limit angle */
    if(Copy_u8Angle > 180)
    {
        Copy_u8Angle = 180;
    }


    /*
     * Map:
     *
     * 0 -> 180 degree
     *
     * into
     *
     * 450 -> 2550 us
     */

    Local_u16PulseUs =
            PCA9685_MIN_PULSE_US
            +
            (u16)(
                (
                    (u32)Copy_u8Angle *
                    (
                        PCA9685_MAX_PULSE_US -
                        PCA9685_MIN_PULSE_US
                    )
                )
                /
                180UL
            );


    HPCA9685_vSetPulseUs(
            Copy_u8Channel,
            Local_u16PulseUs
    );
}
