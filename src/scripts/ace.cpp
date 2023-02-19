
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

	/*
	 *	Util Functions
	 */

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
	void active_brake(bool enabled, Drive& chassis) {
		if (enabled && activeBreakEnabled)
			chassis.set_active_brake(0.1);
		else
			chassis.set_active_brake(0);
	}

	u_int64_t launcherTime = 0;
	// Record Launcher Speeds to sd card
	void recordLauncherStatistics() {
		// Leave if no sd card
		if (!ez::util::IS_SD_CARD)
			return;

		// leave if dont want to write to sd card
		if (!ace::LAUNCHER_LOGGING)
			return;

		launcherTime += ez::util::DELAY_TIME;

		if (launcherEnabled) {
			FILE* launcherFile;
			launcherFile = fopen("/usd/launcher.txt", "a");
			fprintf(launcherFile, (", \n{'msec': " + std::to_string(launcherTime)).c_str());
			fprintf(launcherFile, (", 'rpm': " + std::to_string(launcherMotor.get_actual_velocity() * 6.0) + "}").c_str());
			fclose(launcherFile);
		}
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
		return rad * (180.0 / 3.141592653589793238463);
	}

	/*
	 *	User Control Functions
	 */

	 // Reset all Inputs
	void resetMotors() {
		launcherMotor.move_velocity(0);
		rollerMotor.move_velocity(0);
		conveyorMotor.move_velocity(0);
		intakeMotor.move_velocity(0);

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

	// Current launcher timer
	float launcherTimerDelay = 0;
	// launcherEnabledBool
	bool launcherEnabled = false;

	// Launch Disks, called once per frame
	void launchDisks(bool enabled, float speed, bool isLongDist, bool standby) {
		// If want to run launcher
		if (enabled) {
			recordLauncherStatistics();

			launcherEnabled = true;

			// if less than cutoff
			if (launcherMotor.get_actual_velocity() / 6.0 <= speed - LAUNCHER_MIN_SPEED) {
				// Set velocity to max
				launcherMotor.move_velocity(600);
				spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
				spinMotor(intakeMotor, 0);
				spinMotor(conveyorMotor, 0);
				launcherTimerDelay = 0;
				return;
			}

			// if long distance
			if (isLongDist) {
				// Speed up Launcher and Roller but nothing else
				launcherMotor.move_velocity(speed * 6);
				spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);

				if (!(launcherTimerDelay > launcherTimerDelayMax)) {
					launcherTimerDelay += 10.0;
					return;
				}

				launcherTimerDelay -= launcherTimerDelayMax;
			}

			// if is regular distance
			if (!isLongDist) {
				// Set pid to set velocity
				spinMotor(launcherMotor, SPEED_LAUNCHER_LONG);
			}

			spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
			spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
			spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER);

		}
		else {
			launcherEnabled = false;

			spinMotor(rollerMotor, 0);
			spinMotor(intakeMotor, 0);
			spinMotor(conveyorMotor, 0);

			if (standby)
				spinMotor(launcherMotor, SPEED_LAUNCHER);
			else
				launcherMotor.move_voltage(0);
		}
	}

	// Roller Forward
	void rollerForward(bool enabled, float speed) {
		if (enabled) {
			spinMotor(rollerMotor, SPEED_ROLLER);
		}
		else {
			spinMotor(rollerMotor, 0);
		}
	}

	// Roller Reverse
	void rollerReverse(bool enabled, float speed) {
		if (enabled) {
			spinMotor(rollerMotor, -SPEED_ROLLER);
		}
		else {
			spinMotor(rollerMotor, 0);
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

/**
 *
 *	Drive / Turn with GPS Namespace
 *
 */
namespace ace::gps {

	float curr_turnSpeed = 0;
	float curr_turnAngle = 0;

	float imu_start_angle = 0;

	// init tasks
	void init() {
		pros::Task task_turn_gps(__task_gps_factcheck_angle, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Turning GPS");

		imu_start_angle = gpsSensor.get_heading();
	}

	// Set Turn with GPS
	void __task_gps_factcheck_angle() {
		while (true) {
			// Delay 10 ms
			pros::delay(100);

			// dont do anything if not in autonomous
			if (operation_mode == "auto") {

				if (gpsSensor.get_error() < err_gps_max) {
					float diff = gpsSensor.get_heading() - chassis.imu.get_heading();

					// only fix if error is (+-) 1 degree
					if (std::abs(diff) > err_degree_max) {
						chassis.imu.set_heading(gpsSensor.get_heading());
					}
				}
			}
		}
	}

	// Set Turn
	void set_turn(float angle, float speed, bool waitUntilFinished) {
		curr_turnAngle = angle;
		curr_turnSpeed = speed;

		chassis.set_turn_pid(angle, speed);

		if (waitUntilFinished)
		{
			chassis.wait_drive();
		}
	}

	//	Set Waypoint pos to go to
	void set_waypoint(float x, float y) {
		// 	GPS has too much error, either no tape or no view
		if (gpsSensor.get_error() > err_gps_max)
			return;

		// 	get all current data from GPS
		pros::c::gps_status_s_t status = gpsSensor.get_status();

		// Convert meters to inch
		float gps_x = to_inch(status.x * 1000.0);
		float gps_y = to_inch(status.y * 1000.0);

		float distX = x - gps_x;	// in inch
		float distY = y - gps_y;

		// find angle, magnitude of vector
		float mag = sqrtf(distX * distX + distY * distY);	// in inch
		float theta = to_deg(atan2f(distY, -distX));

		//	if already at position, return
		if (std::abs(distX) <= err_pos_max || std::abs(distY) <= err_pos_max)
			return;

		// Turn to angle
		chassis.set_turn_pid(theta, curr_turnSpeed);
		chassis.wait_drive();

		// Drive to position
		chassis.set_drive_pid(mag, SPEED_DRIVE_AUTO);
		chassis.wait_drive();

		// Turn back to where is supposed to be
		chassis.set_turn_pid(curr_turnAngle, curr_turnSpeed);
		chassis.wait_drive();

		// Recursively call this function again, will either fix further error, or break from that if successful
		set_waypoint(x, y);
	}

}  // namespace ace::gps