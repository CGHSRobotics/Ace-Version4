#include ".\src\ace\ace.h"

#ifndef ACE_AUTON
#define ACE_AUTON

/**
 *	Autonomous Namespace
 */

namespace ace::auton {

	// Autonomous Selector

	extern int autonIndex;
	const int numAutons = 7;
	const std::string autonArray[10] = { "Skills", "Shebang", "Null", "Blue Three", "Blue Two", "Red Three", "Red Two" };


	extern void updateAutonSelection();
	extern void checkAutonButtons();

	// Autonomous
	extern void launchDisks_Auto(float time, float speed, bool isLongDist = false);

	extern void skills_Auto();
	extern void null_Auto();
	extern void threeSide_Auto();
	extern void twoSide_Auto();
	extern void theWholeShebang_Auto();

}  // namespace auton

#endif