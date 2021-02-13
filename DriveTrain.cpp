/*
 * DriveTrain.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: Gaelhawks
 */
#include "WPILib.h"
#include "TalonXVII_main.h"
#include "DriveTrain.h"



DriveTrain::DriveTrain(void)
{
	leftMotor = new VictorSP(PWM_LEFT);
	rightMotor = new VictorSP(PWM_RIGHT);

	leftMotor->SetInverted(true);
	rightMotor->SetInverted(true);
	leftMotor->SetSafetyEnabled(false);
	rightMotor->SetSafetyEnabled(false);

	driveBase = new RobotDrive(leftMotor, rightMotor);

#ifdef USE_GYRO
	gyro = new ADXRS450_Gyro(SPI::kOnboardCS0);	//new gyro class name
	gyro->Calibrate();

	gyroVel = 0.0;
	gyroErr = 0.0;
	gyroErrInt = 0.0;
	gyroOn = true;
#else
	gyroOn = false;
#endif
}

void DriveTrain::DriveControl (float command_x, float command_y, float command_z, bool preciseTurn, bool isAuto)
{
	GetGyroPositionAndVelocity();
	if (!isAuto)  //creates a deadband during teleop
	{
		if(fabs(command_x) <= DRIVE_DEADBAND)
			command_x = 0.0;
		else if(command_x > 0.0)
			command_x = (command_x - DRIVE_DEADBAND)/(1.0 - DRIVE_DEADBAND);
		else
			command_x = (command_x + DRIVE_DEADBAND)/(1.0 - DRIVE_DEADBAND);

		if(fabs(command_y) <= DRIVE_DEADBAND)
			command_y = 0.0;
		else if(command_y > 0.0)
			command_y = (command_y - DRIVE_DEADBAND)/(1.0 - DRIVE_DEADBAND);
		else
			command_y = (command_y + DRIVE_DEADBAND)/(1.0 - DRIVE_DEADBAND);

		if(fabs(command_z) <= DRIVE_DEADBAND)
			command_z = 0.0;
		else if(command_z > 0.0)
			command_z = (command_z - DRIVE_DEADBAND)/(1.0 - DRIVE_DEADBAND);
		else
			command_z = (command_z + DRIVE_DEADBAND)/(1.0 - DRIVE_DEADBAND);
	}

#ifdef USE_GYRO
	float cmdRotate;
	float commandGain;
	if (preciseTurn)
	{
		cmdRotate = command_z * 0.5;
		gyroErrInt = 0.0;
	}
	else
	{
		cmdRotate = command_x;
	}

	if(gyroOn)
	{
		commandGain = 3.0 * 180.0/3.14; //3.5   // command of 3 radians per second at full joystick
		if (isAuto)
			cmdRotate = GyroControl(cmdRotate);  //command rotate is in degrees per second
		else
			cmdRotate = GyroControl(cmdRotate * commandGain);
	}else
	{
		commandGain = 0.8; //0.4;  //0.5
		cmdRotate = cmdRotate * commandGain;
	}
	//command_z = 1.0;
	driveBase->ArcadeDrive(command_y, cmdRotate);
	//printf("command_z %f cmdRotate %f v %f command_y %f\n", command_z, cmdRotate, gyroVel, command_y);
#else
	driveBase->ArcadeDrive(command_y, command_x);//sends normal commands if there is no gyro
#endif
}

void DriveTrain::ResetGyro(void)
{
	gyro->Reset();
}

void DriveTrain::GyroOn(void)
{
#ifdef USE_GYRO
	gyroOn = true;
#else
	gyroOn = false;  //gyro should be always off if there is no gyro or it is not being used
#endif
}

void DriveTrain::GyroOff(void)
{
	gyroOn = false;
}

float DriveTrain::GyroControl(float xCmd)
{
#ifdef USE_GYRO

	gyroErr = xCmd - gyroVel;
	gyroErrInt = gyroErrInt + gyroErr * LOOPTIME;
	float command_x = (K_GYRO / K_ROBOT) * (TAU_ROBOT * gyroErr + gyroErrInt);
	return command_x;
#else
	return xCmd;
#endif
}

double DriveTrain::GetGyroVel()
{
	return gyro->GetRate();
}

void DriveTrain::GetGyroPositionAndVelocity(void)
{
#ifdef USE_GYRO
	/*lastPos = currentPos;
	currentPos = GetArmAngle();
	if(firstTime)
	{
		lastPos = currentPos;
		firstTime = false;
	}
	currentSpeed = (currentPos - lastPos)/LOOPTIME;*/
	//gyroPos = GetGyroPos();
	gyroVel = GetGyroVel();
#endif
}

void DriveTrain::UpdateDash(void)
{
	SmartDashboard::PutNumber("Left Motor", leftMotor->Get());
	SmartDashboard::PutNumber("Right Motor", rightMotor->Get());
	SmartDashboard::PutBoolean("Gyro", gyroOn);
#ifdef USE_GYRO
	SmartDashboard::PutNumber("Gyro Reading", gyro->GetAngle());
#endif
}

void DriveTrain::Abort(void)
{
	DriveControl(0.0, 0.0, 0.0, false, false);
}

