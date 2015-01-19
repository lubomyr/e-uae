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
#include "memory-uae.h"
#include "custom.h"
#include "newcpu.h"
}

int finegrain_backup=32;

namespace widgets
{
void show_settings_CpuFpu(void);

extern void run_menuLoad_guichan(char *curr_path, int aLoadType);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::TabbedArea* tabbedArea;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_cpufpu;
gcn::Image *background_image_cpufpu;
gcn::Icon* background_cpufpu;
gcn::Window *group_cpulevel;
gcn::RadioButton* radioButton_cpu68000;
gcn::RadioButton* radioButton_cpu68010;
gcn::RadioButton* radioButton_cpu68020;
gcn::RadioButton* radioButton_cpu68040;
gcn::RadioButton* radioButton_cpu68060;
gcn::CheckBox* checkBox_fpu;
gcn::Window *group_cpuspeed;
gcn::RadioButton* radioButton_speedMax;
gcn::RadioButton* radioButton_speedReal;
gcn::RadioButton* radioButton_speed1;
gcn::RadioButton* radioButton_speed2;
gcn::RadioButton* radioButton_finegrain;
gcn::Container* backgrd_finegrain;
gcn::Label* label_finegrain;
gcn::DropDown* dropDown_finegrain;
gcn::CheckBox* checkBox_address_space_24;
gcn::CheckBox* checkBox_cpu_compatible;
gcn::CheckBox* checkBox_cpu_cycle_exact;

class finegrainListModel : public gcn::ListModel
{
private:
    std::string ostr[6]= {"32","64","128","192","256","384"};

public:
    int getNumberOfElements() {
        return 6;
    }

    std::string getElementAt(int i) {
        return ostr[i].c_str();
    }
};
finegrainListModel finegrainList;

class CPUButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_cpu68000)
            changed_prefs.cpu_level = 0;
        else if (actionEvent.getSource() == radioButton_cpu68010)
            changed_prefs.cpu_level = 1;
        else if (actionEvent.getSource() == radioButton_cpu68020) {
            if (checkBox_fpu->isSelected())
                changed_prefs.cpu_level = 3;
            else
                changed_prefs.cpu_level = 2;
        } else if (actionEvent.getSource() == radioButton_cpu68040)
            changed_prefs.cpu_level = 4;
        else if (actionEvent.getSource() == radioButton_cpu68060)
            changed_prefs.cpu_level = 6;
        if (actionEvent.getSource() == checkBox_fpu)
            if (checkBox_fpu->isSelected())
                changed_prefs.cpu_level = 3;
            else
                changed_prefs.cpu_level = 2;
        show_settings_CpuFpu();
    }
};
CPUButtonActionListener* cpuButtonActionListener;

class CPUSpeedActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_speedMax)
            changed_prefs.m68k_speed = -1;
        else if (actionEvent.getSource() == radioButton_speedReal)
            changed_prefs.m68k_speed = 0;
        else if (actionEvent.getSource() == radioButton_speed1)
            changed_prefs.m68k_speed = 512;
        else if (actionEvent.getSource() == radioButton_speed2)
            changed_prefs.m68k_speed = 1024;
        else if (actionEvent.getSource() == radioButton_finegrain) {
            backgrd_finegrain->setVisible(true);
            dropDown_finegrain->setVisible(true);
            changed_prefs.m68k_speed = finegrain_backup;
        }
        show_settings_CpuFpu();
    }
};
CPUSpeedActionListener* cpuSpeedActionListener;

class FinegrainActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int fstr[6]= {32,64,128,192,256,384};
        changed_prefs.m68k_speed = fstr[dropDown_finegrain->getSelected()];
        finegrain_backup = changed_prefs.m68k_speed;
        show_settings_CpuFpu();
    }
};
FinegrainActionListener* finegrainActionListener;

class CPUAddr24ActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_address_space_24)
            if (changed_prefs.cpu_level<=3)
                if (checkBox_address_space_24->isSelected())
                    changed_prefs.address_space_24 = 1;
                else
                    changed_prefs.address_space_24 = 0;
        show_settings_CpuFpu();
    }
};
CPUAddr24ActionListener* cpuAddr24ActionListener;

class CPUCompatibleActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_cpu_compatible)
            if (checkBox_cpu_compatible->isSelected())
                changed_prefs.cpu_compatible = 1;
            else
                changed_prefs.cpu_compatible = 0;
    }
};
CPUCompatibleActionListener* cpuCompatibleActionListener;

class CPUCycleExactActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_cpu_cycle_exact)
            if (checkBox_cpu_cycle_exact->isSelected())
                changed_prefs.cpu_cycle_exact = 1;
            else
                changed_prefs.cpu_cycle_exact = 0;
    }
};
CPUCycleExactActionListener* cpuCycleExactActionListener;

void menuCpuFpu_Init()
{
    background_image_cpufpu = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_cpufpu = new gcn::Icon(background_image_cpufpu);

    // Select CPU level
    radioButton_cpu68000 = new gcn::RadioButton("68000", "radiocpugroup");
    radioButton_cpu68000->setPosition(5,10);
    radioButton_cpu68000->setId("68000");
    radioButton_cpu68010 = new gcn::RadioButton("68010", "radiocpugroup");
    radioButton_cpu68010->setPosition(5,40);
    radioButton_cpu68010->setId("68010");
    radioButton_cpu68020 = new gcn::RadioButton("68020", "radiocpugroup");
    radioButton_cpu68020->setPosition(5,70);
    radioButton_cpu68020->setId("68020");
    radioButton_cpu68040 = new gcn::RadioButton("68040", "radiocpugroup");
    radioButton_cpu68040->setPosition(5,100);
    radioButton_cpu68040->setId("68040");
    radioButton_cpu68060 = new gcn::RadioButton("68060", "radiocpugroup");
    radioButton_cpu68060->setPosition(5,130);
    radioButton_cpu68060->setId("68060");
    cpuButtonActionListener = new CPUButtonActionListener();
    radioButton_cpu68000->addActionListener(cpuButtonActionListener);
    radioButton_cpu68010->addActionListener(cpuButtonActionListener);
    radioButton_cpu68020->addActionListener(cpuButtonActionListener);
    radioButton_cpu68040->addActionListener(cpuButtonActionListener);
    radioButton_cpu68060->addActionListener(cpuButtonActionListener);
    group_cpulevel = new gcn::Window("CPU");
    group_cpulevel->setPosition(10,20);
    group_cpulevel->add(radioButton_cpu68000);
    group_cpulevel->add(radioButton_cpu68010);
    group_cpulevel->add(radioButton_cpu68020);
    group_cpulevel->add(radioButton_cpu68040);
    group_cpulevel->add(radioButton_cpu68060);
    group_cpulevel->setMovable(false);
    group_cpulevel->setSize(90,175);
    group_cpulevel->setBaseColor(baseCol);

    checkBox_fpu = new gcn::CheckBox("Fpu");
    checkBox_fpu->setPosition(15,210);
    checkBox_fpu->setId("fpu");
    checkBox_fpu->addActionListener(cpuButtonActionListener);

    checkBox_address_space_24 = new gcn::CheckBox("24-bit addresing");
    checkBox_address_space_24->setPosition(15,240);
    checkBox_address_space_24->setId("24bitAddr");
    cpuAddr24ActionListener = new CPUAddr24ActionListener();
    checkBox_address_space_24->addActionListener(cpuAddr24ActionListener);

    checkBox_cpu_compatible = new gcn::CheckBox("More compatible");
    checkBox_cpu_compatible->setPosition(15,270);
    checkBox_cpu_compatible->setId("CpuCompatible");
    cpuCompatibleActionListener = new CPUCompatibleActionListener();
    checkBox_cpu_compatible->addActionListener(cpuCompatibleActionListener);

    checkBox_cpu_cycle_exact = new gcn::CheckBox("Cycle-exact CPU");
    checkBox_cpu_cycle_exact->setPosition(15,300);
    checkBox_cpu_cycle_exact->setId("CpuCycleExact");
    cpuCycleExactActionListener = new CPUCycleExactActionListener();
    checkBox_cpu_cycle_exact->addActionListener(cpuCycleExactActionListener);

    // Select CPU speed
    radioButton_speedMax = new gcn::RadioButton("Max", "radiospeedgroup");
    radioButton_speedMax->setPosition(5,10);
    radioButton_speedMax->setId("max");
    radioButton_speedReal = new gcn::RadioButton("Real", "radiospeedgroup");
    radioButton_speedReal->setPosition(5,40);
    radioButton_speedReal->setId("real");
    radioButton_speed1 = new gcn::RadioButton("1", "radiospeedgroup");
    radioButton_speed1->setPosition(5,70);
    radioButton_speed1->setId("speed1");
    radioButton_speed2 = new gcn::RadioButton("2", "radiospeedgroup");
    radioButton_speed2->setPosition(5,100);
    radioButton_speed2->setId("speed2");
    radioButton_finegrain = new gcn::RadioButton("finegrain", "radiospeedgroup");
    radioButton_finegrain->setPosition(5,130);
    radioButton_finegrain->setId("RBfinegrain");
    cpuSpeedActionListener = new CPUSpeedActionListener();
    radioButton_speedMax->addActionListener(cpuSpeedActionListener);
    radioButton_speedReal->addActionListener(cpuSpeedActionListener);
    radioButton_speed1->addActionListener(cpuSpeedActionListener);
    radioButton_speed2->addActionListener(cpuSpeedActionListener);
    radioButton_finegrain->addActionListener(cpuSpeedActionListener);
    group_cpuspeed = new gcn::Window("CPU speed");
    group_cpuspeed->setPosition(120,20);
    group_cpuspeed->add(radioButton_speedMax);
    group_cpuspeed->add(radioButton_speedReal);
    group_cpuspeed->add(radioButton_speed1);
    group_cpuspeed->add(radioButton_speed2);
    group_cpuspeed->add(radioButton_finegrain);
    group_cpuspeed->setMovable(false);
    group_cpuspeed->setSize(110,175);
    group_cpuspeed->setBaseColor(baseCol);

    label_finegrain = new gcn::Label("Finegrain");
    label_finegrain->setPosition(4, 2);
    backgrd_finegrain = new gcn::Container();
    backgrd_finegrain->setOpaque(true);
    backgrd_finegrain->setBaseColor(baseColLabel);
    backgrd_finegrain->setPosition(90, 210);
    backgrd_finegrain->setSize(70, 21);
    backgrd_finegrain->add(label_finegrain);
    backgrd_finegrain->setVisible(false);

    dropDown_finegrain = new gcn::DropDown(&finegrainList);
    dropDown_finegrain->setPosition(170,210);
    dropDown_finegrain->setBaseColor(baseCol);
    dropDown_finegrain->setId("finegrain");
    dropDown_finegrain->setVisible(false);
    finegrainActionListener = new FinegrainActionListener();
    dropDown_finegrain->addActionListener(finegrainActionListener);

    window_cpufpu = new gcn::Window("CPU / FPU");
    window_cpufpu->setPosition(137,20);
    window_cpufpu->setMovable(false);
    window_cpufpu->setSize(500,400);
    window_cpufpu->setBaseColor(baseCol);
    window_cpufpu->setVisible(false);

    window_cpufpu->add(background_cpufpu);
    window_cpufpu->add(group_cpulevel);
    window_cpufpu->add(group_cpuspeed);
    window_cpufpu->add(checkBox_fpu);
    window_cpufpu->add(checkBox_address_space_24);
    window_cpufpu->add(checkBox_cpu_compatible);
    window_cpufpu->add(checkBox_cpu_cycle_exact);
    window_cpufpu->add(backgrd_finegrain);
    window_cpufpu->add(dropDown_finegrain);
}

void menuCpuFpu_Exit()
{
    delete cpuButtonActionListener;
    delete cpuSpeedActionListener;
    delete finegrainActionListener;
    delete cpuAddr24ActionListener;
    delete cpuCompatibleActionListener;
    delete cpuCycleExactActionListener;


    delete background_cpufpu;
    delete background_image_cpufpu;

    delete group_cpulevel;
    delete radioButton_cpu68000;
    delete radioButton_cpu68010;
    delete radioButton_cpu68020;
    delete radioButton_cpu68040;
    delete radioButton_cpu68060;
    delete checkBox_fpu;
    delete group_cpuspeed;
    delete radioButton_speedMax;
    delete radioButton_speedReal;
    delete radioButton_speed1;
    delete radioButton_speed2;
    delete radioButton_finegrain;
    delete backgrd_finegrain;
    delete label_finegrain;
    delete dropDown_finegrain;
    delete checkBox_address_space_24;
    delete checkBox_cpu_compatible;
    delete checkBox_cpu_cycle_exact;

    delete window_cpufpu;
}

void show_settings_CpuFpu()
{
    if (changed_prefs.cpu_level==0)
        radioButton_cpu68000->setSelected(true);
    else if (changed_prefs.cpu_level==1)
        radioButton_cpu68010->setSelected(true);
    else if (changed_prefs.cpu_level==2 || changed_prefs.cpu_level==3)
        radioButton_cpu68020->setSelected(true);
    else if (changed_prefs.cpu_level==4)
        radioButton_cpu68040->setSelected(true);
    else if (changed_prefs.cpu_level==6)
        radioButton_cpu68060->setSelected(true);

    if (changed_prefs.cpu_level>=3)
        checkBox_fpu->setSelected(true);
    else
        checkBox_fpu->setSelected(false);

    if (changed_prefs.m68k_speed==-1)
        radioButton_speedMax->setSelected(true);
    else if (changed_prefs.m68k_speed==0)
        radioButton_speedReal->setSelected(true);
    else if (changed_prefs.m68k_speed==512)
        radioButton_speed1->setSelected(true);
    else if (changed_prefs.m68k_speed==1024)
        radioButton_speed2->setSelected(true);
    else if (changed_prefs.m68k_speed>0 || changed_prefs.m68k_speed<512) {
        radioButton_finegrain->setSelected(true);
        backgrd_finegrain->setVisible(true);
        dropDown_finegrain->setVisible(true);
        int fstr[6]= {32,64,128,192,256,384};
        for (int i=0; i<6; i++) {
            if (changed_prefs.m68k_speed==fstr[i])
                dropDown_finegrain->setSelected(i);
        }
    }
    if (changed_prefs.m68k_speed<=0 || changed_prefs.m68k_speed>=512) {
        backgrd_finegrain->setVisible(false);
        dropDown_finegrain->setVisible(false);
    }

    if (changed_prefs.address_space_24!=0)
        checkBox_address_space_24->setSelected(true);
    else
        checkBox_address_space_24->setSelected(false);
    if (changed_prefs.cpu_compatible!=0)
        checkBox_cpu_compatible->setSelected(true);
    else
        checkBox_cpu_compatible->setSelected(false);
    if (changed_prefs.cpu_cycle_exact!=0)
        checkBox_cpu_cycle_exact->setSelected(true);
    else
        checkBox_cpu_cycle_exact->setSelected(false);
}

}
