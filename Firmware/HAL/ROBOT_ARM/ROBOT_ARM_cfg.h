#ifndef ROBOT_ARM_CFG_H_
#define ROBOT_ARM_CFG_H_


/* =========================================
 * PCA9685 Channel Mapping
 * ========================================= */

#define ROBOT_BASE_CH          0
#define ROBOT_SHOULDER_CH      1
#define ROBOT_ELBOW_CH         2
#define ROBOT_WRIST1_CH        3
#define ROBOT_WRIST2_CH        4
#define ROBOT_GRIPPER_CH       5


/* =========================================
 * HOME Positions
 * ========================================= */

#define ROBOT_BASE_HOME        90
#define ROBOT_SHOULDER_HOME    90
#define ROBOT_ELBOW_HOME       90
#define ROBOT_WRIST2_HOME      90

#define ROBOT_GRIPPER_HOME     120


/* =========================================
 * Wrist 1 - Continuous Rotation Servo
 * ========================================= */

#define ROBOT_WRIST1_DIR1      1740
#define ROBOT_WRIST1_STOP      1800
#define ROBOT_WRIST1_DIR2      1950


/* =========================================
 * Gripper Positions
 * ========================================= */

#define ROBOT_GRIPPER_CLOSE    80
#define ROBOT_GRIPPER_MIDDLE   120
#define ROBOT_GRIPPER_OPEN     180
/* =========================================
 * Smooth Movement
 * ========================================= */

#define ROBOT_SMOOTH_DELAY_MS  15U


#endif /* ROBOT_ARM_CFG_H_ */
