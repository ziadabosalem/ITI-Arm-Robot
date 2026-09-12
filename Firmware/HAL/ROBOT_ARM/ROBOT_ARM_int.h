#ifndef ROBOT_ARM_INT_H_
#define ROBOT_ARM_INT_H_


#include "../../LIB/STD_TYPES.h"


/* =========================================
 * Robot Initialization
 * ========================================= */

void HROBOT_vInit(void);


/* =========================================
 * HOME
 * ========================================= */

void HROBOT_vHome(void);


/* =========================================
 * Positional Joints
 * ========================================= */

void HROBOT_vMoveBase(u8 Copy_u8Angle);

void HROBOT_vMoveShoulder(u8 Copy_u8Angle);

void HROBOT_vMoveElbow(u8 Copy_u8Angle);

void HROBOT_vMoveWrist2(u8 Copy_u8Angle);


/* =========================================
 * Wrist 1 - Continuous Rotation
 * ========================================= */

void HROBOT_vWrist1Dir1(void);

void HROBOT_vWrist1Stop(void);

void HROBOT_vWrist1Dir2(void);


/* =========================================
 * Gripper
 * ========================================= */

void HROBOT_vGripperOpen(void);

void HROBOT_vGripperClose(void);

void HROBOT_vGripperMiddle(void);

/* =========================================
 * Direct Gripper Control
 * ========================================= */

void HROBOT_vMoveGripper(u8 Copy_u8Angle);


/* =========================================
 * Get Current Joint Angles
 * ========================================= */

u8 HROBOT_u8GetBaseAngle(void);

u8 HROBOT_u8GetShoulderAngle(void);

u8 HROBOT_u8GetElbowAngle(void);

u8 HROBOT_u8GetWrist2Angle(void);

u8 HROBOT_u8GetGripperAngle(void);

/* =========================================
 * Fast Jog Functions
 * Used for Keypad Manual Control
 * ========================================= */

void HROBOT_vJogBase(u8 Copy_u8Angle);

void HROBOT_vJogShoulder(u8 Copy_u8Angle);

void HROBOT_vJogElbow(u8 Copy_u8Angle);

void HROBOT_vJogWrist2(u8 Copy_u8Angle);

void HROBOT_vJogGripper(u8 Copy_u8Angle);

#endif /* ROBOT_ARM_INT_H_ */
