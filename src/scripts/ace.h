
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
#define BUTTTON_STANDBY DIGITAL_LEFT
#define BUTTON_VAR_LAUNCHER DIGITAL_DOWN

#define BUTTON_AUTON_INCREASE DIGITAL_RIGHT
#define BUTTON_AUTON_DECREASE DIGITAL_X

#define DRIVE_GEAR_RATIO MOTOR_GEARSET_18
#define LAUNCHER_GEAR_RATIO MOTOR_GEARSET_06
#define VAR_LAUNCHER_GEAR_RATIO MOTOR_GEARSET_36
#define CONVEYOR_GEAR_RATIO MOTOR_GEARSET_06
#define INTAKE_GEAR_RATIO MOTOR_GEARSET_18

// namespace cghs
namespace ace {


	/* ========================================================================== */
	/*                                Global Stuff                                */
	/* ========================================================================== */


	extern int alliance;	// inits to 0; aka red alliance

	extern bool activeBreakEnabled;

	const char launcherFile_path[18] = "/usd/launcher.txt";

	extern string operation_mode;

	const double rad2 = std::sqrt(2);


	/* ========================================================================== */
	/*                             Port Configurations                            */
	/* ========================================================================== */

	const int DRIVE_LEFT_FRONT_PORT = 11;
	const int DRIVE_LEFT_BACK_PORT = 12;
	const int DRIVE_RIGHT_FRONT_PORT = 4;
	const int DRIVE_RIGHT_BACK_PORT = 20;
	const int LAUNCHER_PORT = 19;
	const int VAR_LAUNCHER_PORT = 1;
	const int CONVEYOR_PORT = 14;
	const int INTAKE_PORT = 18;

	const int IMU_PORT = 18;
	const int VISION_PORT = 10;
	const int GPS_PORT = 2;


	/* ========================================================================== */
	/*                               Speed Constants                              */
	/* ========================================================================== */

	/* --------------------------- Drive Constants --------------------------- */
	const float SPEED_DRIVE_AUTO = 0.87 * 127.0;
	const float SPEED_DRIVE_AUTO_INTAKE = 0.4 * 127.0;
	const float SPEED_DRIVE_AUTO_ROLLER = 0.25 * 127.0;

	const float SPEED_TURN_AUTO = 0.7 * 127.0;

	/* ------------------------------ Intake Motor ------------------------------ */
	const float SPEED_INTAKE = 100;
	const float SPEED_INTAKE_LAUNCHER = 100;

	/* ----------------------------- Conveyor Motor ----------------------------- */
	const float SPEED_CONVEYOR_INTAKE = 100;
	const float SPEED_CONVEYOR_LAUNCHER = 100;
	const float SPEED_CONVEYOR_LAUNCHER_LONG = 50;

	/* ------------------------------ Roller Motor ------------------------------ */
	const float SPEED_ROLLER = 100;
	const float SPEED_ROLLER_AUTO = 10;
	const float SPEED_ROLLER_AUTO_SKILLS = 75;

	const float ROLLER_TIME_AUTO = 200;
	const float ROLLER_TIME_AUTO_SKILLS = 300;

	/* ----------------------------- Launcher Motor ----------------------------- */
	const float SPEED_LAUNCHER_FLAP = 75;
	const float SPEED_LAUNCHER_SHORT = 80;
	const float SPEED_LAUNCHER_LONG = 100;
	const float SPEED_LAUNCHER_STANDBY = 75;
	const float LAUNCHER_MIN_SPEED = 15.0;

	extern bool LAUNCHER_STANDBY_ENABLED;

	/* ---------------------------- Variable Launcher --------------------------- */
	const float VAR_LAUNCH_ANGLE_DOWN = 0.15 * 225.0;
	const float VAR_LAUNCH_ANGLE_UP = 0.82 * 225.0;

	extern bool var_launcher_enabled;



	/* ========================================================================== */
	/*                             Device Declarations                            */
	/* ========================================================================== */


	/* --------------------------------- Motors --------------------------------- */

	const pros::Motor launcherMotor(LAUNCHER_PORT, LAUNCHER_GEAR_RATIO, true);
	const pros::Motor varLauncherMotor(VAR_LAUNCHER_PORT, VAR_LAUNCHER_GEAR_RATIO, false);
	const pros::Motor conveyorMotor(CONVEYOR_PORT, CONVEYOR_GEAR_RATIO, false);
	const pros::Motor intakeMotor(INTAKE_PORT, INTAKE_GEAR_RATIO, false);

	/* --------------------------------- Endgame -------------------------------- */

	const pros::ADIDigitalOut endgamePneumatics('a', false);

	/* ----------------------------------- GPS ---------------------------------- */

	const float GPS_OffsetX = -100 / 1000.0;
	const float GPS_OffsetY = 136.0 / 1000.0;

	const pros::GPS gpsSensor(GPS_PORT, GPS_OffsetX, GPS_OffsetY);

	/* ------------------------------ Vision Sensor ----------------------------- */

	const pros::Vision visionSensor(VISION_PORT);

	/* ------------------------------ Potentiometer ----------------------------- */

	const pros::ADIPotentiometer potentiometer_varL('b');


	/* ========================================================================== */
	/*                              Utility Functions                             */
	/* ========================================================================== */


	/**
	 * 	@brief
	 * 		Spins Motor PID by percent
	 *
	 * 	@param motor	motor to be spun
	 * 	@param percent	from -100 to 100 (negative is backwards)
	 */
	extern void spinMotor(pros::Motor motor, float percent);

	/**
	 *    @brief Toggles active break
	 *
	 *     @param enabled whether enabled or not
	 */
	extern void active_brake(bool enabled);

	/**
	 *    @brief Resets all motors to 0 voltage. Also disables pneumatics
	 */
	extern void resetMotors();

	const int diskCode = 0;
	extern int launchCount;
	extern int areaCutoff;
	extern bool diskSeen;

	extern void diskCheck();

	/* ----------------------------- Vector 2 Class ----------------------------- */
	class vec2
	{
	public:

		float x;
		float y;

		vec2(float x, float y);

		float mag() { return x * x + y * y; };
		void set(float x, float y) {
			x = x;
			y = y;
		}
		float dir() { return atan2(y, x); }
	};

	vec2::vec2(float x, float y)
	{
		set(x, y);
	}



	/* ========================================================================== */
	/*                            Conversion Functions                            */
	/* ========================================================================== */

	/**
	 *     @brief Convert Celsius to Farenheit
	 *
	 *     @param celsius celsius to be converted to farenheit
	 *     @return float
	 */
	extern float cel_to_faren(float celsius);

	/**
	 *     @brief Convert inch to mm
	 *
	 *
	 * 	   @param inch inch to be converted
	 * 	   @return float
	 */
	extern float to_mm(float inch);

	/**
	 *     @brief Convert mm to inch
	 *
	 *
	 * 	   @param mm
	 *     @return
	 */
	extern float to_inch(float mm);

	// Convert Radians to Degrees
	extern float to_deg(float rad);

	// Convert Degrees to Radians
	extern float to_rad(float deg);


	/* ========================================================================== */
	/*                                User Control                                */
	/* ========================================================================== */

	// toggles intake
	extern void intakeToggle(bool enabled);

	// reverses Intake
	extern void intakeReverse(bool enabled);

	// Rollers
	extern void rollerForward(bool enabled, float speed);
	extern void rollerReverse(bool enabled, float speed);

	// Endgame Toggle
	extern void endgameToggle(bool enabled);

	//Variable Launcher
	extern void varLauncherMove();

	extern void set_var_launcher(bool enabled);
}


/* ========================================================================== */
/*                            Autonomous Namespace                            */
/* ========================================================================== */
namespace ace::auton {

	/* ========================================================================== */
	/*                              Global Variables                              */
	/* ========================================================================== */

	extern float target_angle;
	extern vec2 target_pos;

	extern float curr_turnSpeed;
	extern float curr_driveSpeed;

	extern float imu_start_angle;

	extern int autonIndex;
	const int numAutons = 7;
	const std::string autonArray[10] = {
		"Skls",
		"Shbng",
		"Nll",
		"3-B",
		"2-B",
		"3-R",
		"2-R"
	};


	/* ========================================================================== */
	/*                           Miscellaneous Functions                          */
	/* ========================================================================== */

	/**
	 * 	Turns robot to absolute degrees, tracks angle and speed data
	 * @param angle
	 *		absolute angle to turn robot to
	 * @param speed
	 *		speed (0-127) at which robot should turn
	 * @param waitUntilFinished
	 *		Boolean, if true runs chassis.waitDrive();
	 */
	extern void set_turn(float angle, float speed, bool waitUntilFinished = true);

	extern void set_drive(float distance, float speed, bool waitUntilFinished = true);


	/**
	 *     @brief Update Auton Selection
	 */
	extern void updateAutonSelection();

	/**
	 *     @brief Check Buttons for change in Auton
	 */
	extern void checkAutonButtons();


	/* ========================================================================== */
	/*                             Autonomous Routines                            */
	/* ========================================================================== */

	extern void skills_Auto();
	extern void null_Auto();
	extern void threeSide_Auto();
	extern void twoSide_Auto();
	extern void theWholeShebang_Auto();
}


/* ========================================================================== */
/*                             Launcher Namespace                             */
/* ========================================================================== */
namespace ace::launch {


	/* ========================================================================== */
	/*                            Variable Declarations                           */
	/* ========================================================================== */

	extern float launcherTimerDelay;
	const float launcherTimerDelayMax = 500;

	extern u_int64_t launcherTime;

	extern bool launcherEnabled;
	const bool LAUNCHER_LOGGING = true;

	// Launcher Data Struct
	struct l_data_point
	{
		float msec;
		float rpm;
		float set_volt;
		bool vis_disk;
	};


	/* ========================================================================== */
	/*                            Function Declarations                           */
	/* ========================================================================== */

	// array of launcher data points
	std::vector<l_data_point> l_data_array;

	// launch disks function
	extern void launchDisks(float speed, bool isLongDist = false);

	// launch disks auton
	extern void launchDisks_Auto(float time, float speed, bool isLongDist = false);

	// Set standby to true or false
	extern void set_standby(bool enabled);

	// Records Launcher speed + time to file on sd card
	extern void recordLauncherStatistics();

	// Saves launcher data to file
	extern void saveLauncherData();

	// Saves launcher data to file
	extern void uploadLauncherData();

}


/* ========================================================================== */
/*                                GPS Namespace                               */
/* ========================================================================== */
namespace ace::gps {


	/* ========================================================================== */
	/*                            Variable Declarations                           */
	/* ========================================================================== */

	extern pros::Task __task_gps;

	const float err_degree_max = 1;
	const float err_pos_max = 4;
	const double err_gps_max = 0.025;

	const bool ENABLE_GPS = false;

	extern float startAngle;

	extern pros::c::gps_status_s_t currentStatus;
	extern bool error_is_okay;

	extern void init(float startAngle);

	/* ========================================================================== */
	/*                            Function Declarations                           */
	/* ========================================================================== */

	/**
	 *	Task that runs every 50 ms and fact checks imu angle
	 */
	extern void __task_gps_factcheck();

	extern float get_turn_angle(float angle);

	extern vec2 get_drive_nums();

	/**
	 *	Tell Robot to go to absolute position if not there already
	 *	@param x
	 * 		Absolute X Position in inches with 0 being center of field
	 *	@param y
	 * 		Absolute Y Position in inches with 0 being center of field
	 */
	extern void set_waypoint(float x, float y);
}


#endif
