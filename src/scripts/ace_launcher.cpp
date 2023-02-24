
#include "ace.h"

/* ========================================================================== */
/*                       Launcher Namespace Definitions                       */
/* ========================================================================== */
namespace ace::launch {

	/* ========================================================================== */
	/*                            Variable Definitions                            */
	/* ========================================================================== */

	// launcher time
	u_int64_t launcherTime = 0;
	// Current launcher timer
	float launcherTimerDelay = 0;
	// launcherEnabledBool
	bool launcherEnabled = false;


	/* ========================================================================== */
	/*                            Launch Disks Function                           */
	/* ========================================================================== */
	void launchDisks(bool enabled, float speed, bool isLongDist, bool standby) {

		// If want to run launcher
		if (enabled) {


			launch::recordLauncherStatistics();
			launcherEnabled = true;

			// if less than cutoff
			if (launcherMotor.get_actual_velocity() / 6.0 <= speed - LAUNCHER_MIN_SPEED) {
				// Set velocity to max
				launcherMotor.move_velocity(600);
				spinMotor(intakeMotor, 0);
				spinMotor(conveyorMotor, 0);
				launcherTimerDelay = 0;
				return;
			}

			// if long distance
			if (isLongDist) {
				// Speed up Launcher and Roller but nothing else
				launcherMotor.move_velocity(speed * 6);

				varLauncherMotor.move_absolute(150, 100);

				if (!(launcherTimerDelay > launcherTimerDelayMax)) {
					launcherTimerDelay += 10.0;
					return;
				}

				launcherTimerDelay -= launcherTimerDelayMax;
			}

			// if is regular distance
			if (!isLongDist) {

				varLauncherMotor.move_absolute(0, -100);
				// Set pid to set velocity
				spinMotor(launcherMotor, SPEED_LAUNCHER_LONG);
			}

			spinMotor(intakeMotor, -SPEED_INTAKE_LAUNCHER);
			spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER);

		}
		else {
			launcherEnabled = false;

			spinMotor(intakeMotor, 0);
			spinMotor(conveyorMotor, 0);

			if (standby)
				spinMotor(launcherMotor, SPEED_LAUNCHER);
			else
				launcherMotor.move_voltage(0);
		}
	}


	/* ========================================================================== */
	/*                          Launch Disks Autonomously                         */
	/* ========================================================================== */
	void launchDisks_Auto(float time, float speed, bool isLongDist) {
		float currTime = 0;
		while (currTime < time) {
			launch::launchDisks(true, speed, isLongDist);

			currTime += ez::util::DELAY_TIME;
			pros::delay(ez::util::DELAY_TIME);
		}

		launch::launchDisks(false, 0, false, true);
	}


	/* ========================================================================== */
	/*                         Record Launcher Statistics                         */
	/* ========================================================================== */
	void recordLauncherStatistics() {

		// Leave if no sd card
		if (ez::util::IS_SD_CARD) {
			// leave if dont want to write to sd card
			if (LAUNCHER_LOGGING) {
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


	/* ========================================================================== */
	/*                             Save Launcher Data                             */
	/* ========================================================================== */
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


	/* ========================================================================== */
	/*                            Upload Launcher Data                            */
	/* ========================================================================== */
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
}