
#include "cghs.h"

typedef FILE* pc_file_t;

lv_obj_t* screenHome;
lv_obj_t* screenMenu;

lv_style_t style_bg;
lv_style_t style_tab;
lv_style_t style_red;

lv_obj_t* label;

lv_obj_t* img_var;
lv_obj_t* buttonToMenu;

lv_obj_t* tabview;
lv_obj_t* tab1;
lv_obj_t* tab2;
lv_obj_t* tab3;
lv_obj_t* tab4;

lv_obj_t* buttonToHome;
lv_obj_t* autonDDList;

lv_obj_t* labelTemp;

// File Drivers
static lv_fs_res_t pcfs_open(void* file_p, const char* fn, lv_fs_mode_t mode) {
	errno = 0;
	const char* flags = "";
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
		pc_file_t* fp = (pc_file_t*)file_p;
		*fp = f;
	}

	return LV_FS_RES_OK;
}
static lv_fs_res_t pcfs_close(void* file_p) {
	pc_file_t* fp = (pc_file_t*)file_p;
	fclose(*fp);
	return LV_FS_RES_OK;
}
static lv_fs_res_t pcfs_read(void* file_p, void* buf, uint32_t btr, uint32_t* br) {
	pc_file_t* fp = (pc_file_t*)file_p;
	*br = fread(buf, 1, btr, *fp);
	return LV_FS_RES_OK;
}
static lv_fs_res_t pcfs_seek(void* file_p, uint32_t pos) {
	pc_file_t* fp = (pc_file_t*)file_p;
	fseek(*fp, pos, SEEK_SET);
	return LV_FS_RES_OK;
}
static lv_fs_res_t pcfs_tell(void* file_p, uint32_t* pos_p) {
	pc_file_t* fp = (pc_file_t*)file_p;
	*pos_p = ftell(*fp);
	return LV_FS_RES_OK;
}

// Function called when option on ddlist is selected
static lv_res_t ddlist_action(lv_obj_t* ddlist)
{
	cghs::auton::autonIndex = lv_ddlist_get_selected(ddlist);

	cghs::auton::updateAutonSelection();

	return LV_RES_OK; 	//	Return OK if the drop down list is not deleted
}

// Function called when toMenu button pressed
static lv_res_t toMenuButtonAction(lv_obj_t* button)
{
	lv_scr_load(screenMenu);

	return LV_RES_OK; 	//	Return OK if the drop down list is not deleted
}

// Function called when toHome button pressed
static lv_res_t toHomeButtonAction(lv_obj_t* button)
{
	lv_scr_load(screenHome);

	return LV_RES_OK; 	//	Return OK if the drop down list is not deleted
}



/**
 *
 *	Initializes Lv brain screen
 *
 */
static void init_lv_screen() {

	// Init Driver
	lv_fs_drv_t pcfs_drv;                      //A driver descriptor
	memset(&pcfs_drv, 0, sizeof(lv_fs_drv_t)); //Initialization

	// Set up file driver functions
	pcfs_drv.file_size = sizeof(pc_file_t); //Set up fields...
	pcfs_drv.letter = 'S';
	pcfs_drv.open = pcfs_open;
	pcfs_drv.close = pcfs_close;
	pcfs_drv.read = pcfs_read;
	pcfs_drv.seek = pcfs_seek;
	pcfs_drv.tell = pcfs_tell;
	lv_fs_add_drv(&pcfs_drv);

	//	Create a style
	lv_style_copy(&style_bg, &lv_style_plain);
	style_bg.text.color = LV_COLOR_RED;
	style_bg.body.main_color = LV_COLOR_BLACK;
	style_bg.body.grad_color = LV_COLOR_BLACK;
	style_bg.line.color = LV_COLOR_RED;
	style_bg.line.opa = 0;
	style_bg.body.border.color = LV_COLOR_RED;
	style_bg.body.border.width = 1;
	style_bg.body.padding.ver = 12;


	// Init Screens
	screenHome = lv_obj_create(NULL, NULL);
	lv_obj_set_style(screenHome, &style_bg);
	screenMenu = lv_obj_create(NULL, NULL);
	lv_obj_set_style(screenMenu, &style_bg);

	/*
	 *	Home Screen
	 */
	img_var = lv_img_create(screenHome, NULL);
	lv_img_set_src(img_var, "S:/usd/ace.bin");
	lv_obj_set_pos(img_var, 0, 0);  					// set the position to center

	buttonToMenu = lv_btn_create(screenHome, NULL);
	lv_obj_set_style(buttonToMenu, &style_bg);
	lv_btn_set_style(buttonToMenu, LV_BTN_STYLE_REL, &style_bg);
	lv_cont_set_fit(buttonToMenu, true, true); // Enable resizing horizontally and vertically
	lv_obj_align(buttonToMenu, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
	lv_btn_set_action(buttonToMenu, LV_BTN_ACTION_CLICK, toMenuButtonAction);
	label = lv_label_create(buttonToMenu, NULL);
	lv_label_set_text(label, "Menu");


	/*
	 *	Menu Screen
	 */

	 // Setup tab view
	lv_style_copy(&style_tab, &style_bg);
	style_tab.body.padding.ver = 20;


	lv_style_copy(&style_red, &style_tab);
	style_red.body.main_color = LV_COLOR_RED;
	style_red.body.grad_color = LV_COLOR_RED;

	tabview = lv_tabview_create(screenMenu, NULL);
	lv_tabview_set_sliding(tabview, false);
	lv_obj_set_style(tabview, &style_bg);
	lv_cont_set_fit(tabview, false, false); // Enable resizing horizontally and vertically
	lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_PR, &style_tab);
	lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_REL, &style_tab);
	lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_INDIC, &style_red);


	tab1 = lv_tabview_add_tab(tabview, "Home");
	tab2 = lv_tabview_add_tab(tabview, "Skills");
	tab3 = lv_tabview_add_tab(tabview, "Temp");
	tab4 = lv_tabview_add_tab(tabview, "Speed");


	// Tab 1

	label = lv_label_create(tab1, NULL);
	lv_label_set_text(label, "Press this button to go back to the Home Screen");
	// Button back to Home
	buttonToHome = lv_btn_create(tab1, NULL);
	lv_obj_set_style(buttonToHome, &style_bg);
	lv_btn_set_style(buttonToHome, LV_BTN_STYLE_REL, &style_bg);
	lv_cont_set_fit(buttonToHome, true, true); // Enable resizing horizontally and vertically
	lv_obj_align(buttonToHome, NULL, LV_ALIGN_CENTER, 0, 40);
	lv_btn_set_action(buttonToHome, LV_BTN_ACTION_CLICK, toHomeButtonAction);
	label = lv_label_create(buttonToHome, NULL);
	lv_label_set_text(label, "Home");

	// Tab 2

	label = lv_label_create(tab2, NULL);
	lv_label_set_text(label, "Select Skills");

	// Tab 3

	labelTemp = lv_label_create(tab3, NULL);
	lv_label_set_text(labelTemp, "Temperatur goes here\n");

	// Tab 4

	label = lv_label_create(tab4, NULL);
	lv_label_set_text(label, "Speed of Launcher");


	//	Create a drop down list
	autonDDList = lv_ddlist_create(tab2, NULL);
	lv_ddlist_set_options(autonDDList,
		"Skills\n"
		"Null\n"
		"Three\n"
		"Two\n"
		"Shebang\n"
	);
	lv_obj_align(autonDDList, NULL, LV_ALIGN_CENTER, 0, 10);
	lv_obj_set_style(autonDDList, &style_bg);
	lv_obj_set_free_num(autonDDList, 2);				//	Set a unique ID
	lv_ddlist_set_draw_arrow(autonDDList, true);		//	Turn On Arrow
	lv_ddlist_set_action(autonDDList, ddlist_action);  	//	Set a function to call when anew option is chosen

	lv_ddlist_set_selected(autonDDList, 0);
	ddlist_action(autonDDList);

	// Load Home Screen
	lv_scr_load(screenHome);
}