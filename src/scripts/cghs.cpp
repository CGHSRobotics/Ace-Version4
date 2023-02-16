
#include "cghs.h"

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

namespace cghs {

	int alliance = 0;
	string operation_mode = "no";

	// Spin Motor with Percent
	void spinMotor(pros::Motor motor, float percent) {

		switch (motor.get_gearing())
		{
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


	// Convert Celsius to farenheit
	float cel_to_faren(float celsius) {
		return celsius * 1.8 + 32;
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
		if (!cghs::LAUNCHER_LOGGING)
			return;

		launcherTime += ez::util::DELAY_TIME;

		if (launcherEnabled) {

			FILE* launcherFile;
			launcherFile = fopen("/usd/launcher.txt", "a");
			fprintf(launcherFile, ", \n{'msec': %i, 'rpm': %i}", launcherMotor.get_actual_velocity(), launcherTime);
			fclose(launcherFile);
		}

	}

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
			if (isLongDist)
			{

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

		recordLauncherStatistics();
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
}

/**
 *
 *	Drive / Turn with GPS Namespace
 *
 */
namespace cghs::gps {

	float curr_turnSpeed = 0;
	float curr_turnAngle = 0;
	float curr_turnAngleDiff = 0;

	// init tasks
	void init() {
		pros::Task task_turn_gps(__task_set_turn_gps, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Turning GPS");
	}

	// Set Turn with GPS
	void set_turn_gps(float angle, float speed) {
		curr_turnAngle = angle;
		curr_turnSpeed = speed;
	}
	void __task_set_turn_gps() {

		while (true)
		{
			// Delay 10 ms
			pros::delay(ez::util::DELAY_TIME);

			// dont do anything if not in autonomous
			if (operation_mode != "auto")
				continue;

			float gps_angle = gpsSensor.get_heading();

			if (std::abs(gps_angle - curr_turnAngle) < 1)
				continue;

			curr_turnAngleDiff = gps_angle - chassis.get_gyro();

			chassis.set_turn_pid(curr_turnAngle + curr_turnAngleDiff, curr_turnSpeed);
		}
	}
}