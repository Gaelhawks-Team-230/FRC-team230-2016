/*
 * TalonXVII_main.h
 *
 *  Created on: Jan 16, 2016
 *      Author: Gaelhawks
 */

#ifndef TALONXVII_MAIN_H_
#define TALONXVII_MAIN_H_

#include "Common.h"
#include "Catapult.h"
#include "DriveTrain.h"
#include "DriveDistance.h"
#include "BoulderGrab.h"
#include "DefenseManipulator.h"
#include "PixyCamera.h"

#define DEFENSE_LOW_BAR 			(0)
#define DEFENSE_PORTCULLIS 			(1)
#define DEFENSE_CHEVAL_DE_FRISE 	(2)
#define DEFENSE_RAMPARTS 			(3)
#define DEFENSE_MOAT 				(4)
#define DEFENSE_PORTCULLIS_3		(5)
#define DEFENSE_PORTCULLIS_4		(6)
#define DEFENSE_ROCK_WALL 			(7)
#define DEFENSE_ROUGH_TERRAIN 		(8)
#define DEFENSE_REACH				(-2)
#define DEFENSE_DISABLED			(-1)

#define NO_SHOOT			(0)
#define POSITION_TWO		(2)
#define POSITION_THREE		(3)
#define POSITION_FOUR		(4)
#define POSITION_FIVE		(5)

//#define MAX_GATHERER_SHOOTING_ANGLE		(40.0)
#define AUTO_SHOOTING_TIME				((int)(0.1 * (N1SEC)))
#define AUTO_TURN_90_DEGREES			((int)(90.0/90.0 * (N1SEC)))
#define AUTO_TURN_30_DEGREES			((int)(30.0/30.0 * (N1SEC)))//Distance to rotate/Rotation Rate
#define AUTO_DRIVE_UNDER_LOWBAR			(100.0)//FROM COURTYARD!!!!!!
#define AUTO_DRIVE_FORWARD_SMALL		(15.0)
#define AUTO_SPYBOT_WAIT				((int)(0.1 * (N1SEC)))
#define AUTO_WAIT_TIME					((int)(1.0 * (N1SEC)))
#define AUTO_WAIT_TARGETING				((int)(1.5 * (N1SEC)))

#define AUTO_DRIVE_DISTANCE_BYPASS_DEFENSE (12) // Update With Distance From The Start To Defense
#define AUTO_DRIVE_CMD_BYPASS_DEFENSE      (-0.5)
#define K_AUTO_DRIVE                       (0.01) //Update Once Calculated
#define K_AUTO_TRACK					   (-60.0)
#define AUTO_EXTRA_DRIVE		  		   (18.0)
#define AUTO_DEFENSE_REACH_DISTANCE		   (36.0)
#define AUTO_PORTCULLIS_DISTANCE		   (58.0)
#define AUTO_CHEVAL_DUCK_DISTANCE   	   (-40.0)
#define AUTO_CHEVAL_INFRAME_DISTANCE	   (-52.0)
#define AUTO_ROTATE_COUNT				   (50)

class TalonXVII: public SampleRobot
{
public:
	Joystick *driveStick;
	Joystick *gamepad;

	DriveTrain *drive;
	BoulderGrab *boulderGrab;
	Catapult *catapult;
	DriveDistance *driveDistance;
	DefenseManipulator *rhinoHorn;
	PixyCamera *pixyCam;
	Servo *cameraServo;
	SendableChooser *defenseChooser;
	//SendableChooser *lateralTrimChooser;
	SendableChooser *positionChooser;
	SendableChooser *autoShootingChooser;
	Relay *flashlight;


	const int DEFENSE_lowbar 			= DEFENSE_LOW_BAR;
	const int DEFENSE_portcullis 		= DEFENSE_PORTCULLIS;
	const int DEFENSE_chevaldefrise 	= DEFENSE_CHEVAL_DE_FRISE;
	const int DEFENSE_ramparts 			= DEFENSE_RAMPARTS;
	const int DEFENSE_moat 				= DEFENSE_MOAT;
	const int DEFENSE_Portcullis_3		= DEFENSE_PORTCULLIS_3;
	const int DEFENSE_Portcullis_4 		= DEFENSE_PORTCULLIS_4;
	const int DEFENSE_rockwall 			= DEFENSE_ROCK_WALL;
	const int DEFENSE_roughterrain	 	= DEFENSE_ROUGH_TERRAIN;
	const int DEFENSE_disabled	 		= DEFENSE_DISABLED;
	const int DEFENSE_reach	 			= DEFENSE_REACH;

	const int NO_Shoot			= 0;
	const int POSITION_two		= 2;
	const int POSITION_three	= 3;
	const int POSITION_four		= 4;
	const int POSITION_five		= 5;

	const int NEGATIVE_FOUR_LATERAL		= -4;
	const int NEGATIVE_TWO_LATERAL		= -2;
	const int ZERO_LATERAL				= 0;
	const int POSITIVE_TWO_LATERAL		= 2;
	const int POSITIVE_FOUR_LATERAL		= 4;


	float breachingSpeed[9] = { 0.6, 	//Low Bar Speed
								0.6, 	//Portcullis Speed
								0.6,	//Cheval de Frise Speed
								0.9, 	//Ramparts Speed
								0.8, 	//Moat Speed
								0.6, 	//Portcullis_3 Speed
								0.6, 	//Portcullis_4 Speed
								1.0, 	//Rock Wall Speed
								0.8};	//Rough Terrain Speed

	float autoBreachDistance[9] = { 180.0,	//Low Bar Distance
									120.0,	//Portcullis Distance
									84.0,	//Cheval de Frise Distance
									84.0,	//Ramparts Distance
									84.0,	//Moat Distance
									120.0,	//Portcullis_3 Distance
									120.0,	//Portcullis_4 Distance
									84.0,	//Rock Wall Distance
									84.0};	//Rough Terrain Distance
	int autoMode;
	int defenseSelected;
	int positionSelected;
	int lateralTrim;
	int autoStage;
	int dashCounter;
	int loopCount;

	double loopTime;
	double startTime;
	bool autoLoadedFlag;

	double curTime;
	double waitTime;
	bool loopOverTime;
	bool autoTracking;
	int NumLoopsOverTime;
	float drive_cmd;
	float rotate_cmd;
	float breachDist;

	bool firstTime;

	TalonXVII();
	void RobotInit(void);
	void Autonomous(void);
	void OperatorControl(void);
	void Disabled(void);
	void Test(void);
	double Limit(double min, double max, double curValue);
	void RobotDuck(void);
	void RobotStartConfig(void);
	void Omnicide(void);
	void CommunityService(void);
	void ServiceDash(void);
	void AutoModeSpyBot(void);
	void AutoModeBypassDefense(int defenseType, int position);
};

#endif /* TALONXVII_MAIN_H_ */
