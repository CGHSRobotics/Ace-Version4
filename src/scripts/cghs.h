
#include "main.h"

#ifndef CGHS
#define CGHS


#define BUTTON_INTAKE_TOGGLE DIGITAL_L1
#define BUTTON_INTAKE_REVERSE DIGITAL_L2

#define BUTTON_LAUNCHER DIGITAL_R1
#define BUTTON_LAUNCHER_LONG DIGITAL_R2

#define BUTTON_ROLLER_FORWARD DIGITAL_A
#define BUTTON_ROLLER_REVERSE DIGITAL_B

#define BUTTON_ENDGAME DIGITAL_UP

#define BUTTON_A_BRAKE_TOGGLE DIGITAL_DOWN

#define BUTTON_AUTON_INCREASE DIGITAL_RIGHT
#define BUTTON_AUTON_DECREASE DIGITAL_LEFT


#define DRIVE_GEAR_RATIO MOTOR_GEARSET_18
#define LAUNCHER_GEAR_RATIO MOTOR_GEARSET_06
#define ROLLER_GEAR_RATIO MOTOR_GEARSET_18
#define CONVEYOR_GEAR_RATIO MOTOR_GEARSET_06
#define INTAKE_GEAR_RATIO MOTOR_GEARSET_18

// namespace cghs
namespace cghs {

	extern int autonIndex;
	extern int alliance;	// inits to 0; aka red alliance

	extern bool activeBreakEnabled;

	extern string operation_mode;

	/*
	 *  Motor Ports
	 */
	const int DRIVE_LEFT_FRONT_PORT = 11;
	const int DRIVE_LEFT_BACK_PORT = 12;
	const int DRIVE_RIGHT_FRONT_PORT = 1;
	const int DRIVE_RIGHT_BACK_PORT = 20;
	const int LAUNCHER_PORT = 17;
	const int ROLLER_PORT = 10;
	const int CONVEYOR_PORT = 15;
	const int INTAKE_PORT = 16;

	const int IMU_PORT = 18;
	const int VISION_PORT = 3;
	const int GPS_PORT = 2;

	/*
	 *  Speed Constants
	 */

	// Drive Constants
	const float SPEED_DRIVE_AUTO = 0.8 * 127.0;
	const float SPEED_DRIVE_AUTO_INTAKE = 0.2 * 127.0;
	const float SPEED_DRIVE_AUTO_ROLLER = 0.25 * 127.0;

	const float SPEED_TURN_AUTO = 0.6 * 127.0;

	// Intake Motor 
	const float SPEED_INTAKE = 100;
	const float SPEED_INTAKE_LAUNCHER = 100;

	// Conveyor Motor
	const float SPEED_CONVEYOR_INTAKE = 100;
	const float SPEED_CONVEYOR_LAUNCHER = 50;
	const float SPEED_CONVEYOR_LAUNCHER_LONG = 35;

	// Roller Motor
	const float SPEED_ROLLER = 100;
	const float SPEED_ROLLER_AUTO = 10;
	const float SPEED_ROLLER_AUTO_SKILLS = 55;
	const float SPEED_ROLLER_LAUNCHER = 100;

	const float ROLLER_TIME_AUTO = 200;
	const float ROLLER_TIME_AUTO_SKILLS = 300;

	// Launcher Motor
	const bool LAUNCHER_LOGGING = false;

	const float SPEED_LAUNCHER = 80;
	const float SPEED_LAUNCHER_DRIVER = 80;
	const float SPEED_LAUNCHER_LONG = 100;
	const float SPEED_LAUNCHER_SHORT = 60;
	const float LAUNCHER_MIN_SPEED = 20.0;

	extern float launcherTimerDelay;
	const float launcherTimerDelayMax = 500;

	extern u_int64_t launcherTime;
	extern bool launcherEnabled;

	/*
	 *  Device Declarations
	 */

	// Motors
	const pros::Motor launcherMotor(LAUNCHER_PORT, LAUNCHER_GEAR_RATIO, true);
	const pros::Motor rollerMotor(ROLLER_PORT, ROLLER_GEAR_RATIO, false);
	const pros::Motor conveyorMotor(CONVEYOR_PORT, CONVEYOR_GEAR_RATIO, false);
	const pros::Motor intakeMotor(INTAKE_PORT, INTAKE_GEAR_RATIO, false);

	// Endgame
	const pros::ADIDigitalOut endgamePneumatics('a', false);

	// GPS
	const float GPS_OffsetX = 100;
	const float GPS_OffsetY = 136;

	const pros::GPS gpsSensor(GPS_PORT, GPS_OffsetX, GPS_OffsetY);


	/*
	 *  Util Function Declarations
	 */

	 // Spin Motor PID by percent
	extern void spinMotor(pros::Motor motor, float percent);

	// Toggles Active Break
	extern void active_brake(bool enabled, Drive& chassis);

	// Records Launcher speed + time to file on sd card
	extern void recordLauncherStatistics();

	// Convert Celsius to Farenheit
	extern float cel_to_faren(float celsius);

	/*
	 *	GPS namespace
	 */

	namespace gps {

		extern pros::Task task_turn_gps;

		extern void init();

		extern float curr_turnSpeed;
		extern float curr_turnAngle;

		extern float curr_turnAngleDiff;

		// Set the angle to turn to a certain absolute degree
		extern void set_turn_gps(float angle, float speed);
		extern void __task_set_turn_gps();

		// Set Drive Pid with GPS
		extern void set_waypoint_gps(float distance, float speed, bool slew, bool headCorrection);
	}
	/*
	 *	 User Control
	 */

	// resets motors to voltage of 0
	extern void resetMotors();

	// toggles intake
	extern void intakeToggle(bool enabled);

	// reverses Intake
	extern void intakeReverse(bool enabled);

	extern void launchDisks(bool enabled, float speed, bool isLongDist = false, bool standby = false);

	extern void rollerForward(bool enabled, float speed);
	extern void rollerReverse(bool enabled, float speed);

	extern void endgameToggle(bool enabled);


	/**
	 *	Autonomous Namespace
	 */

	namespace auton {

		// Autonomous Selector

		extern int autonIndex;
		const int numAutons = 5;
		const std::string autonArray[10] = { "Skills", "Shebang", "Three", "Two", "Null" };


		extern void updateAutonSelection();
		extern void checkAutonButtons();

		// Autonomous
		extern void launchDisks_Auto(float time, float speed, bool isLongDist = false);

		extern void skills_Auto();
		extern void null_Auto();
		extern void threeSide_Auto();
		extern void twoSide_Auto();
		extern void theWholeShebang_Auto();

	}  // namespace auton

}  // namespace cghs

#endif
