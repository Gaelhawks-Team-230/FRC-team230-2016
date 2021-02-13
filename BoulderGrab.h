#ifndef BOULDERGRAB_H_
#define BOULDERGRAB_H_

#define GATHERER_UP_POSITION_SPEED    (0.2)       // Only positive values.
#define GATHERER_DOWN_POSITION_SPEED  (-0.2)      // Only negative values.
#define DEGREES_PER_VOLT              (152.0/4.0) // Goes from half a volt up to four volts. This is over a 300 degree rotation span.
#define AT_GOAL_DEADBAND              (0.3)       // The allowed amount of offset for the positioner mechanism. (In degrees)
#define AT_TARGET_ANGLE_TOLERANCE     (0.5)


// Values used for the service mode.
#define IDLE                          (0)
#define GATHERING                     (1)
#define EJECTING                      (2)
#define LOADING                       (3)
#define PRE_LOAD					  (4)

// Values used in calculation for position and velocity control.
#define MAX_VELOCITY                  (3.0)       // Degrees per second (Update -- Current value is only being used for testing).
#define POSITIONER_KP                 (4.0)       // Update value once calculated.
#define POSITIONER_KF                 (20.0)       // Update value once calculated.
#define POSITIONER_KR                 (-200.0)       // Update value once calculated.
#define POSITIONER_TAU_R              (0.1)       // Update value once calculated.

#define CALIBRATION_ANGLE             (120.0)
#define CARRY_ANGLE                   (22.0) //30.0)
#define AUTO_LOAD_ANGLE               (75.0)
#define MAX_SHOOTING_ANGLE			  (25.0)
#define SHOOTING_STOW_ANGLE           (20.0)
#define LOW_BAR_ANGLE                 (10.0)
#ifdef PRACTICE_BOT
#define PICKUP_ANGLE                  (-5.0)
#else
#define PICKUP_ANGLE                  (0.0)
#endif

#define MAX_LOAD_TIME           ((int)(N1SEC * 0.75))
#define MAX_TIMEOUT_COUNT             ((int)(N1SEC * 2.0))   //((int)(N1SEC * 5.0))      // Maximum number of loops before the positioner is supposed to be at target angle.
#define LOAD_BOULDER_SPEED            (0.3)       // The speed at which the boulder is auto-loaded into catapult.

/*
 * BoulderGrab.h
 *
 *  Created on: Jan 22, 2016
 *      Author: Gaelhawks
 */
class BoulderGrab
{
public:
	VictorSP *grabPositioner;
    float motorCmd;

   private:
      // Class Objects
      VictorSP *gatherer;
      AnalogInput *positionerAngleCalculator;
      DigitalInput *boulderDetector;

      // Local variables
      bool isGathererEnabled;
      bool isPositionerInMotion;
      bool autoLoadActive;
      float targetGathererAngle;
      float gathererSpeed;
      float positionerOffsetDegrees;
      float curPos;
      float lastPos;
      float curVelocity;
      float posError;
      float velocityCmd;
      float velocityErrorInt;
      float velocityError;
      float posCmd;
      float deltaPos;
      int gathererMode;
      int autoLoadStage;
      int timeoutCount;
      int loopCount;
      float calibrationCmd;
      float deltaVoltage;


   public:
      BoulderGrab(void);
      void LocalReset(void);
      void StopAll(void);
      void GrabBoulder(float grabSpeed);
      void EjectBoulder(float ejectSpeed);
      void LoadBoulder(float loadSpeed);
      void PreLoadBoulder(float grabSpeed);
      void StopGatherer(void);
      float GetSpeed(void);
      bool HasBoulder(void);
      bool IsAtTargetPosition(void);
      bool IsPositionerInMotion();
      bool OkayToShoot(void);
      float GetGathererAngle(void);
      void PositionGatherer(float targetGathererAngle);
      bool IsAutoLoadActive(void);
      void CalibrateGrabAnalog(void);
      double Limit(double min, double max, double curValue);
      void CalculatePositionerVelocity(void);
      void PositionerPosControl(void);
      void CalculatePositionerPosRamp(void);
      void GathererService(void);
      void AutoLoadBoulder(void);
      void AutoLoadService(void);
      void Duck(void);
      void StartConfig(void);
      void Service();
      void UpdateDash(void);

};



#endif /* BOULDERGRAB_H */
