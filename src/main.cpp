#include "main.h"

#include "cghs.h"

lv_obj_t* img_var;
lv_obj_t* autonDDList;

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


static lv_res_t ddlist_action(lv_obj_t* ddlist)
{
	cghs::auton::autonIndex = lv_ddlist_get_selected(ddlist);

	cghs::auton::updateAutonSelection();

	return LV_RES_OK; 	//	Return OK if the drop down list is not deleted
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

	pros::delay(500);  // Stop the user from doing anything while legacy ports configure.

	chassis.toggle_modify_curve_with_controller(false);
	chassis.set_curve_default(0, 0);

	// Initialize chassis and auton selector
	chassis.initialize();
	//ez::as::initialize();

	// Screen
	pros::lcd::shutdown();

	// Screen Image
	lv_fs_drv_t pcfs_drv;                      //A driver descriptor
	memset(&pcfs_drv, 0, sizeof(lv_fs_drv_t)); //Initialization

	pcfs_drv.file_size = sizeof(pc_file_t); //Set up fields...
	pcfs_drv.letter = 'S';
	pcfs_drv.open = pcfs_open;
	pcfs_drv.close = pcfs_close;
	pcfs_drv.read = pcfs_read;
	pcfs_drv.seek = pcfs_seek;
	pcfs_drv.tell = pcfs_tell;
	lv_fs_add_drv(&pcfs_drv);

	img_var = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(img_var, "S:/usd/ace.bin");
	lv_obj_set_pos(img_var, 0, 0);  					// set the position to center

	//	Create a style
	static lv_style_t style_bg;
	lv_style_copy(&style_bg, &lv_style_plain);
	style_bg.text.color = LV_COLOR_RED;
	style_bg.body.main_color = LV_COLOR_BLACK;
	style_bg.body.grad_color = LV_COLOR_BLACK;
	style_bg.text.font = &lv_font_dejavu_20;

	//	Create a drop down list
	lv_obj_t* autonDDList = lv_ddlist_create(lv_scr_act(), NULL);
	lv_ddlist_set_options(autonDDList,
		"Skills\n"
		"Null\n"
		"Three\n"
		"Two\n"
		"Shebang\n"
	);
	lv_obj_align(autonDDList, NULL, LV_ALIGN_IN_TOP_RIGHT, 10, 100);
	lv_obj_set_style(autonDDList, &style_bg);
	lv_ddlist_set_anim_time(autonDDList, 0);
	lv_obj_set_free_num(autonDDList, 2);				//	Set a unique ID
	lv_ddlist_set_draw_arrow(autonDDList, true);		//	Turn On Arrow
	lv_ddlist_set_action(autonDDList, ddlist_action);  	//	Set a function to call when anew option is chosen

	lv_ddlist_set_selected(autonDDList, 0);
	ddlist_action(autonDDList);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
	while (true) {
		cghs::auton::checkAutonButtons();

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

	// This is preference to what you like to drive on.
	chassis.set_drive_brake(MOTOR_BRAKE_COAST);

	bool intakeToggleEnabled = false;
	bool intakeReverseEnabled = false;

	bool rollerForwardToggle = false;
	bool rollerReverseToggle = false;

	bool launchingDisksEnabled = false;

	bool endgameToggleEnabled = false;

	while (true) {
		cghs::auton::checkAutonButtons();

		chassis.tank();  // Tank control

		// active break Intake
		if (master.get_digital_new_press(BUTTON_A_BRAKE_TOGGLE)) {
			cghs::activeBreakEnabled = !cghs::activeBreakEnabled;
			master.set_text(2, 12, "true");
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
		if (master.get_digital(BUTTON_LAUNCHER)) {
			launchingDisksEnabled = true;
			cghs::launchDisks(true, cghs::SPEED_LAUNCHER);
		}
		if (master.get_digital(BUTTON_LAUNCHER_LONG)) {
			//chassis.set_active_brake(0.1);
			launchingDisksEnabled = true;
			cghs::launchDisksLong(cghs::SPEED_LAUNCHER_LONG);
		}
		if (launchingDisksEnabled && !master.get_digital(BUTTON_LAUNCHER) && !master.get_digital(BUTTON_LAUNCHER_LONG)) {
			chassis.set_active_brake(0);
			launchingDisksEnabled = false;
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
