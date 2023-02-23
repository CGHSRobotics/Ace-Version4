
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


	/* ========================================================================== */
	/*                                Global Stuff                                */
	/* ========================================================================== */

	extern int autonIndex;
	extern int alliance;	// inits to 0; aka red alliance

	extern bool activeBreakEnabled;

	const char launcherFile_path[18] = "/usd/launcher.txt";

	extern string operation_mode;

	extern int autonIndex;
	const int numAutons = 7;
	const std::string autonArray[10] = { "Skills", "Shebang", "Null", "Blue Three", "Blue Two", "Red Three", "Red Two" };

	/* ========================================================================== */
	/*                             Port Configurations                            */
	/* ========================================================================== */

	const int DRIVE_LEFT_FRONT_PORT = 11;
	const int DRIVE_LEFT_BACK_PORT = 12;
	const int DRIVE_RIGHT_FRONT_PORT = 1;
	const int DRIVE_RIGHT_BACK_PORT = 20;
	const int LAUNCHER_PORT = 17;
	const int VAR_LAUNCHER_PORT = 4;
	const int CONVEYOR_PORT = 15;
	const int INTAKE_PORT = 16;

	const int IMU_PORT = 18;
	const int VISION_PORT = 10;
	const int GPS_PORT = 2;


	/* ========================================================================== */
	/*                               Speed Constants                              */
	/* ========================================================================== */

	/* --------------------------- Drive Constants --------------------------- */
	const float SPEED_DRIVE_AUTO = 0.87 * 127.0;
	const float SPEED_DRIVE_AUTO_INTAKE = 0.2 * 127.0;
	const float SPEED_DRIVE_AUTO_ROLLER = 0.25 * 127.0;

	const float SPEED_TURN_AUTO = 0.7 * 127.0;

	/* ------------------------------ Intake Motor ------------------------------ */
	const float SPEED_INTAKE = 100;
	const float SPEED_INTAKE_LAUNCHER = 100;

	/* ----------------------------- Conveyor Motor ----------------------------- */
	const float SPEED_CONVEYOR_INTAKE = 100;
	const float SPEED_CONVEYOR_LAUNCHER = 100;
	const float SPEED_CONVEYOR_LAUNCHER_LONG = 35;

	/* ------------------------------ Roller Motor ------------------------------ */
	const float SPEED_ROLLER = 100;
	const float SPEED_ROLLER_AUTO = 10;
	const float SPEED_ROLLER_AUTO_SKILLS = 55;
	const float SPEED_ROLLER_LAUNCHER = 100;

	const float ROLLER_TIME_AUTO = 200;
	const float ROLLER_TIME_AUTO_SKILLS = 300;

	/* ----------------------------- Launcher Motor ----------------------------- */
	const float SPEED_LAUNCHER = 80;
	const float SPEED_LAUNCHER_DRIVER = 80;
	const float SPEED_LAUNCHER_LONG = 100;
	const float SPEED_LAUNCHER_SHORT = 60;
	const float LAUNCHER_MIN_SPEED = 20.0;


	/* ========================================================================== */
	/*                             Device Declarations                            */
	/* ========================================================================== */


	/* --------------------------------- Motors --------------------------------- */

	const pros::Motor launcherMotor(LAUNCHER_PORT, LAUNCHER_GEAR_RATIO, true);
	const pros::Motor varLauncherMotor(VAR_LAUNCHER_PORT, INTAKE_GEAR_RATIO, false);
	const pros::Motor conveyorMotor(CONVEYOR_PORT, CONVEYOR_GEAR_RATIO, false);
	const pros::Motor intakeMotor(INTAKE_PORT, INTAKE_GEAR_RATIO, false);

	/* --------------------------------- Endgame -------------------------------- */

	const pros::ADIDigitalOut endgamePneumatics('a', false);

	/* ----------------------------------- GPS ---------------------------------- */

	const float GPS_OffsetX = -100 / 1000.0;
	const float GPS_OffsetY = 136.0 / 1000.0;

	const pros::GPS gpsSensor(GPS_PORT, GPS_OffsetX, GPS_OffsetY);


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


	/* ========================================================================== */
	/*                        Autonomous Selector Functions                       */
	/* ========================================================================== */

	/**
	 *     @brief Update Auton Selection
	 */
	extern void updateAutonSelection();

	/**
	 *     @brief Check Buttons for change in Auton
	 */
	extern void checkAutonButtons();


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

	extern void rollerForward(bool enabled, float speed);
	extern void rollerReverse(bool enabled, float speed);

	extern void endgameToggle(bool enabled);

}


/* ========================================================================== */
/*                            Autonomous Namespace                            */
/* ========================================================================== */
namespace ace::auton {

	// Autonomous Routines
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
		float set_rpm;
	};


	/* ========================================================================== */
	/*                            Function Declarations                           */
	/* ========================================================================== */

	// array of launcher data points
	std::vector<l_data_point> l_data_array;

	// launch disks function
	extern void launchDisks(bool enabled, float speed, bool isLongDist = false, bool standby = false);

	// launch disks auton
	extern void launchDisks_Auto(float time, float speed, bool isLongDist = false);

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

	extern pros::Task task_turn_gps;

	const float err_degree_max = 2;
	const float err_pos_max = 4;
	const float err_gps_max = 0.25;

	extern void init();

	extern float curr_turnAngle;
	extern float curr_turnSpeed;

	extern float imu_start_angle;

	/* ========================================================================== */
	/*                            Function Definitions                            */
	/* ========================================================================== */

	/**
	 *	Task that runs every 50 ms and fact checks imu angle
	 */
	extern void __task_gps_factcheck_angle();

	/**
	 * 	Turns robot to absolute degrees, tracks angle and speed data as well as compensates for gps
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


/* ========================================================================== */
/*                               LVGL Namespace                               */
/* ========================================================================== */
namespace ace::lvgl {

}

#endif
