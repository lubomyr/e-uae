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
}

extern void extractFileName(char * str,char *buffer);

namespace widgets
{
void show_settings_Rom(void);

extern void run_menuLoad_guichan(char *curr_path, int aLoadType);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_rom;
gcn::Image *background_image_rom;
gcn::Icon* background_rom;
gcn::Button* button_main_rom;
gcn::Button* button_ext_rom;
gcn::Button* button_key_rom;
gcn::TextField* textField_main_rom;
gcn::TextField* textField_ext_rom;
gcn::TextField* textField_key_rom;

class RomButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        char tempPath[300];
        strcpy(tempPath, prefs_get_attr ("rom_path"));
        strcpy(currentDir,tempPath);
        if (actionEvent.getSource() == button_main_rom) {
            run_menuLoad_guichan(currentDir, MENU_SELECT_MAIN_ROM);
        }
        if (actionEvent.getSource() == button_ext_rom) {
            run_menuLoad_guichan(currentDir, MENU_SELECT_EXT_ROM);
        }
        if (actionEvent.getSource() == button_key_rom) {
            run_menuLoad_guichan(currentDir, MENU_SELECT_KEY_ROM);
        }
    }
};
RomButtonActionListener* romButtonActionListener;

void menuRom_Init()
{
    background_image_rom = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_rom = new gcn::Icon(background_image_rom);

    // Select ROM
    button_main_rom = new gcn::Button("Main ROM file");
    button_main_rom->setSize(100,30);
    button_main_rom->setPosition(10,25);
    button_main_rom->setBaseColor(baseCol);
    button_main_rom->setId("MainRom");
    romButtonActionListener = new RomButtonActionListener();
    button_main_rom->addActionListener(romButtonActionListener);

    textField_main_rom = new gcn::TextField("                                                            ");
    textField_main_rom->setSize(350,22);
    textField_main_rom->setPosition(125,30);
    textField_main_rom->setEnabled(false);
    textField_main_rom->setBaseColor(baseCol);

    button_ext_rom = new gcn::Button("Ext.ROM file");
    button_ext_rom->setSize(100,30);
    button_ext_rom->setPosition(10,105);
    button_ext_rom->setBaseColor(baseCol);
    button_ext_rom->setId("ExtRom");
    button_ext_rom->addActionListener(romButtonActionListener);

    textField_ext_rom = new gcn::TextField("                                                            ");
    textField_ext_rom->setSize(350,22);
    textField_ext_rom->setPosition(125,110);
    textField_ext_rom->setEnabled(false);
    textField_ext_rom->setBaseColor(baseCol);

    button_key_rom = new gcn::Button("Key file");
    button_key_rom->setSize(100,30);
    button_key_rom->setPosition(10,185);
    button_key_rom->setBaseColor(baseCol);
    button_key_rom->setId("KeyRom");
    button_key_rom->addActionListener(romButtonActionListener);

    textField_key_rom = new gcn::TextField("                                                            ");
    textField_key_rom->setSize(350,22);
    textField_key_rom->setPosition(125,190);
    textField_key_rom->setEnabled(false);
    textField_key_rom->setBaseColor(baseCol);

    window_rom = new gcn::Window("ROM");
    window_rom->setPosition(137,20);
    window_rom->setMovable(false);
    window_rom->setSize(500,400);
    window_rom->setBaseColor(baseCol);
    window_rom->setVisible(false);

    window_rom->add(background_rom);
    window_rom->add(button_main_rom);
    window_rom->add(textField_main_rom);
    window_rom->add(button_ext_rom);
    window_rom->add(textField_ext_rom);
    window_rom->add(button_key_rom);
    window_rom->add(textField_key_rom);
}

void menuRom_Exit()
{
    delete background_rom;
    delete background_image_rom;
    delete button_main_rom;
    delete textField_main_rom;
    delete button_ext_rom;
    delete textField_ext_rom;
    delete button_key_rom;
    delete textField_key_rom;

    delete window_rom;
}

void show_settings_Rom()
{
    static char tmpMainRom[256];
    static char tmpExtRom[256];
    static char tmpKeyRom[256];
    if (strcmp(changed_prefs.romfile, "")==0)
        textField_main_rom->setText("insert Kickstart Rom file");
    else {
        extractFileName(changed_prefs.romfile, tmpMainRom);
        textField_main_rom->setText(tmpMainRom);
    }
    if (strcmp(changed_prefs.romextfile, "")==0)
        textField_ext_rom->setText("not selected");
    else {
        extractFileName(changed_prefs.romextfile, tmpExtRom);
        textField_key_rom->setText(tmpExtRom);
    }
    if (strcmp(changed_prefs.keyfile, "")==0)
        textField_key_rom->setText("not selected");
    else {
        extractFileName(changed_prefs.keyfile, tmpKeyRom);
        textField_key_rom->setText(tmpKeyRom);
    }
}

}
