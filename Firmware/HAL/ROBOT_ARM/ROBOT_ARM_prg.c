#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>


#include "../../LIB/STD_TYPES.h"

#include "../PCA9685/PCA9685_int.h"

#include "ROBOT_ARM_int.h"
#include "ROBOT_ARM_cfg.h"
#include "ROBOT_ARM_prv.h"



/* =========================================
 * Current Joint Positions
 * ========================================= */

static u8 Global_u8BaseAngle =
        ROBOT_BASE_HOME;

static u8 Global_u8ShoulderAngle =
        ROBOT_SHOULDER_HOME;

static u8 Global_u8ElbowAngle =
        ROBOT_ELBOW_HOME;

static u8 Global_u8Wrist2Angle =
        ROBOT_WRIST2_HOME;

static u8 Global_u8GripperAngle =
        ROBOT_GRIPPER_HOME;



/* =========================================
 * Private Smooth Movement Function
 * ========================================= */

static void HROBOT_vMoveSmooth(
        u8 Copy_u8Channel,
        u8 *Copy_pu8CurrentAngle,
        u8 Copy_u8TargetAngle
)
{
    s16 Local_s16Angle;


    /* Limit target angle */
    if(Copy_u8TargetAngle > ROBOT_MAX_ANGLE)
    {
        Copy_u8TargetAngle =
                ROBOT_MAX_ANGLE;
    }


    /* =====================================
     * Move Up
     * ===================================== */

    if(Copy_u8TargetAngle >
       *Copy_pu8CurrentAngle)
    {
        for(Local_s16Angle =
                *Copy_pu8CurrentAngle;
            Local_s16Angle <=
                Copy_u8TargetAngle;
            Local_s16Angle++)
        {
            HPCA9685_vSetServoAngle(
                    Copy_u8Channel,
                    (u8)Local_s16Angle
            );

            _delay_ms(
                    ROBOT_SMOOTH_DELAY_MS
            );
        }
    }


    /* =====================================
     * Move Down
     * ===================================== */

    else if(Copy_u8TargetAngle <
            *Copy_pu8CurrentAngle)
    {
        for(Local_s16Angle =
                *Copy_pu8CurrentAngle;
            Local_s16Angle >=
                Copy_u8TargetAngle;
            Local_s16Angle--)
        {
            HPCA9685_vSetServoAngle(
                    Copy_u8Channel,
                    (u8)Local_s16Angle
            );

            _delay_ms(
                    ROBOT_SMOOTH_DELAY_MS
            );
        }
    }


    /* Same Angle */
    else
    {
        HPCA9685_vSetServoAngle(
                Copy_u8Channel,
                Copy_u8TargetAngle
        );
    }


    *Copy_pu8CurrentAngle =
            Copy_u8TargetAngle;
}



/* =========================================
 * Robot Initialization
 * ========================================= */

void HROBOT_vInit(void)
{
    /*
     * PCA9685 initialization
     * is done here so main does not
     * need to know about PCA9685.
     */

    HPCA9685_vInit();


    /*
     * Stop continuous wrist first.
     */
    HPCA9685_vSetPulseUs(
            ROBOT_WRIST1_CH,
            ROBOT_WRIST1_STOP
    );


    /*
     * Initial positions.
     */
    HPCA9685_vSetServoAngle(
            ROBOT_BASE_CH,
            ROBOT_BASE_HOME
    );


    HPCA9685_vSetServoAngle(
            ROBOT_SHOULDER_CH,
            ROBOT_SHOULDER_HOME
    );


    HPCA9685_vSetServoAngle(
            ROBOT_ELBOW_CH,
            ROBOT_ELBOW_HOME
    );


    HPCA9685_vSetServoAngle(
            ROBOT_WRIST2_CH,
            ROBOT_WRIST2_HOME
    );


    HPCA9685_vSetServoAngle(
            ROBOT_GRIPPER_CH,
            ROBOT_GRIPPER_HOME
    );
}



/* =========================================
 * HOME Position
 * ========================================= */

void HROBOT_vHome(void)
{
    /* Stop Wrist 1 */
    HROBOT_vWrist1Stop();


    /* Return positional motors */
    HROBOT_vMoveBase(
            ROBOT_BASE_HOME
    );

    HROBOT_vMoveShoulder(
            ROBOT_SHOULDER_HOME
    );

    HROBOT_vMoveElbow(
            ROBOT_ELBOW_HOME
    );

    HROBOT_vMoveWrist2(
            ROBOT_WRIST2_HOME
    );

    HROBOT_vGripperMiddle();
}



/* =========================================
 * Base
 * ========================================= */

void HROBOT_vMoveBase(
        u8 Copy_u8Angle
)
{
    HROBOT_vMoveSmooth(
            ROBOT_BASE_CH,
            &Global_u8BaseAngle,
            Copy_u8Angle
    );
}



/* =========================================
 * Shoulder
 * ========================================= */

void HROBOT_vMoveShoulder(
        u8 Copy_u8Angle
)
{
    HROBOT_vMoveSmooth(
            ROBOT_SHOULDER_CH,
            &Global_u8ShoulderAngle,
            Copy_u8Angle
    );
}



/* =========================================
 * Elbow
 * ========================================= */

void HROBOT_vMoveElbow(
        u8 Copy_u8Angle
)
{
    HROBOT_vMoveSmooth(
            ROBOT_ELBOW_CH,
            &Global_u8ElbowAngle,
            Copy_u8Angle
    );
}



/* =========================================
 * Wrist 2
 * ========================================= */

void HROBOT_vMoveWrist2(
        u8 Copy_u8Angle
)
{
    HROBOT_vMoveSmooth(
            ROBOT_WRIST2_CH,
            &Global_u8Wrist2Angle,
            Copy_u8Angle
    );
}



/* =========================================
 * Wrist 1 - Direction 1
 * ========================================= */

void HROBOT_vWrist1Dir1(void)
{
    HPCA9685_vSetPulseUs(
            ROBOT_WRIST1_CH,
            ROBOT_WRIST1_DIR1
    );
}



/* =========================================
 * Wrist 1 - STOP
 * ========================================= */

void HROBOT_vWrist1Stop(void)
{
    HPCA9685_vSetPulseUs(
            ROBOT_WRIST1_CH,
            ROBOT_WRIST1_STOP
    );
}



/* =========================================
 * Wrist 1 - Direction 2
 * ========================================= */

void HROBOT_vWrist1Dir2(void)
{
    HPCA9685_vSetPulseUs(
            ROBOT_WRIST1_CH,
            ROBOT_WRIST1_DIR2
    );
}



/* =========================================
 * Gripper OPEN
 * ========================================= */

void HROBOT_vGripperOpen(void)
{
    HROBOT_vMoveSmooth(
            ROBOT_GRIPPER_CH,
            &Global_u8GripperAngle,
            ROBOT_GRIPPER_OPEN
    );
}



/* =========================================
 * Gripper CLOSE
 * ========================================= */

void HROBOT_vGripperClose(void)
{
    HROBOT_vMoveSmooth(
            ROBOT_GRIPPER_CH,
            &Global_u8GripperAngle,
            ROBOT_GRIPPER_CLOSE
    );
}



/* =========================================
 * Gripper MIDDLE
 * ========================================= */

void HROBOT_vGripperMiddle(void)
{
    HROBOT_vMoveSmooth(
            ROBOT_GRIPPER_CH,
            &Global_u8GripperAngle,
            ROBOT_GRIPPER_MIDDLE
    );
}
/* =========================================
 * Move Gripper To Specific Angle
 * ========================================= */

void HROBOT_vMoveGripper(
        u8 Copy_u8Angle
)
{
    /* Minimum safe gripper angle */
    if(Copy_u8Angle < ROBOT_GRIPPER_CLOSE)
    {
        Copy_u8Angle =
                ROBOT_GRIPPER_CLOSE;
    }

    /* Maximum gripper angle */
    if(Copy_u8Angle > ROBOT_GRIPPER_OPEN)
    {
        Copy_u8Angle =
                ROBOT_GRIPPER_OPEN;
    }


    HROBOT_vMoveSmooth(
            ROBOT_GRIPPER_CH,
            &Global_u8GripperAngle,
            Copy_u8Angle
    );
}


/* =========================================
 * Get J1 - Base Angle
 * ========================================= */

u8 HROBOT_u8GetBaseAngle(void)
{
    return Global_u8BaseAngle;
}


/* =========================================
 * Get J2 - Shoulder Angle
 * ========================================= */

u8 HROBOT_u8GetShoulderAngle(void)
{
    return Global_u8ShoulderAngle;
}


/* =========================================
 * Get J3 - Elbow Angle
 * ========================================= */

u8 HROBOT_u8GetElbowAngle(void)
{
    return Global_u8ElbowAngle;
}


/* =========================================
 * Get J5 - Wrist2 Angle
 * ========================================= */

u8 HROBOT_u8GetWrist2Angle(void)
{
    return Global_u8Wrist2Angle;
}


/* =========================================
 * Get J6 - Gripper Angle
 * ========================================= */

u8 HROBOT_u8GetGripperAngle(void)
{
    return Global_u8GripperAngle;
}
/* =========================================
 * FAST JOG - J1 BASE
 * ========================================= */

void HROBOT_vJogBase(u8 Copy_u8Angle)
{
    if(Copy_u8Angle > 180)
    {
        Copy_u8Angle = 180;
    }

    HPCA9685_vSetServoAngle(
            ROBOT_BASE_CH,
            Copy_u8Angle
    );

    Global_u8BaseAngle =
            Copy_u8Angle;
}


/* =========================================
 * FAST JOG - J2 SHOULDER
 * ========================================= */

void HROBOT_vJogShoulder(u8 Copy_u8Angle)
{
    if(Copy_u8Angle > 180)
    {
        Copy_u8Angle = 180;
    }

    HPCA9685_vSetServoAngle(
            ROBOT_SHOULDER_CH,
            Copy_u8Angle
    );

    Global_u8ShoulderAngle =
            Copy_u8Angle;
}


/* =========================================
 * FAST JOG - J3 ELBOW
 * ========================================= */

void HROBOT_vJogElbow(u8 Copy_u8Angle)
{
    if(Copy_u8Angle > 180)
    {
        Copy_u8Angle = 180;
    }

    HPCA9685_vSetServoAngle(
            ROBOT_ELBOW_CH,
            Copy_u8Angle
    );

    Global_u8ElbowAngle =
            Copy_u8Angle;
}


/* =========================================
 * FAST JOG - J5 WRIST2
 * ========================================= */

void HROBOT_vJogWrist2(u8 Copy_u8Angle)
{
    if(Copy_u8Angle > 180)
    {
        Copy_u8Angle = 180;
    }

    HPCA9685_vSetServoAngle(
            ROBOT_WRIST2_CH,
            Copy_u8Angle
    );

    Global_u8Wrist2Angle =
            Copy_u8Angle;
}


/* =========================================
 * FAST JOG - J6 GRIPPER
 * ========================================= */

void HROBOT_vJogGripper(u8 Copy_u8Angle)
{
    if(Copy_u8Angle < ROBOT_GRIPPER_CLOSE)
    {
        Copy_u8Angle =
                ROBOT_GRIPPER_CLOSE;
    }

    if(Copy_u8Angle > ROBOT_GRIPPER_OPEN)
    {
        Copy_u8Angle =
                ROBOT_GRIPPER_OPEN;
    }

    HPCA9685_vSetServoAngle(
            ROBOT_GRIPPER_CH,
            Copy_u8Angle
    );

    Global_u8GripperAngle =
            Copy_u8Angle;
}
