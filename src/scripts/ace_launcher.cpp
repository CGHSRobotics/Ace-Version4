
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
	bool flapEnabled = false;

	/* ========================================================================== */
	/*                            Launch Disks Function                           */
	/* ========================================================================== */
	void launchDisks(float speed, bool isLongDist) {

		if (var_launcher_enabled = true) {
			ace::varLauncherMotor.move_absolute(VAR_LAUNCH_ANGLE_DOWN, 100);
			
		} else {
			ace::varLauncherMotor.move_absolute(VAR_LAUNCH_ANGLE_DOWN, -100);
		}
		
		

		// If want to run launcher
		if (speed > 0.0) {

			/* ------------------------------ General Stuff ----------------------------- */

			launch::recordLauncherStatistics();
			diskCheck();
			launcherEnabled = true;

			if (isLongDist)
				varLauncherMotor.move_absolute(VAR_LAUNCH_ANGLE_DOWN, -100);
			else
				varLauncherMotor.move_absolute(VAR_LAUNCH_ANGLE_UP, 100);


			/* --------------------------- If Less Than Cutoff -------------------------- */
			if (launcherMotor.get_actual_velocity() / 6.0 <= speed - LAUNCHER_MIN_SPEED) {
				// Set velocity to max
				spinMotor(launcherMotor, 100);
				spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
				spinMotor(conveyorMotor, 0);
				launcherTimerDelay = 0;

				return;
			}

			/* ---------------------------- Long Distance ---------------------------- */
			if (isLongDist) {

				// Set pid to set velocity
				launcherMotor.move_velocity(speed * 6);

				if (launcherTimerDelay < launcherTimerDelayMax) {
					launcherTimerDelay += 10.0;
				}
				else {
					launcherTimerDelay -= launcherTimerDelayMax;

					// Spin intake and conveyor to launch disks
					spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
					spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER);
				}
			}

			/* ------------------------- Short Distance ------------------------- */
			if (!isLongDist) {

				if (var_launcher_enabled = true) {
					spinMotor(launcherMotor, LAUNCHER_FLAP_SPEED);
				} else {
					// Set pid to set velocity
					spinMotor(launcherMotor, SPEED_LAUNCHER_SHORT);			
				}

				// Spin intake and conveyor to launch disks
				spinMotor(intakeMotor, SPEED_INTAKE_LAUNCHER);
				spinMotor(conveyorMotor, -SPEED_CONVEYOR_LAUNCHER);
			}
		}
		else {
			launcherEnabled = false;

			spinMotor(intakeMotor, 0);
			spinMotor(conveyorMotor, 0);

			// if standby, set to 50% else set to passive 0 volts
			if (LAUNCHER_STANDBY_ENABLED)
				spinMotor(launcherMotor, SPEED_LAUNCHER_STANDBY);
			else
				launcherMotor.move_voltage(0);
		}
	}


	/* ========================================================================== */
	/*                          Launch Disks Autonomously                         */
	/* ========================================================================== */
	void launchDisks_Auto(float time, float speed, bool isLongDist) {
		launchCount = 0;
		float currTime = 0;
		while (currTime < time) {
			launch::launchDisks(speed, isLongDist);

			currTime += ez::util::DELAY_TIME;
			pros::delay(ez::util::DELAY_TIME);
		}

		launch::launchDisks(0, false);
	}


	/* ========================================================================== */
	/*                                 Set Standby                                */
	/* ========================================================================== */
	void set_standby(bool enabled) {
		LAUNCHER_STANDBY_ENABLED = enabled;
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
					curr_point.set_volt = launcherMotor.get_voltage();
					curr_point.vis_disk = diskSeen;
					l_data_array.push_back(curr_point);


					l_data_point point_2fr_ago = l_data_array[l_data_array.size() - 2];

					// if data point to frames ago was actually 20msec ago
					if ((int)curr_point.msec - 20 == (int)point_2fr_ago.msec) {
						// if launcher is enabled
						if (launcherEnabled) {
							// if rpm dropped by about 10% in two frames while rpm is greater than 40
							if (curr_point.rpm < point_2fr_ago.rpm * 0.95 && point_2fr_ago.rpm > 40.0 * 6.0) {
								printf("LAUNCH DETECTED");
								//master.rumble(".");
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

		printf(("Delete File: " + std::to_string(remove(launcherFile_path))).c_str());

		FILE* launcherFile;
		launcherFile = fopen(launcherFile_path, "a");

		fprintf(launcherFile, "\n/start data\n");
		for (size_t i = 0; i < l_data_array.size(); i++)
		{
			fprintf(launcherFile, (std::to_string((int)l_data_array[i].msec)).c_str());
			fprintf(launcherFile, " ");
			fprintf(launcherFile, (std::to_string((int)l_data_array[i].rpm)).c_str());
			fprintf(launcherFile, " ");
			fprintf(launcherFile, (std::to_string((int)l_data_array[i].set_volt)).c_str());
			fprintf(launcherFile, " ");
			fprintf(launcherFile, (std::to_string(l_data_array[i].vis_disk)).c_str());
			fprintf(launcherFile, " \n");
		}

		fprintf(launcherFile, "\n/end data\n");

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
		float lines_per_msec = 1;

		while (true)
		{

			if (!fgets(buff, buff_size, launcherFile))
			{
				break;
			}

			printf(buff);

			pros::delay(10);
		}

		fclose(launcherFile);
	}
}