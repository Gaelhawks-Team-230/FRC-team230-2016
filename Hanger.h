/*
 * Hanger.h
 *
 *  Created on: Feb 16, 2016
 *      Author: Gaelhawks
 */

#ifndef SRC_HANGER_H_
#define SRC_HANGER_H_

#define LIFT_SPEED			(0.75)
#define SLOW_LIFT_SPEED		(0.55)

#define ROTATING_TO_POSTION	(0)
#define LIFTING_ROBOT		(1)

class Hanger
{
	private:
		VictorSP *rightHangerArm;
		VictorSP *leftHangerArm;
		Encoder *rightArmEncoder;
		Encoder *leftArmEncoder;

		int hangerState;

   	public:
		Hanger(void);
		void LocalReset(void);
		void StopAll(void);
		void RotateToPosition(void);
		void LiftRobot(void);
		void RotateService(void);
		void LiftService(void);
		void UpdateDash(void);
		void Service(void);
		void Duck(void);
		void StartConfig(void);
};



#endif /* SRC_HANGER_H_ */
