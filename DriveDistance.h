/*

 * DriveDistance.h
 *
 *  Created on: Jan 31, 2016
 *      Author: jacob_000
 */
#ifndef DRIVEDISTANCE_H_
#define DRIVEDISTANCE_H_

#include "WPILib.h"
#include "Common.h"
/*
#define WHEEL_CIRCUMFERENCE			(25.132)//8 inch diameter
#define DRIVE_PULSE_COUNT	        (360.0)
#define DISTANCE_PER_PULSE          (WHEEL_CIRCUMFERENCE/DRIVE_PULSE_COUNT)
*/
#define DISTANCE_PER_PULSE			(0.05974) //calculated 23 inches with (404 + 367)/2 pulses

class DriveDistance
{
public:

	DriveDistance(void);
	void Service(void);
	double GetLeftDistance(void);
	double GetRightDistance(void);
	double GetAverageDistance(void);
	void RestartEncoder(void);
	void UpdateDash(void);


	Encoder *encoderLeft;
	Encoder *encoderRight;


	double distancePerPulse;
};




#endif /* SRC_DRIVEDISTANCE_H_ */
