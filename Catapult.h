/*
 * Catapult.h
 *
 *  Created on: Jan 31, 2016
 *      Author: kcs26_000
 */

#ifndef SRC_CATAPULT_H_
#define SRC_CATAPULT_H_

#define IDLE_STAGE 		(0)
#define SHOOTING_STAGE  (1)
#define ROLL_STAGE		(2)
#define WAIT_STAGE		(3)
#define RETURN_STAGE 	(4)

#define STOPPED_STATE		(0)
#define SHOOTING_STATE		(1)
#define ROLLING_STATE		(2)
#define DUCK_STATE			(3)
#define LONG_SHOT_STATE		(4)
#define BATTER_SHOT_STATE	(5)
#define CORNER_SHOT_STATE	(6)

#define ROLLING_SPEED			(0.1f)
#define SHOOTING_SPEED			(-1.0f)
#define RETURN_SPEED			(0.1f)
#define ZERO_SPEED				(0.0f)
#define WAIT_SPEED				(0.025f)

#define CATAPULT_ENCODER_PULSES		(360.0)
#define TEETH_PER_MOTOR_SHAFT		(48.0)
#define TEETH_PER_ARM_GEAR			(22.0)

#define ROLL_ARM_ANGLE		(30.0)
#define	MAX_ARM_ANGLE		(90.0)
#define MIN_ARM_ANGLE		(0.0)

#define WAIT_FOR_ROLL	((int)(N1SEC * 2.0))
#define RETURN_RATE		(60.0)
#define CATAPULT_KP		(4.0)
#define CATAPULT_KF		(10.0)
#define CATAPULT_KR		(-1048.5)
#define KP_HOLD			(0.01)
#define KV_HOLD			(0.001)
#define CATAPULT_TAU_R	(0.1)

//SHOOTING CONSTANTS
#define COAST_COUNT		(40)
#define RETURN_COUNT	(125)  // May want to make this longer?  150ish?

//LONG SHOT CONSTANTS
#define LONG_COAST_COUNT		(40)
#define LONG_RETURN_COUNT		(125)

//BATTER SHOT CONSTANTS
#define BATTER_COAST_COUNT		(40)
#define BATTER_RETURN_COUNT		(125)

//CORNER SHOOTING CONSTANTS
#define CORNER_COAST_COUNT		(40)
#define CORNER_RETURN_COUNT		(125)  // May want to make this longer?  150ish?


//ROLLING/SOFT SHOT CONSTANTS
#define ROLL_SHOOTING_COUNT		(25)//20)
#define ROLL_COAST_COUNT		(60)
#define ROLL_RETURN_COUNT		(150)
#define ROLL_FIRE_VEL			(180.0) //200.0)

#define DUCK_RETURN_COUNT		(100)

#define RETURN_POS				(-5.0)
#define WAIT_FOR_AT_BOTTOM		((int)(N1SEC * 0.5))

class Catapult
{
private:
	VictorSP *swingArm;
	Encoder *armEncoder;

	int loopCount;
	int shootingStage;
	int rollingStage;
	int catapultServiceState;

	double encoderAngle;
	bool firstTime;

	//float launchProfile[8] = {-0.2, -0.35, -0.6, -0.75, -0.9, -1.0, -1.0, -1.0};
	//#define SHOOTING_COUNT	(8)
	//float launchProfile[8] = {-0.2, -0.35, -0.6, -0.65, -0.7, -0.85, -1.0, -1.0};
	//#define SHOOTING_COUNT	(8)
	//float launchProfile[11] = {-0.2, -0.2, -0.3, -0.35, -0.55, -0.70, -0.85, -0.9, -0.95, -1.0, -1.0};
#define SHOOTING_COUNT	(11)
	float launchProfile[SHOOTING_COUNT] 		   = {-0.2, -0.2, -0.3, -0.35, -0.55, -0.70, -0.75, -0.85, -0.9, -0.95, -1.0};
#define LONG_SHOOTING_COUNT	(12)
	float longLaunchProfile[LONG_SHOOTING_COUNT]   = {-0.2, -0.2, -0.2, -0.3, -0.35, -0.55, -0.70, -0.75, -0.85, -0.9, -0.95, -1.0};
#define BATTER_SHOOTING_COUNT	(12)
	float batterLaunchProfile[BATTER_SHOOTING_COUNT] = {-0.4, -0.7, -1.0, -1.0, -0.6, -0.4, -0.1, -0.0, -0.0, -0.0, -0.0, -0.0};
#define CORNER_SHOOTING_COUNT	(11)
	float cornerlaunchProfile[CORNER_SHOOTING_COUNT] = {-0.2, -0.3, -0.35, -0.55, -0.70, -0.75, -0.85, -0.9, -0.95, -1.0, -0.5};



	float lastPos;
	float errorInt;
	float velCmd;
	float posCmd;
	float motorCmd;
	float currentSpeed;
	float currentPos;
	float posError;
	float error;
	int atBottomCount;

public:
	Catapult(void);
	bool IsArmShooting(void);
	bool IsArmRolling(void);
	bool ReadyForBoulder(void);
	double GetArmAngle(void);
	double Limit(double min, double max, double curVal);
	void Shoot(void);
	void Roll(void);
	void LongShot(void);
	void BatterShot(void);
	void CornerShot(void);
	void GetPositionAndVelocity(void);
	void PositionControl(float posCmd);
	void VelocityControl(float velCmd);
	void LocalReset(void);
	void StopAll(void);
	void SetArmSpeed(void);
	void UpdateDash(void);
	void Service(void);
	void ShootingServiceRoutine(void);
	void RollingServiceRoutine(void);
	void LongShotServiceRoutine(void);
	void BatterShotServiceRoutine(void);
	void CornerShotServiceRoutine(void);
	void DuckService(void);
	void Duck(void);
	void StartConfig(void);
	void ArmStartAtBottom(void);
};


#endif /* SRC_CATAPULT_H_ */
