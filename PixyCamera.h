/*
 * PixyCamera.h
 *
 *  Created on: Mar 30, 2016
 *      Author: Gaelhawks
 */
#ifndef SRC_PIXYCAMERA_H_
#define SRC_PIXYCAMERA_H_

#define CENTERED_GOAL_VOLTAGE	(1.79)
#define POS_TWO_GOAL_VOLTAGE	(1.79)
#define POS_FIVE_GOAL_VOLTAGE	(1.65)
#define VOLTAGE_TOLERANCE		(0.2)

class PixyCamera
{
public:
	AnalogInput *goalLocator;
	DigitalInput *goalDetector;

	int lateralTrimValue;

	PixyCamera(void);
	void LocalReset(void);
	void StopAll(void);
	bool GoalDetected(void);
	bool OKtoShoot(float desiredGoal);
	float GoalLocation(void);
	void SetLateralTrim(int trimValueIn);
	void UpdateDash(void);
	void Service(void);
};



#endif /* SRC_PIXYCAMERA_H_ */
