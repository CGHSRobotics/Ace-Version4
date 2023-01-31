#pragma once

#include "lvgl.cpp"
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


#define DRIVE_GEAR_RATIO MOTOR_GEARSET_18
#define LAUNCHER_GEAR_RATIO MOTOR_GEARSET_06
#define ROLLER_GEAR_RATIO MOTOR_GEARSET_18
#define CONVEYOR_GEAR_RATIO MOTOR_GEARSET_06
#define INTAKE_GEAR_RATIO MOTOR_GEARSET_18



// namespace cghs
namespace cghs {

	// Chassis constructor
	Drive chassis(
		// Left Chassis Ports (negative port will reverse it!)
		{ -cghs::DRIVE_LEFT_FRONT_PORT, -cghs::DRIVE_LEFT_BACK_PORT },
		// Right Chassis Ports (negative port will reverse it!)
		{ cghs::DRIVE_RIGHT_FRONT_PORT, cghs::DRIVE_RIGHT_BACK_PORT },
		// IMU Port
		cghs::IMU_PORT,
		// Wheel Diameter (Remember, 4" wheels are actually 4.125!)
		3.25,
		// Cartridge RPM
		200,
		// External Gear Ratio (MUST BE DECIMAL)
		0.6);

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


	/*
	 *  Speed Constants
	 */

	 // Drive Constants
	bool activeBreakEnabled = false;

	const float SPEED_DRIVE_AUTO = 0.75 * 127.0;
	const float SPEED_DRIVE_AUTO_INTAKE = 0.25 * 127.0;

	const float SPEED_TURN_AUTO = 0.5 * 127.0;

	// Intake Motor
	const float SPEED_INTAKE = 100;
	const float SPEED_INTAKE_LAUNCHER = 100;

	// Conveyor Motor
	const float SPEED_CONVEYOR_INTAKE = 100;
	const float SPEED_CONVEYOR_LAUNCHER = 50;
	const float SPEED_CONVEYOR_LAUNCHER_LONG = 35;

	// Roller Motor
	const float SPEED_ROLLER = 100;
	const float SPEED_ROLLER_AUTO = 25;
	const float SPEED_ROLLER_LAUNCHER = 100;

	// Launcher Motor
	const float SPEED_LAUNCHER = 80;
	const float SPEED_LAUNCHER_LONG = 100;
	const float LAUNCHER_MIN_SPEED = 0.9;

	extern float launcherTimerDelay;
	const float launcherTimerDelayMax = 1;

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

	/*
	 *  Function Declarations
	 */

	 // Util Functions
	extern void spinMotor(pros::Motor motor, float percent);

	extern void active_brake(bool enabled, Drive chassis);

	// User Control
	extern void resetInputs();

	extern void intakeToggle(bool enabled);
	extern void intakeReverse(bool enabled);

	extern void launchDisks(bool enabled, float speed);
	extern void launchDisksLong(float speed);

	extern void rollerForward(bool enabled, float speed);
	extern void rollerReverse(bool enabled, float speed);

	extern void endgameToggle(bool enabled);

	namespace auton {

		// Autonomous
		extern void launchDisks_Auto(float time, float speed);
		extern void launchDisksLong_Auto(float time);

		extern void skills_Auto();
		extern void null_Auto();
		extern void threeSide_Auto();
		extern void twoSide_Auto();
		extern void theWholeShebang_Auto();

	}  // namespace auton

}  // namespace cghs

#endif
