
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

	/* ========================================================================== */
	/*                                 Global Vars                                */
	/* ========================================================================== */

	int alliance = 0;
	string operation_mode = "no";

	// bool whether standby enabled or not
	bool LAUNCHER_STANDBY_ENABLED = false;

	bool var_launcher_enabled = false;


	/* ========================================================================== */
	/*                              Utility Functions                             */
	/* ========================================================================== */


	int launchCount = 0;
	int areaCutoff = 1000;
	const int betweenDiskTimer_max = 5;
	int betweenDiskTimer = betweenDiskTimer_max;
	bool diskSeen = false;
	/* ------------------- Checks For Disk Using Vision Sensor ------------------ */
	void diskCheck() {

		if (betweenDiskTimer < betweenDiskTimer_max) {
			betweenDiskTimer++;
			return;
		}

		pros::vision_object_s_t detectedDisk = visionSensor.get_by_sig(0, diskCode);

		if (detectedDisk.height * detectedDisk.width >= areaCutoff) {
			diskSeen = true;
		}
		else if (diskSeen) {
			diskSeen = false;
			launchCount = launchCount + 1;
			master.rumble("-");
			betweenDiskTimer = 0;
		}
	}


	/* ------------------------- Spin Motor With Percent ------------------------ */
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

	/* ---------------------------- Sets Active Break --------------------------- */
	bool activeBreakEnabled = false;
	void active_brake(bool enabled) {
		if (enabled && activeBreakEnabled)
			chassis.set_active_brake(0.1);
		else
			chassis.set_active_brake(0);
	}


	/* ========================================================================== */
	/*                               Unit Conversion                              */
	/* ========================================================================== */

	/* ---------------------- Convert Celsius To Farenheit ---------------------- */
	float cel_to_faren(float celsius) {
		return celsius * 1.8 + 32;
	}

	/* --------------------------- Convert Inch To Mm --------------------------- */
	float to_mm(float inch) {
		return inch * 25.4;
	}

	/* --------------------------- Convert Inch To Mm --------------------------- */
	float to_inch(float mm) {
		return mm / 25.4;
	}

	/* -------------------------- Convert Rad To Degree ------------------------- */
	float to_deg(float rad) {
		float deg = rad * (180.0 / 3.141592653589793238463);

		if (deg < 0 && false)
			deg += 360.0;

		return deg;
	}


	/* ========================================================================== */
	/*                           User Control Functions                           */
	/* ========================================================================== */

	/* ---------------------------- Reset All Inputs ---------------------------- */
	void resetMotors() {
		launcherMotor.move_voltage(0);
		conveyorMotor.move_voltage(0);
		intakeMotor.move_voltage(0);

		LAUNCHER_STANDBY_ENABLED = false;

		endgamePneumatics.set_value(false);
	}

	/* ------------------------------ Intake Toggle ----------------------------- */
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

	/* ----------------------------- Intake Reverse ----------------------------- */
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

	/* ----------------------------- Roller Forward ----------------------------- */
	void rollerForward(bool enabled, float speed) {
		if (enabled)
			spinMotor(intakeMotor, SPEED_ROLLER);
		else
			spinMotor(intakeMotor, 0);

	}

	/* ----------------------------- Roller Reverse ----------------------------- */
	void rollerReverse(bool enabled, float speed) {
		if (enabled)
			spinMotor(intakeMotor, -SPEED_ROLLER);
		else
			spinMotor(intakeMotor, 0);
	}

	/* ----------------------------- Endgame Toggle ----------------------------- */
	void endgameToggle(bool enabled) {
		if (enabled) {
			endgamePneumatics.set_value(1);
		}
		else {
			endgamePneumatics.set_value(0);
		}
	}

	/* ---------------------------- Variable Launcher --------------------------- */
	void varLauncherMove() {
		if (var_launcher_enabled) {
			varLauncherMotor.move_absolute(VAR_LAUNCH_ANGLE_UP, 100);
		}
		else {
			varLauncherMotor.move_absolute(VAR_LAUNCH_ANGLE_DOWN, -100);
		}
	}

	void set_var_launcher(bool enabled) {
		var_launcher_enabled = enabled;
		varLauncherMove();
	}


}  // namespace ace
