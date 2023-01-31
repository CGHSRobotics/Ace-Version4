

#include "cghs.h"

// Spin Motor with Percent
void cghs::spinMotor(pros::Motor motor, float percent) {
	motor.move_voltage(12000.0 * (percent / 100.0));
}

// Sets Active Break
void cghs::active_brake(bool enabled, Drive chassis) {
	if (enabled && cghs::activeBreakEnabled)
		chassis.set_active_brake(0.1);
	else
		chassis.set_active_brake(0);
}

// Reset all Inputs
void cghs::resetInputs() {
	cghs::launcherMotor.move_velocity(0);
	cghs::rollerMotor.move_velocity(0);
	cghs::conveyorMotor.move_velocity(0);
	cghs::intakeMotor.move_velocity(0);
}

// Intake Toggle
void cghs::intakeToggle(bool enabled) {
	if (enabled) {
		spinMotor(cghs::intakeMotor, SPEED_INTAKE);
		spinMotor(cghs::conveyorMotor, SPEED_CONVEYOR_INTAKE);
	}
	else {
		spinMotor(cghs::intakeMotor, 0);
		spinMotor(cghs::conveyorMotor, 0);
	}
}

// Intake Reverse
void cghs::intakeReverse(bool enabled) {
	if (enabled) {
		spinMotor(cghs::intakeMotor, -SPEED_INTAKE);
		spinMotor(cghs::conveyorMotor, -SPEED_CONVEYOR_INTAKE);
	}
	else {
		spinMotor(cghs::intakeMotor, 0);
		spinMotor(cghs::conveyorMotor, 0);
	}
}

// Launch Disks
float cghs::launcherTimerDelay = 0;
void cghs::launchDisksLong(float speed) {
	if (cghs::launcherMotor.get_actual_velocity() / 6.0 <= LAUNCHER_MIN_SPEED * speed) {
		spinMotor(launcherMotor, 100);
		spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
		spinMotor(intakeMotor, 0);
		spinMotor(conveyorMotor, 0);
		cghs::launcherTimerDelay = 0;
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
void cghs::launchDisks(bool enabled, float speed) {
	if (enabled) {
		if (cghs::launcherMotor.get_actual_velocity() / 6.0 <= LAUNCHER_MIN_SPEED * speed) {
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

// Roller Forward
void cghs::rollerForward(bool enabled, float speed) {
	if (enabled) {
		spinMotor(cghs::rollerMotor, SPEED_ROLLER);
	}
	else {
		spinMotor(cghs::rollerMotor, 0);
	}
}

// Roller Reverse
void cghs::rollerReverse(bool enabled, float speed) {
	if (enabled) {
		spinMotor(cghs::rollerMotor, -SPEED_ROLLER);
	}
	else {
		spinMotor(cghs::rollerMotor, 0);
	}
}

// Endgame Toggle
void cghs::endgameToggle(bool enabled) {
	if (enabled) {
		cghs::endgamePneumatics.set_value(1);
	}
	else {
		cghs::endgamePneumatics.set_value(0);
	}
}

/*
 *   Autonomous
 */

void cghs::auton::launchDisks_Auto(float time, float speed) {
	float currTime = 0;
	while (currTime < time) {
		launchDisks(true, speed);

		currTime += ez::util::DELAY_TIME;
		pros::delay(ez::util::DELAY_TIME);
	}

	launchDisks(false, 0);
}

void cghs::auton::launchDisksLong_Auto(float time) {
	float currTime = 0;
	while (currTime < time) {
		launchDisksLong(cghs::SPEED_LAUNCHER_LONG);

		currTime += ez::util::DELAY_TIME;
		pros::delay(ez::util::DELAY_TIME);
	}

	launchDisks(false, 0);
}
