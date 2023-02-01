
#include "cghs.h"

/**
 *
 *	Autonomous Routines
 *	- called by auton selector
 *	- given reference to chassis as paramater
 *
 */

namespace cghs::auton {

	/**
	 *
	 *	@brief Skills Autonomous Routine
	 *
		@details Gets ...
	 *
	 *	@param chassis  Reference to Chassis object
	 *
	 */
	void skills_Auto(Drive& chassis) {
		// Do Skills
	}

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
	void threeSide_Auto(Drive& chassis) {

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
		launchDisksShort_Auto(2000, 80);

		// Turn 45 Degrees to the Right
		chassis.set_turn_pid(45, SPEED_TURN_AUTO);
		chassis.wait_drive();

		// Charge Forward and push disks
		chassis.set_drive_pid(65, SPEED_DRIVE_AUTO, true);
		chassis.wait_until(36);
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
	void twoSide_Auto(Drive& chassis) {

		// Fire two low goals
		launchDisksShort_Auto(2000, 80);

		chassis.set_drive_pid(-24, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		chassis.set_turn_pid(90, SPEED_TURN_AUTO);
		chassis.wait_drive();
		chassis.set_drive_pid(-12, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		rollerForward(true, SPEED_ROLLER_AUTO);
		pros::delay(100);
		rollerForward(false, 0);

		chassis.set_drive_pid(2, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(45, SPEED_TURN_AUTO);
		chassis.wait_drive();

		intakeToggle(true);
		chassis.set_drive_pid(74, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		chassis.set_drive_pid(10, SPEED_DRIVE_AUTO);
		chassis.wait_drive();
		chassis.set_drive_pid(-10, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		chassis.set_turn_pid(135, SPEED_TURN_AUTO);
		chassis.wait_drive();

		chassis.set_drive_pid(6, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		intakeToggle(false);
		launchDisksLong_Auto(8000);
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
	void theWholeShebang_Auto(Drive& chassis) {
		launchDisks_Auto(2, 80);
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
	void null_Auto(Drive& chassis) {
		// Do Nothing
	}
}


/**
 *
 *	Auton Selector
 *
 */
namespace cghs::auton {

	/**
	 *	@brief Constructor for auton selector class
	 *
	 *  @param givenAutonList Reference to ddlist object that stores string of selected auton
	 */
	Selector::Selector(lv_obj_t* givenAutonList) {
		printf("Initializing cghs::auton Selector...");
		printf("Hacking the mainframe...");
		printf("Establishing Backdoor access...");

		autonList = givenAutonList;
	}


	/**
	 *	@brief Calls Auton based on string stored in ddlist
	 *
	 *  @param chassis Reference to chassis object
	 */
	void Selector::callSelectedAuton(Drive& chassis) {

		char sel_str[32];
		lv_ddlist_get_selected_str(autonList, sel_str);

		if (sel_str == "Skills")
		{
			skills_Auto(chassis);
		}
		else if (sel_str == "Null")
		{
			null_Auto(chassis);
		}
		else if (sel_str == "Three")
		{
			threeSide_Auto(chassis);
		}
		else if (sel_str == "Two")
		{
			twoSide_Auto(chassis);
		}
		else if (sel_str == "Shebang")
		{
			theWholeShebang_Auto(chassis);
		}
		else
		{
			printf("ERROR: no skills found %d", sel_str);
		}
	}


	void Selector::checkButtons() {

		if (master.get_digital_new_press(BUTTON_AUTON_INCREASE))
		{
			index++;
		}

		if (master.get_digital_new_press(BUTTON_AUTON_DECREASE))
		{
			index--;
		}

		if (index < 0)
		{
			index += numberAuton;
		}

		if (index >= numberAuton)
		{
			index -= numberAuton;
		}

		lv_ddlist_set_selected(autonList, index);
	}
}


/**
 *
 *	Miscellaneous Auton Functions
 *
 */
namespace cghs::auton {

	// 	Launch Disks Autonomously
	void launchDisks_Auto(float time, float speed) {
		float currTime = 0;
		while (currTime < time) {
			launchDisks(true, speed);

			currTime += ez::util::DELAY_TIME;
			pros::delay(ez::util::DELAY_TIME);
		}

		launchDisks(false, 0);
	}

	//	Launch disks long distance autonomously
	void launchDisksLong_Auto(float time) {
		float currTime = 0;
		while (currTime < time) {
			launchDisksLong(cghs::SPEED_LAUNCHER_LONG);

			currTime += ez::util::DELAY_TIME;
			pros::delay(ez::util::DELAY_TIME);
		}

		launchDisks(false, 0);
	}

	//	Launch disks short distance autonomously
	void launchDisksShort_Auto(float time, float speed) {
		float currTime = 0;
		while (currTime < time) {
			launchDisksShort(speed);

			currTime += ez::util::DELAY_TIME;
			pros::delay(ez::util::DELAY_TIME);
		}

		launchDisks(false, 0);
	}
}