
#include "main.h"

#ifndef ACE
#define ACE


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
#define ROLLER_GEAR_RATIO MOTOR_GEARSET_06
#define CONVEYOR_GEAR_RATIO MOTOR_GEARSET_06
#define INTAKE_GEAR_RATIO MOTOR_GEARSET_18

// namespace cghs
namespace ace {

	extern int autonIndex;
	extern int alliance;	// inits to 0; aka red alliance

	extern bool activeBreakEnabled;

	const char launcherFile_path[18] = "/usd/launcher.txt";

	extern string operation_mode;

	/*
	 *  Motor Ports
	 */
	const int DRIVE_LEFT_FRONT_PORT = 11;
	const int DRIVE_LEFT_BACK_PORT = 12;
	const int DRIVE_RIGHT_FRONT_PORT = 1;
	const int DRIVE_RIGHT_BACK_PORT = 20;
	const int LAUNCHER_PORT = 17;
	const int ROLLER_PORT = 4;
	const int CONVEYOR_PORT = 15;
	const int INTAKE_PORT = 16;

	const int IMU_PORT = 18;
	const int VISION_PORT = 10;
	const int GPS_PORT = 2;

	/*
	 *  Speed Constants
	 */

	 // Drive Constants
	const float SPEED_DRIVE_AUTO = 0.87 * 127.0;
	const float SPEED_DRIVE_AUTO_INTAKE = 0.2 * 127.0;
	const float SPEED_DRIVE_AUTO_ROLLER = 0.25 * 127.0;

	const float SPEED_TURN_AUTO = 0.7 * 127.0;

	// Intake Motor 
	const float SPEED_INTAKE = 100;
	const float SPEED_INTAKE_LAUNCHER = 100;

	// Conveyor Motor
	const float SPEED_CONVEYOR_INTAKE = 100;
	const float SPEED_CONVEYOR_LAUNCHER = 100;
	const float SPEED_CONVEYOR_LAUNCHER_LONG = 35;

	// Roller Motor
	const float SPEED_ROLLER = 100;
	const float SPEED_ROLLER_AUTO = 10;
	const float SPEED_ROLLER_AUTO_SKILLS = 55;
	const float SPEED_ROLLER_LAUNCHER = 100;

	const float ROLLER_TIME_AUTO = 200;
	const float ROLLER_TIME_AUTO_SKILLS = 300;

	// Launcher Motor
	const bool LAUNCHER_LOGGING = true;

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
	const pros::Motor conveyorMotor(CONVEYOR_PORT, CONVEYOR_GEAR_RATIO, false);
	const pros::Motor intakeMotor(INTAKE_PORT, INTAKE_GEAR_RATIO, false);

	// Endgame
	const pros::ADIDigitalOut endgamePneumatics('a', false);

	// GPS
	const float GPS_OffsetX = -100 / 1000.0;
	const float GPS_OffsetY = 136.0 / 1000.0;

	const pros::GPS gpsSensor(GPS_PORT, GPS_OffsetX, GPS_OffsetY);

	/*
	 *  Util Function Declarations
	 */

	 // Spin Motor PID by percent
	extern void spinMotor(pros::Motor motor, float percent);

	// Toggles Active Break
	extern void active_brake(bool enabled, Drive& chassis);


	/*
	 *	Launcher Data
	 */

	 // Launcher data class
	struct l_data_point
	{
		float msec;
		float rpm;
		float set_rpm;
	};

	std::vector<l_data_point> l_data_array;

	// Records Launcher speed + time to file on sd card
	extern void recordLauncherStatistics();

	// Saves launcher data to file
	extern void saveLauncherData();

	// Saves launcher data to file
	extern void uploadLauncherData();


	/*
	 *	Unit Conversion Functions
	 */

	 // Convert Celsius to Farenheit
	extern float cel_to_faren(float celsius);

	// Convert inch to mm
	extern float to_mm(float inch);

	// Convert inch to mm
	extern float to_inch(float mm);

	// Convert Radians to Degrees
	extern float to_deg(float rad);

	// Convert Degrees to Radians
	extern float to_rad(float deg);


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

	/*
	 *	GPS namespace
	 */

	namespace gps {

		extern pros::Task task_turn_gps;

		const float err_degree_max = 2;
		const float err_pos_max = 4;
		const float err_gps_max = 0.25;

		extern void init();

		extern float curr_turnAngle;
		extern float curr_turnSpeed;

		extern float imu_start_angle;

		// fact check imu with gps if gps is in acceptable error rate
		extern void __task_gps_factcheck_angle();

		/**
		 * @brief
		 * 		Turns robot to absolute degrees, tracks angle and speed data as well as compensates for gps
		 * @param angle
		 *		absolute angle to turn robot to
		 * @param speed
		 *		speed (0-127) at which robot should turn
		 * @param waitUntilFinished
		 *		Boolean, if true runs chassis.waitDrive();
		 */
		extern void set_turn(float angle, float speed, bool waitUntilFinished = true);

		/**
		 *	Tell Robot to go to absolute position if not there already
		 *	@param x
		 * 		Absolute X Position in inches with 0 being center of field
		 *	@param y
		 * 		Absolute Y Position in inches with 0 being center of field
		 */
		extern void set_waypoint(float x, float y);
	}

}  // namespace cghs

#endif