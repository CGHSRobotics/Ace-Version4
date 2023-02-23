
#include "ace.h"

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