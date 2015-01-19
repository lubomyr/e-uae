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

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
}

namespace widgets
{
void show_settings_Control(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_control;
gcn::Image *background_image_control;
gcn::Icon* background_control;
gcn::CheckBox* checkBox_status_line;

class StatuslineCheckBoxActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_status_line) {
            if (checkBox_status_line->isSelected())
                changed_prefs.leds_on_screen=1;
            else
                changed_prefs.leds_on_screen=0;
        }
    }
};
StatuslineCheckBoxActionListener* statuslineCheckBoxActionListener;

void menuControl_Init()
{
    background_image_control = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_control = new gcn::Icon(background_image_control);

    checkBox_status_line = new gcn::CheckBox("Status line");
    checkBox_status_line->setPosition(10,20);
    checkBox_status_line->setId("Status line");
    statuslineCheckBoxActionListener = new StatuslineCheckBoxActionListener();
    checkBox_status_line->addActionListener(statuslineCheckBoxActionListener);

    window_control = new gcn::Window("Control");
    window_control->setPosition(137,20);
    window_control->setMovable(false);
    window_control->setSize(500,400);
    window_control->setBaseColor(baseCol);
    window_control->setVisible(false);

    window_control->add(background_control);
    window_control->add(checkBox_status_line);
}

void menuControl_Exit()
{
    delete statuslineCheckBoxActionListener;

    delete background_control;
    delete background_image_control;

    delete checkBox_status_line;

    delete window_control;
}

void show_settings_Control()
{
    if (changed_prefs.leds_on_screen==0)
        checkBox_status_line->setSelected(false);
    else
        checkBox_status_line->setSelected(true);
}

}
