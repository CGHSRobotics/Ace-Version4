
#include "ace.h"

/**
 *
 *	Autonomous Routines
 *	- called by auton selector
 *	- given reference to chassis as paramater
 *
 */

namespace ace::auton {

	/**
	 *
	 *	@brief Skills Autonomous Routine
	 *
	 *	@details Gets ...
	 *
	 *	@param chassis  Reference to Chassis object
	 *
	 */
	void skills_Auto() {

		printf(("\n GPS  " + std::to_string(gpsSensor.get_heading())).c_str());
		printf(("\n IMU  " + std::to_string(chassis.get_gyro())).c_str());

		gps::set_turn(45, SPEED_TURN_AUTO);

		launchDisks(false, 50, false, true);

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
		chassis.set_drive_pid(-16, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);
		chassis.set_drive_pid(12, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(90, SPEED_TURN_AUTO);

		chassis.set_drive_pid(52, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(85, SPEED_TURN_AUTO);
		intakeToggle(false);

		launchDisks_Auto(8000, SPEED_LAUNCHER, false);

		chassis.set_drive_pid(-16, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		gps::set_turn(180, SPEED_TURN_AUTO);
		intakeToggle(true);

		gps::set_waypoint(-36, -12);

		gps::set_turn(135, SPEED_TURN_AUTO, false);

		gps::set_waypoint(-12, 12);

		gps::set_turn(45, SPEED_TURN_AUTO, true);

		intakeToggle(false);
		launchDisks_Auto(80, SPEED_LAUNCHER);

		launcherMotor.move_voltage(0);

	}
	/*
	void skills_Auto() {

		// Get Roller 1
		chassis.set_drive_pid(-4, SPEED_DRIVE_AUTO_ROLLER);
		chassis.wait_drive();
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);
		chassis.set_drive_pid(8, SPEED_DRIVE_AUTO);
		chassis.wait_drive();


		chassis.set_turn_pid(-45, SPEED_TURN_AUTO);
		chassis.wait_drive();

		intakeToggle(true);
		chassis.set_drive_pid(20, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(90, SPEED_TURN_AUTO);
		chassis.wait_drive();

		// Get Roller 2
		chassis.set_drive_pid(-10, SPEED_DRIVE_AUTO_ROLLER);
		chassis.wait_drive();
		intakeToggle(false);
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);
		chassis.set_drive_pid(11, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(0, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(44, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(-8, SPEED_TURN_AUTO);
		chassis.wait_drive();

		// Shoot First 3
		launchDisks_Auto(4500, 80.0);

		chassis.set_turn_pid(0, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(-14, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		intakeToggle(true);
		chassis.set_turn_pid(90, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(20, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(44, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(36, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(-4, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(-50, SPEED_TURN_AUTO);
		chassis.wait_drive();

		// Shoot Second Three
		pros::delay(1000);
		intakeToggle(false);
		launchDisks_Auto(4000, 90.0);

		chassis.set_drive_pid(7.5, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(0, SPEED_TURN_AUTO);
		chassis.wait_drive();

		intakeToggle(true);

		chassis.set_drive_pid(47, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(-7, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(-90, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(-12, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(-100, SPEED_TURN_AUTO);
		chassis.wait_drive();

		// Shoot Third Three
		pros::delay(2000);
		intakeToggle(false);
		launchDisks_Auto(4000, SPEED_LAUNCHER);

		chassis.set_turn_pid(-90, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(-42, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(-180, SPEED_TURN_AUTO);
		chassis.wait_drive();

		// Third Roller
		chassis.set_drive_pid(-12, SPEED_DRIVE_AUTO_ROLLER);
		chassis.wait_drive();
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);
		chassis.set_drive_pid(8, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		intakeToggle(true);
		chassis.set_turn_pid(-225, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(24, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(-90, SPEED_TURN_AUTO);
		chassis.wait_drive();

		// Fourth Roller
		intakeToggle(false);
		chassis.set_drive_pid(-6, SPEED_DRIVE_AUTO_ROLLER);
		chassis.wait_drive();
		rollerForward(true, SPEED_ROLLER_AUTO_SKILLS);
		pros::delay(ROLLER_TIME_AUTO_SKILLS);
		rollerForward(false, 0);
		chassis.set_drive_pid(24, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(-135, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(-14, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		if (true)
		{
			endgameToggle(true);
			pros::delay(5000);
			endgameToggle(false);
		}

	}
	*/

	/**
	 *	@brief Three Side Autonomous Routine
	 *
	 *	@details
	 *		Gets 1 roller, shoots two in low goal,
	 *	 	collects 3 and then fires three into highgoal
	 *
	 *	@param chassis  Reference to Chassis object
	 *
	 */
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
		launchDisks_Auto(2000, SPEED_LAUNCHER_SHORT);

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

	/**
	 *	@brief Two Side Autonomous Routine
	 *
	 *	@details
	 *		Gets 1 roller, shoots two in low goal,
	 *	 	collects 3 and then fires three into highgoal
	 *
	 *	@param chassis  Reference to Chassis object
	 *
	 */
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
		launchDisks_Auto(8000, SPEED_LAUNCHER_LONG, true);
	}

	/**
	 *	@brief The Whole Shebang Autonomous Routine
	 *
	 *	@details
	 *		Gets 2 roller, shoots two in low goal,
	 *	 	collects 3 and then fires three into highgoal
	 *
	 *	@param chassis  Reference to Chassis object
	 *
	 */
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
		launchDisks_Auto(8000, SPEED_LAUNCHER_LONG, true);
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

	/**
	 *	@brief Null Autonomous Routine
	 *
	 *	@details
	 *		Does Nothing
	 *
	 *	@param chassis  Reference to Chassis object
	 *
	 */
	void null_Auto() {

		gps::set_waypoint(-24, -24);

		while (true)
		{
			pros::delay(ez::util::DELAY_TIME);
		}

	}
}


/**
 *
 *	Miscellaneous Auton Functions
 *
 */
namespace ace::auton {

	// 	Launch Disks Autonomously
	void launchDisks_Auto(float time, float speed, bool isLongDist) {
		float currTime = 0;
		while (currTime < time) {
			launchDisks(true, speed, isLongDist);

			currTime += ez::util::DELAY_TIME;
			pros::delay(ez::util::DELAY_TIME);
		}

		launchDisks(false, 0, false, true);
	}

	int autonIndex = 0;

	// Updates auton selection and prints to screen
	void updateAutonSelection() {
		if (autonIndex < 0)
		{
			autonIndex += numAutons;
		}
		if (autonIndex >= numAutons)
		{
			autonIndex -= numAutons;
		}

		std::string str = ace::auton::autonArray[ace::auton::autonIndex];

		printf("\n Auton Changed To: %s", str.c_str());
	}

	// Adjust Auton based off of buttons
	void checkAutonButtons() {

		if (master.get_digital_new_press(BUTTON_AUTON_INCREASE))
		{
			autonIndex++;
			updateAutonSelection();
		}
		if (master.get_digital_new_press(BUTTON_AUTON_DECREASE))
		{
			autonIndex--;
			updateAutonSelection();
		}
	}
}