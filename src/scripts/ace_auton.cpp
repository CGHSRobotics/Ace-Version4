
#include "ace.h"

namespace ace::auton {

	/* ========================================================================== */
	/*                                 Skills Auto                                */
	/* ========================================================================== */
	void skills_Auto() {

		/* -------------------------------- Init GPS -------------------------------- */
		gps::init(0);

		set_turn(0, SPEED_TURN_AUTO);

		launch::set_standby(true);
		launch::launchDisks(0, false);
		set_var_launcher(false);
		set_drive(-3, SPEED_DRIVE_AUTO_ROLLER);

		/* ------------------------------ Get Roller 1 ------------------------------ */

		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);

		/* ------------------- Intake 1 Disk / Move To 2nd Roller ------------------- */
		set_drive(5, SPEED_DRIVE_AUTO);
		intakeToggle(true);
		set_turn(-45, SPEED_TURN_AUTO);
		set_drive(12 * rad2, SPEED_DRIVE_AUTO);

		set_turn(90, SPEED_TURN_AUTO);
		set_drive(-14, SPEED_DRIVE_AUTO_ROLLER);

		/* ------------------------------ Get Roller 2 ------------------------------ */
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);

		set_drive(5.5, SPEED_DRIVE_AUTO);

		/* -------------------------- Move To Red High Goal ------------------------- */
		set_turn(0, SPEED_TURN_AUTO);
		set_drive(60, SPEED_DRIVE_ULTRA_FAST);

		/* ------------------------------ Shoot 3 Disks ----------------------------- */
		intakeToggle(false);
		set_turn(-5, SPEED_TURN_AUTO);
		launch::launchDisks_Auto(4000, SPEED_LAUNCHER_SHORT);

		/* ------------------ Intake 3 Disks Along Edge Of Low Goal ----------------- */

		set_turn(0, SPEED_TURN_AUTO);
		set_drive(0, SPEED_DRIVE_AUTO);
		intakeToggle(true);

		set_turn(88, SPEED_TURN_AUTO);
		set_drive(36, SPEED_DRIVE_AUTO_INTAKE);
		set_turn(90, SPEED_TURN_AUTO);
		set_drive(-35, SPEED_DRIVE_AUTO);
		set_turn(0, SPEED_TURN_AUTO);

		set_drive(0, SPEED_DRIVE_AUTO);
		set_turn(0, SPEED_TURN_AUTO);

		/* ------------------------ Shoot 2nd Set Of 3 Disks ------------------------ */
		intakeToggle(false);
		set_turn(-5, SPEED_TURN_AUTO);
		launch::launchDisks_Auto(4000, SPEED_LAUNCHER_SHORT);

		/* -------------------------- Intake Diagonal Disks ------------------------- */
		set_turn(0, SPEED_TURN_AUTO);
		set_drive(-26, SPEED_DRIVE_ULTRA_FAST);

		set_turn(90, SPEED_TURN_AUTO);
		intakeToggle(true);

		set_drive(24, SPEED_DRIVE_ULTRA_FAST);
		set_turn(45, SPEED_TURN_AUTO);

		set_drive(24 * rad2, SPEED_DRIVE_ULTRA_FAST);


		/* --------------------- Shoot 3rd Set Of 3 Without Flap -------------------- */
		set_turn(-45, SPEED_TURN_AUTO);
		set_drive(2.5 * rad2, SPEED_DRIVE_AUTO);
		var_launcher_enabled = false;
		varLauncherMove();
		set_turn(-60, SPEED_TURN_AUTO);
		intakeToggle(false);
		launch::launchDisks_Auto(4000, SPEED_LAUNCHER_SHORT);
		set_turn(-45, SPEED_TURN_AUTO);

		/* --------------- Intake Other 3 Disks Along Edge Of Low Goal -------------- */
		intakeToggle(true);

		set_turn(-2, SPEED_TURN_AUTO);

		set_drive(54, SPEED_DRIVE_AUTO_INTAKE);

		set_drive(-6, SPEED_DRIVE_AUTO);
		set_turn(-90, SPEED_TURN_AUTO);

		/* ---------------------- Shoot 4th Set Of 3 Without Flap ---------------------- */
		set_var_launcher(false);
		set_turn(-110, SPEED_TURN_AUTO);
		pros::delay(1000);
		intakeToggle(false);
		launch::launchDisks_Auto(4000, SPEED_LAUNCHER_SHORT);
		set_turn(-90, SPEED_TURN_AUTO);

		/* ---------------------------- Move To Roller 3 ---------------------------- */

		set_drive(-50, SPEED_DRIVE_ULTRA_FAST);

		set_turn(-180, SPEED_TURN_AUTO);

		set_drive(-6, SPEED_DRIVE_AUTO);

		/* ------------------------------ Get Roller 3 ------------------------------ */
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);

		set_drive(4, SPEED_DRIVE_AUTO);

		/* ---------------------------- Move To Roller 4 ---------------------------- */
		intakeToggle(true);

		set_turn(-225, SPEED_TURN_AUTO);

		set_drive(16 * rad2, SPEED_DRIVE_AUTO);

		set_turn(-90, SPEED_TURN_AUTO);

		set_drive(-12, SPEED_DRIVE_AUTO);

		/* ------------------------------ Get Roller 4 ------------------------------ */
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);

		set_drive(20, SPEED_DRIVE_AUTO);

		set_turn(-135, SPEED_TURN_AUTO);

		set_drive(-12, SPEED_DRIVE_AUTO);

		endgameToggle(true);
		pros::delay(200);
		endgameToggle(false);

		resetMotors();
	}


	/* ========================================================================== */
	/*                               Three Side Auto                              */
	/* ========================================================================== */
	void threeSide_Auto() {

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
	void twoSide_Auto() {

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
	void theWholeShebang_Auto() {

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
	void null_Auto() {

		gps::set_waypoint(-24, -24);

		while (true)
		{
			pros::delay(ez::util::DELAY_TIME);
		}

	}
}
