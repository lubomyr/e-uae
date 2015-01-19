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


namespace widgets
{
void show_settings_OnScreen(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_onScreen;
gcn::Image *background_image_onScreen;
gcn::Icon* background_onScreen;
gcn::CheckBox* checkBox_onscreen_control;

class OnScreenCheckBoxActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_onscreen_control) {
            if (checkBox_onscreen_control->isSelected())
                mainMenu_onScreen=1;
            else
                mainMenu_onScreen=0;
        }
    }
};
OnScreenCheckBoxActionListener* onScreenCheckBoxActionListener;

void menuOnScreen_Init()
{
    background_image_onScreen = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_onScreen = new gcn::Icon(background_image_onScreen);

    checkBox_onscreen_control = new gcn::CheckBox("On-screen control");
    checkBox_onscreen_control->setPosition(10,20);
    checkBox_onscreen_control->setId("OnScrCtrl");
    onScreenCheckBoxActionListener = new OnScreenCheckBoxActionListener();
    checkBox_onscreen_control->addActionListener(onScreenCheckBoxActionListener);

    window_onScreen = new gcn::Window("OnScreen");
    window_onScreen->setPosition(137,20);
    window_onScreen->setMovable(false);
    window_onScreen->setSize(500,400);
    window_onScreen->setBaseColor(baseCol);
    window_onScreen->setVisible(false);

    window_onScreen->add(background_onScreen);
    window_onScreen->add(checkBox_onscreen_control);
}

void menuOnScreen_Exit()
{
    delete onScreenCheckBoxActionListener;

    delete background_onScreen;
    delete background_image_onScreen;
    delete checkBox_onscreen_control;
    delete window_onScreen;
}

void show_settings_OnScreen()
{
    if (mainMenu_onScreen==0)
        checkBox_onscreen_control->setSelected(false);
    else if (mainMenu_onScreen==1)
        checkBox_onscreen_control->setSelected(true);
}

}
