#pragma once

#include "cghs.cpp"

/*
 *	Skills Auto
 *	- does skills
 */
void cghs::auton::skills_Auto() {
  // Do Skills
}

//  Three Side Auto
//  - Gets Roller and 3 in High Goal and 2 in Low Goal
void cghs::auton::threeSide_Auto() {
  // Get Roller
  chassis.set_drive_pid(-4, 0.5 * 127.0, false);
  chassis.wait_drive();
  cghs::rollerForward(true, SPEED_ROLLER_AUTO);
  pros::delay(250);
  cghs::rollerForward(false, 0);
  chassis.set_drive_pid(4, 0.5 * 127.0, false);
  chassis.wait_drive();

  // Turn 90 Degrees and get 2 Low Goals
  chassis.set_turn_pid(90, SPEED_TURN_AUTO);
  chassis.wait_drive();
  chassis.set_drive_pid(4, SPEED_DRIVE_AUTO);
  chassis.wait_drive();
  launchDisks_Auto(4000, 60);

  // Turn 45 Degrees to the Right
  chassis.set_turn_pid(45, SPEED_TURN_AUTO);
  chassis.wait_drive();

  // Charge Forward and push disks
  chassis.set_drive_pid(65, SPEED_DRIVE_AUTO, true);
  chassis.wait_until(36);
  cghs::intakeToggle(true);
  chassis.set_max_speed(SPEED_DRIVE_AUTO_INTAKE);
  chassis.wait_drive();
  cghs::intakeToggle(false);

  cghs::intakeReverse(true);
  pros::delay(250);
  cghs::intakeReverse(false);

  cghs::intakeToggle(true);
  chassis.set_turn_pid(-47, SPEED_TURN_AUTO);
  chassis.wait_drive();

  chassis.set_drive_pid(4, SPEED_DRIVE_AUTO, true);
  chassis.wait_drive();
  cghs::intakeToggle(false);

  launchDisksLong_Auto(8000);
}

//  Two Side Auto
//  - Gets Roller and 3 in High Goal and 2 in Low Goal
void cghs::auton::twoSide_Auto() {
  // Fire two low goals
  launchDisks_Auto(4000, 80);

  chassis.set_drive_pid(-12, SPEED_DRIVE_AUTO);
  chassis.wait_drive();
  chassis.set_turn_pid(90, SPEED_TURN_AUTO);
  chassis.wait_drive();
  chassis.set_drive_pid(-12, SPEED_DRIVE_AUTO);
  chassis.wait_drive();

  cghs::rollerForward(true, SPEED_ROLLER_AUTO);
  pros::delay(1000);
  cghs::rollerForward(false, 0);

  chassis.set_drive_pid(4, SPEED_DRIVE_AUTO);
  chassis.wait_drive();

  chassis.set_turn_pid(45, SPEED_TURN_AUTO);
  chassis.wait_drive();

  chassis.set_drive_pid(68, SPEED_DRIVE_AUTO);
  chassis.wait_until(34);
  cghs::intakeToggle(true);
  chassis.set_max_speed(SPEED_DRIVE_AUTO);
  chassis.wait_drive();

  chassis.set_turn_pid(90, SPEED_TURN_AUTO);
  chassis.wait_drive();

  cghs::intakeToggle(false);
  launchDisksLong_Auto(8000);
}

//  The Whole Shebang Autos
//  - Gets Roller and 3 in High Goal and other Roller
void cghs::auton::theWholeShebang_Auto() {
  launchDisks_Auto(2, 80);
}

//  Null Auto
//  - Gets Roller and 3 in High Goal and 2 in Low Goal
void cghs::auton::null_Auto() {
  // Do Nothing
}

/*
 *	Auton Selector
 *
 *
 */
