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

namespace widgets
{
void show_settings_Custom(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_custom;
gcn::Image *background_image_custom;
gcn::Icon* background_custom;

void menuCustom_Init()
{
    background_image_custom = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_custom = new gcn::Icon(background_image_custom);

    window_custom = new gcn::Window("Custom");
    window_custom->setPosition(137,20);
    window_custom->setMovable(false);
    window_custom->setSize(500,400);
    window_custom->setBaseColor(baseCol);
    window_custom->setVisible(false);

    window_custom->add(background_custom);
}

void menuCustom_Exit()
{
    delete background_custom;
    delete background_image_custom;

    delete window_custom;
}

void show_settings_Custom()
{

}

}
