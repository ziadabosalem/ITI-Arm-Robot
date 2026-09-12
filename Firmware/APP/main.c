//finalllllll

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>

#include "../LIB/STD_TYPES.h"

#include "../MCAL/TWI/TWI_int.h"
#include "../MCAL/EEPROM/EEPROM_int.h"

#include "../HAL/KPD/KPD_int.h"
#include "../HAL/PCA9685/PCA9685_int.h"
#include "../HAL/ROBOT_ARM/ROBOT_ARM_int.h"
#include "../HAL/LCD_I2C/LCD_I2C_int.h"


/* =====================================================
 * JOINT NUMBERS
 * ===================================================== */

#define JOINT_1_BASE          1U
#define JOINT_2_SHOULDER      2U
#define JOINT_3_ELBOW         3U
#define JOINT_4_WRIST1        4U
#define JOINT_5_WRIST2        5U
#define JOINT_6_GRIPPER       6U


/* =====================================================
 * PCA9685 CHANNELS
 * ===================================================== */

#define J1_CHANNEL            0U
#define J2_CHANNEL            1U
#define J3_CHANNEL            2U
#define J5_CHANNEL            4U
#define J6_CHANNEL            5U


/* =====================================================
 * SERVO LIMITS
 * ===================================================== */

#define SERVO_MIN_TICK        92U
#define SERVO_HOME_TICK       307U
#define SERVO_MAX_TICK        522U


/* =====================================================
 * GRIPPER LIMITS
 * ===================================================== */

#define GRIPPER_MIN_TICK      283U
#define GRIPPER_HOME_TICK     379U
#define GRIPPER_MAX_TICK      522U


/* =====================================================
 * MANUAL JOG SPEED
 * ===================================================== */

#define JOG_TICK_STEP         1U
#define JOG_DELAY_MS          5U


/* =====================================================
 * RELEASE CONFIRMATION
 * ===================================================== */

#define RELEASE_CONFIRM_MS    30U


/* =====================================================
 * WRIST 1 STATES
 * ===================================================== */

#define WRIST1_STOP_STATE     0U
#define WRIST1_DIR1_STATE     1U
#define WRIST1_DIR2_STATE     2U


/* =====================================================
 * EEPROM POSE MEMORY MAP
 *
 * Address 0:
 * Pose Magic Number
 *
 * Addresses 16 -> 25:
 *
 * 16,17 -> J1
 * 18,19 -> J2
 * 20,21 -> J3
 * 22,23 -> J5
 * 24,25 -> J6
 * ===================================================== */

#define EEPROM_POSE_MAGIC_ADDRESS     0U
#define EEPROM_POSE_MAGIC_VALUE       0xA5U

#define EEPROM_J1_ADDRESS             16U
#define EEPROM_J2_ADDRESS             18U
#define EEPROM_J3_ADDRESS             20U
#define EEPROM_J5_ADDRESS             22U
#define EEPROM_J6_ADDRESS             24U


/* =====================================================
 * CURRENT POSITION
 * ===================================================== */

static u16 APP_u16J1Tick =
        SERVO_HOME_TICK;

static u16 APP_u16J2Tick =
        SERVO_HOME_TICK;

static u16 APP_u16J3Tick =
        SERVO_HOME_TICK;

static u16 APP_u16J5Tick =
        SERVO_HOME_TICK;

static u16 APP_u16J6Tick =
        GRIPPER_HOME_TICK;


/* =====================================================
 * APPLICATION VARIABLES
 * ===================================================== */

static u8 APP_u8SelectedJoint =
        JOINT_1_BASE;

static u8 APP_u8PreviousKey =
        NO_KEY;

static u8 APP_u8Wrist1State =
        WRIST1_STOP_STATE;

static u8 APP_u8JogActive =
        0U;


/* =====================================================
 * KEYPAD MAP
 * ===================================================== */

static const u8 APP_u8KeyMap[3][3] =
{
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'}
};


/* =====================================================
 * LCD SEND STRING
 * ===================================================== */

static void APP_vLCDSendString(
        const char *Copy_pcString
)
{
    while(*Copy_pcString != '\0')
    {
        HLCD_I2C_vSendChar(
                (u8)(*Copy_pcString)
        );

        Copy_pcString++;
    }
}


/* =====================================================
 * PRINT 3 DIGITS
 * ===================================================== */

static void APP_vPrint3Digits(
        u8 Copy_u8Value
)
{
    HLCD_I2C_vSendChar(
            (Copy_u8Value / 100U) + '0'
    );

    HLCD_I2C_vSendChar(
            ((Copy_u8Value / 10U) % 10U) + '0'
    );

    HLCD_I2C_vSendChar(
            (Copy_u8Value % 10U) + '0'
    );
}


/* =====================================================
 * TICK -> ANGLE
 * ===================================================== */

static u8 APP_u8TickToAngle(
        u16 Copy_u16Tick
)
{
    u32 Local_u32Angle;


    if(Copy_u16Tick <= SERVO_MIN_TICK)
    {
        return 0U;
    }


    if(Copy_u16Tick >= SERVO_MAX_TICK)
    {
        return 180U;
    }


    Local_u32Angle =
            (u32)(Copy_u16Tick - SERVO_MIN_TICK)
            *
            180UL;


    Local_u32Angle /=
            (SERVO_MAX_TICK - SERVO_MIN_TICK);


    return (u8)Local_u32Angle;
}


/* =====================================================
 * UPDATE FULL LCD
 *
 * Row 0:
 * J1,J2,J3
 *
 * Row 1:
 * J4,J5,J6
 * ===================================================== */

static void APP_vUpdateFullLCD(void)
{
    /* =============================
     * ROW 0
     * ============================= */

    HLCD_I2C_vSetCursorPosition(
            0,
            0
    );


    APP_vPrint3Digits(
            APP_u8TickToAngle(
                    APP_u16J1Tick
            )
    );


    HLCD_I2C_vSendChar(',');


    APP_vPrint3Digits(
            APP_u8TickToAngle(
                    APP_u16J2Tick
            )
    );


    HLCD_I2C_vSendChar(',');


    APP_vPrint3Digits(
            APP_u8TickToAngle(
                    APP_u16J3Tick
            )
    );


    /* =============================
     * ROW 1
     * ============================= */

    HLCD_I2C_vSetCursorPosition(
            1,
            0
    );


    APP_vPrint3Digits(
            APP_u8Wrist1State
    );


    HLCD_I2C_vSendChar(',');


    APP_vPrint3Digits(
            APP_u8TickToAngle(
                    APP_u16J5Tick
            )
    );


    HLCD_I2C_vSendChar(',');


    APP_vPrint3Digits(
            APP_u8TickToAngle(
                    APP_u16J6Tick
            )
    );
}


/* =====================================================
 * SHOW COMMAND MENU
 * ===================================================== */

static void APP_vShowCommandMenu(void)
{
    HLCD_I2C_vClearScreen();


    HLCD_I2C_vSetCursorPosition(
            0,
            0
    );

    APP_vLCDSendString(
            "1:SAVE 2:LOAD"
    );


    HLCD_I2C_vSetCursorPosition(
            1,
            0
    );

    APP_vLCDSendString(
            "3:HOME 9:BACK"
    );
}


/* =====================================================
 * WAIT UNTIL KEYPAD RELEASED
 * ===================================================== */

static void APP_vWaitKeysReleased(void)
{
    while(
        HKPD_u8GetHeldKey(
                APP_u8KeyMap
        )
        !=
        NO_KEY
    )
    {
        _delay_ms(5);
    }


    _delay_ms(20);
}


/* =====================================================
 * STOP SELECTED JOINT
 * ===================================================== */

static void APP_vStopSelectedJoint(void)
{
    switch(APP_u8SelectedJoint)
    {
        case JOINT_1_BASE:

            HPCA9685_vSetPWM(
                    J1_CHANNEL,
                    0,
                    0
            );

            break;


        case JOINT_2_SHOULDER:

            HPCA9685_vSetPWM(
                    J2_CHANNEL,
                    0,
                    0
            );

            break;


        case JOINT_3_ELBOW:

            HPCA9685_vSetPWM(
                    J3_CHANNEL,
                    0,
                    0
            );

            break;


        case JOINT_4_WRIST1:

            HROBOT_vWrist1Stop();

            APP_u8Wrist1State =
                    WRIST1_STOP_STATE;

            break;


        case JOINT_5_WRIST2:

            HPCA9685_vSetPWM(
                    J5_CHANNEL,
                    0,
                    0
            );

            break;


        case JOINT_6_GRIPPER:

            HPCA9685_vSetPWM(
                    J6_CHANNEL,
                    0,
                    0
            );

            break;


        default:

            break;
    }
}


/* =====================================================
 * MOVE NEGATIVE
 * ===================================================== */

static void APP_vMoveNegative(void)
{
    switch(APP_u8SelectedJoint)
    {
        /* =============================
         * J1
         * ============================= */

        case JOINT_1_BASE:

            if(APP_u16J1Tick > SERVO_MIN_TICK)
            {
                APP_u16J1Tick -=
                        JOG_TICK_STEP;


                if(APP_u16J1Tick < SERVO_MIN_TICK)
                {
                    APP_u16J1Tick =
                            SERVO_MIN_TICK;
                }


                HPCA9685_vSetPWM(
                        J1_CHANNEL,
                        0,
                        APP_u16J1Tick
                );
            }

            break;


        /* =============================
         * J2
         * ============================= */

        case JOINT_2_SHOULDER:

            if(APP_u16J2Tick > SERVO_MIN_TICK)
            {
                APP_u16J2Tick -=
                        JOG_TICK_STEP;


                if(APP_u16J2Tick < SERVO_MIN_TICK)
                {
                    APP_u16J2Tick =
                            SERVO_MIN_TICK;
                }


                HPCA9685_vSetPWM(
                        J2_CHANNEL,
                        0,
                        APP_u16J2Tick
                );
            }

            break;


        /* =============================
         * J3
         * ============================= */

        case JOINT_3_ELBOW:

            if(APP_u16J3Tick > SERVO_MIN_TICK)
            {
                APP_u16J3Tick -=
                        JOG_TICK_STEP;


                if(APP_u16J3Tick < SERVO_MIN_TICK)
                {
                    APP_u16J3Tick =
                            SERVO_MIN_TICK;
                }


                HPCA9685_vSetPWM(
                        J3_CHANNEL,
                        0,
                        APP_u16J3Tick
                );
            }

            break;


        /* =============================
         * J4
         * ============================= */

        case JOINT_4_WRIST1:

            HROBOT_vWrist1Dir1();

            APP_u8Wrist1State =
                    WRIST1_DIR1_STATE;

            break;


        /* =============================
         * J5
         * ============================= */

        case JOINT_5_WRIST2:

            if(APP_u16J5Tick > SERVO_MIN_TICK)
            {
                APP_u16J5Tick -=
                        JOG_TICK_STEP;


                if(APP_u16J5Tick < SERVO_MIN_TICK)
                {
                    APP_u16J5Tick =
                            SERVO_MIN_TICK;
                }


                HPCA9685_vSetPWM(
                        J5_CHANNEL,
                        0,
                        APP_u16J5Tick
                );
            }

            break;


        /* =============================
         * J6
         * ============================= */

        case JOINT_6_GRIPPER:

            if(APP_u16J6Tick > GRIPPER_MIN_TICK)
            {
                APP_u16J6Tick -=
                        JOG_TICK_STEP;


                if(APP_u16J6Tick < GRIPPER_MIN_TICK)
                {
                    APP_u16J6Tick =
                            GRIPPER_MIN_TICK;
                }


                HPCA9685_vSetPWM(
                        J6_CHANNEL,
                        0,
                        APP_u16J6Tick
                );
            }

            break;


        default:

            break;
    }
}


/* =====================================================
 * MOVE POSITIVE
 * ===================================================== */

static void APP_vMovePositive(void)
{
    switch(APP_u8SelectedJoint)
    {
        /* =============================
         * J1
         * ============================= */

        case JOINT_1_BASE:

            if(APP_u16J1Tick < SERVO_MAX_TICK)
            {
                APP_u16J1Tick +=
                        JOG_TICK_STEP;


                if(APP_u16J1Tick > SERVO_MAX_TICK)
                {
                    APP_u16J1Tick =
                            SERVO_MAX_TICK;
                }


                HPCA9685_vSetPWM(
                        J1_CHANNEL,
                        0,
                        APP_u16J1Tick
                );
            }

            break;


        /* =============================
         * J2
         * ============================= */

        case JOINT_2_SHOULDER:

            if(APP_u16J2Tick < SERVO_MAX_TICK)
            {
                APP_u16J2Tick +=
                        JOG_TICK_STEP;


                if(APP_u16J2Tick > SERVO_MAX_TICK)
                {
                    APP_u16J2Tick =
                            SERVO_MAX_TICK;
                }


                HPCA9685_vSetPWM(
                        J2_CHANNEL,
                        0,
                        APP_u16J2Tick
                );
            }

            break;


        /* =============================
         * J3
         * ============================= */

        case JOINT_3_ELBOW:

            if(APP_u16J3Tick < SERVO_MAX_TICK)
            {
                APP_u16J3Tick +=
                        JOG_TICK_STEP;


                if(APP_u16J3Tick > SERVO_MAX_TICK)
                {
                    APP_u16J3Tick =
                            SERVO_MAX_TICK;
                }


                HPCA9685_vSetPWM(
                        J3_CHANNEL,
                        0,
                        APP_u16J3Tick
                );
            }

            break;


        /* =============================
         * J4
         * ============================= */

        case JOINT_4_WRIST1:

            HROBOT_vWrist1Dir2();

            APP_u8Wrist1State =
                    WRIST1_DIR2_STATE;

            break;


        /* =============================
         * J5
         * ============================= */

        case JOINT_5_WRIST2:

            if(APP_u16J5Tick < SERVO_MAX_TICK)
            {
                APP_u16J5Tick +=
                        JOG_TICK_STEP;


                if(APP_u16J5Tick > SERVO_MAX_TICK)
                {
                    APP_u16J5Tick =
                            SERVO_MAX_TICK;
                }


                HPCA9685_vSetPWM(
                        J5_CHANNEL,
                        0,
                        APP_u16J5Tick
                );
            }

            break;


        /* =============================
         * J6
         * ============================= */

        case JOINT_6_GRIPPER:

            if(APP_u16J6Tick < GRIPPER_MAX_TICK)
            {
                APP_u16J6Tick +=
                        JOG_TICK_STEP;


                if(APP_u16J6Tick > GRIPPER_MAX_TICK)
                {
                    APP_u16J6Tick =
                            GRIPPER_MAX_TICK;
                }


                HPCA9685_vSetPWM(
                        J6_CHANNEL,
                        0,
                        APP_u16J6Tick
                );
            }

            break;


        default:

            break;
    }
}


/* =====================================================
 * HOME
 * ===================================================== */

static void APP_vHome(void)
{
    HROBOT_vWrist1Stop();


    APP_u16J1Tick =
            SERVO_HOME_TICK;

    APP_u16J2Tick =
            SERVO_HOME_TICK;

    APP_u16J3Tick =
            SERVO_HOME_TICK;

    APP_u16J5Tick =
            SERVO_HOME_TICK;

    APP_u16J6Tick =
            GRIPPER_HOME_TICK;

    APP_u8Wrist1State =
            WRIST1_STOP_STATE;


    HPCA9685_vSetPWM(
            J1_CHANNEL,
            0,
            APP_u16J1Tick
    );


    HPCA9685_vSetPWM(
            J2_CHANNEL,
            0,
            APP_u16J2Tick
    );


    HPCA9685_vSetPWM(
            J3_CHANNEL,
            0,
            APP_u16J3Tick
    );


    HPCA9685_vSetPWM(
            J5_CHANNEL,
            0,
            APP_u16J5Tick
    );


    HPCA9685_vSetPWM(
            J6_CHANNEL,
            0,
            APP_u16J6Tick
    );


    APP_vUpdateFullLCD();
}


/* =====================================================
 * SAVE CURRENT POSE TO EEPROM
 * ===================================================== */

static void APP_vSavePose(void)
{
    /*
     * First invalidate old pose.
     */

    MEEPROM_vWriteByte(
            EEPROM_POSE_MAGIC_ADDRESS,
            0U
    );


    /* =====================================
     * SAVE J1
     * ===================================== */

    MEEPROM_vWriteWord(
            EEPROM_J1_ADDRESS,
            APP_u16J1Tick
    );


    /* =====================================
     * SAVE J2
     * ===================================== */

    MEEPROM_vWriteWord(
            EEPROM_J2_ADDRESS,
            APP_u16J2Tick
    );


    /* =====================================
     * SAVE J3
     * ===================================== */

    MEEPROM_vWriteWord(
            EEPROM_J3_ADDRESS,
            APP_u16J3Tick
    );


    /* =====================================
     * SAVE J5
     * ===================================== */

    MEEPROM_vWriteWord(
            EEPROM_J5_ADDRESS,
            APP_u16J5Tick
    );


    /* =====================================
     * SAVE J6
     * ===================================== */

    MEEPROM_vWriteWord(
            EEPROM_J6_ADDRESS,
            APP_u16J6Tick
    );


    /*
     * Mark pose VALID only after
     * all data has been written.
     */

    MEEPROM_vWriteByte(
            EEPROM_POSE_MAGIC_ADDRESS,
            EEPROM_POSE_MAGIC_VALUE
    );
}


/* =====================================================
 * CHECK IF SAVED POSE EXISTS
 * ===================================================== */

static u8 APP_u8PoseExists(void)
{
    if(
        MEEPROM_u8ReadByte(
                EEPROM_POSE_MAGIC_ADDRESS
        )
        ==
        EEPROM_POSE_MAGIC_VALUE
    )
    {
        return 1U;
    }


    return 0U;
}


/* =====================================================
 * LOAD + REPLAY SAVED POSE
 *
 * Returns:
 *
 * 1 = Success
 * 0 = Invalid / No Pose
 * ===================================================== */

static u8 APP_u8ReplayPose(void)
{
    u16 Local_u16J1;
    u16 Local_u16J2;
    u16 Local_u16J3;
    u16 Local_u16J5;
    u16 Local_u16J6;


    /* =====================================
     * CHECK MAGIC
     * ===================================== */

    if(APP_u8PoseExists() == 0U)
    {
        return 0U;
    }


    /* =====================================
     * READ EEPROM
     * ===================================== */

    Local_u16J1 =
            MEEPROM_u16ReadWord(
                    EEPROM_J1_ADDRESS
            );


    Local_u16J2 =
            MEEPROM_u16ReadWord(
                    EEPROM_J2_ADDRESS
            );


    Local_u16J3 =
            MEEPROM_u16ReadWord(
                    EEPROM_J3_ADDRESS
            );


    Local_u16J5 =
            MEEPROM_u16ReadWord(
                    EEPROM_J5_ADDRESS
            );


    Local_u16J6 =
            MEEPROM_u16ReadWord(
                    EEPROM_J6_ADDRESS
            );


    /* =====================================
     * VALIDATE SAVED VALUES
     * ===================================== */

    if(
        Local_u16J1 < SERVO_MIN_TICK
        ||
        Local_u16J1 > SERVO_MAX_TICK
    )
    {
        return 0U;
    }


    if(
        Local_u16J2 < SERVO_MIN_TICK
        ||
        Local_u16J2 > SERVO_MAX_TICK
    )
    {
        return 0U;
    }


    if(
        Local_u16J3 < SERVO_MIN_TICK
        ||
        Local_u16J3 > SERVO_MAX_TICK
    )
    {
        return 0U;
    }


    if(
        Local_u16J5 < SERVO_MIN_TICK
        ||
        Local_u16J5 > SERVO_MAX_TICK
    )
    {
        return 0U;
    }


    if(
        Local_u16J6 < GRIPPER_MIN_TICK
        ||
        Local_u16J6 > GRIPPER_MAX_TICK
    )
    {
        return 0U;
    }


    /* =====================================
     * STOP J4
     *
     * Continuous servo is not part of
     * Pose Save.
     * ===================================== */

    HROBOT_vWrist1Stop();

    APP_u8Wrist1State =
            WRIST1_STOP_STATE;


    /* =====================================
     * UPDATE RAM VALUES
     * ===================================== */

    APP_u16J1Tick =
            Local_u16J1;

    APP_u16J2Tick =
            Local_u16J2;

    APP_u16J3Tick =
            Local_u16J3;

    APP_u16J5Tick =
            Local_u16J5;

    APP_u16J6Tick =
            Local_u16J6;


    /* =====================================
     * MOVE ROBOT TO SAVED POSE
     *
     * NO LCD TRAFFIC HERE.
     * ===================================== */

    HPCA9685_vSetPWM(
            J1_CHANNEL,
            0,
            APP_u16J1Tick
    );


    HPCA9685_vSetPWM(
            J2_CHANNEL,
            0,
            APP_u16J2Tick
    );


    HPCA9685_vSetPWM(
            J3_CHANNEL,
            0,
            APP_u16J3Tick
    );


    HPCA9685_vSetPWM(
            J5_CHANNEL,
            0,
            APP_u16J5Tick
    );


    HPCA9685_vSetPWM(
            J6_CHANNEL,
            0,
            APP_u16J6Tick
    );


    return 1U;
}


/* =====================================================
 * COMMAND MENU
 *
 * 1 SAVE
 * 2 LOAD / REPLAY
 * 3 HOME
 * 9 BACK
 * ===================================================== */

static void APP_vCommandMenu(void)
{
    u8 Local_u8Key;


    APP_vShowCommandMenu();


    while(1)
    {
        Local_u8Key =
                HKPD_u8GetPressedKey(
                        APP_u8KeyMap
                );


        /* =================================
         * 1 -> SAVE POSE
         * ================================= */

        if(Local_u8Key == '1')
        {
            APP_vSavePose();


            HLCD_I2C_vClearScreen();


            HLCD_I2C_vSetCursorPosition(
                    0,
                    0
            );

            APP_vLCDSendString(
                    "POSE SAVED"
            );


            HLCD_I2C_vSetCursorPosition(
                    1,
                    0
            );

            APP_vLCDSendString(
                    "EEPROM OK"
            );


            _delay_ms(1000);


            HLCD_I2C_vClearScreen();

            APP_vUpdateFullLCD();


            return;
        }


        /* =================================
         * 2 -> LOAD / REPLAY
         * ================================= */

        else if(Local_u8Key == '2')
        {
            if(APP_u8PoseExists() == 0U)
            {
                HLCD_I2C_vClearScreen();


                HLCD_I2C_vSetCursorPosition(
                        0,
                        0
                );

                APP_vLCDSendString(
                        "NO SAVED POSE"
                );


                HLCD_I2C_vSetCursorPosition(
                        1,
                        0
                );

                APP_vLCDSendString(
                        "SAVE FIRST"
                );


                _delay_ms(1200);


                APP_vShowCommandMenu();
            }

            else
            {
                /*
                 * Display status BEFORE movement.
                 */

                HLCD_I2C_vClearScreen();


                HLCD_I2C_vSetCursorPosition(
                        0,
                        0
                );

                APP_vLCDSendString(
                        "POSE REPLAY"
                );


                HLCD_I2C_vSetCursorPosition(
                        1,
                        0
                );

                APP_vLCDSendString(
                        "MOVING..."
                );


                /*
                 * Load and command motors.
                 */

                if(APP_u8ReplayPose() == 1U)
                {
                    /*
                     * During this time:
                     *
                     * NO LCD updates.
                     */

                    _delay_ms(1500);


                    /*
                     * Final LCD values.
                     */

                    HLCD_I2C_vClearScreen();

                    APP_vUpdateFullLCD();
                }

                else
                {
                    HLCD_I2C_vClearScreen();


                    HLCD_I2C_vSetCursorPosition(
                            0,
                            0
                    );

                    APP_vLCDSendString(
                            "POSE ERROR"
                    );


                    HLCD_I2C_vSetCursorPosition(
                            1,
                            0
                    );

                    APP_vLCDSendString(
                            "BAD DATA"
                    );


                    _delay_ms(1200);


                    HLCD_I2C_vClearScreen();

                    APP_vUpdateFullLCD();
                }


                return;
            }
        }


        /* =================================
         * 3 -> HOME
         * ================================= */

        else if(Local_u8Key == '3')
        {
            APP_vHome();

            return;
        }


        /* =================================
         * 9 -> BACK
         * ================================= */

        else if(Local_u8Key == '9')
        {
            HLCD_I2C_vClearScreen();

            APP_vUpdateFullLCD();

            return;
        }
    }
}


/* =====================================================
 * WELCOME SCREEN
 * ===================================================== */

static void APP_vShowWelcomeScreen(void)
{
    HLCD_I2C_vClearScreen();


    /* =====================================
     * ROW 0
     * ===================================== */

    HLCD_I2C_vSetCursorPosition(
            0,
            0
    );


    APP_vLCDSendString(
            "WELCOME TO"
    );


    /* =====================================
     * ROW 1
     * ===================================== */

    HLCD_I2C_vSetCursorPosition(
            1,
            0
    );


    APP_vLCDSendString(
            "ARM ROBOT"
    );


    /*
     * Welcome message duration.
     */

    _delay_ms(1500);
}


/* =====================================================
 * STARTUP MENU
 *
 * 1 -> MANUAL OPERATION
 * 2 -> SAVED POSITION
 * ===================================================== */

static void APP_vShowStartupMenu(void)
{
    HLCD_I2C_vClearScreen();


    /* =====================================
     * ROW 0
     * ===================================== */

    HLCD_I2C_vSetCursorPosition(
            0,
            0
    );


    APP_vLCDSendString(
            "1:MANUAL OP"
    );


    /* =====================================
     * ROW 1
     * ===================================== */

    HLCD_I2C_vSetCursorPosition(
            1,
            0
    );


    APP_vLCDSendString(
            "2:SAVED POS"
    );
}


/* =====================================================
 * STARTUP SELECTION
 *
 * 1 -> MANUAL
 *
 * 2 -> LOAD SAVED POSITION
 *      then continue to manual operation
 * ===================================================== */

static void APP_vStartupSelection(void)
{
    u8 Local_u8Key;


    /* =================================================
     * WELCOME SCREEN
     * ================================================= */

    APP_vShowWelcomeScreen();


    /* =================================================
     * STARTUP MENU
     * ================================================= */

    while(1)
    {
        /*
         * Show menu once.
         */

        APP_vShowStartupMenu();


        /*
         * Wait for only 1 or 2.
         *
         * No LCD refresh while waiting.
         */

        do
        {
            Local_u8Key =
                    HKPD_u8GetPressedKey(
                            APP_u8KeyMap
                    );
        }
        while(
            Local_u8Key != '1'
            &&
            Local_u8Key != '2'
        );


        /* =============================================
         * 1 -> MANUAL OPERATION
         * ============================================= */

        if(Local_u8Key == '1')
        {
            APP_vWaitKeysReleased();


            /*
             * Return directly to normal
             * manual system.
             */

            return;
        }


        /* =============================================
         * 2 -> SAVED POSITION
         * ============================================= */

        else if(Local_u8Key == '2')
        {
            APP_vWaitKeysReleased();


            /* =========================================
             * CHECK SAVED POSE
             * ========================================= */

            if(APP_u8PoseExists() == 0U)
            {
                HLCD_I2C_vClearScreen();


                HLCD_I2C_vSetCursorPosition(
                        0,
                        0
                );


                APP_vLCDSendString(
                        "NO SAVED POSE"
                );


                HLCD_I2C_vSetCursorPosition(
                        1,
                        0
                );


                APP_vLCDSendString(
                        "SAVE FIRST"
                );


                _delay_ms(1500);


                /*
                 * Return to startup menu.
                 */

                continue;
            }


            /* =========================================
             * SAVED POSE EXISTS
             * ========================================= */

            HLCD_I2C_vClearScreen();


            HLCD_I2C_vSetCursorPosition(
                    0,
                    0
            );


            APP_vLCDSendString(
                    "SAVED POSITION"
            );


            HLCD_I2C_vSetCursorPosition(
                    1,
                    0
            );


            APP_vLCDSendString(
                    "MOVING..."
            );


            /*
             * Command robot.
             *
             * APP_u8ReplayPose itself performs
             * NO LCD updates.
             */

            if(APP_u8ReplayPose() == 1U)
            {
                /*
                 * Allow physical movement to finish.
                 *
                 * LCD stays untouched here.
                 */

                _delay_ms(1500);


                /*
                 * ONLY AFTER movement:
                 * display final values.
                 */

                HLCD_I2C_vClearScreen();


                APP_vUpdateFullLCD();


                /*
                 * Continue to normal manual mode.
                 */

                return;
            }


            /* =========================================
             * INVALID EEPROM DATA
             * ========================================= */

            else
            {
                HLCD_I2C_vClearScreen();


                HLCD_I2C_vSetCursorPosition(
                        0,
                        0
                );


                APP_vLCDSendString(
                        "POSE ERROR"
                );


                HLCD_I2C_vSetCursorPosition(
                        1,
                        0
                );


                APP_vLCDSendString(
                        "BAD DATA"
                );


                _delay_ms(1500);


                /*
                 * Return to startup menu.
                 */

                continue;
            }
        }
    }
}


/* =====================================================
 * MAIN
 * ===================================================== */

int main(void)
{
    u8 Local_u8Key;
    u8 Local_u8ConfirmKey;


    /* =================================================
     * INITIALIZATION
     * ================================================= */

    MTWI_vInit();

    HROBOT_vInit();

    HLCD_I2C_vInit();

    HKPD_vInit();


    /* =================================================
     * INITIAL VALUES
     * ================================================= */

    APP_u8SelectedJoint =
            JOINT_1_BASE;

    APP_u8PreviousKey =
            NO_KEY;

    APP_u8Wrist1State =
            WRIST1_STOP_STATE;

    APP_u8JogActive =
            0U;


    /* =================================================
     * STARTUP
     *
     * Welcome To Arm Robot
     *
     * Then:
     *
     * 1 -> Manual Operation
     * 2 -> Saved Position
     * ================================================= */

    APP_vStartupSelection();


    /* =================================================
     * SHOW CURRENT VALUES
     *
     * If Manual was selected:
     * shows current/home values.
     *
     * If Saved Position was selected:
     * shows saved position values.
     * ================================================= */

    HLCD_I2C_vClearScreen();

    APP_vUpdateFullLCD();


    /* =================================================
     * MAIN LOOP
     * ================================================= */

    while(1)
    {
        Local_u8Key =
                HKPD_u8GetHeldKey(
                        APP_u8KeyMap
                );


        /* =================================================
         * JOG MODE ACTIVE
         *
         * LCD IS FROZEN.
         * ================================================= */

        if(APP_u8JogActive == 1U)
        {
            /* =============================================
             * HOLD 7
             * ============================================= */

            if(Local_u8Key == '7')
            {
                if(APP_u8SelectedJoint ==
                   JOINT_4_WRIST1)
                {
                    if(APP_u8PreviousKey != '7')
                    {
                        APP_vMoveNegative();
                    }
                }

                else
                {
                    APP_vMoveNegative();

                    _delay_ms(
                            JOG_DELAY_MS
                    );
                }


                APP_u8PreviousKey =
                        '7';


                continue;
            }


            /* =============================================
             * HOLD 8
             * ============================================= */

            else if(Local_u8Key == '8')
            {
                if(APP_u8SelectedJoint ==
                   JOINT_4_WRIST1)
                {
                    if(APP_u8PreviousKey != '8')
                    {
                        APP_vMovePositive();
                    }
                }

                else
                {
                    APP_vMovePositive();

                    _delay_ms(
                            JOG_DELAY_MS
                    );
                }


                APP_u8PreviousKey =
                        '8';


                continue;
            }


            /* =============================================
             * POSSIBLE RELEASE
             * ============================================= */

            else if(Local_u8Key == NO_KEY)
            {
                /*
                 * Don't trust first NO_KEY.
                 */

                _delay_ms(
                        RELEASE_CONFIRM_MS
                );


                Local_u8ConfirmKey =
                        HKPD_u8GetHeldKey(
                                APP_u8KeyMap
                        );


                /* =========================================
                 * REAL RELEASE
                 * ========================================= */

                if(Local_u8ConfirmKey == NO_KEY)
                {
                    /*
                     * Stop current movement.
                     */

                    APP_vStopSelectedJoint();


                    /*
                     * Exit Jog Mode.
                     */

                    APP_u8JogActive =
                            0U;


                    APP_u8PreviousKey =
                            NO_KEY;


                    /*
                     * IMPORTANT:
                     *
                     * LCD updates ONLY NOW,
                     * after movement has stopped.
                     *
                     * Same original logic.
                     */

                    APP_vUpdateFullLCD();


                    continue;
                }


                /* =========================================
                 * FALSE RELEASE
                 *
                 * Still holding 7
                 * ========================================= */

                else if(Local_u8ConfirmKey == '7')
                {
                    if(APP_u8SelectedJoint !=
                       JOINT_4_WRIST1)
                    {
                        APP_vMoveNegative();

                        _delay_ms(
                                JOG_DELAY_MS
                        );
                    }


                    APP_u8PreviousKey =
                            '7';


                    continue;
                }


                /* =========================================
                 * FALSE RELEASE
                 *
                 * Still holding 8
                 * ========================================= */

                else if(Local_u8ConfirmKey == '8')
                {
                    if(APP_u8SelectedJoint !=
                       JOINT_4_WRIST1)
                    {
                        APP_vMovePositive();

                        _delay_ms(
                                JOG_DELAY_MS
                        );
                    }


                    APP_u8PreviousKey =
                            '8';


                    continue;
                }
            }
        }


        /* =================================================
         * NORMAL MANUAL MODE
         * ================================================= */


        /* =============================================
         * NEW KEY PRESS
         * ============================================= */

        if(Local_u8Key != APP_u8PreviousKey)
        {
            switch(Local_u8Key)
            {
                /* =========================
                 * SELECT J1
                 * ========================= */

                case '1':

                    APP_u8SelectedJoint =
                            JOINT_1_BASE;

                    break;


                /* =========================
                 * SELECT J2
                 * ========================= */

                case '2':

                    APP_u8SelectedJoint =
                            JOINT_2_SHOULDER;

                    break;


                /* =========================
                 * SELECT J3
                 * ========================= */

                case '3':

                    APP_u8SelectedJoint =
                            JOINT_3_ELBOW;

                    break;


                /* =========================
                 * SELECT J4
                 * ========================= */

                case '4':

                    APP_u8SelectedJoint =
                            JOINT_4_WRIST1;

                    break;


                /* =========================
                 * SELECT J5
                 * ========================= */

                case '5':

                    APP_u8SelectedJoint =
                            JOINT_5_WRIST2;

                    break;


                /* =========================
                 * SELECT J6
                 * ========================= */

                case '6':

                    APP_u8SelectedJoint =
                            JOINT_6_GRIPPER;

                    break;


                /* =========================
                 * KEY 9
                 *
                 * OPEN COMMAND MENU
                 * ========================= */

                case '9':

                    /*
                     * First wait until user
                     * releases key 9.
                     */

                    APP_vWaitKeysReleased();


                    /*
                     * Open:
                     *
                     * 1 SAVE
                     * 2 LOAD
                     * 3 HOME
                     * 9 BACK
                     */

                    APP_vCommandMenu();


                    APP_u8PreviousKey =
                            NO_KEY;


                    continue;


                default:

                    break;
            }
        }


        /* =============================================
         * START NEGATIVE JOG
         * ============================================= */

        if(Local_u8Key == '7')
        {
            APP_u8JogActive =
                    1U;


            APP_vMoveNegative();


            APP_u8PreviousKey =
                    '7';


            if(APP_u8SelectedJoint !=
               JOINT_4_WRIST1)
            {
                _delay_ms(
                        JOG_DELAY_MS
                );
            }


            continue;
        }


        /* =============================================
         * START POSITIVE JOG
         * ============================================= */

        else if(Local_u8Key == '8')
        {
            APP_u8JogActive =
                    1U;


            APP_vMovePositive();


            APP_u8PreviousKey =
                    '8';


            if(APP_u8SelectedJoint !=
               JOINT_4_WRIST1)
            {
                _delay_ms(
                        JOG_DELAY_MS
                );
            }


            continue;
        }


        /* =============================================
         * SAVE KEY STATE
         * ============================================= */

        APP_u8PreviousKey =
                Local_u8Key;
    }


    return 0;
}
