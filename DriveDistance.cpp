/*
 * DriveDistance.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: jacob_000
 */
#include "WPILib.h"
#include "TalonXVII_main.h"
#include "DriveDistance.h"


DriveDistance::DriveDistance()
{
	distancePerPulse = DISTANCE_PER_PULSE;		//circumference of wheel divided by number of pulses on wheel encoder
	encoderLeft = new Encoder(DIGIN_ENCODER_LEFT_ONE, DIGIN_ENCODER_LEFT_TWO);
	encoderLeft->SetDistancePerPulse(-1.0 * distancePerPulse);   // left motor is inverted
	encoderLeft->Reset();
	encoderRight = new Encoder(DIGIN_ENCODER_RIGHT_ONE, DIGIN_ENCODER_RIGHT_TWO);
	encoderRight->SetDistancePerPulse(distancePerPulse);
	encoderRight->Reset();
}

void DriveDistance::Service()
{

}

double DriveDistance::GetLeftDistance()
{
	return	encoderLeft->GetDistance();
}

double DriveDistance::GetRightDistance()
{
	return encoderRight->GetDistance();
}

double DriveDistance::GetAverageDistance()
{
	return ((GetLeftDistance() + GetRightDistance()));
}

void DriveDistance::RestartEncoder()
{
	encoderLeft->Reset();
	encoderRight->Reset();
}


void DriveDistance::UpdateDash()
{
	SmartDashboard::PutNumber("Distance Traveled", GetAverageDistance());
	SmartDashboard::PutNumber("Distance left", GetLeftDistance());
	SmartDashboard::PutNumber("Distance right", GetRightDistance());
}

