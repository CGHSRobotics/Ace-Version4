
#include "ace.h"


/* ========================================================================== */
/*                        Miscellaneous Auton Functions                       */
/* ========================================================================== */
namespace ace::auton {

	/* ========================================================================== */
	/*                              Global Variables                              */
	/* ========================================================================== */


	int autonIndex = 0;

	float target_angle = 0;
	vec2 target_pos(0, 0);

	float curr_turnSpeed = 0;
	float curr_driveSpeed = 0;

	/* ========================================================================== */
	/*                            Function Definitions                            */
	/* ========================================================================== */


	/* -------------------------------- Set Turn -------------------------------- */
	void set_turn(float angle, float speed, bool waitUntilFinished) {

		// Update target values
		target_angle = angle;
		curr_turnSpeed = speed;

		// get corrective angle from gps
		if (gps::ENABLE_GPS)
			angle = gps::get_turn_angle(angle);

		// set chassis pid
		chassis.set_turn_pid(angle, speed);

		// wait drive if desired
		if (waitUntilFinished)
			chassis.wait_drive();
	}

	/* -------------------------------- Set Move -------------------------------- */
	void set_drive(float distance, float speed, bool waitUntilFinished) {

		// find new constant values
		float x = distance * -cos(target_angle);
		float y = distance * sin(target_angle);
		target_pos.x += x;
		target_pos.y += y;
		curr_driveSpeed = speed;

		vec2 nums(0, 0);

		if (gps::ENABLE_GPS)
		{
			//nums = gps::get_drive_nums();
		}
		else if (!gps::ENABLE_GPS || (nums.x == 0 && nums.y == 0))
		{
			// set drive pid
			chassis.set_drive_pid(distance, speed);
		}

		// wait drive if desired
		if (waitUntilFinished)
			chassis.wait_drive();
	}

	/* -------------- Updates Auton Selection And Prints To Screen -------------- */
	void updateAutonSelection() {
		if (autonIndex < 0)
		{
			autonIndex += numAutons;
		}
		if (autonIndex >= numAutons)
		{
			autonIndex -= numAutons;
		}

		std::string str = autonArray[autonIndex];

		printf("\n Auton Changed To: %s", str.c_str());
	}

	/* -------------------- Adjust Auton Based Off Of Buttons ------------------- */
	void checkAutonButtons() {

		if (master.get_digital_new_press(BUTTON_AUTON_INCREASE))
		{
			autonIndex++;
			updateAutonSelection();
		}
		if (master.get_digital_new_press(BUTTON_AUTON_DECREASE))
		{
			autonIndex--;
			updateAutonSelection();
		}
	}
}


/* ========================================================================== */
/*                       Drive / Turn With GPS Namespace                      */
/* ========================================================================== */
namespace ace::gps {

	pros::c::gps_status_s_t currentState;
	bool error_is_okay = false;

	float startAngle = 0;


	/* ------------------------------- Init Tasks ------------------------------- */
	void init(float startAngle) {

		pros::Task __task_gps(__task_gps_factcheck, TASK_PRIORITY_DEFAULT - 1, TASK_STACK_DEPTH_DEFAULT, "GPS");

		startAngle = startAngle;
	}

	/* -------------------------------- GPS Task -------------------------------- */
	void __task_gps_factcheck() {

		while (true) {

			// Delay 10 ms
			pros::delay(20);

			if (!ENABLE_GPS)
				continue;

			error_is_okay = gpsSensor.get_error() < err_gps_max;

			if (error_is_okay) {
				currentStatus = gpsSensor.get_status();

				// Convert meters to inch
				currentStatus.x = to_inch(currentStatus.x / 1000.0);
				currentStatus.y = to_inch(currentStatus.y / 1000.0);
			}


		}
	}

	/* ----------------- Get New Turn Angle Based On Adjustments ---------------- */
	float get_turn_angle(float angle) {

		// if too much error
		if (!error_is_okay)
			return angle;

		float diff = (currentStatus.yaw - startAngle) - chassis.imu.get_rotation();

		if (diff > err_degree_max)
			return angle + diff;

		return angle;
	}

	/* --------------------- Get Drive Nums; Angle, Mag Etc --------------------- */
	vec2 get_drive_nums() {

		vec2 output(0, 0);

		// if too much error
		if (!error_is_okay)
			return output;

		output.x = auton::target_pos.x - currentStatus.x;
		output.y = auton::target_pos.y - currentStatus.x;

		return output;
	}

	/* ------------------------ Set Waypoint Pos To Go To ----------------------- */
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

		printf(("\nCurr X " + std::to_string(gps_x)).c_str());
		printf(("\nCurr Y " + std::to_string(gps_y)).c_str());
		printf(("\nTurning to " + std::to_string(theta)).c_str());
		printf(("\nMove X " + std::to_string(distX)).c_str());
		printf(("\nMove Y " + std::to_string(distY)).c_str());
		printf("\n\n");

		// Turn to angle
		chassis.set_turn_pid(theta, ace::auton::curr_turnSpeed);
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
				chassis.set_turn_pid(theta + 18 * (new_theta / std::abs(new_theta)), ace::auton::curr_turnSpeed);
			}
			else {
				// Turn to angle
				chassis.set_turn_pid(new_theta, ace::auton::curr_turnSpeed);
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