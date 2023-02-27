
#include "ace.h"

namespace ace {

	/* ========================================================================== */
	/*                                 Skills Auto                                */
	/* ========================================================================== */
	void auton::skills_Auto() {

		chassis.imu.set_rotation(gpsSensor.get_heading());

		printf(("\n GPS  " + std::to_string(gpsSensor.get_heading())).c_str());
		printf(("\n GPS ERROR  " + std::to_string(gpsSensor.get_error())).c_str());
		printf(("\n IMU  " + std::to_string(chassis.get_gyro())).c_str());

		gps::set_turn(45, SPEED_TURN_AUTO);

		launch::set_standby(true);

		// Get Roller 1
		chassis.set_drive_pid(-4, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);
		intakeToggle(true);
		chassis.set_drive_pid(16, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(180, SPEED_TURN_AUTO);

		// Get Roller 2
		chassis.set_drive_pid(-12, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);
		chassis.set_drive_pid(6, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(90, SPEED_TURN_AUTO);

		chassis.set_drive_pid(70, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(85, SPEED_TURN_AUTO);
		intakeToggle(false);

		var_launcher_enabled = true;
		launch::launchDisks_Auto(8000, SPEED_LAUNCHER_SHORT);

		chassis.set_drive_pid(-16, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(180, SPEED_TURN_AUTO);
		intakeToggle(true);

		chassis.set_drive_pid(24, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(135, SPEED_TURN_AUTO, false);

		chassis.set_drive_pid(24 * 1.41, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(45, SPEED_TURN_AUTO, true);

		intakeToggle(false);
		launch::launchDisks_Auto(80, SPEED_LAUNCHER_SHORT);

		launcherMotor.move_voltage(0);

	}


	/* ========================================================================== */
	/*                               Three Side Auto                              */
	/* ========================================================================== */
	void auton::threeSide_Auto() {

		// Get Roller
		chassis.set_drive_pid(-4, 0.5 * 127.0, false);
		chassis.wait_drive();
		rollerForward(true, SPEED_ROLLER_AUTO);
		pros::delay(250);
		rollerForward(false, 0);
		chassis.set_drive_pid(4, 0.5 * 127.0, false);
		chassis.wait_drive();

		// Turn 90 Degrees and get 2 Low Goals
		chassis.set_turn_pid(90, SPEED_TURN_AUTO);
		chassis.wait_drive();
		chassis.set_drive_pid(4, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		launch::launchDisks_Auto(2000, SPEED_LAUNCHER_SHORT);

		/*
		// Turn 45 Degrees to the Right
		chassis.set_turn_pid(45, SPEED_TURN_AUTO);
		chassis.wait_drive();

		// Charge Forward and push disks
		chassis.set_drive_pid(65, SPEED_DRIVE_AUTO);
		chassis.wait_until(40);
		intakeToggle(true);
		chassis.set_max_speed(SPEED_DRIVE_AUTO_INTAKE);
		chassis.wait_drive();
		intakeToggle(false);

		intakeReverse(true);
		pros::delay(250);
		intakeReverse(false);

		intakeToggle(true);
		chassis.set_turn_pid(-47, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(4, SPEED_DRIVE_AUTO, true);
		chassis.wait_drive();
		intakeToggle(false);

		launchDisksLong_Auto(8000);
		*/
	}


	/* ========================================================================== */
	/*                                Two Side Auto                               */
	/* ========================================================================== */
	void auton::twoSide_Auto() {

		// Fire two low goals
		//launchDisksShort_Auto(1500, 60);

		chassis.set_drive_pid(-24, SPEED_DRIVE_AUTO, true);
		chassis.wait_drive();
		chassis.set_turn_pid(90, SPEED_TURN_AUTO);
		chassis.wait_drive();
		chassis.set_drive_pid(-8, SPEED_DRIVE_AUTO_ROLLER);
		chassis.wait_drive();

		rollerForward(true, SPEED_ROLLER_AUTO);
		pros::delay(ROLLER_TIME_AUTO);
		rollerForward(false, 0);

		chassis.set_drive_pid(2, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(45, SPEED_TURN_AUTO);
		chassis.wait_drive();

		launcherMotor.move_velocity(600);

		intakeToggle(true);
		chassis.set_drive_pid(74, SPEED_DRIVE_AUTO, true);
		chassis.wait_drive();
		chassis.set_drive_pid(10, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		chassis.set_drive_pid(-8, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(135, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(8, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		intakeToggle(false);
		launch::launchDisks_Auto(8000, SPEED_LAUNCHER_LONG, true);
	}


	/* ========================================================================== */
	/*                           The Whole Shebang Auto                           */
	/* ========================================================================== */
	void auton::theWholeShebang_Auto() {

		chassis.set_drive_pid(-24, SPEED_DRIVE_AUTO, true);
		chassis.wait_drive();
		chassis.set_turn_pid(90, SPEED_TURN_AUTO);
		chassis.wait_drive();
		chassis.set_drive_pid(-7, 0.4 * 127.0);
		chassis.wait_drive();

		rollerForward(true, SPEED_ROLLER_AUTO);
		pros::delay(100);
		rollerForward(false, 0);

		chassis.set_drive_pid(4, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(45, SPEED_TURN_AUTO);
		chassis.wait_drive();

		launcherMotor.move_velocity(600);

		intakeToggle(true);
		chassis.set_drive_pid(74, SPEED_DRIVE_AUTO, true);
		chassis.wait_drive();
		chassis.set_drive_pid(10, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		chassis.set_drive_pid(-8, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(135, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(8, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		intakeToggle(false);
		launch::launchDisks_Auto(8000, SPEED_LAUNCHER_LONG, true);
		intakeToggle(true);

		chassis.set_drive_pid(-8, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(45, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(70, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(180, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(-8, SPEED_DRIVE_AUTO_ROLLER);
		chassis.wait_drive();

		rollerForward(true, SPEED_ROLLER_AUTO);
		pros::delay(ROLLER_TIME_AUTO);
		rollerForward(false, 0);

		chassis.set_drive_pid(4, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
	}


	/* ========================================================================== */
	/*                                  Null Auto                                 */
	/* ========================================================================== */
	void auton::null_Auto() {

		gps::set_waypoint(-24, -24);

		while (true)
		{
			pros::delay(ez::util::DELAY_TIME);
		}

	}
}
