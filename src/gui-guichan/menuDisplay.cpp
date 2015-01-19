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
#include "custom.h"
}

namespace widgets
{
void show_settings_Display(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_display;
gcn::Image *background_image_display;
gcn::Icon* background_display;

gcn::Container* backgrd_framerate;
gcn::Label* label_framerate;
gcn::DropDown* dropDown_framerate;

class framerateListModel : public gcn::ListModel
{
private:
    std::ostringstream ostr[6];

public:
    framerateListModel() {
        for (int j=0; j<6; j++)
            ostr[j] << j;
    }

    int getNumberOfElements() {
        return 5;
    }

    std::string getElementAt(int i) {
        return ostr[i].str().c_str();
    }
};
framerateListModel framerateList;

class FramerateActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        changed_prefs.gfx_framerate = dropDown_framerate->getSelected();
    }
};
FramerateActionListener* framerateActionListener;

void menuDisplay_Init()
{
    background_image_display = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_display = new gcn::Icon(background_image_display);

    label_framerate = new gcn::Label("Framerate");
    label_framerate->setPosition(4, 2);
    backgrd_framerate = new gcn::Container();
    backgrd_framerate->setOpaque(true);
    backgrd_framerate->setBaseColor(baseColLabel);
    backgrd_framerate->setPosition(20, 20);
    backgrd_framerate->setSize(85, 21);
    backgrd_framerate->add(label_framerate);

    dropDown_framerate = new gcn::DropDown(&framerateList);
    dropDown_framerate->setPosition(110,20);
    dropDown_framerate->setBaseColor(baseCol);
    dropDown_framerate->setId("finegrain");
    framerateActionListener = new FramerateActionListener();
    dropDown_framerate->addActionListener(framerateActionListener);

    window_display = new gcn::Window("Display");
    window_display->setPosition(137,20);
    window_display->setMovable(false);
    window_display->setSize(500,400);
    window_display->setBaseColor(baseCol);
    window_display->setVisible(false);

    window_display->add(background_display);
    window_display->add(backgrd_framerate);
    window_display->add(dropDown_framerate);
}

void menuDisplay_Exit()
{
    delete background_display;
    delete background_image_display;

    delete backgrd_framerate;
    delete label_framerate;
    delete dropDown_framerate;

    delete window_display;
}

void show_settings_Display()
{
    dropDown_framerate->setSelected(changed_prefs.gfx_framerate);
}

}
