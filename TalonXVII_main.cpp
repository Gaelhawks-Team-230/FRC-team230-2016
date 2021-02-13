#include "WPILib.h"
#include "TalonXVII_main.h"

/**
 * This is a demo program showing the use of the RobotDrive class.
 */
TalonXVII::TalonXVII()
{
	//Note SmartDashboard is not initialized here, wait until RobotInit to make SmartDashboard calls
	defenseChooser = new SendableChooser();
	//lateralTrimChooser = new SendableChooser();
	positionChooser = new SendableChooser();

	driveStick = new Joystick(0);
	gamepad = new Joystick(1);
	cameraServo = new Servo(PWM_CAMERA_SERVO);
	flashlight = new Relay(RELAY_FLASHLIGHT);


	drive = new DriveTrain();
	catapult = new Catapult();
	boulderGrab = new BoulderGrab();
	driveDistance = new DriveDistance();
	rhinoHorn = new DefenseManipulator();
	pixyCam = new PixyCamera();

	dashCounter = 0;
	firstTime = true;
	autoLoadedFlag = false;

	loopTime = LOOPTIME;
	startTime = 0.0;
	curTime = 0.0;
	waitTime = 0.0;
	loopOverTime = false;
	NumLoopsOverTime = 0;
	loopCount = 0;
	defenseSelected = 0;
	autoTracking = false;
	flashlight->Set(Relay::kOff);
}

void TalonXVII::RobotInit()
{
	defenseChooser->AddDefault("Low Bar", (void*)&DEFENSE_lowbar);
	defenseChooser->AddObject("Portcullis", (void*)&DEFENSE_portcullis);
	defenseChooser->AddObject("Cheval De Frise", (void*)&DEFENSE_chevaldefrise);
	defenseChooser->AddObject("Ramparts", (void*)&DEFENSE_ramparts);
	defenseChooser->AddObject("Moat", (void*)&DEFENSE_moat);
	defenseChooser->AddObject("2 ball", (void*)&DEFENSE_lowbar);
	defenseChooser->AddObject("Rock Wall", (void*)&DEFENSE_rockwall);
	defenseChooser->AddObject("Rough Terrain", (void*)&DEFENSE_roughterrain);
	defenseChooser->AddObject("0-DISABLE", (void*)&DEFENSE_disabled);
	defenseChooser->AddObject("Reach ONLY", (void*)&DEFENSE_reach);
	SmartDashboard::PutData("Select Defense", defenseChooser);

	positionChooser->AddDefault("No Shoot", (void*)&NO_Shoot);
	positionChooser->AddObject("Position 2", (void*)&POSITION_two);
	positionChooser->AddObject("Position 3", (void*)&POSITION_three);
	positionChooser->AddObject("Position 4", (void*)&POSITION_four);
	positionChooser->AddObject("Position 5", (void*)&POSITION_five);
	SmartDashboard::PutData("Select Position", positionChooser);

/*	lateralTrimChooser->AddObject("-4", (void*)&NEGATIVE_FOUR_LATERAL);
	lateralTrimChooser->AddObject("-2", (void*)&NEGATIVE_TWO_LATERAL);
	lateralTrimChooser->AddDefault("0", (void*)&ZERO_LATERAL);
	lateralTrimChooser->AddObject("2", (void*)&POSITIVE_TWO_LATERAL);
	lateralTrimChooser->AddObject("4", (void*)&POSITIVE_FOUR_LATERAL);
	SmartDashboard::PutData("Select Lateral Trim", lateralTrimChooser);
	*/

	ServiceDash();
#ifdef USE_CAMERA
	CameraServer::GetInstance()->SetQuality(50);
	//CameraServer::GetInstance()->SetQuality(0);
	//the camera name (ex "cam0") can be found through the roborio web interface
	CameraServer::GetInstance()->StartAutomaticCapture("cam0");
#endif
}

/*
** Runs when disabled
*/
void TalonXVII::Disabled()
{

	Wait(1.0);
	autoLoadedFlag = false;

	while (!IsEnabled())
	{
		defenseSelected = *((int*)defenseChooser->GetSelected());
		SmartDashboard::PutNumber("selection: ", defenseSelected);

		positionSelected = *((int*)positionChooser->GetSelected());
		SmartDashboard::PutNumber("position selection: ", positionSelected);

		// give it some time to catch its breath
		Wait(0.5);
	}

}

/**
 * This autonomous (along with the chooser code above) shows how to select between different autonomous modes
 * using the dashboard. The sendable chooser code works with the Java SmartDashboard. If you prefer the LabVIEW
 * Dashboard, remove all of the chooser code and uncomment the GetString line to get the auto name from the text box
 * below the Gyro
 *
 * You can add additional auto modes by adding additional comparisons to the if-else structure below with additional strings.
 * If using the SendableChooser make sure to add them to the chooser code above as well.
 */
void TalonXVII::Autonomous()
{
	printf("defense selected %d\n", defenseSelected);
	defenseSelected = *((int*)defenseChooser->GetSelected());
	SmartDashboard::PutNumber("selection: ", defenseSelected);
	printf("defense selected %d\n", defenseSelected);

	printf("position selected %d\n", positionSelected);
	positionSelected = *((int*)positionChooser->GetSelected());
	SmartDashboard::PutNumber("position selection: ", positionSelected);
	printf("position selected %d\n", positionSelected);


	if(firstTime)
	{
		boulderGrab->CalibrateGrabAnalog();
		catapult->ArmStartAtBottom();
		firstTime = false;
	}

	int loopCounter=0;
	autoLoadedFlag = false;

	autoStage = 0;
	autoMode = 2;

	Wait(0.1);

#ifdef CHECK_LOOPTIMING
	double endTime;
	double totalLoopTime;
#endif


	drive->GyroOn();

	double initialStartTime;
	initialStartTime = GetTime();
	startTime = initialStartTime - loopTime;
//defenseSelected = 0;
printf("defense selected %d\n", defenseSelected);

	while (IsAutonomous() && IsEnabled())
	{
		startTime += loopTime;
		loopCounter++;

		if (defenseSelected == DEFENSE_disabled)
		{
			rotate_cmd = 0.0;
			drive_cmd = 0.0;
		}
		else
		{
			AutoModeBypassDefense(defenseSelected, positionSelected);
		}

		/*
		** COMMUNITY SERVICE
		*/
		// NOTE rotate_cmd is in degrees per second and drive_cmd is +/-1.0
		drive->DriveControl(rotate_cmd, drive_cmd, 0.0, false, true);
		CommunityService();
		ServiceDash();

		// ENFORCE LOOP TIMING
		curTime = GetTime();
		waitTime = loopTime - (curTime - startTime);
		if (waitTime < 0.0)
		{
			printf("WARNING! LOOP IS OVERTIME by %f seconds\n", waitTime*-1);
			loopOverTime = true;
			NumLoopsOverTime++;
		}
		else
		{
			Wait(waitTime);
			loopOverTime = false;

#ifdef CHECK_LOOPTIMING
			endTime = GetTime();
			totalLoopTime = endTime - startTime;
			printf("startTime: %f  curTime : %f  endTime: %f	Wait Time: %f  This Loop Time: %f  Total Delta Time: %f [avg: %f] \n",
					startTime, curTime, endTime, waitTime, totalLoopTime, endTime-initialStartTime, (endTime-initialStartTime)/loopCounter);
#endif

		}
	}
	Omnicide();
};


/**
 * Runs the motors with arcade steering.
 */
void TalonXVII::OperatorControl()
{
	int loopCounter=0;
	//bool init_flag = true;

	if(firstTime)
	{
		boulderGrab->CalibrateGrabAnalog();
		catapult->ArmStartAtBottom();
		firstTime = false;
	}

#ifdef CHECK_LOOPTIMING
	double endTime;
	double totalLoopTime;
#endif

	double initialStartTime;
	initialStartTime = GetTime();
	startTime = initialStartTime - loopTime;

	while (IsOperatorControl() && IsEnabled())
	{
		startTime += loopTime;
		loopCounter++;

		//lateralTrim = *((int*)lateralTrimChooser->GetSelected());
        //pixyCam->SetLateralTrim(lateralTrim);

		// BOULDER GRABBER OPERATIONS
		if(gamepad->GetRawButton(BUTTON_CARRY_POSITION)) // Button X
		{
			boulderGrab->PositionGatherer(CARRY_ANGLE);
		}
		if(gamepad->GetRawButton(BUTTON_PICKUP_POSITION)) // Button A
		{
			boulderGrab->PositionGatherer(PICKUP_ANGLE);
		}
		if(gamepad->GetRawButton(BUTTON_BOULDER_AUTOLOAD)) //Button B
		{
			boulderGrab->AutoLoadBoulder();
			//boulderGrab->LoadBoulder(0.5);
		}
		if(gamepad->GetRawButton(BUTTON_BOULDER_INFRAME)) //Button Y
		{
			boulderGrab->PositionGatherer(CALIBRATION_ANGLE);
		}

		if(gamepad->GetRawButton(BUTTON_BOULDER_GRAB)) // Right Bumper
		{
			boulderGrab->GrabBoulder(0.8);
		}
		else
		{
			if(gamepad->GetRawButton(BUTTON_BOULDER_EJECT)) // Right Trigger
			{
				boulderGrab->EjectBoulder(1.0);
			}
			else
			{
				boulderGrab->StopGatherer();
			}
		}
		if(gamepad->GetRawButton(BUTTON_STOP_GATHERER))
		{
			boulderGrab->StopAll();
		}

		// CATAPULT OPERATIONS
		if(gamepad->GetRawButton(BUTTON_CATAPULT_SHOOT))// Left Bumper
		{
			if(gamepad->GetRawAxis(SHOOT_LEVEL_AXIS) < -0.75)
			{
				catapult->Shoot();
			}
			else if(gamepad->GetRawAxis(SHOOT_LEVEL_AXIS) > 0.75)
			{
				catapult->LongShot();
			}
			else if(gamepad->GetRawAxis(CORNER_SHOOT_AXIS) < -0.75)
			{
				catapult->CornerShot();
			}

		}
		if(gamepad->GetRawButton(BUTTON_CATAPULT_ROLL))// Left Trigger
		{
			if(gamepad->GetRawAxis(SHOOT_LEVEL_AXIS) < -0.75)
			{
				catapult->BatterShot();
			}
			else if(gamepad->GetRawAxis(SHOOT_LEVEL_AXIS) > 0.75)
			{
				catapult->Roll();
			};
		}


		// DEFENSE MANIPULATOR OPERATIONS
		if(gamepad->GetPOV(0) == DPAD_RHINO_DOWN) // Button A
		{
			rhinoHorn->SetHornIn();
		}
		else if(gamepad->GetPOV(0) == DPAD_RHINO_UP) // Button A
		{
			rhinoHorn->SetHornOut();
		}


		//JOYSTICK BUTTONS
		if (driveStick->GetRawButton(ENABLE_GYRO_BUTTON))
		{
			drive->GyroOn();
		}
		else if (driveStick->GetRawButton(DISABLE_GYRO_BUTTON))
		{
			drive->GyroOff();
		}
		if(driveStick->GetRawButton(START_CONFIG_BUTTON))
		{
			RobotStartConfig();
		}
		if(driveStick->GetRawButton(DUCK_BUTTON))
		{
			RobotDuck();
		}
		if(driveStick->GetRawButton(ROTATE_CAMERA_FRONT))
		{
			cameraServo->Set(0.0);
		}
		if(driveStick->GetRawButton(ROTATE_CAMERA_BACK))
		{
		cameraServo->Set(0.95);
		}
		if(driveStick->GetRawButton(FLASHLIGHT_ON))
		{
			flashlight->Set(Relay::kOn);
		}
		else
		{
			flashlight->Set(Relay::kOff);
		}

#ifdef USE_AUTOTRACKING
		if(driveStick->GetRawButton(GOAL_TRACKING_BUTTON) && pixyCam->GoalDetected())
		{
			rotate_cmd = (CENTERED_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
			autoTracking = true;
			drive->DriveControl(rotate_cmd, driveStick->GetY(), 0.0, false, autoTracking);
		}
		else
		{
			rotate_cmd = driveStick->GetX();
			autoTracking = false;
			drive->DriveControl(driveStick->GetX(), driveStick->GetY(), driveStick->GetZ(), driveStick->GetRawButton(TURN_MODE_BUTTON), autoTracking);
		}
#else
		drive->DriveControl(driveStick->GetX(), driveStick->GetY(), driveStick->GetZ(), driveStick->GetRawButton(TURN_MODE_BUTTON), false);
#endif

		/*
		** COMMUNITY SERVICE
		*/
		CommunityService();
		ServiceDash();

		// ENFORCE LOOP TIMING
		curTime = GetTime();
		waitTime = loopTime - (curTime - startTime);
		if (waitTime < 0.0)
		{
			printf("WARNING! LOOP IS OVERTIME by %f seconds\n", waitTime*-1);
			loopOverTime = true;
			NumLoopsOverTime++;
		}
		else
		{
			Wait(waitTime);
			loopOverTime = false;

#ifdef CHECK_LOOPTIMING
			endTime = GetTime();
			totalLoopTime = endTime - startTime;
			printf("startTime: %f  curTime : %f  endTime: %f	Wait Time: %f  This Loop Time: %f  Total Delta Time: %f [avg: %f] \n",
					startTime, curTime, endTime, waitTime, totalLoopTime, endTime-initialStartTime, (endTime-initialStartTime)/loopCounter);
#endif
		}
	}
	Omnicide();
}

/**
 * Runs during test mode
 */
void TalonXVII::Test()
{

}

void TalonXVII::RobotDuck()
{
	boulderGrab->Duck();
	catapult->Duck();
	rhinoHorn->Duck();
}

void TalonXVII::RobotStartConfig()
{
	boulderGrab->StartConfig();
	catapult->StartConfig();
	rhinoHorn->StartConfig();
}

double TalonXVII::Limit(double min, double max, double curValue)
{
	if(curValue > max)
		return max;
	if(curValue < min)
		return min;
	return curValue;
}

void TalonXVII::Omnicide()
{
	boulderGrab->StopAll();
	catapult->StopAll();
	drive->Abort();
	driveDistance->RestartEncoder();
	rhinoHorn->StopAll();
}

void TalonXVII::CommunityService()
{
	driveDistance->Service();
	boulderGrab->Service();
	catapult->Service();
	rhinoHorn->Service();
	pixyCam->Service();
}

void TalonXVII::ServiceDash()
{
	if(dashCounter == 10)
	{
		SmartDashboard::PutBoolean("loopOverTime: ", loopOverTime);
		SmartDashboard::PutNumber("waitTime: ", waitTime);
		boulderGrab->UpdateDash();
		catapult->UpdateDash();
		drive->UpdateDash();
		driveDistance->UpdateDash();
		rhinoHorn->UpdateDash();
		pixyCam->UpdateDash();
	}
	else
	{
		dashCounter++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////  AUTONOMOUS MODES  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TalonXVII::AutoModeSpyBot()
{
	switch(autoStage)
	{
		case 0://start encoders
			driveDistance->RestartEncoder();
			drive_cmd = 0.0; rotate_cmd = 0.0;
			loopCount = 0;
			autoStage++;
			break;

		case 1://move forward a little
			if(driveDistance->GetAverageDistance() < AUTO_DRIVE_FORWARD_SMALL)
			{
				drive_cmd = -0.45; rotate_cmd = 0.0;
			}
			else
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
				driveDistance->RestartEncoder();
				boulderGrab->AutoLoadBoulder();
				loopCount = 0;
				autoStage++;
			}
			break;

		case 2://auto-load boulder
			if(boulderGrab->OkayToShoot())
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
				loopCount = 0;
				autoStage++;
				catapult->Shoot();
			}
			else
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
			}
			break;

		case 3://fire catapult
			if(catapult->IsArmShooting())
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
				loopCount++;
			}
			else
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
				loopCount = 0;
				autoStage++;
			}
			break;

		case 4://turn 90 degrees
			if(loopCount <= AUTO_TURN_90_DEGREES)
			{
				drive_cmd = 0.0; rotate_cmd = 90.0;
				loopCount++;
			}
			else
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
				loopCount = 0;
				autoStage++;
			}
			break;

		case 5://duck
			RobotDuck();
			drive_cmd = 0.0; rotate_cmd = 0.0;
			loopCount = 0;
			autoStage++;
			break;

		case 6://go forward under lowbar from courtyard
			if(driveDistance->GetAverageDistance() < AUTO_DRIVE_UNDER_LOWBAR)//FROM COURTYARD!!
			{
				drive_cmd = breachingSpeed[DEFENSE_LOW_BAR]; rotate_cmd = 0.0;
			}
			else
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
				driveDistance->RestartEncoder();
				loopCount = 0;
				autoStage++;
			}
			break;

		case 7://wait in neutral zone
			if(loopCount >= AUTO_SPYBOT_WAIT)
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
				loopCount = 0;
				autoStage++;
			}
			else
			{
				loopCount++;
			}
			break;

		/*case 8://back up back under lowbar
			if(driveDistance->GetAverageDistance() < autoBreachDistance[DEFENSE_LOW_BAR])
			{
				drive_cmd = -1.0 * breachingSpeed[DEFENSE_LOW_BAR]; rotate_cmd = 0.0;
				loopCount++;
			}
			else
			{
				drive_cmd = 0.0; rotate_cmd = 0.0;
				driveDistance->RestartEncoder();
				loopCount = 0;
				autoStage++;
			}
			break;*/
	}
}

void TalonXVII::AutoModeBypassDefense(int defenseType, int position)
{

	switch(autoStage)
	{
		case 0: // Drive until the robot reaches the defense.
			driveDistance->RestartEncoder();
			autoStage++;
			if (defenseType == DEFENSE_LOW_BAR)
			{
				RobotDuck();
			}
			if (defenseType == DEFENSE_PORTCULLIS)
			{
				rhinoHorn->SetHornOut();
				RobotDuck();
			}
			if (defenseType == DEFENSE_CHEVAL_DE_FRISE)
			{
				rhinoHorn->SetHornOut();
			}
			break;

		case 1: // Bypass and/or Manipulate Defense
			switch(defenseType)
			{
				case DEFENSE_REACH:
					if(driveDistance->GetAverageDistance() < AUTO_DEFENSE_REACH_DISTANCE)
					{
						drive_cmd = -0.75; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
						loopCount++;
					}
					else
					{
						drive_cmd = 0.0; rotate_cmd = 0.0;
						loopCount = 0;
						autoStage = 10;
					}
					break;

				case DEFENSE_LOW_BAR: // Low_Bar
				case DEFENSE_RAMPARTS: //Ramparts
				case DEFENSE_MOAT: //Moat
				case DEFENSE_ROCK_WALL: //Rock_Wall
					if(driveDistance->GetAverageDistance() < autoBreachDistance[defenseType])
					{
						drive_cmd = -1.0 * breachingSpeed[defenseType]; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
						loopCount++;
					}
					else
					{
						drive_cmd = 0.0; rotate_cmd = 0.0;
						loopCount = 0;
						autoStage++;
					}
					break;

				case DEFENSE_CHEVAL_DE_FRISE: //Cheval_De_Frise
				case DEFENSE_PORTCULLIS: //Portcullis
					switch(position)
					{
						case NO_SHOOT:
							breachDist = autoBreachDistance[defenseType];
							break;

						case POSITION_TWO:
						case POSITION_FIVE:
							breachDist = autoBreachDistance[defenseType];
							break;

						case POSITION_THREE:
						case POSITION_FOUR:
							breachDist = autoBreachDistance[defenseType] - 24;
							break;
					}

					if(driveDistance->GetAverageDistance() > AUTO_PORTCULLIS_DISTANCE)
					{
						rhinoHorn->SetHornIn();
					}
					if(driveDistance->GetAverageDistance() < breachDist)
					{
						drive_cmd = -1.0 * breachingSpeed[defenseType]; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
						loopCount++;
					}
					else
					{
						drive_cmd = 0.0; rotate_cmd = 0.0;
						loopCount = 0;
						autoStage++;
					}
					break;

				case DEFENSE_ROUGH_TERRAIN: //Rough_Terrain
					switch(position)
					{
						case NO_SHOOT:
						case POSITION_TWO:
							if(driveDistance->GetAverageDistance() < (autoBreachDistance[defenseType] + 18))
							{
								drive_cmd = -1.0 * breachingSpeed[defenseType]; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;

						case POSITION_THREE:
						case POSITION_FOUR:
						case POSITION_FIVE:
							if(driveDistance->GetAverageDistance() < autoBreachDistance[defenseType])
							{
								drive_cmd = -1.0 * breachingSpeed[defenseType]; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;
					}
					break;
			}
			break;

		case 2: //Waits until robot lands
			switch(defenseType)
			{
				case DEFENSE_REACH:
				case DEFENSE_RAMPARTS: //Ramparts
				case DEFENSE_MOAT: //Moat
				case DEFENSE_ROCK_WALL: //Rock_Wall
				case DEFENSE_ROUGH_TERRAIN: //Rough_Terrain
					driveDistance->RestartEncoder();
					loopCount++;
					if(loopCount > AUTO_WAIT_TIME)
					{
						autoStage++;
					}
					break;

				case DEFENSE_LOW_BAR: // Low_Bar
					driveDistance->RestartEncoder();
					if(!autoLoadedFlag)
					{
						boulderGrab->AutoLoadBoulder();
						autoLoadedFlag = true;
					}
					loopCount++;
					if(loopCount > AUTO_WAIT_TIME)
					{
						autoStage++;
						loopCount = 0;
					}
					break;

				case DEFENSE_PORTCULLIS: //Portcullis
					switch(position)
					{
						case NO_SHOOT:
							driveDistance->RestartEncoder();
							loopCount++;
							if(loopCount > AUTO_WAIT_TIME)
							{
								autoStage++;
							}
							break;

						case POSITION_TWO:
						case POSITION_THREE:
						case POSITION_FOUR:
						case POSITION_FIVE:
							driveDistance->RestartEncoder();
							if(!autoLoadedFlag)
							{
								boulderGrab->AutoLoadBoulder();
								autoLoadedFlag = true;
							}
							loopCount++;
							if(loopCount > AUTO_WAIT_TIME)
							{
								autoStage++;
								loopCount = 0;
							}
							break;
					}
					break;

				case DEFENSE_CHEVAL_DE_FRISE: //Cheval_De_Frise
					switch(position)
					{
						case NO_SHOOT:
							driveDistance->RestartEncoder();
							loopCount++;
							if(loopCount > AUTO_WAIT_TIME)
							{
								autoStage++;
							}
							break;

						case POSITION_TWO:
						case POSITION_THREE:
						case POSITION_FOUR:
						case POSITION_FIVE:
							driveDistance->RestartEncoder();
							if(!autoLoadedFlag)
							{
								boulderGrab->AutoLoadBoulder();
								autoLoadedFlag = true;
							}
							loopCount++;
							if(loopCount > AUTO_WAIT_TIME)
							{
								autoStage++;
								loopCount = 0;
							}
							break;
					}
					break;
			}
			break;


		case 3: //Drives foward to make sure defense is breached
			switch(defenseType)
			{
				case DEFENSE_LOW_BAR: // Low_Bar
                    // after loading pickup grabber so we don't impede turning
					if(!boulderGrab->IsAutoLoadActive())
					{
						boulderGrab->StartConfig();
					}
				    //turn until we see the goal
					if(pixyCam->GoalDetected() && loopCount >= AUTO_ROTATE_COUNT)
					{
						drive_cmd = 0.0; rotate_cmd = 0.0;
						loopCount = 0;
						autoStage++;
					}
					else
					{
						drive_cmd = 0.15; rotate_cmd = 45.0;
						loopCount++;
					}
					break;

				/*case DEFENSE_PORTCULLIS_3: //Portcullis_3
				    //turn until we see the goal
					if(!boulderGrab->IsAutoLoadActive())
					{
						if(pixyCam->GoalDetected())
						{
							drive_cmd = 0.0; rotate_cmd = 0.0;
							loopCount = 0;
							autoStage++;
						}
						else
						{
							drive_cmd = 0.0; rotate_cmd = 45.0;
							loopCount++;
						}
					}
					break;

				case DEFENSE_PORTCULLIS_4: //Portcullis_4
				    //turn until we see the goal
					if(!boulderGrab->IsAutoLoadActive())
					{
						if(pixyCam->GoalDetected())
						{
							drive_cmd = 0.0; rotate_cmd = 0.0;
							loopCount = 0;
							autoStage++;
						}
						else
						{
							drive_cmd = 0.0; rotate_cmd = -45.0;
							loopCount++;
						}
					}
					break;*/

				case DEFENSE_PORTCULLIS: //Portcullis
					switch(position)
					{
						case NO_SHOOT:
							drive_cmd = 0.0; rotate_cmd = 0.0;
							break;

						case POSITION_TWO:
						case POSITION_THREE:
							if(!boulderGrab->IsAutoLoadActive())
							{
								if(pixyCam->GoalDetected())
								{
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
								else
								{
									drive_cmd = 0.0; rotate_cmd = 45.0;
									loopCount++;
								}
							}
							break;

						case POSITION_FOUR:
						case POSITION_FIVE:
							if(!boulderGrab->IsAutoLoadActive())
							{
								if(pixyCam->GoalDetected())
								{
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
								else
								{
									drive_cmd = 0.0; rotate_cmd = -45.0;
									loopCount++;
								}
							}
							break;
					}
					break;

				case DEFENSE_CHEVAL_DE_FRISE: //Cheval_De_Frise
					switch(position)
					{
						case NO_SHOOT:
							drive_cmd = 0.0; rotate_cmd = 0.0;
							break;

						case POSITION_TWO:
						case POSITION_THREE:
							if(!boulderGrab->IsAutoLoadActive())
							{
								if(pixyCam->GoalDetected())
								{
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
								else
								{
									drive_cmd = 0.0; rotate_cmd = 45.0;
									loopCount++;
								}
							}
							break;

						case POSITION_FOUR:
						case POSITION_FIVE:
							if(!boulderGrab->IsAutoLoadActive())
							{
								if(pixyCam->GoalDetected())
								{
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
								else
								{
									drive_cmd = 0.0; rotate_cmd = -45.0;
									loopCount++;
								}
							}
							break;
					}
					break;

				case DEFENSE_ROUGH_TERRAIN: //Rough_Terrain
					switch(position)
					{
						case NO_SHOOT:
							if(driveDistance->GetAverageDistance() < AUTO_EXTRA_DRIVE)
							{
								drive_cmd = -1.0 * breachingSpeed[defenseType]; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;

						case POSITION_TWO:
						case POSITION_THREE:
							if(pixyCam->GoalDetected())
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 45.0;
								loopCount++;
							}
							break;

						case POSITION_FOUR:
						case POSITION_FIVE:
							if(pixyCam->GoalDetected())
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = -45.0;
								loopCount++;
							}
							break;
					}
					break;

				case DEFENSE_RAMPARTS: //Ramparts
				case DEFENSE_MOAT: //Moat
				case DEFENSE_ROCK_WALL: //Rock_Wall
					if(driveDistance->GetAverageDistance() < AUTO_EXTRA_DRIVE)
					{
						drive_cmd = -1.0 * breachingSpeed[defenseType]; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
						loopCount++;
					}
					else
					{
						drive_cmd = 0.0; rotate_cmd = 0.0;
						loopCount = 0;
						autoStage++;
					}
					break;
			}
			break;

		case 4:
			switch(defenseType)
			{
				case DEFENSE_LOW_BAR: // Low_Bar
				//case DEFENSE_PORTCULLIS_3: //Portcullis_3
				//case DEFENSE_PORTCULLIS_4: //Portcullis_4
					//autoTrack for few seconds to align
					if(!boulderGrab->IsAutoLoadActive())
					{
						boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
					}
                    if(loopCount < AUTO_WAIT_TARGETING)
					{
						drive_cmd = 0.0; rotate_cmd = (CENTERED_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
						loopCount++;
					}
					else
					{
						drive_cmd = 0.0; rotate_cmd = 0.0;
						loopCount = 0;
						autoStage++;
					}
					break;

				case DEFENSE_ROUGH_TERRAIN: //Rough_Terrain
					switch(position)
					{
						case NO_SHOOT:
							if(!autoLoadedFlag)
							{
								boulderGrab->AutoLoadBoulder();
								autoLoadedFlag = true;
							}
							break;

						case POSITION_TWO:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (POS_TWO_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;

						case POSITION_THREE:
						case POSITION_FOUR:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (CENTERED_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;

						case POSITION_FIVE:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (POS_FIVE_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;
					}
					break;

				case DEFENSE_PORTCULLIS: //Portcullis
					switch(position)
					{
						case NO_SHOOT:
							if(!autoLoadedFlag)
							{
								boulderGrab->AutoLoadBoulder();
								autoLoadedFlag = true;
							}
							break;

						case POSITION_TWO:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (POS_TWO_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;

						case POSITION_THREE:
						case POSITION_FOUR:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (CENTERED_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;

						case POSITION_FIVE:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (POS_FIVE_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;
					}
					break;

				case DEFENSE_RAMPARTS: //Ramparts
				case DEFENSE_MOAT: //Moat
				case DEFENSE_ROCK_WALL: //Rock_Wall
					if(!autoLoadedFlag)
					{
						boulderGrab->AutoLoadBoulder();
						autoLoadedFlag = true;
					}
					break;

				case DEFENSE_CHEVAL_DE_FRISE: //Cheval_De_Frise
					switch(position)
					{
						case NO_SHOOT:
							if(!autoLoadedFlag)
							{
								boulderGrab->AutoLoadBoulder();
								autoLoadedFlag = true;
							}
							break;

						case POSITION_TWO:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (POS_TWO_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;

						case POSITION_THREE:
						case POSITION_FOUR:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (CENTERED_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;

						case POSITION_FIVE:
							if(!boulderGrab->IsAutoLoadActive())
							{
								boulderGrab->PositionGatherer(SHOOTING_STOW_ANGLE);
							}
							if(loopCount < AUTO_WAIT_TARGETING)
							{
								drive_cmd = 0.0; rotate_cmd = (POS_FIVE_GOAL_VOLTAGE - pixyCam->GoalLocation()) * K_AUTO_TRACK;
								loopCount++;
							}
							else
							{
								drive_cmd = 0.0; rotate_cmd = 0.0;
								loopCount = 0;
								autoStage++;
							}
							break;
					}
					break;
			}
			break;

		case 5:
			switch(defenseType)
			{
				case DEFENSE_LOW_BAR:
					if(pixyCam->OKtoShoot(CENTERED_GOAL_VOLTAGE))
					{
						if(!boulderGrab->IsAutoLoadActive())
						{
							//catapult->LongShot();
							//catapult->Shoot();
							catapult->CornerShot();
							drive_cmd = 0.0; rotate_cmd = 0.0;
							loopCount = 0;
							autoStage++;
						}
					}
                    break;

				case DEFENSE_ROUGH_TERRAIN: //Rough_Terrain
					switch(position)
					{
						case NO_SHOOT:
							break;

						case POSITION_TWO:
							if(pixyCam->OKtoShoot(POS_TWO_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
		                    break;

						case POSITION_THREE:
						case POSITION_FOUR:
							if(pixyCam->OKtoShoot(CENTERED_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
		                    break;

						case POSITION_FIVE:
							if(pixyCam->OKtoShoot(POS_FIVE_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
		                    break;
					}
					break;

				case DEFENSE_PORTCULLIS:
					switch(position)
					{
						case NO_SHOOT:
							break;

						case POSITION_TWO:
							if(pixyCam->OKtoShoot(POS_TWO_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
							break;

						case POSITION_THREE:
						case POSITION_FOUR:
							if(pixyCam->OKtoShoot(CENTERED_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
							break;

						case POSITION_FIVE:
							if(pixyCam->OKtoShoot(POS_FIVE_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
							break;
					}
					break;

				case DEFENSE_CHEVAL_DE_FRISE: //Cheval_De_Frise
					switch(position)
					{
						case NO_SHOOT:
							break;

						case POSITION_TWO:
							if(pixyCam->OKtoShoot(POS_TWO_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
							break;

						case POSITION_THREE:
						case POSITION_FOUR:
							if(pixyCam->OKtoShoot(CENTERED_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
							break;

						case POSITION_FIVE:
							if(pixyCam->OKtoShoot(POS_FIVE_GOAL_VOLTAGE))
							{
								if(!boulderGrab->IsAutoLoadActive())
								{
									//catapult->LongShot();
									catapult->CornerShot();
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
							break;
					}
					break;
			}
			break;

		case 6:
			switch(defenseType)
			{
				case DEFENSE_ROUGH_TERRAIN: //Rough_Terrain
					switch(position)
					{
						case NO_SHOOT:
							break;

						case POSITION_TWO:
						case POSITION_THREE:
						case POSITION_FOUR:
						case POSITION_FIVE:
							if(!catapult->IsArmShooting())
							{
								if(driveDistance->GetAverageDistance() < AUTO_EXTRA_DRIVE)
								{
									drive_cmd = -1.0 * breachingSpeed[defenseType]; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
									loopCount++;
								}
								else
								{
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
							break;
					}
					break;

				case DEFENSE_PORTCULLIS:
					switch(position)
					{
						case NO_SHOOT:
							break;

						case POSITION_TWO:
						case POSITION_THREE:
						case POSITION_FOUR:
						case POSITION_FIVE:
							if(!catapult->IsArmShooting())
							{
								if(driveDistance->GetAverageDistance() < AUTO_EXTRA_DRIVE)
								{
									drive_cmd = -1.0 * breachingSpeed[defenseType]; rotate_cmd = 0.0;//NEGATIVE COMMAND MOVES FORWARD!!!
									loopCount++;
								}
								else
								{
									drive_cmd = 0.0; rotate_cmd = 0.0;
									loopCount = 0;
									autoStage++;
								}
							}
							break;
					}
					break;
			}
			break;
	}
	//printf("Bypass_Defense: Auto-Mode Ended!");
}

START_ROBOT_CLASS(TalonXVII)
