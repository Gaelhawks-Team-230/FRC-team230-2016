/*
 * Hanger.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: Gaelhawks
 */
#include "WPILib.h"
#include "Common.h"
#include "TalonXVII_main.h"
#include "Hanger.h"


Hanger::Hanger()//497 pulses per revolution
{
	rightHangerArm = new VictorSP(PWM_RIGHT_HANGER_ARM);
	leftHangerArm = new VictorSP(PWM_LEFT_HANGER_ARM);
	rightArmEncoder = new Encoder(RIGHT_ARM_ENCODER_ONE, RIGHT_ARM_ENCODER_TWO);
	leftArmEncoder = new Encoder(LEFT_ARM_ENCODER_ONE, LEFT_ARM_ENCODER_TWO);
	rightArmEncoder->Reset();
	leftArmEncoder->Reset();
	LocalReset();
}


// Set initial values for ALL objects and variables
void Hanger::LocalReset()
{
	rightHangerArm->Set(0.0);
	leftHangerArm->Set(0.0);
}

//
void Hanger::StopAll()
{
	rightHangerArm->Set(0.0);
	leftHangerArm->Set(0.0);
}

void Hanger::RotateToPosition()
{
	//hangerState = ROTATE_TO_POSITION;
}

void Hanger::LiftRobot()
{
	//hangerState = LIFT_ROBOT;
}

void Hanger::RotateService()
{

}

void Hanger::LiftService()
{

}

void Hanger::Service()
{
}

// Update the display of all relevant information in your class
void Hanger::UpdateDash()
{
	//examples:
	//SmartDashboard::PutNumber("Sample PWM: ", sampleMotor->Get());
	//SmartDashboard::PutBoolean("Sample Enabled: ", isEnabled);
}

void Hanger::Duck()
{

}

void Hanger::StartConfig()
{

}


