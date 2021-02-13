#ifndef DEFENSEMANIPULATOR_H_
#define DEFENSEMANIPULATOR_H_

#define ENCODER_PULSES              (360.0) // Update with the amount of pulses on the encoder.
#define AT_ANGLE_DEADBAND           (0.3)   // The allowed amount of angle offset for the arm. (In degrees)
#define ARM_UP_SPEED                (0.2)   // Speed up when positioning arm.
#define ARM_DOWN_SPEED              (-0.2)  // Speed down when positioning arm.
#define TARGET_STALL				((int)(N1SEC * 0.5))
#define STALL_CURRENT				(6.0)
#define MOTOR_CURRENT_IN			(5)

/*
 * DefenseManipulator.h
 *
 *  Created on: Jan 19, 2016
 *      Author: michaelk230
 */
class DefenseManipulator
{
	private:
		// Create objects needed by this class
	    VictorSP *rhinoHorn;
	    PowerDistributionPanel *pdp;

		// Declare local variables that are used in the class to store values.
	    double motorCurrent;
	    float motorCmd;
	    int stallCurrentCounter;

   	public:
		DefenseManipulator(void);
		void LocalReset(void);
		void StopAll(void);
		void SetHornIn(void);
		void SetHornOut(void);
		//void SetHorn(float motorIn);
		void ArmService(void);
		bool IsArmInMotion(void);
		void GetCurrent(void);
		void UpdateDash(void);
		void Service(void);
		void Duck(void);
		void StartConfig(void);
};



#endif /* DEFENSEMANIPULATOR_H_ */
