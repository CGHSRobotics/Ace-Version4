
#include "scripts/ace.h"

#include "scripts/ace_lvgl.cpp"

// pros terminal > output.txt


/* ========================================================================== */
/*                                Screen Update                               */
/* ========================================================================== */
void screenUpdate() {

	while (true) {

		// 	Controller Text
		std::string str_selectedAuton = ace::auton::autonArray[ace::auton::autonIndex];
		master.set_text(2, 0, (str_selectedAuton + "  Standby: " + std::to_string(ace::LAUNCHER_STANDBY_ENABLED)).c_str());

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
			"\nVar_Launch: " + std::to_string(ace::cel_to_faren(ace::varLauncherMotor.get_temperature())) +
			"\nIntake: " + std::to_string(ace::cel_to_faren(ace::intakeMotor.get_temperature())) +
			"\nDTS: " + std::to_string(ace::cel_to_faren(ace::conveyorMotor.get_temperature()))
			).c_str()
		);

		pros::delay(50);
	}
}


/* ========================================================================== */
/*                                 Initialize                                 */
/* ========================================================================== */
void initialize() {

	ace::operation_mode = "init";

	printf("\n Initializing... \n");

	pros::delay(500);  // Stop the user from doing anything while legacy ports configure.

	chassis.toggle_modify_curve_with_controller(false);
	chassis.set_curve_default(10, 0);

	// Initialize chassis and auton selector
	chassis.initialize();

	ace::launcherMotor.set_brake_mode(MOTOR_BRAKE_COAST);

	ace::varLauncherMotor.set_brake_mode(MOTOR_BRAKE_HOLD);
	ace::varLauncherMotor.set_encoder_units(MOTOR_ENCODER_DEGREES);
	printf(std::to_string(ace::potentiometer_varL.get_value()).c_str());
	ace::varLauncherMotor.set_zero_position((ace::potentiometer_varL.get_value() / 4095.0) * 225.0);
	ace::varLauncherMotor.move_absolute(ace::VAR_LAUNCH_ANGLE_DOWN, -100);

	// Shut down pros gfx library
	pros::lcd::shutdown();

	// Start LVGL Code
	init_lv_screen();

	// Start screen update task, runs every 50 ms
	pros::Task __task_screenUpdate(screenUpdate);
	__task_screenUpdate.set_priority(TASK_PRIORITY_DEFAULT - 1);
}


/* ========================================================================== */
/*                                  Disabled                                  */
/* ========================================================================== */
void disabled() {
	ace::operation_mode = "disabled";

	while (true) {
		ace::resetMotors();
		ace::auton::checkAutonButtons();

		pros::delay(ez::util::DELAY_TIME);
	}
}

/* ========================================================================== */
/*                        Competition Init (Don't Use)                        */
/* ========================================================================== */
void competition_initialize() {}


/* ========================================================================== */
/*                             Autonomous Function                            */
/* ========================================================================== */
void autonomous() {

	ace::operation_mode = "auto";

	ace::resetMotors();

	chassis.reset_pid_targets();                // Resets PID targets to 0
	chassis.reset_gyro();						// Reset gyro position to 0
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
	else if (str == "3-B")
	{
		ace::auton::threeSide_Auto();
	}
	else if (str == "2-B")
	{
		ace::auton::twoSide_Auto();
	}
	else if (str == "Shebang")
	{
		ace::auton::theWholeShebang_Auto();
	}
	else if (str == "2-R")
	{
		ace::auton::twoSide_Auto();
	}
	else if (str == "3-R")
	{
		ace::auton::theWholeShebang_Auto();
	}
	else
	{
		printf("ERROR: no skills found %d", str);
	}

}


/* ========================================================================== */
/*                                User Control                                */
/* ========================================================================== */
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

		/* ------------------------------ Toggle Intake ----------------------------- */
		if (master.get_digital_new_press(BUTTON_INTAKE_TOGGLE)) {
			intakeToggleEnabled = !intakeToggleEnabled;
			ace::intakeToggle(intakeToggleEnabled);
		}

		/* ------------------------ Emergency Reverse Intake ------------------------ */
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

		/* -------------------------- Roller Forward Intake ------------------------- */
		if (master.get_digital_new_press(BUTTON_ROLLER_FORWARD)) {
			rollerForwardToggle = true;
			ace::rollerForward(true, ace::SPEED_ROLLER);
		}
		if (rollerForwardToggle && !master.get_digital(BUTTON_ROLLER_FORWARD)) {
			rollerForwardToggle = false;
			ace::rollerForward(false, 0);
		}

		/* -------------------------- Roller Reverse Intake ------------------------- */
		if (master.get_digital_new_press(BUTTON_ROLLER_REVERSE)) {
			rollerReverseToggle = true;
			ace::rollerReverse(true, ace::SPEED_ROLLER);
		}
		if (rollerReverseToggle && !master.get_digital(BUTTON_ROLLER_REVERSE)) {
			rollerReverseToggle = false;
			ace::rollerReverse(false, 0);
		}

		/* ------------------------------ Launch Disks ------------------------------ */
		if (master.get_digital(BUTTON_LAUNCHER_LONG)) {

			// 	Launch Disks from Long Distance
			ace::active_brake(true);
			ace::launch::launchDisks(ace::SPEED_LAUNCHER_LONG, true);
		}
		if (master.get_digital(BUTTON_LAUNCHER)) {

			// 	Launch Disks from Normal Range
			ace::active_brake(true);
			ace::launch::launchDisks(ace::SPEED_LAUNCHER_SHORT);
		}
		if (ace::launch::launcherEnabled && !master.get_digital(BUTTON_LAUNCHER) && !master.get_digital(BUTTON_LAUNCHER_LONG)) {

			//	Disable Launching
			ace::active_brake(false);
			ace::launch::launchDisks(0.0);
		}

		/* ----------------------------- Endgame Toggle ----------------------------- */
		if (master.get_digital(BUTTON_ENDGAME)) {
			endgameToggleEnabled = true;
			ace::endgameToggle(true);
		}
		if (endgameToggleEnabled && !master.get_digital(BUTTON_ENDGAME)) {
			endgameToggleEnabled = false;
			ace::endgameToggle(false);
		}

		/* ---------------------------- Launcher Standby ---------------------------- */
		if (master.get_digital_new_press(BUTTTON_STANDBY)) {
			ace::LAUNCHER_STANDBY_ENABLED = !ace::LAUNCHER_STANDBY_ENABLED;
		}

		/* ---------------------------- Variable Launcher --------------------------- */
		if (master.get_digital_new_press(BUTTON_VAR_LAUNCHER)) {
			ace::var_launcher_enabled = !ace::var_launcher_enabled;
			ace::varLauncherMove();
		}

		pros::delay(ez::util::DELAY_TIME);
	}
}
