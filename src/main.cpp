#include "scripts/ace.h"

#include "scripts/ace_lvgl.cpp"


void screenUpdate() {

	while (true) {

		// 	Controller Text
		std::string str_selectedAuton = ace::auton::autonArray[ace::auton::autonIndex];
		master.set_text(2, 0, (str_selectedAuton + "          ").c_str());

		//	Tab 2 - Auton
		lv_label_set_text(auton_label, ((string)"Selected: " + str_selectedAuton).c_str());

		//	Tab 3 - Temp
		lv_label_set_text(labelTemp, (
			(string)"Temp (Max is 130F): \n" +
			"\nL Front: " + std::to_string(ace::cel_to_faren(chassis.left_motors[0].get_temperature())) +
			"\nL Back: " + std::to_string(ace::cel_to_faren(chassis.left_motors[1].get_temperature())) +
			"\nR Front: " + std::to_string(ace::cel_to_faren(chassis.right_motors[0].get_temperature())) +
			"\nR Back: " + std::to_string(ace::cel_to_faren(chassis.right_motors[1].get_temperature()))
			).c_str()
		);
		lv_label_set_text(labelTemp2, (
			(string)"  \n" +
			"\nLauncher: " + std::to_string(ace::cel_to_faren(ace::launcherMotor.get_temperature())) +
			"\nRoller: " + std::to_string(ace::cel_to_faren(ace::rollerMotor.get_temperature())) +
			"\nIntake: " + std::to_string(ace::cel_to_faren(ace::intakeMotor.get_temperature())) +
			"\nDTS: " + std::to_string(ace::cel_to_faren(ace::conveyorMotor.get_temperature()))
			).c_str()
		);

		pros::delay(50);
	}
}


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {

	ace::operation_mode = "init";

	// Print our branding over your terminal :D
	//ez::print_ez_template();

	printf("\n Initializing... \n");

	pros::delay(500);  // Stop the user from doing anything while legacy ports configure.

	chassis.toggle_modify_curve_with_controller(false);
	chassis.set_curve_default(10, 0);

	// Initialize chassis and auton selector
	chassis.initialize();

	// Shut down pros gfx library
	pros::lcd::shutdown();

	// Start LVGL Code
	init_lv_screen();

	// Start screen update task, runs every 50 ms
	pros::Task __task_screenUpdate(screenUpdate);
	__task_screenUpdate.set_priority(TASK_PRIORITY_DEFAULT - 1);

	// init gps logic
	ace::gps::init();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
	ace::operation_mode = "disabled";


	while (true) {
		ace::resetMotors();
		ace::auton::checkAutonButtons();

		screenUpdate();

		pros::delay(ez::util::DELAY_TIME);
	}
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
	// . . .
}


/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {

	ace::operation_mode = "auto";

	ace::resetMotors();

	chassis.reset_pid_targets();                // Resets PID targets to 0
	//chassis.reset_gyro();                       // Reset gyro position to 0
	chassis.reset_drive_sensor();               // Reset drive sensors to 0
	chassis.set_drive_brake(MOTOR_BRAKE_HOLD);  // Set motors to hold.  This helps autonomous consistency.

	std::string str = ace::auton::autonArray[ace::auton::autonIndex];

	printf("\n\nCalling Auton...	%s \n\n", str.c_str());

	if (str == "Skills")
	{
		ace::auton::skills_Auto();
	}
	else if (str == "Null")
	{
		ace::auton::null_Auto();
	}
	else if (str == "Blue Three")
	{
		ace::auton::threeSide_Auto();
	}
	else if (str == "Blue Two")
	{
		ace::auton::twoSide_Auto();
	}
	else if (str == "Shebang")
	{
		ace::auton::theWholeShebang_Auto();
	}
	else if (str == "Red Two")
	{
		ace::auton::twoSide_Auto();
	}
	else if (str == "Red Three")
	{
		ace::auton::theWholeShebang_Auto();
	}
	else
	{
		printf("ERROR: no skills found %d", str);
	}

}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

void opcontrol() {

	ace::operation_mode = "user";
	ace::resetMotors();

	// This is preference to what you like to drive on.
	chassis.set_drive_brake(MOTOR_BRAKE_COAST);

	bool intakeToggleEnabled = false;
	bool intakeReverseEnabled = false;

	bool rollerForwardToggle = false;
	bool rollerReverseToggle = false;

	bool endgameToggleEnabled = false;

	while (true) {
		ace::auton::checkAutonButtons();

		chassis.tank();  // Tank control

		// active break Intake
		/*if (master.get_digital_new_press(BUTTON_A_BRAKE_TOGGLE)) {
			ace::activeBreakEnabled = !ace::activeBreakEnabled;
		}*/

		// Toggle Intake
		if (master.get_digital_new_press(BUTTON_INTAKE_TOGGLE)) {
			intakeToggleEnabled = !intakeToggleEnabled;
			ace::intakeToggle(intakeToggleEnabled);
		}

		// Emergency Reverse Intake
		if (master.get_digital_new_press(BUTTON_INTAKE_REVERSE)) {
			intakeToggleEnabled = false;
			ace::intakeToggle(false);
			intakeReverseEnabled = true;
			ace::intakeReverse(true);
		}
		if (intakeReverseEnabled && !master.get_digital(BUTTON_INTAKE_REVERSE)) {
			intakeReverseEnabled = false;
			ace::intakeReverse(false);
		}

		// Roller Forward Intake
		if (master.get_digital_new_press(BUTTON_ROLLER_FORWARD)) {
			rollerForwardToggle = true;
			ace::rollerForward(true, ace::SPEED_ROLLER);
		}
		if (rollerForwardToggle && !master.get_digital(BUTTON_ROLLER_FORWARD)) {
			rollerForwardToggle = false;
			ace::rollerForward(false, 0);
		}

		// Roller Reverse Intake
		if (master.get_digital_new_press(BUTTON_ROLLER_REVERSE)) {
			rollerReverseToggle = true;
			ace::rollerReverse(true, ace::SPEED_ROLLER);
		}
		if (rollerReverseToggle && !master.get_digital(BUTTON_ROLLER_REVERSE)) {
			rollerReverseToggle = false;
			ace::rollerReverse(false, 0);
		}

		// Launch Disks
		if (master.get_digital(BUTTON_LAUNCHER_LONG)) {

			// 	Launch Disks from Long Distance
			ace::active_brake(true, chassis);
			ace::launchDisks(true, ace::SPEED_LAUNCHER_LONG, true);
		}
		if (master.get_digital(BUTTON_LAUNCHER)) {

			// 	Launch Disks from Normal Range
			ace::active_brake(true, chassis);
			ace::launchDisks(true, ace::SPEED_LAUNCHER_DRIVER);
		}
		if (ace::launcherEnabled && !master.get_digital(BUTTON_LAUNCHER) && !master.get_digital(BUTTON_LAUNCHER_LONG)) {

			//	Disable Launching
			ace::active_brake(false, chassis);
			ace::launchDisks(false, 0.0);
		}

		// Endgame Toggle
		if (master.get_digital(BUTTON_ENDGAME)) {
			endgameToggleEnabled = true;
			ace::endgameToggle(true);
		}
		if (endgameToggleEnabled && !master.get_digital(BUTTON_ENDGAME)) {
			endgameToggleEnabled = false;
			ace::endgameToggle(false);
		}

		pros::delay(ez::util::DELAY_TIME);  // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
	}
}
