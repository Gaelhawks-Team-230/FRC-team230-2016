/*
 * Common.h
 *
 *  Created on: Jan 17, 2016
 *      Author: Gaelhawks
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "TalonXVII_main.h"

/**********************\
 *  Feature Enablers  *
\**********************/
#define USE_GYRO
#define USE_DEFENSE_MANIPULATOR
//#define CHECK_LOOPTIMING
//#define USE_CAMERA
#define USE_AUTOTRACKING
//#define PRACTICE_BOT

 /********************\
  * Global Accessors *
 \********************/
//example: #define GLOBAL_DRIVE	        	(((TalonXVII&)(RobotBase::getInstance())).drive)
#define GLOBAL_GRABBER	        	(((TalonXVII&)(RobotBase::getInstance())).boulderGrab)
#define GLOBAL_CATAPULT             (((TalonXVII&)(RobotBase::getInstance())).catapult)


/*******************\
 *  Global Values  *
\*******************/
#define PI	 						(3.14159265)
#define RAD_TO_DEG 					(180.0/PI)
#define DEG_TO_RAD					(PI/180.0)
#define X_SCALING_FACTOR 			(1)
#define Y_SCALING_FACTOR			(0.65)

#define JOYSTICK_DEADBAND			(0.3)
#define DRIVE_DEADBAND				(0.1)

#define LOOPTIME 					(0.02)   // (0.005)   // (0.01)
#define SAMPLES_PER_SECOND			(1.0/LOOPTIME)
#define N1SEC  						((int) SAMPLES_PER_SECOND)


typedef enum
{
	// example: USB_JOY_DRIVE	= 1,
	// example: USB_GAMEPAD		= 2,
} usbs;

typedef enum
{
	PWM_LEFT				= 0,
	PWM_RIGHT				= 1,
	PWM_CATAPULT_ARM		= 2,
	PWM_GRABPOSITIONER      = 3,
	PWM_BOULDERGRAB         = 4,
	PWM_DEFENSE_MANIPULATOR	= 5,
	PWM_RIGHT_HANGER_ARM	= 6,
	PWM_LEFT_HANGER_ARM		= 7,
	PWM_CAMERA_SERVO		= 8,

} pwms;

typedef enum
{
	GYRO_ANALOG_INPUT   = 1,
	GRAB_ANALOG_INPUT   = 0,
	PIXY_ANALOG_INPUT	= 4,  //MXP analog 0
} analogs;

typedef enum
{
	// example: CLAW_PISTON1		= 0,
	// example: CLAW_PISTON2		= 4,
}solenoids;

typedef enum
{
	DIGIN_ENCODER_LEFT_ONE		= 0,
	DIGIN_ENCODER_LEFT_TWO		= 1,
	DIGIN_ENCODER_RIGHT_ONE		= 2,
	DIGIN_ENCODER_RIGHT_TWO		= 3,
	DIGIN_ENCODER_CATAPULT_ONE	= 4,
	DIGIN_ENCODER_CATAPULT_TWO	= 5,
	RIGHT_ARM_ENCODER_ONE       = 6,
	RIGHT_ARM_ENCODER_TWO       = 7,
	GRAB_BOULDER_DETECTOR       = 9,
	LEFT_ARM_ENCODER_ONE        = 10,
	LEFT_ARM_ENCODER_TWO        = 11,
	PIXY_GOAL_DETECTOR			= 17, //MXP DIO7
} digins;

/*
** List of gamepad (USB_GAMEPAD below) button and axis assignments
*/
// y-axis
#define DPAD_UP						(-1.0)
#define DPAD_DOWN					(1.0)
// x-axis
#define DPAD_LEFT					(-1.0)
#define DPAD_RIGHT					(1.0)

typedef enum
{
	TURN_MODE_BUTTON            = 1,     // Changes turning mode on driveStick
	ENABLE_GYRO_BUTTON 			= 3,		//re-enable gyro function
	DISABLE_GYRO_BUTTON			= 4,		//allows the driver to turn of gyro
	START_CONFIG_BUTTON			= 7,
	DUCK_BUTTON					= 8,
	FLASHLIGHT_ON				= 9,
	GOAL_TRACKING_BUTTON		= 10,
	ROTATE_CAMERA_BACK		    = 11,
	ROTATE_CAMERA_FRONT      	= 12,
} joystick_buttons;

typedef enum
{
	SHOOT_LEVEL_AXIS            = 3,
	CORNER_SHOOT_AXIS			= 1,

} gamepad_axes;

typedef enum
{
	DPAD_RHINO_UP  		= 0,
	DPAD_RHINO_DOWN		= 180,
} gamepad_pov;

typedef enum
{
	BUTTON_PICKUP_POSITION			= 1, // Button X
	BUTTON_CARRY_POSITION			= 2, // Button A
	BUTTON_BOULDER_AUTOLOAD			= 3, // Button B
	BUTTON_BOULDER_INFRAME      	= 4, // Button Y
	BUTTON_CATAPULT_SHOOT			= 5, // Left Bumper
	BUTTON_BOULDER_GRAB				= 6, // Right Bumper
	BUTTON_CATAPULT_ROLL			= 7, // Left Trigger
	BUTTON_BOULDER_EJECT    		= 8, // Right Trigger
	BUTTON_STOP_GATHERER			= 10, // Right Front Button
	// 9 Left Front Button
} gamepad_buttons;

typedef enum
{
	RELAY_FLASHLIGHT   = 0,
} relays;

#endif /* COMMON_H_ */
