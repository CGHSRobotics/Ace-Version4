

#include "cghs.h"

// Spin Motor with Percent
void cghs::spinMotor(pros::Motor motor, float percent) {
  switch (motor.get_gearing()) {
    case MOTOR_GEARSET_06:
      motor.move_velocity(600 * percent);
      break;

    case MOTOR_GEARSET_18:
      motor.move_velocity(200 * percent);
      break;

    case MOTOR_GEARSET_36:
      motor.move_velocity(100 * percent);
      break;

    default:
      break;
  }
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
  } else {
    spinMotor(cghs::intakeMotor, 0);
    spinMotor(cghs::conveyorMotor, 0);
  }
}

// Intake Reverse
void cghs::intakeReverse(bool enabled) {
  if (enabled) {
    spinMotor(cghs::intakeMotor, -SPEED_INTAKE);
    spinMotor(cghs::conveyorMotor, -SPEED_CONVEYOR_INTAKE);
  } else {
    spinMotor(cghs::intakeMotor, 0);
    spinMotor(cghs::conveyorMotor, 0);
  }
}

// Launch Disks
void cghs::launchDisks(bool enabled) {
  if (enabled) {

    if (cghs::launcherMotor.get_actual_velocity() / 6.0 <= LAUNCHER_MIN_SPEED * SPEED_LAUNCHER) {
      spinMotor(launcherMotor, SPEED_LAUNCHER);
      conveyorMotor.move_voltage(0);
      rollerMotor.move_voltage(0);
      intakeMotor.move_voltage(0);
    } else {
      launcherMotor.move_voltage((12000 / 100) * SPEED_LAUNCHER);
      spinMotor(conveyorMotor, SPEED_CONVEYOR_LAUNCHER);
      spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
      spinMotor(rollerMotor, SPEED_ROLLER_LAUNCHER);
    }
  } else {

    launcherMotor.move_voltage(0.0);
    conveyorMotor.move_voltage(0);
    rollerMotor.move_voltage(0);
    intakeMotor.move_voltage(0);
  }
}

// Roller Forward
void cghs::rollerForward(bool enabled) {
  if (enabled) {
    spinMotor(cghs::rollerMotor, SPEED_ROLLER);
  } else {
    spinMotor(cghs::rollerMotor, 0);
  }
}

// Roller Reverse
void cghs::rollerReverse(bool enabled) {
  if (enabled) {
    spinMotor(cghs::rollerMotor, -SPEED_ROLLER);
  } else {
    spinMotor(cghs::rollerMotor, 0);
  }
}

// Endgame Toggle
void cghs::endgameToggle(bool enabled) {
  if (enabled) {
    cghs::endgamePneumatics.set_value(1);
  } else {
    cghs::endgamePneumatics.set_value(0);
  }
}