#include "main.h"

#include "cghs.h"

// Chassis constructor
Drive chassis(
    // Left Chassis Ports (negative port will reverse it!)
    //   the first port is the sensored port (when trackers are not used!)
    {-cghs::DRIVE_LEFT_FRONT_PORT, -cghs::DRIVE_LEFT_BACK_PORT}

    // Right Chassis Ports (negative port will reverse it!)
    //   the first port is the sensored port (when trackers are not used!)
    ,
    {cghs::DRIVE_RIGHT_FRONT_PORT, cghs::DRIVE_RIGHT_BACK_PORT}

    // IMU Port
    ,
    cghs::IMU_PORT

    // Wheel Diameter (Remember, 4" wheels are actually 4.125!)
    //    (or tracking wheel diameter)
    ,
    3.25

    // Cartridge RPM
    //   (or tick per rotation if using tracking wheels)
    ,
    200

    // External Gear Ratio (MUST BE DECIMAL)
    //    (or gear ratio of tracking wheel)
    // eg. if your drive is 84:36 where the 36t is powered, your RATIO would be 2.333.
    // eg. if your drive is 36:60 where the 60t is powered, your RATIO would be 0.6.
    ,
    0.6

    // Uncomment if using tracking wheels
    /*
    // Left Tracking Wheel Ports (negative port will reverse it!)
    // ,{1, 2} // 3 wire encoder
    // ,8 // Rotation sensor

    // Right Tracking Wheel Ports (negative port will reverse it!)
    // ,{-3, -4} // 3 wire encoder
    // ,-9 // Rotation sensor
    */

    // Uncomment if tracking wheels are plugged into a 3 wire expander
    // 3 Wire Port Expander Smart Port
    // ,1
);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  // Print our branding over your terminal :D
  ez::print_ez_template();

  pros::delay(500);  // Stop the user from doing anything while legacy ports configure.

  // These are already defaulted to these buttons, but you can change the left/right curve buttons here!
  // chassis.set_left_curve_buttons (pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT); // If using tank, only the left side is used.
  // chassis.set_right_curve_buttons(pros::E_CONTROLLER_DIGITAL_Y,    pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
  ez::as::auton_selector.add_autons({
      Auton("Skills Side", cghs::skills_Auto),
      Auton("Null Side", cghs::null_Auto),
      Auton("Three Side", cghs::threeSide_Auto),
      Auton("Two Side", cghs::twoSide_Auto),
      Auton("Shebang Side", cghs::theWholeShebang_Auto),
  });

  chassis.toggle_modify_curve_with_controller(false);
  chassis.set_curve_default(0, 0);

  // Initialize chassis and auton selector
  chassis.initialize();
  ez::as::initialize();

  master.clear_line(2);
  master.set_text(2, 0, ez::as::auton_selector.Autons[ez::as::auton_selector.current_auton_page].Name);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  while (true) {
    cghs::select_Auton();

    pros::delay(ez::util::DELAY_TIME);
  }
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
  // . . .
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  chassis.reset_pid_targets();                // Resets PID targets to 0
  chassis.reset_gyro();                       // Reset gyro position to 0
  chassis.reset_drive_sensor();               // Reset drive sensors to 0
  chassis.set_drive_brake(MOTOR_BRAKE_HOLD);  // Set motors to hold.  This helps autonomous consistency.

  ez::as::auton_selector.call_selected_auton();  // Calls selected auton from autonomous selector.
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

typedef FILE *pc_file_t;

static lv_fs_res_t pcfs_open(void *file_p, const char *fn, lv_fs_mode_t mode) {
  errno = 0;
  const char *flags = "";
  if (mode == LV_FS_MODE_WR)
    flags = "wb";
  else if (mode == LV_FS_MODE_RD)
    flags = "rb";
  else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
    flags = "a+";

  char buf[256];
  sprintf(buf, "/%s", fn);
  pc_file_t f = fopen(buf, flags);

  if (f == NULL)
    return LV_FS_RES_UNKNOWN;
  else {
    fseek(f, 0, SEEK_SET);
    pc_file_t *fp = (pc_file_t *)file_p;
    *fp = f;
  }

  return LV_FS_RES_OK;
}

static lv_fs_res_t pcfs_close(void *file_p) {
  pc_file_t *fp = (pc_file_t *)file_p;
  fclose(*fp);
  return LV_FS_RES_OK;
}

static lv_fs_res_t pcfs_read(void *file_p, void *buf, uint32_t btr, uint32_t *br) {
  pc_file_t *fp = (pc_file_t *)file_p;
  *br = fread(buf, 1, btr, *fp);
  return LV_FS_RES_OK;
}

static lv_fs_res_t pcfs_seek(void *file_p, uint32_t pos) {
  pc_file_t *fp = (pc_file_t *)file_p;
  fseek(*fp, pos, SEEK_SET);
  return LV_FS_RES_OK;
}

static lv_fs_res_t pcfs_tell(void *file_p, uint32_t *pos_p) {
  pc_file_t *fp = (pc_file_t *)file_p;
  *pos_p = ftell(*fp);
  return LV_FS_RES_OK;
}

void opcontrol() {
  // Screen
  pros::lcd::shutdown();

  // This is preference to what you like to drive on.
  chassis.set_drive_brake(MOTOR_BRAKE_COAST);

  bool intakeToggleEnabled = false;
  bool intakeReverseEnabled = false;

  bool rollerForwardToggle = false;
  bool rollerReverseToggle = false;

  bool launchingDisksEnabled = false;

  bool endgameToggleEnabled = false;

  lv_fs_drv_t pcfs_drv;                      /*A driver descriptor*/
  memset(&pcfs_drv, 0, sizeof(lv_fs_drv_t)); /*Initialization*/

  pcfs_drv.file_size = sizeof(pc_file_t); /*Set up fields...*/
  pcfs_drv.letter = 'S';
  pcfs_drv.open = pcfs_open;
  pcfs_drv.close = pcfs_close;
  pcfs_drv.read = pcfs_read;
  pcfs_drv.seek = pcfs_seek;
  pcfs_drv.tell = pcfs_tell;
  lv_fs_add_drv(&pcfs_drv);

  lv_obj_t *img_var = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img_var, "S:/usd/ace.bin");
  lv_obj_set_pos(img_var, 0, 0);  // set the position to center

  while (true) {
    chassis.tank();  // Tank control

    cghs::select_Auton();

    // Toggle Intake
    /*if (master.get_digital_new_press(BUTTON_A_BRAKE_TOGGLE)) {
      cghs::activeBreakEnabled = !cghs::activeBreakEnabled;
      master.set_text(2, 12, "true");
    }*/

    // Toggle Intake
    if (master.get_digital_new_press(BUTTON_INTAKE_TOGGLE)) {
      intakeToggleEnabled = !intakeToggleEnabled;
      cghs::intakeToggle(intakeToggleEnabled);
    }

    // Emergency Reverse Intake
    if (master.get_digital_new_press(BUTTON_INTAKE_REVERSE)) {
      intakeToggleEnabled = false;
      cghs::intakeToggle(false);
      intakeReverseEnabled = true;
      cghs::intakeReverse(true);
    }
    if (intakeReverseEnabled && !master.get_digital(BUTTON_INTAKE_REVERSE)) {
      intakeReverseEnabled = false;
      cghs::intakeReverse(false);
    }

    // Roller Forward Intake
    if (master.get_digital_new_press(BUTTON_ROLLER_FORWARD)) {
      rollerForwardToggle = true;
      cghs::rollerForward(true, cghs::SPEED_ROLLER);
    }
    if (rollerForwardToggle && !master.get_digital(BUTTON_ROLLER_FORWARD)) {
      rollerForwardToggle = false;
      cghs::rollerForward(false, 0);
    }

    // Roller Reverse Intake
    if (master.get_digital_new_press(BUTTON_ROLLER_REVERSE)) {
      rollerReverseToggle = true;
      cghs::rollerReverse(true, cghs::SPEED_ROLLER);
    }
    if (rollerReverseToggle && !master.get_digital(BUTTON_ROLLER_REVERSE)) {
      rollerReverseToggle = false;
      cghs::rollerReverse(false, 0);
    }

    // Launch Disks
    if (master.get_digital(BUTTON_LAUNCHER)) {
      launchingDisksEnabled = true;
      cghs::launchDisks(true, cghs::SPEED_LAUNCHER);
    }
    if (master.get_digital(BUTTON_LAUNCHER_LONG)) {
      chassis.set_active_brake(0.1);
      launchingDisksEnabled = true;
      cghs::launchDisksLong(cghs::SPEED_LAUNCHER_LONG);
    }
    if (launchingDisksEnabled && !master.get_digital(BUTTON_LAUNCHER) && !master.get_digital(BUTTON_LAUNCHER_LONG)) {
      chassis.set_active_brake(0);
      launchingDisksEnabled = false;
      cghs::launchDisks(false, 0.0);
    }

    // Endgame Toggle
    if (master.get_digital(BUTTON_ENDGAME)) {
      endgameToggleEnabled = true;
      cghs::endgameToggle(true);
    }
    if (endgameToggleEnabled && !master.get_digital(BUTTON_ENDGAME)) {
      endgameToggleEnabled = false;
      cghs::endgameToggle(false);
    }

    pros::delay(ez::util::DELAY_TIME);  // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
  }
}
