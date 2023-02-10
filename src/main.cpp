#include "scripts/cghs.h"

#include "scripts/lvgl.cpp"

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
	0.6
);

int screenUpdateCounter = 0;
const int screenUpdateCounterMax = 50;

void screenUpdate() {

	if (screenUpdateCounter >= screenUpdateCounterMax) {
		screenUpdateCounter -= screenUpdateCounterMax;

		std::string str = cghs::auton::autonArray[cghs::auton::autonIndex];
		master.set_text(2, 0, (str + "  " + "ABrake: " + std::to_string(cghs::activeBreakEnabled) + "   ").c_str());

		lv_label_set_text(labelTemp, (
			(string)"Temperature in Celsius (Max is 55C): \n" +
			"\nLeft Front: " + std::to_string(chassis.left_motors[0].get_temperature()) +
			"\nLeft Back: " + std::to_string(chassis.left_motors[1].get_temperature()) +
			"\nRight Front: " + std::to_string(chassis.right_motors[0].get_temperature()) +
			"\nRight Back: " + std::to_string(chassis.right_motors[1].get_temperature()) +
			"\n" +
			"\nLauncher: " + std::to_string(cghs::launcherMotor.get_temperature()) +
			"\nRoller: " + std::to_string(cghs::rollerMotor.get_temperature()) +
			"\nConveyor: " + std::to_string(cghs::conveyorMotor.get_temperature()) +
			"\nIntake: " + std::to_string(cghs::intakeMotor.get_temperature())
			).c_str()

		);
	}
	screenUpdateCounter += ez::util::DELAY_TIME;
}



/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// Print our branding over your terminal :D
	//ez::print_ez_template();

	printf("\n Initializing... \n");

	cghs::activeBreakEnabled = true;

	pros::delay(500);  // Stop the user from doing anything while legacy ports configure.

	chassis.toggle_modify_curve_with_controller(false);
	chassis.set_curve_default(0, 0);

	// Initialize chassis and auton selector
	chassis.initialize();
	//ez::as::initialize();

	// Screen
	pros::lcd::shutdown();

	init_lv_screen();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
	while (true) {
		cghs::resetMotors();
		cghs::auton::checkAutonButtons();

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

	cghs::resetMotors();

	chassis.reset_pid_targets();                // Resets PID targets to 0
	chassis.reset_gyro();                       // Reset gyro position to 0
	chassis.reset_drive_sensor();               // Reset drive sensors to 0
	chassis.set_drive_brake(MOTOR_BRAKE_HOLD);  // Set motors to hold.  This helps autonomous consistency.

	std::string str = cghs::auton::autonArray[cghs::auton::autonIndex];

	printf("\n\nCalling Auton...	%s \n\n", str.c_str());

	if (str == "Skills")
	{
		cghs::auton::skills_Auto(chassis);
	}
	else if (str == "Null")
	{
		cghs::auton::null_Auto(chassis);
	}
	else if (str == "Three")
	{
		cghs::auton::threeSide_Auto(chassis);
	}
	else if (str == "Two")
	{
		cghs::auton::twoSide_Auto(chassis);
	}
	else if (str == "Shebang")
	{
		cghs::auton::theWholeShebang_Auto(chassis);
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

	cghs::resetMotors();

	// This is preference to what you like to drive on.
	chassis.set_drive_brake(MOTOR_BRAKE_COAST);

	bool intakeToggleEnabled = false;
	bool intakeReverseEnabled = false;

	bool rollerForwardToggle = false;
	bool rollerReverseToggle = false;

	bool endgameToggleEnabled = false;

	while (true) {
		cghs::auton::checkAutonButtons();
		screenUpdate();

		chassis.tank();  // Tank control

		// active break Intake
		if (master.get_digital_new_press(BUTTON_A_BRAKE_TOGGLE)) {
			cghs::activeBreakEnabled = !cghs::activeBreakEnabled;
		}

		// Toggle Intake
		if (master.get_digital_new_press(BUTTON_INTAKE_TOGGLE)) {
			intakeToggleEnabled = !intakeToggleEnabled;
			cghs::intakeToggle(intakeToggleEnabled);
		}

		// Emergency Reverse Intake
		if (master.get_digital_new_press(BUTTON_INTAKE_REVERSE)) {
			intakeToggleEnabled = false;
			cghs::intakeToggle(false);
			intakeReverseEnabled = true;
			cghs::intakeReverse(true);
		}
		if (intakeReverseEnabled && !master.get_digital(BUTTON_INTAKE_REVERSE)) {
			intakeReverseEnabled = false;
			cghs::intakeReverse(false);
		}

		// Roller Forward Intake
		if (master.get_digital_new_press(BUTTON_ROLLER_FORWARD)) {
			rollerForwardToggle = true;
			cghs::rollerForward(true, cghs::SPEED_ROLLER);
		}
		if (rollerForwardToggle && !master.get_digital(BUTTON_ROLLER_FORWARD)) {
			rollerForwardToggle = false;
			cghs::rollerForward(false, 0);
		}

		// Roller Reverse Intake
		if (master.get_digital_new_press(BUTTON_ROLLER_REVERSE)) {
			rollerReverseToggle = true;
			cghs::rollerReverse(true, cghs::SPEED_ROLLER);
		}
		if (rollerReverseToggle && !master.get_digital(BUTTON_ROLLER_REVERSE)) {
			rollerReverseToggle = false;
			cghs::rollerReverse(false, 0);
		}

		// Launch Disks
		if (master.get_digital(BUTTON_LAUNCHER_LONG)) {

			// 	Launch Disks from Long Distance
			cghs::active_brake(true, chassis);
			cghs::launchDisks(true, cghs::SPEED_LAUNCHER_LONG, true);
		}
		if (master.get_digital(BUTTON_LAUNCHER)) {

			// 	Launch Disks from Normal Range
			cghs::active_brake(true, chassis);
			cghs::launchDisks(true, cghs::SPEED_LAUNCHER);
		}
		if (cghs::launcherEnabled && !master.get_digital(BUTTON_LAUNCHER) && !master.get_digital(BUTTON_LAUNCHER_LONG)) {

			//	Disable Launching
			cghs::active_brake(false, chassis);
			cghs::launchDisks(false, 0.0);
		}

		// Endgame Toggle
		if (master.get_digital(BUTTON_ENDGAME)) {
			endgameToggleEnabled = true;
			cghs::endgameToggle(true);
		}
		if (endgameToggleEnabled && !master.get_digital(BUTTON_ENDGAME)) {
			endgameToggleEnabled = false;
			cghs::endgameToggle(false);
		}

		pros::delay(ez::util::DELAY_TIME);  // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
	}
}
