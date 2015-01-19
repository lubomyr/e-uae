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
void show_settings_Sound(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_sound;
gcn::Image *background_image_sound;
gcn::Icon* background_sound;

void menuSound_Init()
{
    background_image_sound = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_sound = new gcn::Icon(background_image_sound);

    window_sound = new gcn::Window("Sound");
    window_sound->setPosition(137,20);
    window_sound->setMovable(false);
    window_sound->setSize(500,400);
    window_sound->setBaseColor(baseCol);
    window_sound->setVisible(false);

    window_sound->add(background_sound);
}

void menuSound_Exit()
{
    delete background_sound;
    delete background_image_sound;

    delete window_sound;
}

void show_settings_Sound()
{

}

}
