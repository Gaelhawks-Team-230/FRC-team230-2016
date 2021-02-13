/*
 * Catapult.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: kcs26_000
 */
#include "WPILib.h"
#include "Common.h"
#include "BoulderGrab.h"
#include "TalonXVII_main.h"
#include "Catapult.h"


Catapult::Catapult()
{
	swingArm = new VictorSP(PWM_CATAPULT_ARM);
	armEncoder = new Encoder(DIGIN_ENCODER_CATAPULT_ONE, DIGIN_ENCODER_CATAPULT_TWO);
	armEncoder->SetDistancePerPulse((360.0/CATAPULT_ENCODER_PULSES) * (TEETH_PER_ARM_GEAR/TEETH_PER_MOTOR_SHAFT));
	armEncoder->Reset();

	LocalReset();
}


// Set initial values for ALL objects and variables
void Catapult::LocalReset()
{
	swingArm->Set(0.0);
	armEncoder->Reset();
	encoderAngle = GetArmAngle();
	catapultServiceState = STOPPED_STATE;
	shootingStage = IDLE_STAGE;
	rollingStage = IDLE_STAGE;
	firstTime = true;
	loopCount = 0;
	atBottomCount = 0;
	lastPos = 0.0;
	errorInt = 0.0;
	velCmd = 0.0;
	posCmd = 0.0;
	motorCmd = 0.0;
	currentSpeed = 0.0;
	currentPos = 0.0;
	posError = 0.0;
	error = 0.0;
}

void Catapult::StopAll()
{
	swingArm->Set(0.0);
	LocalReset();
}

double Catapult::GetArmAngle()//gets encoder distance and converts it to degrees
{
	double angle;
	angle = armEncoder->GetDistance();
	//angle = angle * (360.0/CATAPULT_ENCODER_PULSES) * (TEETH_PER_ARM_GEAR/TEETH_PER_MOTOR_SHAFT);

	return angle;
}

//calculates the current position and velocity of the catapult
void Catapult::GetPositionAndVelocity(void)
{
	lastPos = currentPos;
	currentPos = GetArmAngle();
	if(firstTime)
	{
		lastPos = currentPos;
		firstTime = false;
	}
	currentSpeed = (currentPos - lastPos)/LOOPTIME;
}

void Catapult::PositionControl(float posCmd)
{
	posError = posCmd - currentPos;

	velCmd = posError * CATAPULT_KP;
	error = velCmd - currentSpeed;
	errorInt = errorInt + error*LOOPTIME;

	motorCmd = CATAPULT_KF/CATAPULT_KR * (errorInt + CATAPULT_TAU_R * error);
	motorCmd = Limit(-0.1, 0.1, motorCmd);
}

void Catapult::VelocityControl(float velCmd)
{
	error = velCmd - currentSpeed;
	errorInt = errorInt + error*LOOPTIME;
	motorCmd = CATAPULT_KF/CATAPULT_KR * (errorInt + CATAPULT_TAU_R * error);
	motorCmd = Limit(-0.3, 0.3, motorCmd);
}

//creates a limit on the motor command if called
double Catapult::Limit(double min, double max, double curVal)
{
	if (curVal > max)
			return max;
	if (curVal < min)
			return min;
	return curVal;
}

//returns true if arm is firing/returning and false if not
bool Catapult::IsArmShooting()
{
	if(catapultServiceState == STOPPED_STATE)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//returns true if arm is rolling, waiting and/or returning, and false if not
bool Catapult::IsArmRolling()
{
	if(catapultServiceState == STOPPED_STATE)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//returns true if arm is all the way at the bottom and ready for a boulder
bool Catapult::ReadyForBoulder()
{
	float armAngle = GetArmAngle();

	if(armAngle <= 5.0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//puts the catapult into shooting state when Service function is called
void Catapult::Shoot()
{
	if(GLOBAL_GRABBER->OkayToShoot())
	{
		if(catapultServiceState == STOPPED_STATE)
		{
			loopCount = 0;
			catapultServiceState = SHOOTING_STATE;
		}
	}
	else
	{
		GLOBAL_GRABBER->PositionGatherer(SHOOTING_STOW_ANGLE);
	}
}

void Catapult::LongShot()
{
	if(GLOBAL_GRABBER->OkayToShoot())
	{
		if(catapultServiceState == STOPPED_STATE)
		{
			loopCount = 0;
			catapultServiceState = LONG_SHOT_STATE;
		}
	}
	else
	{
		GLOBAL_GRABBER->PositionGatherer(SHOOTING_STOW_ANGLE);
	}
}

void Catapult::BatterShot()
{
	if(GLOBAL_GRABBER->OkayToShoot())
	{
		if(catapultServiceState == STOPPED_STATE)
		{
			loopCount = 0;
			catapultServiceState = BATTER_SHOT_STATE;
		}
	}
	else
	{
		GLOBAL_GRABBER->PositionGatherer(SHOOTING_STOW_ANGLE);
	}
}

void Catapult::CornerShot()
{
	if(GLOBAL_GRABBER->OkayToShoot())
	{
		if(catapultServiceState == STOPPED_STATE)
		{
			loopCount = 0;
			catapultServiceState = CORNER_SHOT_STATE;
		}
	}
	else
	{
		GLOBAL_GRABBER->PositionGatherer(SHOOTING_STOW_ANGLE);
	}
}

//puts the catapult into rolling state when Service function is called
void Catapult::Roll()
{
	if(GLOBAL_GRABBER->OkayToShoot())
	{
		if(catapultServiceState == STOPPED_STATE)
		{
			loopCount = 0;
			catapultServiceState = ROLLING_STATE;
		}
	}
	else
	{
		GLOBAL_GRABBER->PositionGatherer(SHOOTING_STOW_ANGLE);
	}
}

//puts the catapult into Duck state when Service function is called
void Catapult::Duck()
{
	if(catapultServiceState == STOPPED_STATE)
	{
		loopCount = 0;
		atBottomCount = 0;
		catapultServiceState = DUCK_STATE;
	}

}

//puts the catapult into Start Configuration state when Service function is called
void Catapult::StartConfig()
{
	Duck();
}

//sends command to the motor
void Catapult::SetArmSpeed(void)
{
	motorCmd = Limit(-1.0, 1.0, motorCmd);//gives a limit to the command before it is sent to the motor
	swingArm->Set(motorCmd);
}

void Catapult::ShootingServiceRoutine(void)
{
	if(loopCount < SHOOTING_COUNT)
	{
		errorInt = 0.0;
		motorCmd = launchProfile[loopCount];
		SetArmSpeed();
	}
	else
	{
		if(loopCount < COAST_COUNT)
		{
			velCmd = 0.0;
			atBottomCount = 0;
		}
		else
		{
			velCmd = -RETURN_RATE;
			if(lastPos != currentPos)
			{
				atBottomCount = 0;
			}
			else
			{
				atBottomCount++;
				if (atBottomCount >= WAIT_FOR_AT_BOTTOM)
				{
					// we are done waiting... let it exit shooting mode below
					loopCount = RETURN_COUNT;
				}
			}
		}
		VelocityControl(velCmd);
	}
	loopCount++;
	if(loopCount >= RETURN_COUNT)
	{
		loopCount = 0;
		catapultServiceState = STOPPED_STATE;
		armEncoder->Reset();
		errorInt = 0.0;
		motorCmd = 0.0;
	}
}

void Catapult::RollingServiceRoutine(void)
{
	if(loopCount < ROLL_SHOOTING_COUNT)
	{
		velCmd = ROLL_FIRE_VEL;
	}
	else if(loopCount < ROLL_COAST_COUNT)
	{
		velCmd = 0.0;
		atBottomCount = 0;
	}
	else
	{
		velCmd = -RETURN_RATE;
		if(lastPos != currentPos)
		{
			atBottomCount = 0;
		}
		else
		{
			atBottomCount++;
			if (atBottomCount >= WAIT_FOR_AT_BOTTOM)
			{
				// we are done waiting... let it exit shooting mode below
				loopCount = RETURN_COUNT;
			}
		}
	}
	VelocityControl(velCmd);
	loopCount++;
	if(loopCount >= ROLL_RETURN_COUNT)
	{
		loopCount = 0;
		catapultServiceState = STOPPED_STATE;
		armEncoder->Reset();
		errorInt = 0.0;
		motorCmd = 0.0;
	}
}

void Catapult::LongShotServiceRoutine()
{
	if(loopCount < LONG_SHOOTING_COUNT)
	{
		errorInt = 0.0;
		motorCmd = longLaunchProfile[loopCount];
		SetArmSpeed();
	}
	else
	{
		if(loopCount < LONG_COAST_COUNT)
		{
			velCmd = 0.0;
			atBottomCount = 0;
		}
		else
		{
			velCmd = -RETURN_RATE;
			if(lastPos != currentPos)
			{
				atBottomCount = 0;
			}
			else
			{
				atBottomCount++;
				if (atBottomCount >= WAIT_FOR_AT_BOTTOM)
				{
					// we are done waiting... let it exit shooting mode below
					loopCount = RETURN_COUNT;
				}
			}
		}
		VelocityControl(velCmd);
	}
	loopCount++;
	if(loopCount >= LONG_RETURN_COUNT)
	{
		loopCount = 0;
		catapultServiceState = STOPPED_STATE;
		armEncoder->Reset();
		errorInt = 0.0;
		motorCmd = 0.0;
	}
}

void Catapult::BatterShotServiceRoutine()
{
	if(loopCount < BATTER_SHOOTING_COUNT)
	{
		errorInt = 0.0;
		motorCmd = batterLaunchProfile[loopCount];
		SetArmSpeed();
	}
	else
	{
		if(loopCount < BATTER_COAST_COUNT)
		{
			velCmd = 0.0;
			atBottomCount = 0;
		}
		else
		{
			velCmd = -RETURN_RATE;
			if(lastPos != currentPos)
			{
				atBottomCount = 0;
			}
			else
			{
				atBottomCount++;
				if (atBottomCount >= WAIT_FOR_AT_BOTTOM)
				{
					// we are done waiting... let it exit shooting mode below
					loopCount = RETURN_COUNT;
				}
			}
		}
		VelocityControl(velCmd);
	}
	loopCount++;
	if(loopCount >= BATTER_RETURN_COUNT)
	{
		loopCount = 0;
		catapultServiceState = STOPPED_STATE;
		armEncoder->Reset();
		errorInt = 0.0;
		motorCmd = 0.0;
	}
}
void Catapult::CornerShotServiceRoutine(void)
{
	if(loopCount < CORNER_SHOOTING_COUNT)
	{
		errorInt = 0.0;
		motorCmd = cornerlaunchProfile[loopCount];
		SetArmSpeed();
	}
	else
	{
		if(loopCount < CORNER_COAST_COUNT)
		{
			velCmd = 0.0;
			atBottomCount = 0;
		}
		else
		{
			velCmd = -RETURN_RATE;
			if(lastPos != currentPos)
			{
				atBottomCount = 0;
			}
			else
			{
				atBottomCount++;
				if (atBottomCount >= WAIT_FOR_AT_BOTTOM)
				{
					// we are done waiting... let it exit shooting mode below
					loopCount = CORNER_RETURN_COUNT;
				}
			}
		}
		VelocityControl(velCmd);
	}
	loopCount++;
	if(loopCount >= CORNER_RETURN_COUNT)
	{
		loopCount = 0;
		catapultServiceState = STOPPED_STATE;
		armEncoder->Reset();
		errorInt = 0.0;
		motorCmd = 0.0;
	}
}
// All classes need a Service function, even if
void Catapult::Service()
{
	GetPositionAndVelocity();

	switch(catapultServiceState)
	{
		case SHOOTING_STATE:
			ShootingServiceRoutine();
			break;

		case ROLLING_STATE:
			RollingServiceRoutine();
			break;

		case DUCK_STATE:
			DuckService();
			break;

		case LONG_SHOT_STATE:
			LongShotServiceRoutine();
			break;

		case BATTER_SHOT_STATE:
			BatterShotServiceRoutine();
			break;

		case CORNER_SHOT_STATE:
			CornerShotServiceRoutine();
			break;
		default:
			motorCmd = Limit(-0.1, 0.1, KP_HOLD*currentPos + KV_HOLD*currentSpeed);
			break;
	}

	//printf("Motor Command %f Arm Position %f Arm Velocity %f \n",motorCmd, currentPos, currentSpeed);
	SetArmSpeed();
}

//sends catapult to the bottom to make sure the catapult is completely lowered when the robot turns on
void Catapult::ArmStartAtBottom()
{
	int currentHeight = 500;
	int lastHeight = 1000;
	int loopCount = 0;

	motorCmd = 0.2;
	SetArmSpeed();
	Wait(0.1);

	while(currentHeight != lastHeight)
	{
		lastHeight = currentHeight;
		currentHeight = armEncoder->GetDistance();
		loopCount++;
		if(loopCount >= 1000)
		{
			break;
		}
		Wait(0.04);
	}

	motorCmd = 0.0;
	SetArmSpeed();
	armEncoder->Reset();
}

void Catapult::DuckService()
{
	velCmd = -RETURN_RATE;
	if(lastPos != currentPos)
	{
		atBottomCount = 0;
	}
	else
	{
		atBottomCount++;
		if (atBottomCount >= WAIT_FOR_AT_BOTTOM)
		{
			// we are done waiting... let it exit shooting mode below
			loopCount = RETURN_COUNT;
		}
	}
	VelocityControl(velCmd);

	loopCount++;
	if(loopCount >= DUCK_RETURN_COUNT)
	{
		loopCount = 0;
		catapultServiceState = STOPPED_STATE;
		armEncoder->Reset();
		errorInt = 0.0;
		motorCmd = 0.0;
	}
}

void Catapult::UpdateDash()
{
	SmartDashboard::PutNumber("Catapult Encoder: ", armEncoder->Get());
	SmartDashboard::PutNumber("Catapult Angle: ", GetArmAngle());
	SmartDashboard::PutNumber("Catapult motorCmd: ", motorCmd);
	//SmartDashboard::PutBoolean("Catapult Shooting: ", IsArmShooting());
	//SmartDashboard::PutBoolean("Boulder Rolling: ", IsArmRolling());
}

