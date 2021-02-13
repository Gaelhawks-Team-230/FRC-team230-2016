/*
 * DriveTrain.h
 *
 *  Created on: Jan 26, 2016
 *      Author: Gaelhawks
 */
#include "WPILib.h"
#include "Common.h"


#ifndef SRC_DRIVETRAIN_H_
#define SRC_DRIVETRAIN_H_

#define K_GYRO      (20.0)
#define K_ROBOT     (610)
#define TAU_ROBOT   (0.15)


class DriveTrain
{
public:
	RobotDrive *driveBase;

	VictorSP *leftMotor;
	VictorSP *rightMotor;

#ifdef USE_GYRO
	ADXRS450_Gyro *gyro;

	double gyroVel;
	double gyroErr;
	double gyroErrInt;
#endif
	bool gyroOn;


	DriveTrain(void);
	void DriveControl (float command_x, float command_y, float command_z, bool preciseTurn, bool isAuto);
	void ResetGyro(void);
	void GyroOn(void);
	void GyroOff(void);
	float GyroControl(float xCmd);
	double GetGyroVel();
	void GetGyroPositionAndVelocity(void);
	void UpdateDash(void);
	void Abort(void);

};



#endif /* SRC_DRIVETRAIN_H_ */
