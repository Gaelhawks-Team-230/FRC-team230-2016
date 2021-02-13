#include "WPILib.h"
#include "Common.h"
#include "DefenseManipulator.h"


// CONSTRUCTOR for Defense Manipulator
// -Includes basic functions for the defense manipulator mechanism.
DefenseManipulator::DefenseManipulator()
{
#ifdef USE_DEFENSE_MANIPULATOR
	// Create objects needed by this class
	rhinoHorn = new VictorSP(PWM_DEFENSE_MANIPULATOR);
	pdp = new PowerDistributionPanel();

	// call the class initialization function
	LocalReset();


	// code sample from http://wpilib.screenstepslive.com/s/4485/m/13809/l/219414-power-distribution-panel
	//Create an instance of the PowerDistributionPanel object to use it:
	//PowerDistributionPanel pdp = new PowerDistributionPanel();

	//Reading the per-channel current on the PDP:
	//You can read the current on individual channels of the PDP using the PowerDistributionPanel object.
	//To read the current on channel 1 use the method getCurrent:
	//double current = pdp.getCurrent(1);
	//This will return the current value in amps.
	//Note: the channel numbers are 0-based.
#endif
}

// Reset the values of flags and stop motors.
void DefenseManipulator::LocalReset()
{
	// Stop the motors.
	motorCmd = 0.0;
	stallCurrentCounter = 0;
}

// Stop all motors.
void DefenseManipulator::StopAll()
{
#ifdef USE_DEFENSE_MANIPULATOR
	motorCmd = 0.0;
	rhinoHorn->Set(motorCmd); //send motor command to motor
#endif
}
/*
// Move the arm out to lift defense
void DefenseManipulator::SetHorn(float motorIn)
{
	motorCmd = motorIn;
	rhinoHorn->Set(motorCmd);
}
*/

// Move the arm out to lift defense
void DefenseManipulator::SetHornOut()
{
	motorCmd = 1.0;
}

void DefenseManipulator::SetHornIn()
{
	motorCmd = -1.0;
}
// Returns true if arm is extended.
bool DefenseManipulator::IsArmInMotion()
{
    return (motorCmd != 0.f);
}

// Get the angle of the arm based on the encoder count.
void DefenseManipulator::GetCurrent()
{
#ifdef USE_DEFENSE_MANIPULATOR
	motorCurrent = pdp->GetCurrent(MOTOR_CURRENT_IN); //make into #define
#endif
}

// Update the DashBoard
void DefenseManipulator::UpdateDash()
{
//	SmartDashboard::PutBoolean("Arm Activated: ", IsArmInMotion());
	GetCurrent();
	SmartDashboard::PutNumber("Rhino Motor Current: ", motorCurrent);
	//SmartDashboard::PutNumber("Rhino Motor Command: ", motorCmd);
}

// Required service function for the class.
void DefenseManipulator::Service()
{
#ifdef USE_DEFENSE_MANIPULATOR
	if (IsArmInMotion())
	{
		GetCurrent();
		if(motorCurrent > STALL_CURRENT)
		{
			stallCurrentCounter++; // add stall count by one
			if(stallCurrentCounter > TARGET_STALL)
			{
				motorCmd = 0.0;// if stall current is more than the TARGET_STALL then stop motor
			}
		}
		else //else stall count equals 0
		{
			stallCurrentCounter = 0;
		}
	}
	else
	{
		stallCurrentCounter = 0;
	}
	rhinoHorn->Set(motorCmd); //send motor command to motor
#endif
}

void DefenseManipulator::Duck(void)
{
	//SetHornOut();
}

void DefenseManipulator::StartConfig(void)
{
	SetHornIn();
}
