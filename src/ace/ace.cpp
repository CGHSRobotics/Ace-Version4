
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
		if (ez::util::IS_SD_CARD) {
			// leave if dont want to write to sd card
			if (ace::LAUNCHER_LOGGING) {
				launcherTime += ez::util::DELAY_TIME;
				if (launcherEnabled) {
					l_data_point curr_point;
					curr_point.msec = launcherTime;
					curr_point.rpm = launcherMotor.get_actual_velocity() * 6;
					curr_point.set_rpm = launcherMotor.get_target_velocity() * 6;
					l_data_array.push_back(curr_point);


					l_data_point point_2fr_ago = l_data_array[l_data_array.size() - 2];

					// if data point to frames ago was actually 20msec ago
					if ((int)curr_point.msec - 20 == (int)point_2fr_ago.msec) {
						// if launcher is enabled
						if (launcherEnabled) {
							// if rpm dropped by about 10% in two frames while rpm is greater than 40
							if (curr_point.rpm < point_2fr_ago.rpm * 0.95 && point_2fr_ago.rpm > 40.0 * 6.0) {
								printf("LAUNCH DETECTED");
								master.rumble(".");
							}
						}
					}
				}
			}
		}
	}

	void saveLauncherData() {

		remove(launcherFile_path);

		FILE* launcherFile;
		launcherFile = fopen(launcherFile_path, "a");

		fprintf(launcherFile, "\n/start data");
		for (size_t i = 0; i < l_data_array.size(); i++)
		{
			fprintf(launcherFile, "\n{");
			fprintf(launcherFile, ("'msec': " + std::to_string(l_data_array[i].msec)).c_str());
			fprintf(launcherFile, (", 'rpm': " + std::to_string(l_data_array[i].rpm)).c_str());
			fprintf(launcherFile, (", 'set_rpm': " + std::to_string(l_data_array[i].set_rpm)).c_str());
			fprintf(launcherFile, "}");

			if (i != l_data_array.size() - 1)
				fprintf(launcherFile, ",");

		}

		fprintf(launcherFile, "\n/end data");

		fclose(launcherFile);
	}

	void uploadLauncherData() {
		FILE* launcherFile;
		launcherFile = fopen(launcherFile_path, "r");

		int buff_size = 64;
		char buff[buff_size];

		float line_counter = 0;
		float lines_per_msec = 5;

		while (true)
		{
			line_counter += 1.0 / lines_per_msec;

			if (line_counter >= 1)
			{
				pros::delay(1);
				line_counter -= 1.0;
				continue;
			}

			if (!fgets(buff, buff_size, launcherFile))
			{
				break;
			}

			printf(buff);
		}

		fclose(launcherFile);
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
				//spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
				spinMotor(intakeMotor, 0);
				spinMotor(conveyorMotor, 0);
				launcherTimerDelay = 0;
				return;
			}
			//Check here
			/*
			int diskCode = 0
			int launchCount = 0
			bool disksFired = false
			void diskCheck(){
				visionObject rtn = visionSensor.get_by_code(0, diskCode);
				if visionObject = (0, diskCode) {
					launchCount = launchCount + 1;
				}
				if launchCount % 3 = (1) {
					disksFired = true

				} else {
					disksFired = false

				}

			}
			*/
			// if long distance
			if (isLongDist) {
				// Speed up Launcher and Roller but nothing else
				launcherMotor.move_velocity(speed * 6);
				//spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);

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

			//spinMotor(rollerMotor, -SPEED_ROLLER_LAUNCHER);
			spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
			spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER);

		}
		else {
			launcherEnabled = false;

			//spinMotor(rollerMotor, 0);
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

/**
 *
 *	Drive / Turn with GPS Namespace
 *
 */
namespace ace::gps {

	float curr_turnSpeed = SPEED_TURN_AUTO;
	float curr_turnAngle = 0;

	// init tasks
	void init() {
		pros::Task task_turn_gps(__task_gps_factcheck_angle, TASK_PRIORITY_DEFAULT - 1, TASK_STACK_DEPTH_DEFAULT, "Turning GPS");
	}

	// Set Turn with GPS
	void __task_gps_factcheck_angle() {
		while (true) {

			// dont do anything if not in autonomous
			if (true) {

				if (gpsSensor.get_error() < err_gps_max) {
					chassis.imu.set_heading(gpsSensor.get_heading());
				}
			}

			// Delay 10 ms
			pros::delay(50);
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
		float gps_x = to_inch(-status.y * 1000.0);
		float gps_y = to_inch(status.x * 1000.0);
		// Find Distance to target
		float distX = x - gps_x;	// in inch
		float distY = y - gps_y;
		// find angle, magnitude of vector
		float mag = sqrtf(distX * distX + distY * distY);	// in inch
		float theta = to_deg(atan2f(distY, -distX));

		//	if already at position, return
		if (std::abs(mag) <= err_pos_max)
			return;

		// Turn to angle
		chassis.set_turn_pid(theta, curr_turnSpeed);
		chassis.wait_drive();

		// Drive to position
		chassis.set_drive_pid(mag, SPEED_DRIVE_AUTO);

		while (true) {

			// 	GPS has too much error, either no tape or no view
			if (gpsSensor.get_error() > err_gps_max)
				break;

			// 	get all current data from GPS
			pros::c::gps_status_s_t status = gpsSensor.get_status();

			// Convert meters to inch
			float new_gps_x = to_inch(-status.y * 1000.0);
			float new_gps_y = to_inch(status.x * 1000.0);
			// Find Distance to target
			float new_distX = x - new_gps_x;	// in inch
			float new_distY = y - new_gps_y;
			// find angle, magnitude of vector
			float new_mag = sqrtf(new_distX * new_distX + new_distY * new_distY);	// in inch
			float new_theta = to_deg(atan2f(new_distY, -new_distX));

			//	if already at position, return
			if (std::abs(mag) <= err_pos_max)
				break;

			if (std::abs(theta) - std::abs(new_theta) > 18) {
				// Turn to angle
				chassis.set_turn_pid(theta + 18 * (new_theta / std::abs(new_theta)), curr_turnSpeed);
			}
			else {
				// Turn to angle
				chassis.set_turn_pid(new_theta, curr_turnSpeed);
			}

			chassis.set_drive_pid(mag, SPEED_DRIVE_AUTO);

			pros::delay(50);
		}
	}

	/*void set_waypoint(float x, float y) {
		// 	GPS has too much error, either no tape or no view
		if (gpsSensor.get_error() > err_gps_max)
			return;

		// 	get all current data from GPS
		pros::c::gps_status_s_t status = gpsSensor.get_status();

		// Convert meters to inch
		float gps_x = to_inch(-status.y * 1000.0);
		float gps_y = to_inch(status.x * 1000.0);

		float distX = x - gps_x;	// in inch
		float distY = y - gps_y;

		// find angle, magnitude of vector
		float mag = sqrtf(distX * distX + distY * distY);	// in inch
		float theta = to_deg(atan2f(distY, -distX));

		//	if already at position, return
		if (std::abs(mag) <= err_pos_max)
			return;

		printf(("\nCurr X " + std::to_string(gps_x)).c_str());
		printf(("\nCurr Y " + std::to_string(gps_y)).c_str());
		printf(("\nTurning to " + std::to_string(theta)).c_str());
		printf(("\nMove X " + std::to_string(distX)).c_str());
		printf(("\nMove Y " + std::to_string(distY)).c_str());
		printf("\n\n");

		bool slew_toggle = false;
		if (mag > 14)
			slew_toggle = true;

		// Turn to angle
		chassis.set_turn_pid(theta, curr_turnSpeed);
		chassis.wait_drive();

		// Drive to position
		chassis.set_drive_pid(mag, SPEED_DRIVE_AUTO);
		chassis.wait_until(mag - 12);
		chassis.set_max_speed(127.0 * 0.3);
		chassis.wait_drive();

		// Recursively call this function again, will either fix further error, or break from that if successful
		set_waypoint(x, y);
	}*/

}  // namespace ace::gps