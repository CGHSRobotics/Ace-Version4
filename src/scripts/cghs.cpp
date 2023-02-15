
#include "cghs.h"

namespace cghs {

	int alliance = 0;

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
			launcherFile = fopen("usd/launcher.txt", "a");
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
	void launchDisks(bool enabled, float speed, bool isLongDist) {

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
				launcherMotor.move_velocity(speed * 6);
			}

			spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
			spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
			spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER);

		}
		else {

			launcherEnabled = false;

			spinMotor(launcherMotor, 0);
			spinMotor(rollerMotor, 0);
			spinMotor(intakeMotor, 0);
			spinMotor(conveyorMotor, 0);
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