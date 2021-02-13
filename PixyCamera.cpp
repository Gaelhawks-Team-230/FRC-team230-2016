/*
 * PixyCamera.cpp
 *
 *  Created on: Mar 30, 2016
 *      Author: Gaelhawks
 */
#include "WPILib.h"
#include "Common.h"
#include "PixyCamera.h"



PixyCamera::PixyCamera()
{
	goalLocator = new AnalogInput(PIXY_ANALOG_INPUT);
	goalDetector = new DigitalInput(PIXY_GOAL_DETECTOR);

	LocalReset();
}

// Set initial values for ALL objects and variables
void PixyCamera::LocalReset()
{
	lateralTrimValue = 0;
}


void PixyCamera::StopAll()
{
}


bool PixyCamera::GoalDetected()
{
	if(goalDetector->Get())
	{
		return true;
	}
	else
	{
		return false;
	}
}


float PixyCamera::GoalLocation()
{
	float cameraVoltage;

	cameraVoltage = goalLocator->GetVoltage();
	return cameraVoltage;
}

void PixyCamera::SetLateralTrim(int trimValueIn)
{
	lateralTrimValue = trimValueIn;
}


bool PixyCamera::OKtoShoot(float desiredGoal)
{
	// use lateralTrimValue in here somehow???
    // like: (CENTERED_GOAL_VOLTAGE + lateralTrimValue*0.1)
	if(GoalDetected() && (fabs)(GoalLocation() - desiredGoal) < VOLTAGE_TOLERANCE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// All classes need a Service function, even if
void PixyCamera::Service()
{
}


void PixyCamera::UpdateDash()
{
	SmartDashboard::PutBoolean("Pixy OK to Shoot: ", OKtoShoot(CENTERED_GOAL_VOLTAGE));
	SmartDashboard::PutBoolean("Pixy Goal Detected: ", GoalDetected());
	SmartDashboard::PutNumber("Pixy Camera Voltage: ", GoalLocation());
	SmartDashboard::PutNumber("Pixy Lateral Trim: ", lateralTrimValue);
}


