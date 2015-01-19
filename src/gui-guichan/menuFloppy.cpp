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

int current_drive=0;

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "disk.h"
}

void extractFileName(char * str,char *buffer)
{
    char *p=str+strlen(str)-1;
#ifdef WIN32
    while(*p != '\\')
#else
    while(*p != '/')
#endif
        p--;
    p++;
    strcpy(buffer,p);
}

namespace widgets
{
void show_settings_Floppy(void);

extern void run_menuLoad_guichan(char *curr_path, int aLoadType);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;
extern gcn::Widget* activateAfterClose;

// Tab Main
gcn::Window *window_floppy;
gcn::Image *background_image_floppy;
gcn::Icon* background_floppy;
gcn::Button* button_df0;
gcn::Button* button_df1;
gcn::Button* button_df2;
gcn::Button* button_df3;
gcn::TextField* textField_df0;
gcn::TextField* textField_df1;
gcn::TextField* textField_df2;
gcn::TextField* textField_df3;
gcn::Button* button_ejectdf0;
gcn::Button* button_ejectdf1;
gcn::Button* button_ejectdf2;
gcn::Button* button_ejectdf3;
gcn::Button* button_ejectallfloppy;
gcn::Window *group_floppy_number;
gcn::RadioButton* radioButton_floppy1;
gcn::RadioButton* radioButton_floppy2;
gcn::RadioButton* radioButton_floppy3;
gcn::RadioButton* radioButton_floppy4;
gcn::Window *group_floppy_speed;
gcn::RadioButton* radioButton_floppy_speed1x;
gcn::RadioButton* radioButton_floppy_speed2x;
gcn::RadioButton* radioButton_floppy_speed4x;
gcn::RadioButton* radioButton_floppy_speed8x;

class DFxButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        char tempPath[300];
        strcpy(tempPath, prefs_get_attr ("floppy_path"));
        strcpy(currentDir,tempPath);
        if (actionEvent.getSource() == button_df0) {
            current_drive=0;
            activateAfterClose = button_df0;
            run_menuLoad_guichan(currentDir, MENU_LOAD_FLOPPY);
        }
        if (actionEvent.getSource() == button_df1) {
            current_drive=1;
            activateAfterClose = button_df1;
            run_menuLoad_guichan(currentDir, MENU_LOAD_FLOPPY);
        }
        if (actionEvent.getSource() == button_df2) {
            current_drive=2;
            activateAfterClose = button_df2;
            run_menuLoad_guichan(currentDir, MENU_LOAD_FLOPPY);
        }
        if (actionEvent.getSource() == button_df3) {
            current_drive=3;
            activateAfterClose = button_df3;
            run_menuLoad_guichan(currentDir, MENU_LOAD_FLOPPY);
        }
    }
};
DFxButtonActionListener* dfxButtonActionListener;

class EjectButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == button_ejectallfloppy) {
            for (int i=0; i < 4 ; i++)
                strcpy(changed_prefs.df[i], "");
        }
        if (actionEvent.getSource() == button_ejectdf0) {
            strcpy(changed_prefs.df[0], "");
        }
        if (actionEvent.getSource() == button_ejectdf1) {
            strcpy(changed_prefs.df[1], "");
        }
        if (actionEvent.getSource() == button_ejectdf2) {
            strcpy(changed_prefs.df[2], "");
        }
        if (actionEvent.getSource() == button_ejectdf3) {
            strcpy(changed_prefs.df[3], "");
        }
        show_settings_Floppy();
    }
};
EjectButtonActionListener* ejectButtonActionListener;


class FloppyNumButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_floppy1)
            changed_prefs.nr_floppies=1;
        else if (actionEvent.getSource() == radioButton_floppy2)
            changed_prefs.nr_floppies=2;
        else if (actionEvent.getSource() == radioButton_floppy3)
            changed_prefs.nr_floppies=3;
        else if (actionEvent.getSource() == radioButton_floppy4)
            changed_prefs.nr_floppies=4;
        show_settings_Floppy();
    }
};
FloppyNumButtonActionListener* floppyNumButtonActionListener;


class FloppySpeedButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_floppy_speed1x)
            changed_prefs.floppy_speed=100;
        else if (actionEvent.getSource() == radioButton_floppy_speed2x)
            changed_prefs.floppy_speed=200;
        else if (actionEvent.getSource() == radioButton_floppy_speed4x)
            changed_prefs.floppy_speed=400;
        else if (actionEvent.getSource() == radioButton_floppy_speed8x)
            changed_prefs.floppy_speed=800;
    }
};
FloppySpeedButtonActionListener* floppySpeedButtonActionListener;

void menuFloppy_Init()
{
    background_image_floppy = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_floppy = new gcn::Icon(background_image_floppy);

    // Button and text for drives
    button_df0 = new gcn::Button("DF0");
    button_df0->setSize(34,22);
    button_df0->setPosition(10,20);
    button_df0->setBaseColor(baseCol);
    button_df0->setId("DF0");
    button_df1 = new gcn::Button("DF1");
    button_df1->setSize(34,22);
    button_df1->setPosition(10,60);
    button_df1->setBaseColor(baseCol);
    button_df1->setId("DF1");
    button_df2 = new gcn::Button("DF2");
    button_df2->setSize(34,22);
    button_df2->setPosition(10,100);
    button_df2->setBaseColor(baseCol);
    button_df2->setId("DF2");
    button_df3 = new gcn::Button("DF3");
    button_df3->setSize(34,22);
    button_df3->setPosition(10,140);
    button_df3->setBaseColor(baseCol);
    button_df3->setId("DF3");
    dfxButtonActionListener = new DFxButtonActionListener();
    button_df0->addActionListener(dfxButtonActionListener);
    button_df1->addActionListener(dfxButtonActionListener);
    button_df2->addActionListener(dfxButtonActionListener);
    button_df3->addActionListener(dfxButtonActionListener);
    textField_df0 = new gcn::TextField("insert disk image                                                            ");
    textField_df0->setSize(380,22);
    textField_df0->setPosition(50,20);
    textField_df0->setEnabled(false);
    textField_df0->setBaseColor(baseCol);
    textField_df1 = new gcn::TextField("insert disk image                                                            ");
    textField_df1->setSize(380,22);
    textField_df1->setPosition(50,60);
    textField_df1->setEnabled(false);
    textField_df1->setBaseColor(baseCol);
    textField_df2 = new gcn::TextField("insert disk image                                                            ");
    textField_df2->setSize(380,22);
    textField_df2->setPosition(50,100);
    textField_df2->setEnabled(false);
    textField_df2->setBaseColor(baseCol);
    textField_df3 = new gcn::TextField("insert disk image                                                            ");
    textField_df3->setSize(380,22);
    textField_df3->setPosition(50,140);
    textField_df3->setEnabled(false);
    textField_df3->setBaseColor(baseCol);
    button_ejectallfloppy = new gcn::Button("Eject All Drives");
    button_ejectallfloppy->setHeight(30);
    button_ejectallfloppy->setPosition(170,190);
    button_ejectallfloppy->setBaseColor(baseCol);
    button_ejectallfloppy->setId("Eject");
    ejectButtonActionListener = new EjectButtonActionListener();
    button_ejectallfloppy->addActionListener(ejectButtonActionListener);
    button_ejectdf0 = new gcn::Button("Eject");
    button_ejectdf0->setHeight(22);
    button_ejectdf0->setBaseColor(baseCol);
    button_ejectdf0->setPosition(440,20);
    button_ejectdf0->setId("ejectDF0");
    button_ejectdf1 = new gcn::Button("Eject");
    button_ejectdf1->setHeight(22);
    button_ejectdf1->setBaseColor(baseCol);
    button_ejectdf1->setPosition(440,60);
    button_ejectdf1->setId("ejectDF1");
    button_ejectdf2 = new gcn::Button("Eject");
    button_ejectdf2->setHeight(22);
    button_ejectdf2->setBaseColor(baseCol);
    button_ejectdf2->setPosition(440,100);
    button_ejectdf2->setId("ejectDF2");
    button_ejectdf3 = new gcn::Button("Eject");
    button_ejectdf3->setHeight(22);
    button_ejectdf3->setBaseColor(baseCol);
    button_ejectdf3->setPosition(440,140);
    button_ejectdf3->setId("ejectDF3");
    button_ejectdf0->addActionListener(ejectButtonActionListener);
    button_ejectdf1->addActionListener(ejectButtonActionListener);
    button_ejectdf2->addActionListener(ejectButtonActionListener);
    button_ejectdf3->addActionListener(ejectButtonActionListener);

    // Select Number of drives
    radioButton_floppy1 = new gcn::RadioButton("1", "radiofloppynrgroup");
    radioButton_floppy1->setPosition(5,10);
    radioButton_floppy1->setId("Drives1");
    radioButton_floppy2 = new gcn::RadioButton("2", "radiofloppynrgroup");
    radioButton_floppy2->setPosition(5,40);
    radioButton_floppy2->setId("Drives2");
    radioButton_floppy3 = new gcn::RadioButton("3", "radiofloppynrgroup");
    radioButton_floppy3->setPosition(5,70);
    radioButton_floppy3->setId("Drives3");
    radioButton_floppy4 = new gcn::RadioButton("4", "radiofloppynrgroup");
    radioButton_floppy4->setPosition(5,100);
    radioButton_floppy4->setId("Drives4");
    floppyNumButtonActionListener = new FloppyNumButtonActionListener();
    radioButton_floppy1->addActionListener(floppyNumButtonActionListener);
    radioButton_floppy2->addActionListener(floppyNumButtonActionListener);
    radioButton_floppy3->addActionListener(floppyNumButtonActionListener);
    radioButton_floppy4->addActionListener(floppyNumButtonActionListener);
    group_floppy_number = new gcn::Window("Drives");
    group_floppy_number->setPosition(20,190);
    group_floppy_number->add(radioButton_floppy1);
    group_floppy_number->add(radioButton_floppy2);
    group_floppy_number->add(radioButton_floppy3);
    group_floppy_number->add(radioButton_floppy4);
    group_floppy_number->setMovable(false);
    group_floppy_number->setSize(55,145);
    group_floppy_number->setBaseColor(baseCol);

    // Select Floppy speed
    radioButton_floppy_speed1x = new gcn::RadioButton("1x", "radiofloppyspeedgroup");
    radioButton_floppy_speed1x->setPosition(5,10);
    radioButton_floppy_speed1x->setId("Speed1x");
    radioButton_floppy_speed2x = new gcn::RadioButton("2x", "radiofloppyspeedgroup");
    radioButton_floppy_speed2x->setPosition(60,10);
    radioButton_floppy_speed2x->setId("Speed2x");
    radioButton_floppy_speed4x = new gcn::RadioButton("4x", "radiofloppyspeedgroup");
    radioButton_floppy_speed4x->setPosition(5,40);
    radioButton_floppy_speed4x->setId("Speed4x");
    radioButton_floppy_speed8x = new gcn::RadioButton("8x", "radiofloppyspeedgroup");
    radioButton_floppy_speed8x->setPosition(60,40);
    radioButton_floppy_speed8x->setId("Speed8x");
    floppySpeedButtonActionListener = new FloppySpeedButtonActionListener();
    radioButton_floppy_speed1x->addActionListener(floppySpeedButtonActionListener);
    radioButton_floppy_speed2x->addActionListener(floppySpeedButtonActionListener);
    radioButton_floppy_speed4x->addActionListener(floppySpeedButtonActionListener);
    radioButton_floppy_speed8x->addActionListener(floppySpeedButtonActionListener);
    group_floppy_speed = new gcn::Window("Floppy Speed");
    group_floppy_speed->setPosition(360,190);
    group_floppy_speed->add(radioButton_floppy_speed1x);
    group_floppy_speed->add(radioButton_floppy_speed2x);
    group_floppy_speed->add(radioButton_floppy_speed4x);
    group_floppy_speed->add(radioButton_floppy_speed8x);
    group_floppy_speed->setMovable(false);
    group_floppy_speed->setSize(115,85);
    group_floppy_speed->setBaseColor(baseCol);

    window_floppy = new gcn::Window("Floppy Drives");
    window_floppy->setPosition(137,20);
    window_floppy->setMovable(false);
    window_floppy->setSize(500,400);
    window_floppy->setBaseColor(baseCol);
    window_floppy->setVisible(false);

    window_floppy->add(background_floppy);
    window_floppy->add(button_df0);
    window_floppy->add(textField_df0);
    window_floppy->add(button_ejectdf0);
    window_floppy->add(button_df1);
    window_floppy->add(textField_df1);
    window_floppy->add(button_ejectdf1);
    window_floppy->add(button_df2);
    window_floppy->add(textField_df2);
    window_floppy->add(button_ejectdf2);
    window_floppy->add(button_df3);
    window_floppy->add(textField_df3);
    window_floppy->add(button_ejectdf3);
    window_floppy->add(button_ejectallfloppy);
    window_floppy->add(group_floppy_number);
    window_floppy->add(group_floppy_speed);

}

void menuFloppy_Exit()
{
    delete dfxButtonActionListener;
    delete ejectButtonActionListener;

    delete background_floppy;
    delete background_image_floppy;

    delete button_df0;
    delete button_df1;
    delete button_df2;
    delete button_df3;
    delete textField_df0;
    delete textField_df1;
    delete textField_df2;
    delete textField_df3;
    delete button_ejectdf0;
    delete button_ejectdf1;
    delete button_ejectdf2;
    delete button_ejectdf3;
    delete button_ejectallfloppy;
    delete group_floppy_number;
    delete radioButton_floppy1;
    delete radioButton_floppy2;
    delete radioButton_floppy3;
    delete radioButton_floppy4;
    delete group_floppy_speed;
    delete radioButton_floppy_speed1x;
    delete radioButton_floppy_speed2x;
    delete radioButton_floppy_speed4x;
    delete radioButton_floppy_speed8x;
    delete window_floppy;
}

void show_settings_Floppy()
{
    static char tmpDF0[256];
    static char tmpDF1[256];
    static char tmpDF2[256];
    static char tmpDF3[256];
    int nr_drives=changed_prefs.nr_floppies;
    if (nr_drives<4)
        textField_df3->setText("disabled");
    if (nr_drives<3)
        textField_df2->setText("disabled");
    if (nr_drives<2)
        textField_df1->setText("disabled");

    if(strcmp(changed_prefs.df[0], "")==0)
        textField_df0->setText("insert disk image");
    else {
        extractFileName(changed_prefs.df[0], tmpDF0);
        textField_df0->setText(tmpDF0);
    }
    if (nr_drives>1) {
        if (strcmp(changed_prefs.df[1], "")==0)
            textField_df1->setText("insert disk image");
        else {
            extractFileName(changed_prefs.df[1], tmpDF1);
            textField_df1->setText(tmpDF1);
        }
    }
    if (nr_drives>2) {
        if (strcmp(changed_prefs.df[2], "")==0)
            textField_df2->setText("insert disk image");
        else {
            extractFileName(changed_prefs.df[2], tmpDF2);
            textField_df2->setText(tmpDF2);
        }
    }
    if (nr_drives>3) {
        if (strcmp(changed_prefs.df[3], "")==0)
            textField_df3->setText("insert disk image");
        else {
            extractFileName(changed_prefs.df[3], tmpDF3);
            textField_df3->setText(tmpDF3);
        }
    }
    if (changed_prefs.nr_floppies==1)
        radioButton_floppy1->setSelected(true);
    else if (changed_prefs.nr_floppies==2)
        radioButton_floppy2->setSelected(true);
    else if (changed_prefs.nr_floppies==3)
        radioButton_floppy3->setSelected(true);
    else if (changed_prefs.nr_floppies==4)
        radioButton_floppy4->setSelected(true);

    if (changed_prefs.floppy_speed==100)
        radioButton_floppy_speed1x->setSelected(true);
    else if (changed_prefs.floppy_speed==200)
        radioButton_floppy_speed2x->setSelected(true);
    else if (changed_prefs.floppy_speed==400)
        radioButton_floppy_speed4x->setSelected(true);
    else if (changed_prefs.floppy_speed==800)
        radioButton_floppy_speed8x->setSelected(true);
}

}
