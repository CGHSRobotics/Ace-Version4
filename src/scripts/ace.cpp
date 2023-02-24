
#include "ace.h"

Drive chassis(
	// Left Chassis Ports (negative port will reverse it!)
	{ -ace::DRIVE_LEFT_FRONT_PORT, -ace::DRIVE_LEFT_BACK_PORT },
	// Right Chassis Ports (negative port will reverse it!)
	{ ace::DRIVE_RIGHT_FRONT_PORT, ace::DRIVE_RIGHT_BACK_PORT },
	// IMU Port
	ace::IMU_PORT,
	// Wheel Diameter (Remember, 4" wheels are actually 4.125!)
	3.25,
	// Cartridge RPM
	200,
	// External Gear Ratio (MUST BE DECIMAL)
	0.6);

namespace ace {

	/*
	 *	Global Vars
	 */

	int alliance = 0;
	string operation_mode = "no";

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

		std::string str = ace::autonArray[ace::autonIndex];

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


	/*
	 *	Util Functions
	 */

	 //Check here

	int diskCode = 0;
	int launchCount = 0;
	bool disksFired = false;
	int areaCutoff = 5;

	void diskCheck() {

		pros::vision_object_s_t detectedDisk = visionSensor.get_by_sig(0, diskCode);
		if (detectedDisk.height * detectedDisk.width >= areaCutoff) {
			launchCount = launchCount + 1;
		}
		if (launchCount % 3 == 1) {
			disksFired = true;
		}
		else {
			disksFired = false;
		}
	}


	// Spin Motor with Percent
	void spinMotor(pros::Motor motor, float percent) {
		switch (motor.get_gearing()) {
		case MOTOR_GEARSET_06:
			percent *= 6;
			break;

		case MOTOR_GEARSET_18:
			percent *= 2;
			break;

		case MOTOR_GEARSET_36:
			percent *= 1;
			break;

		default:
			printf("ERROR: GEARING IS NO WORKY");
			return;
		}

		motor.move_velocity(percent);
	}

	// Sets Active Break
	bool activeBreakEnabled = false;
	void active_brake(bool enabled) {
		if (enabled && activeBreakEnabled)
			chassis.set_active_brake(0.1);
		else
			chassis.set_active_brake(0);
	}

	/*
	 *	Unit Conversion
	 */

	 // Convert Celsius to farenheit
	float cel_to_faren(float celsius) {
		return celsius * 1.8 + 32;
	}

	// Convert inch to mm
	float to_mm(float inch) {
		return inch * 25.4;
	}

	// Convert inch to mm
	float to_inch(float mm) {
		return mm / 25.4;
	}

	// Convert rad to degree
	float to_deg(float rad) {
		float deg = rad * (180.0 / 3.141592653589793238463);

		if (deg < 0 && false)
			deg += 360.0;

		return deg;
	}

	/*
	 *	User Control Functions
	 */

	 // Reset all Inputs
	void resetMotors() {
		launcherMotor.move_voltage(0);
		//rollerMotor.move_voltage(0);
		conveyorMotor.move_voltage(0);
		intakeMotor.move_voltage(0);

		endgamePneumatics.set_value(false);
	}

	// Intake Toggle
	void intakeToggle(bool enabled) {
		if (enabled) {
			spinMotor(intakeMotor, SPEED_INTAKE);
			spinMotor(conveyorMotor, SPEED_CONVEYOR_INTAKE);
		}
		else {
			spinMotor(intakeMotor, 0);
			spinMotor(conveyorMotor, 0);
		}
	}

	// Intake Reverse
	void intakeReverse(bool enabled) {
		if (enabled) {
			spinMotor(intakeMotor, -SPEED_INTAKE);
			spinMotor(conveyorMotor, -SPEED_CONVEYOR_INTAKE);
		}
		else {
			spinMotor(intakeMotor, 0);
			spinMotor(conveyorMotor, 0);
		}
	}

	// Roller Forward
	void rollerForward(bool enabled, float speed) {
		if (enabled) {
			//spinMotor(rollerMotor, SPEED_ROLLER);
			spinMotor(conveyorMotor, SPEED_ROLLER);
		}
		else {
			//spinMotor(rollerMotor, 0);
			spinMotor(conveyorMotor, SPEED_ROLLER);
		}
	}

	// Roller Reverse
	void rollerReverse(bool enabled, float speed) {
		if (enabled) {
			//spinMotor(rollerMotor, -SPEED_ROLLER);
		}
		else {
			//spinMotor(rollerMotor, 0);
		}
	}

	// Endgame Toggle
	void endgameToggle(bool enabled) {
		if (enabled) {
			endgamePneumatics.set_value(1);
		}
		else {
			endgamePneumatics.set_value(0);
		}
	}
}  // namespace ace
