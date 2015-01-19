#include <algorithm>
#ifdef ANDROIDSDL
#include <android/log.h>
#endif
#include <guichan.hpp>
#include <iostream>
#include <sstream>
#include <SDL/SDL_ttf.h>
#include <guichan/sdl.hpp>
#include "sdltruetypefont.hpp"
#include "rw_config.h"

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "uae.h"
#include "memory-uae.h"
#include "custom.h"
#include "gui.h"
#include "newcpu.h"
#include "filesys.h"
#include "threaddep/thread.h"
#include "audio.h"
#include "savestate.h"
#include "debug.h"
#include "inputdevice.h"
#include "xwin.h"
#include "picasso96.h"
#include "version.h"
}

namespace widgets
{
void show_settings_Paths(void);
extern void run_menuLoad_guichan(char *curr_path, int aLoadType);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_paths;
gcn::Image *background_image_paths;
gcn::Icon* background_paths;

gcn::Button* button_rom_path;
gcn::Button* button_floppy_path;
gcn::Button* button_hardfile_path;
gcn::TextField* textField_rom_path;
gcn::TextField* textField_floppy_path;
gcn::TextField* textField_hardfile_path;

class PathsButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == button_rom_path) {
            run_menuLoad_guichan(currentDir, MENU_SELECT_ROMPATH);
        }
        if (actionEvent.getSource() == button_floppy_path) {
            run_menuLoad_guichan(currentDir, MENU_SELECT_FLOPPYPATH);
        }
        if (actionEvent.getSource() == button_hardfile_path) {
            run_menuLoad_guichan(currentDir, MENU_SELECT_HARDFILEPATH);
        }
    }
};
PathsButtonActionListener* pathsButtonActionListener;

void menuPaths_Init()
{
    background_image_paths = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_paths = new gcn::Icon(background_image_paths);

    // Select ROM
    button_rom_path = new gcn::Button("Rom Path");
    button_rom_path->setSize(100,30);
    button_rom_path->setPosition(10,25);
    button_rom_path->setBaseColor(baseCol);
    button_rom_path->setId("RomPath");
    pathsButtonActionListener = new PathsButtonActionListener();
    button_rom_path->addActionListener(pathsButtonActionListener);

    textField_rom_path = new gcn::TextField("                                                            ");
    textField_rom_path->setSize(350,22);
    textField_rom_path->setPosition(125,30);
    textField_rom_path->setEnabled(false);
    textField_rom_path->setBaseColor(baseCol);

    button_floppy_path = new gcn::Button("Floppy path");
    button_floppy_path->setSize(100,30);
    button_floppy_path->setPosition(10,105);
    button_floppy_path->setBaseColor(baseCol);
    button_floppy_path->setId("FloppyPath");
    button_floppy_path->addActionListener(pathsButtonActionListener);

    textField_floppy_path = new gcn::TextField("                                                            ");
    textField_floppy_path->setSize(350,22);
    textField_floppy_path->setPosition(125,110);
    textField_floppy_path->setEnabled(false);
    textField_floppy_path->setBaseColor(baseCol);

    button_hardfile_path = new gcn::Button("HDF path");
    button_hardfile_path->setSize(100,30);
    button_hardfile_path->setPosition(10,185);
    button_hardfile_path->setBaseColor(baseCol);
    button_hardfile_path->setId("HDFpath");
    button_hardfile_path->addActionListener(pathsButtonActionListener);

    textField_hardfile_path = new gcn::TextField("                                                            ");
    textField_hardfile_path->setSize(350,22);
    textField_hardfile_path->setPosition(125,190);
    textField_hardfile_path->setEnabled(false);
    textField_hardfile_path->setBaseColor(baseCol);

    window_paths = new gcn::Window("Paths");
    window_paths->setPosition(137,20);
    window_paths->setMovable(false);
    window_paths->setSize(500,400);
    window_paths->setBaseColor(baseCol);
    window_paths->setVisible(false);

    window_paths->add(background_paths);
    window_paths->add(button_rom_path);
    window_paths->add(textField_rom_path);
    window_paths->add(button_floppy_path);
    window_paths->add(textField_floppy_path);
    window_paths->add(button_hardfile_path);
    window_paths->add(textField_hardfile_path);
}

void menuPaths_Exit()
{
    delete pathsButtonActionListener;

    delete background_paths;
    delete background_image_paths;
    delete button_rom_path;
    delete textField_rom_path;
    delete button_floppy_path;
    delete textField_floppy_path;
    delete button_hardfile_path;
    delete textField_hardfile_path;

    delete window_paths;
}

void show_settings_Paths()
{
    textField_rom_path->setText(prefs_get_attr ("rom_path"));
    textField_floppy_path->setText(prefs_get_attr ("floppy_path"));
    textField_hardfile_path->setText(prefs_get_attr ("hardfile_path"));
}

}
