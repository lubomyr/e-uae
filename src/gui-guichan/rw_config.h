
/* What is being loaded */
#define MENU_LOAD_FLOPPY 1
#define MENU_LOAD_HDF 2
#define MENU_LOAD_DIR 3
#define MENU_SELECT_MAIN_ROM 4
#define MENU_SELECT_EXT_ROM 5
#define MENU_SELECT_KEY_ROM 6
#define MENU_SELECT_ROMPATH 7
#define MENU_SELECT_FLOPPYPATH 8
#define MENU_SELECT_HARDFILEPATH 9

extern int current_drive;
extern char launchDir[300];
extern char currentDir[300];
extern int mainMenu_onScreen;