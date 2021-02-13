
#include "WPILib.h"
#include "Common.h"
#include "BoulderGrab.h"


// Constructor for Boulder Grab
// - Class for the boulder gathering mechanism and positioner.
BoulderGrab::BoulderGrab()
{
   gatherer = new VictorSP(PWM_BOULDERGRAB);
   grabPositioner = new VictorSP(PWM_GRABPOSITIONER);
   positionerAngleCalculator = new AnalogInput(GRAB_ANALOG_INPUT);
   boulderDetector = new DigitalInput(GRAB_BOULDER_DETECTOR);
   positionerOffsetDegrees = 0.0;
   LocalReset();
}

// Resets And Initializes Motors, Flags, and Encoder.
void BoulderGrab::LocalReset()
{
   timeoutCount = 0;
   // Set all flags to default value. (Initialization)
   isPositionerInMotion = false;
   targetGathererAngle = 0.0;
   motorCmd = 0.0;

   // Update flags indicating that gatherer has been stopped/reset.
   gathererMode = IDLE;
   gathererSpeed = 0.0;
   isGathererEnabled = false;

   autoLoadActive = false;
   autoLoadStage = 0;
   deltaVoltage = 0.0;
   calibrationCmd = 0.0;
}

bool BoulderGrab::IsAutoLoadActive()
{
	return (autoLoadActive);
}

// Stop all motors, reset encoder, and reset all flags. -- Used in omnicide function.
void BoulderGrab::StopAll()
{
   gatherer->Set(0.0); // Stops the gatherer without using the service function.
   grabPositioner->Set(0.0);
   LocalReset();
}

// Activate the VictorSP responsible for gathering the boulders (at a positive speed).
void BoulderGrab::GrabBoulder(float grabSpeed)
{
   isGathererEnabled = true;
   gathererSpeed = -1.0 * grabSpeed;
   gathererMode = GATHERING;
}

void BoulderGrab::PreLoadBoulder(float grabSpeed)
{
   isGathererEnabled = true;
   gathererSpeed = -1.0 * grabSpeed;
   gathererMode = PRE_LOAD;
}

// Places boulder in lower goal using the gathering mechanism.
void BoulderGrab::EjectBoulder(float ejectSpeed)
{
   isGathererEnabled = true;
   gathererSpeed = ejectSpeed;
   gathererMode = EJECTING;
}

// Move ball from Gatherer into the catapult.
void BoulderGrab::LoadBoulder(float loadSpeed)
{
   isGathererEnabled = true;
   gathererSpeed = -1.0 * loadSpeed;
   gathererMode = LOADING;
}

// Stops the gatherer mechanism.
void BoulderGrab::StopGatherer()
{
	if(gathererMode != LOADING && gathererMode != PRE_LOAD)
	{
		isGathererEnabled = false;
		gathererSpeed = 0.0;
		gathererMode = IDLE;
		//printf("Gatherer Stopped!");
	}
}

// Use VictorSP to position the gatherer (Required to successfully use the service function called after this function).
void BoulderGrab::PositionGatherer(float setTargetGathererAngle)
{
   targetGathererAngle = setTargetGathererAngle;
   isPositionerInMotion = true;
   timeoutCount = 0;
}

// Returns the gatherer value used to set the speed of the gatherer VictorSP.
float BoulderGrab::GetSpeed()
{
   return gathererSpeed;
}

// Returns true if the boulder has been collected.
bool BoulderGrab::HasBoulder()
{
	if (boulderDetector->Get())
	{
	   return(false);
	}
	else
	{
	   return(true);
	}
}

bool BoulderGrab::OkayToShoot()
{
	if(GetGathererAngle() < MAX_SHOOTING_ANGLE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Returns the current angle of the gatherer using the analog's voltage.
float BoulderGrab::GetGathererAngle()
{
   float gathererAngle = (positionerAngleCalculator->GetVoltage() * DEGREES_PER_VOLT) - positionerOffsetDegrees;
   return gathererAngle;
}

bool BoulderGrab::IsAtTargetPosition()
{
	if (posError < AT_TARGET_ANGLE_TOLERANCE)
	   return true;
	else
		return false;
}

bool BoulderGrab::IsPositionerInMotion()
{
	return isPositionerInMotion;
}

// Function responsible for the auto-calibration of the analog.
void BoulderGrab::CalibrateGrabAnalog()
{
	float curVoltage = 0;
	float lastVoltage = 4;
	int loopCount = 0;
	float calibrationPos;
	calibrationCmd = -1.0;
	grabPositioner->Set(calibrationCmd);
	Wait(0.1);
	// Continue moving the motor until it reaches the minimum angle.
	while (curVoltage != lastVoltage)
	{
		lastVoltage = curVoltage;
		curVoltage = positionerAngleCalculator->GetVoltage();
		deltaVoltage = curVoltage - lastVoltage;
		calibrationCmd = calibrationCmd + (0.02 - 2 * deltaVoltage);
		grabPositioner->Set(calibrationCmd);
		loopCount++;
        // If the loop continues for too long, break the loop.
		if (loopCount >= 1000)
		{
			printf("Positioner calibration stopped!\n");
			break;
		}
		Wait(0.04);
	}
	// When the angle is equivalent to the minimum angle:
	printf("Positioner at maximum height!\n");
	grabPositioner->Set(0.0);
	calibrationPos = (positionerAngleCalculator->GetVoltage() * DEGREES_PER_VOLT);
	positionerOffsetDegrees = calibrationPos - CALIBRATION_ANGLE;

	curPos = GetGathererAngle(); // Set the new current position.
	lastPos = curPos; // Set current position to last position.
		curVelocity = (curPos - lastPos)/LOOPTIME; // Calculate the positioner's current velocity.
		velocityErrorInt = 0.0;
}

// Limit function.
double BoulderGrab::Limit(double min, double max, double curValue)
{
	if(curValue > max)
		return max;
	if(curValue < min)
		return min;
	return curValue;
}

// Constantly calculates velocity used for position control.
void BoulderGrab::CalculatePositionerVelocity()
{
	lastPos = curPos; // Set current position to last position.
	curPos = GetGathererAngle(); // Set the new current position.
	curVelocity = (curPos - lastPos)/LOOPTIME; // Calculate the positioner's current velocity.
}
// Calculate values needed in the position ramp function.
void BoulderGrab::PositionerPosControl()
{
	//CalculatePositionerVelocity();
	posError = targetGathererAngle - curPos;
	velocityCmd = POSITIONER_KP * posError;
	velocityError = velocityCmd - curVelocity;
	velocityErrorInt = velocityErrorInt + velocityError * LOOPTIME;
	velocityErrorInt = Limit(-10.0, 10.0, velocityErrorInt);

	 // For Velocity Control.
	motorCmd = (POSITIONER_KF/POSITIONER_KR) * (velocityError * POSITIONER_TAU_R + velocityErrorInt);
	motorCmd = Limit(-1.0, 1.0, motorCmd); // Limited motor command (speed).


	// Used for calculation of the KF, KP, KR, and TAU_R values.
	//printf("targetA: %f, curA: %f, curV: %f, mCmd: %f, vCmd %f, vError %f, vErrorInt %f \n", targetGathererAngle, curPos, curVelocity, motorCmd, velocityCmd, velocityError, velocityErrorInt);
}


// Calculate the position ramp for the positioner mechanism.
void BoulderGrab::CalculatePositionerPosRamp()
{
	//PositionerPosControl();
	deltaPos = MAX_VELOCITY * LOOPTIME;
	posCmd = posCmd + deltaPos;
}

// Service function for the gatherer mechanism.
void BoulderGrab::GathererService()
{
   // If the gatherer is enabled by either the Eject or Gather function, this part of service function will run.
   switch (gathererMode)
   {
      case GATHERING:
    	 // When the ball is successfully collected, stop this part of the service function
         /*if (HasBoulder())
         {
    	    gathererSpeed = 0.0;
            isGathererEnabled = false;
            gathererMode = IDLE;
         }*/
         break;

      case PRE_LOAD:
    	 // When the ball is successfully collected, stop this part of the service function
         if (HasBoulder())
         {
    	    gathererSpeed = 0.0;
            isGathererEnabled = false;
            gathererMode = IDLE;
         }
         break;

     case LOADING:
   	     // If the loading function was called, the speed of the gatherer will be set to a negative value.
         if (!HasBoulder())
         { 
            // When the ball is successfully loaded, stop this part of the service function.
          	gathererSpeed = 0.0;
            isGathererEnabled = false;
            gathererMode = IDLE; 
         }
         break;

      case EJECTING:
         // If the ejecting function was called, the speed of the gatherer will be set to a negative value.
         /*if (!HasBoulder())
         { // When the ball is successfully ejected, stop this part of the service function.
          	 gathererSpeed = 0.0;
            isGathererEnabled = false;
            gathererMode = IDLE; // May want to keep in EJECTING state if want to turn back on.
         }*/
         break;

      case IDLE:
      default:
	      gathererSpeed = 0.0;
	      isGathererEnabled = false;
          break;
   }
}

// Activate the Auto Load Service Function
void BoulderGrab::AutoLoadBoulder()
{
	if(!autoLoadActive)
	{
		autoLoadActive = true;
		autoLoadStage = 0;
	}
}

// Transfer boulder from the gatherer to the catapult.
void BoulderGrab::AutoLoadService()
{
	switch(autoLoadStage)
	{
		case 0:
			// Move catapult down to bottom.
			GLOBAL_CATAPULT->StartConfig();
			PreLoadBoulder(0.7);
			autoLoadStage++;
			break;

		case 1: // Wait until the catapult is at position.
			if(GLOBAL_CATAPULT->ReadyForBoulder())
			{
				// Move positioner to loading angle.
			    PositionGatherer(AUTO_LOAD_ANGLE);
			    autoLoadStage++;
			}
			break;

		case 2: // Wait until positioner at target angle.
			if (IsAtTargetPosition())
			{
			   // Load boulder into the catapult.
			   LoadBoulder(LOAD_BOULDER_SPEED);
			   autoLoadStage++;
			   loopCount = 0;
			}
			break;

		case 3: // Move positioner to angle where catapult is clear to shoot.
			loopCount++;
			if (!HasBoulder() || loopCount > MAX_LOAD_TIME)
			{
	         	gathererSpeed = 0.0;
	            isGathererEnabled = false;
	            gathererMode = IDLE;

				PositionGatherer(SHOOTING_STOW_ANGLE);
				autoLoadStage++;
			}
			break;

		case 4: // Move positioner to angle where catapult is clear to shoot.
			if (IsAtTargetPosition())
			{
				autoLoadActive = false;
				//printf("Boulder Loaded Into Catapult!\n");
				autoLoadStage++;
			}
			break;
	}
}

// Used when ducking under the lowbar.
void BoulderGrab::Duck()
{
	PositionGatherer(LOW_BAR_ANGLE);
}

// Sets the position of the positioner to its starting angle.
void BoulderGrab::StartConfig()
{
	PositionGatherer(CALIBRATION_ANGLE);
}

// Calls the service function, required for the class.
void BoulderGrab::Service()
{
   // Gets a constantly update value for the positioner's velocity.
   CalculatePositionerVelocity();

   if (autoLoadActive)
   {
	   AutoLoadService();
   }

   // ***The PositionGatherer() function MUST be called for this part of the service function to work. If statement verifies that the speed target angle was set.***
   if (isPositionerInMotion && (timeoutCount < MAX_TIMEOUT_COUNT))
   {
	   PositionerPosControl();
	   CalculatePositionerPosRamp();
	   timeoutCount++;
   }
   else
   {
	   motorCmd = 0.0;
	   isPositionerInMotion = false;
	   timeoutCount = 0;
   }
   grabPositioner->Set(motorCmd);

   // ***The GrabBoulder(), LoadBoulder(), or EjectBoulder() function MUST be called for this part of the service function to activate.***
   if (isGathererEnabled)
   {
      GathererService();
   }
   else
   {
	  gathererSpeed = 0.0;
   }
   gatherer->Set(gathererSpeed);
}

// Updates the DashBoard with all values relevant to the function.
void BoulderGrab::UpdateDash()
{
   SmartDashboard::PutBoolean("Has Boulder: ", HasBoulder());
   //SmartDashboard::PutBoolean("Gatherer Enabled: ", isGathererEnabled);
   //SmartDashboard::PutBoolean("Positioner Activated: ", isPositionerInMotion); // Can be removed -- used for testing.
   SmartDashboard::PutBoolean("Positioner At Target Angle: ", IsAtTargetPosition()); // Can be removed -- used for testing.
   SmartDashboard::PutNumber("Positioner Current Angle(degrees): ", GetGathererAngle());
   SmartDashboard::PutNumber("Positioner Analog Voltage: ", positionerAngleCalculator->GetVoltage()); // Can be removed -- used for testing.
   SmartDashboard::PutNumber("Positioner Offset (Degrees): ", positionerOffsetDegrees); // Can be removed -- used for testing.
   SmartDashboard::PutNumber("Motor Command: ", motorCmd);
}
