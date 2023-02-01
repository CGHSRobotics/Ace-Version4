
#include "cghs.h"

namespace cghs {

	// Spin Motor with Percent
	void spinMotor(pros::Motor motor, float percent) {
		motor.move_voltage(12000.0 * (percent / 100.0));
	}

	// Sets Active Break
	bool activeBreakEnabled = false;
	void active_brake(bool enabled, Drive chassis) {
		if (enabled && activeBreakEnabled)
			chassis.set_active_brake(0.1);
		else
			chassis.set_active_brake(0);
	}

	// Reset all Inputs
	void resetInputs() {
		launcherMotor.move_velocity(0);
		rollerMotor.move_velocity(0);
		conveyorMotor.move_velocity(0);
		intakeMotor.move_velocity(0);
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

	// Launch Disks, called once per frame
	void launchDisks(bool enabled, float speed) {
		if (enabled) {
			if (launcherMotor.get_actual_velocity() / 6.0 <= speed - 20.0) {
				spinMotor(launcherMotor, speed);
				spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
				spinMotor(intakeMotor, 0);
				spinMotor(conveyorMotor, 0);
			}
			else {
				spinMotor(launcherMotor, speed);
				spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
				spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
				spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER);
			}
		}
		else {
			spinMotor(launcherMotor, 0);
			spinMotor(rollerMotor, 0);
			spinMotor(intakeMotor, 0);
			spinMotor(conveyorMotor, 0);
		}
	}

	// Launch disks long distance
	float launcherTimerDelay = 0;
	void launchDisksLong(float speed) {
		if (launcherMotor.get_actual_velocity() / 6.0 <= speed - 20.0) {
			spinMotor(launcherMotor, 100);
			spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
			spinMotor(intakeMotor, 0);
			spinMotor(conveyorMotor, 0);
			launcherTimerDelay = 0;
		}
		else {
			spinMotor(launcherMotor, speed);
			spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
			if (launcherTimerDelay >= launcherTimerDelayMax) {
				spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
				spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER_LONG);
			}
			else if (launcherTimerDelay / 1000.0 < launcherTimerDelayMax) {
				launcherTimerDelay += 10.0 / 1000.0;
			}
		}
	}

	// Launch Disks, called once per frame
	void launchDisksShort(float speed) {
		spinMotor(launcherMotor, speed);
		spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
		spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
		spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER);
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