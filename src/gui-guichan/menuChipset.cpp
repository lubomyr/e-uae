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
//#include "rw_config.h"

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "custom.h"
}

namespace widgets
{
void show_settings_Chipset(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_chipset;
gcn::Image *background_image_chipset;
gcn::Icon* background_chipset;

gcn::Window *group_chipset;
gcn::RadioButton* radioButton_chipsetocs;
gcn::RadioButton* radioButton_chipsetecsagnus;
gcn::RadioButton* radioButton_chipsetecsfull;
gcn::RadioButton* radioButton_chipsetaga;
gcn::Window *group_collevel;
gcn::RadioButton* radioButton_col_none;
gcn::RadioButton* radioButton_col_sprites;
gcn::RadioButton* radioButton_col_playfield;
gcn::RadioButton* radioButton_col_full;
gcn::CheckBox* checkBox_ntsc;
gcn::CheckBox* checkBox_immediate_blits;
gcn::CheckBox* checkBox_blitter_cycle_exact;

class ChipsetButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_chipsetocs)
            changed_prefs.chipset_mask = 0;
        else if (actionEvent.getSource() == radioButton_chipsetecsagnus)
            changed_prefs.chipset_mask = 1;
        else if (actionEvent.getSource() == radioButton_chipsetecsfull)
            changed_prefs.chipset_mask = 3;
        else if (actionEvent.getSource() == radioButton_chipsetaga)
            changed_prefs.chipset_mask = 7;
    }
};
ChipsetButtonActionListener* chipsetButtonActionListener;

class CollevelButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_col_none)
            changed_prefs.collision_level = 0;
        else if (actionEvent.getSource() == radioButton_col_sprites)
            changed_prefs.collision_level = 1;
        else if (actionEvent.getSource() == radioButton_col_playfield)
            changed_prefs.collision_level = 2;
        else if (actionEvent.getSource() == radioButton_col_full)
            changed_prefs.collision_level = 3;
    }
};
CollevelButtonActionListener* collevelButtonActionListener;

class NTSCModeActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_ntsc)
            if (checkBox_ntsc->isSelected())
                changed_prefs.ntscmode = 1;
            else
                changed_prefs.ntscmode = 0;
    }
};
NTSCModeActionListener* ntscModeActionListener;

class BlitterCycleExactActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_immediate_blits)
            if (checkBox_immediate_blits->isSelected())
                changed_prefs.immediate_blits = 1;
            else
                changed_prefs.immediate_blits = 0;
    }
};
BlitterCycleExactActionListener* blitterCycleExactActionListener;

class ImmediateBlitsActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_blitter_cycle_exact)
            if (checkBox_blitter_cycle_exact->isSelected())
                changed_prefs.blitter_cycle_exact = 1;
            else
                changed_prefs.blitter_cycle_exact = 0;
    }
};
ImmediateBlitsActionListener* immediateBlitsActionListener;

void menuChipset_Init()
{
    background_image_chipset = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_chipset = new gcn::Icon(background_image_chipset);

    // Select Chipset
    radioButton_chipsetocs = new gcn::RadioButton("OCS", "radiochipsetgroup");
    radioButton_chipsetocs->setPosition(5,10);
    radioButton_chipsetocs->setId("OCS");
    radioButton_chipsetecsagnus = new gcn::RadioButton("ECS Agnus", "radiochipsetgroup");
    radioButton_chipsetecsagnus->setPosition(5,40);
    radioButton_chipsetecsagnus->setId("ECSAgnus");
    radioButton_chipsetecsfull = new gcn::RadioButton("Full ECS", "radiochipsetgroup");
    radioButton_chipsetecsfull->setPosition(5,70);
    radioButton_chipsetecsfull->setId("ECSFull");
    radioButton_chipsetaga = new gcn::RadioButton("AGA", "radiochipsetgroup");
    radioButton_chipsetaga->setPosition(5,100);
    radioButton_chipsetaga->setId("AGA");
    chipsetButtonActionListener = new ChipsetButtonActionListener();
    radioButton_chipsetocs->addActionListener(chipsetButtonActionListener);
    radioButton_chipsetecsagnus->addActionListener(chipsetButtonActionListener);
    radioButton_chipsetecsfull->addActionListener(chipsetButtonActionListener);
    radioButton_chipsetaga->addActionListener(chipsetButtonActionListener);
    group_chipset = new gcn::Window("Chipset");
    group_chipset->setPosition(10,10);
    group_chipset->add(radioButton_chipsetocs);
    group_chipset->add(radioButton_chipsetecsagnus);
    group_chipset->add(radioButton_chipsetecsfull);
    group_chipset->add(radioButton_chipsetaga);
    group_chipset->setMovable(false);
    group_chipset->setSize(125,145);
    group_chipset->setBaseColor(baseCol);

    // Select Collision level
    radioButton_col_none = new gcn::RadioButton("None", "radiocollevelgroup");
    radioButton_col_none->setPosition(5,10);
    radioButton_col_none->setId("OCS");
    radioButton_col_sprites = new gcn::RadioButton("sprites", "radiocollevelgroup");
    radioButton_col_sprites->setPosition(5,40);
    radioButton_col_sprites->setId("ECSAgnus");
    radioButton_col_playfield = new gcn::RadioButton("Playfield", "radiocollevelgroup");
    radioButton_col_playfield->setPosition(5,70);
    radioButton_col_playfield->setId("ECSFull");
    radioButton_col_full = new gcn::RadioButton("Full", "radiochipsetgroup");
    radioButton_col_full->setPosition(5,100);
    radioButton_col_full->setId("AGA");
    collevelButtonActionListener = new CollevelButtonActionListener();
    radioButton_col_none->addActionListener(collevelButtonActionListener);
    radioButton_col_sprites->addActionListener(collevelButtonActionListener);
    radioButton_col_playfield->addActionListener(collevelButtonActionListener);
    radioButton_col_full->addActionListener(collevelButtonActionListener);
    group_collevel = new gcn::Window("Collision level");
    group_collevel->setPosition(150,10);
    group_collevel->add(radioButton_col_none);
    group_collevel->add(radioButton_col_sprites);
    group_collevel->add(radioButton_col_playfield);
    group_collevel->add(radioButton_col_full);
    group_collevel->setMovable(false);
    group_collevel->setSize(125,145);
    group_collevel->setBaseColor(baseCol);

    checkBox_ntsc = new gcn::CheckBox("NTSC");
    checkBox_ntsc->setPosition(15,180);
    checkBox_ntsc->setId("ntsc");
    ntscModeActionListener = new NTSCModeActionListener();
    checkBox_ntsc->addActionListener(ntscModeActionListener);

    checkBox_immediate_blits = new gcn::CheckBox("Immediate blitter");
    checkBox_immediate_blits->setPosition(15,210);
    checkBox_immediate_blits->setId("24bitAddr");
    immediateBlitsActionListener = new ImmediateBlitsActionListener();
    checkBox_immediate_blits->addActionListener(immediateBlitsActionListener);

    checkBox_blitter_cycle_exact = new gcn::CheckBox("Blitter Cycle-exact");
    checkBox_blitter_cycle_exact->setPosition(15,240);
    checkBox_blitter_cycle_exact->setId("CpuCompatible");
    blitterCycleExactActionListener = new BlitterCycleExactActionListener();
    checkBox_blitter_cycle_exact->addActionListener(blitterCycleExactActionListener);

    window_chipset = new gcn::Window("Chipset");
    window_chipset->setPosition(137,20);
    window_chipset->setMovable(false);
    window_chipset->setSize(500,400);
    window_chipset->setBaseColor(baseCol);
    window_chipset->setVisible(false);

    window_chipset->add(background_chipset);
    window_chipset->add(group_chipset);
    window_chipset->add(group_collevel);
    window_chipset->add(checkBox_ntsc);
    window_chipset->add(checkBox_immediate_blits);
    window_chipset->add(checkBox_blitter_cycle_exact);
}

void menuChipset_Exit()
{
    delete chipsetButtonActionListener;
    delete collevelButtonActionListener;
    delete ntscModeActionListener;
    delete blitterCycleExactActionListener;
    delete immediateBlitsActionListener;

    delete background_chipset;
    delete background_image_chipset;

    delete group_chipset;
    delete radioButton_chipsetocs;
    delete radioButton_chipsetecsagnus;
    delete radioButton_chipsetecsfull;
    delete radioButton_chipsetaga;
    delete group_collevel;
    delete radioButton_col_none;
    delete radioButton_col_sprites;
    delete radioButton_col_playfield;
    delete radioButton_col_full;
    delete checkBox_ntsc;
    delete checkBox_immediate_blits;
    delete checkBox_blitter_cycle_exact;

    delete window_chipset;
}

void show_settings_Chipset()
{
    if (changed_prefs.chipset_mask == 0)
        radioButton_chipsetocs->setSelected(true);
    else if (changed_prefs.chipset_mask == 1)
        radioButton_chipsetecsagnus->setSelected(true);
    else if (changed_prefs.chipset_mask == 3)
        radioButton_chipsetecsfull->setSelected(true);
    else if (changed_prefs.chipset_mask == 7)
        radioButton_chipsetaga->setSelected(true);

    if (changed_prefs.collision_level == 0)
        radioButton_col_none->setSelected(true);
    else if (changed_prefs.collision_level == 1)
        radioButton_col_sprites->setSelected(true);
    else if (changed_prefs.collision_level == 2)
        radioButton_col_playfield->setSelected(true);
    else if (changed_prefs.collision_level == 3)
        radioButton_col_full->setSelected(true);

    if (changed_prefs.ntscmode!=0)
        checkBox_ntsc->setSelected(true);
    else
        checkBox_ntsc->setSelected(false);

    if (changed_prefs.immediate_blits!=0)
        checkBox_immediate_blits->setSelected(true);
    else
        checkBox_immediate_blits->setSelected(false);

    if (changed_prefs.blitter_cycle_exact!=0)
        checkBox_blitter_cycle_exact->setSelected(true);
    else
        checkBox_blitter_cycle_exact->setSelected(false);
}

}
