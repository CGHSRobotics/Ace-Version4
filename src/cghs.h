#pragma once

#include "main.h"

#ifndef CGHS
#define CGHS
// namespace cghs
namespace cghs {

/*
 *  Controller Stuff
 */

#define BUTTON_INTAKE_TOGGLE DIGITAL_L1
#define BUTTON_INTAKE_REVERSE DIGITAL_L2
#define BUTTON_LAUNCHER DIGITAL_R1

/*
 *  Motor Ports
 */
const int DRIVE_LEFT_FRONT_PORT = 11;
const int DRIVE_LEFT_BACK_PORT = 12;
const int DRIVE_RIGHT_FRONT_PORT = 1;
const int DRIVE_RIGHT_BACK_PORT = 20;
const int LAUNCHER_PORT = 17;
const int ROLLER_PORT = 10;
const int CONVEYOR_PORT = 15;
const int INTAKE_PORT = 16;

const int IMU_PORT = 2;

/*
 *  Physical Constants
 */

#define DRIVE_GEAR_RATIO MOTOR_GEARSET_18
#define LAUNCHER_GEAR_RATIO MOTOR_GEARSET_06
#define ROLLER_GEAR_RATIO MOTOR_GEARSET_18
#define CONVEYOR_GEAR_RATIO MOTOR_GEARSET_06
#define INTAKE_GEAR_RATIO MOTOR_GEARSET_18

/*
 *  Speed Constants
 */

// Intake Motor
const float SPEED_INTAKE = 100;
const float SPEED_INTAKE_LAUNCHER = 100;

// Conveyor Motor
const float SPEED_CONVEYOR_INTAKE = 100;
const float SPEED_CONVEYOR_LAUNCHER = 100;

// Roller Motor
const float SPEED_ROLLER = 20;
const float SPEED_ROLLER_LAUNCHER = 100;

// Launcher Motor
const float SPEED_LAUNCHER = 100;
const float LAUNCHER_MIN_SPEED = 0.8;

/*
 *  Device Declarations
 */

// Motors
const pros::Motor launcherMotor(MOTOR_LAUNCHER_PORT, LAUNCHER_GEAR_RATIO, true);
const pros::Motor rollerMotor(MOTOR_ROLLER_PORT, ROLLER_GEAR_RATIO, false);
const pros::Motor conveyorMotor(MOTOR_CONVEYOR_PORT, CONVEYOR_GEAR_RATIO, false);
const pros::Motor intakeMotor(MOTOR_INTAKE_PORT, INTAKE_GEAR_RATIO, false);

/*
 *  Function Declarations
 */

extern void spinMotor(pros::Motor motor, float percent);

// User Control
extern void resetInputs();

extern void intakeToggle(bool enabled);
extern void intakeReverse(bool enabled);

extern void launchDisks(bool enabled);

/*
 *  Function Definitions
 */

}  // namespace cghs

#endif