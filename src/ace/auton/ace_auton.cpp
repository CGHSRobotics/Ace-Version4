#include "ace_auton.h"

/**
 *
 *	Miscellaneous Auton Functions
 *
 */
namespace ace::auton {

	// 	Launch Disks Autonomously
	void launchDisks_Auto(float time, float speed, bool isLongDist) {
		float currTime = 0;
		while (currTime < time) {
			launchDisks(true, speed, isLongDist);

			currTime += ez::util::DELAY_TIME;
			pros::delay(ez::util::DELAY_TIME);
		}

		launchDisks(false, 0, false, true);
	}

	int autonIndex = 0;

	// Updates auton selection and prints to screen
	void updateAutonSelection() {
		if (autonIndex < 0)
		{
			autonIndex += numAutons;
		}
		if (autonIndex >= numAutons)
		{
			autonIndex -= numAutons;
		}

		std::string str = ace::auton::autonArray[ace::auton::autonIndex];

		printf("\n Auton Changed To: %s", str.c_str());
	}

	// Adjust Auton based off of buttons
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