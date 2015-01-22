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
#include "savestate.h"
#include "disk.h"
#include "filesys.h"
}

extern bool guiActive;
extern int guiOpenScreen;

#define extterms files[q].size()>=4 && files[q].substr(files[q].size()-4)
#define ssterms extterms!=".uss" && extterms!=".USS" && extterms!=".Uss"
#define ext2terms ssstring.size()>=4 && ssstring.substr(ssstring.size()-4)
#define ss2terms ext2terms!=".uss" && ext2terms!=".USS" && ext2terms!=".Uss"

static char savestate_filename_default[255]= {
    '/', 't', 'm', 'p', '/', 'n', 'u', 'l', 'l', '.', 'u', 's', 's', '\0'
};

char *savestate_filename=(char *)&savestate_filename_default[0];

extern char launchDir[300];

static void BuildBaseDir(char *filename)
{
    strcpy(filename, "");
    strcat(filename, launchDir);
    strcat(filename, "/savestates");
//        strcpy(filename, prefs_get_attr ("savestate_path"));
}

namespace widgets
{
void show_settings_SaveState(void);
void showWarning(const char *msg, const char *msg2 = "");
void showInfo(const char *msg, const char *msg2 = "");

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_savestate;
gcn::Image *background_image_savestate;
gcn::Icon* background_savestate;

gcn::Button* button_ss_load;
gcn::Button* button_ss_save;
gcn::Button* button_ss_delete;
gcn::TextField* textField_ss;
gcn::ListBox* savestatelistBox;
gcn::ScrollArea* savestateScrollArea;
extern gcn::TextField* textTempSavestate;

class SavestateListModel : public gcn::ListModel
{
    std::vector<std::string> files;

public:
    SavestateListModel(const char * path) {
        changeDir(path);
    }

    int getNumberOfElements() {
        return files.size();
    }

    std::string getElementAt(int i) {
        if(i >= files.size() || i < 0)
            return "---";
        return files[i];
    }

    void changeDir(const char * path) {
        files.clear();
        DIR *dir;
        struct dirent *dent;
        dir = opendir(path);
        if(dir != NULL) {
            while((dent=readdir(dir))!=NULL) {
                if(!(dent->d_type == DT_DIR))
                    files.push_back(dent->d_name);
            }
            closedir(dir);
        }
        std::sort(files.begin(), files.end());
        for (int q=0; q<files.size(); q++) {
            if (ssterms) {
                files.erase(files.begin()+q);
                q--;
            }
        }
    }
};
SavestateListModel savestateList("savestates");
//SavestateListModel savestateList(prefs_get_attr ("savestate_path"));

class SsLoadButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        selected_item = savestatelistBox->getSelected();
        BuildBaseDir(savestate_filename);
        strcat(savestate_filename, "/");
        strcat(savestate_filename, savestateList.getElementAt(selected_item).c_str());
        guiActive=false;
        savestate_state = STATE_DORESTORE;
        restore_state (savestate_filename);
//        showInfo("Savestate file loaded.");
    }
};
SsLoadButtonActionListener* ssloadButtonActionListener;


class SsSaveButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (!guiOpenScreen) {
            char filename[256]="";
            std::string ssstring;

            BuildBaseDir(savestate_filename);
            strcat(savestate_filename, "/");
            ssstring = textField_ss->getText().c_str();
            strcat(savestate_filename, textField_ss->getText().c_str());
            // check extension of editable name
            if (ss2terms || ssstring.size()<5)
                strcat(savestate_filename, ".uss");
            savestate_state = STATE_DOSAVE;
            guiActive=false;
            save_state(savestate_filename, "");
//			showInfo("Savestate file saved.");
            BuildBaseDir(savestate_filename);
            savestateList = savestate_filename;
        } else
            showWarning("Please Start emulation");

    }
};
SsSaveButtonActionListener* sssaveButtonActionListener;

class SsDeleteButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        selected_item = savestatelistBox->getSelected();
        BuildBaseDir(savestate_filename);
        strcat(savestate_filename, "/");
        strcat(savestate_filename, savestateList.getElementAt(selected_item).c_str());
        if(unlink(savestate_filename)) {
            showWarning("Failed to delete savestate.");
        } else {
            BuildBaseDir(savestate_filename);
            savestateList = savestate_filename;
        }
    }
};
SsDeleteButtonActionListener* ssdeleteButtonActionListener;

class SavestatelistBoxActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        selected_item = savestatelistBox->getSelected();
        textField_ss->setText(savestateList.getElementAt(selected_item).c_str());
    }
};
SavestatelistBoxActionListener* savestatelistBoxActionListener;

void menuSaveState_Init()
{
    background_image_savestate = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_savestate = new gcn::Icon(background_image_savestate);

    button_ss_load = new gcn::Button("Load");
    button_ss_load->setId("ssLoad");
    button_ss_load->setPosition(416,10);
    button_ss_load->setSize(70, 26);
    button_ss_load->setBaseColor(baseCol);
    ssloadButtonActionListener = new SsLoadButtonActionListener();
    button_ss_load->addActionListener(ssloadButtonActionListener);
    button_ss_save = new gcn::Button("Save");
    button_ss_save->setId("ssSave");
    button_ss_save->setPosition(416,65);
    button_ss_save->setSize(70, 26);
    button_ss_save->setBaseColor(baseCol);
    sssaveButtonActionListener = new SsSaveButtonActionListener();
    button_ss_save->addActionListener(sssaveButtonActionListener);
    button_ss_delete = new gcn::Button("Delete");
    button_ss_delete->setId("ssDelete");
    button_ss_delete->setPosition(416,120);
    button_ss_delete->setSize(70, 26);
    button_ss_delete->setBaseColor(baseCol);
    ssdeleteButtonActionListener = new SsDeleteButtonActionListener();
    button_ss_delete->addActionListener(ssdeleteButtonActionListener);

    textField_ss = new gcn::TextField("");
    textField_ss->setId("ssText");
    textField_ss->setPosition(10,10);
    textField_ss->setSize(395,22);
    textField_ss->setBaseColor(baseCol);

    savestatelistBox = new gcn::ListBox(&savestateList);
    savestatelistBox->setId("configList");
    savestatelistBox->setSize(650,245);
    savestatelistBox->setBaseColor(baseCol);
    savestatelistBox->setWrappingEnabled(true);
    savestatelistBoxActionListener = new SavestatelistBoxActionListener();
    savestatelistBox->addActionListener(savestatelistBoxActionListener);
    savestateScrollArea = new gcn::ScrollArea(savestatelistBox);
    savestateScrollArea->setFrameSize(1);
    savestateScrollArea->setPosition(10,40);
    savestateScrollArea->setSize(395,283);
    savestateScrollArea->setScrollbarWidth(20);
    savestateScrollArea->setBaseColor(baseCol);

    window_savestate = new gcn::Window("SaveState");
    window_savestate->setPosition(137,20);
    window_savestate->setMovable(false);
    window_savestate->setSize(500,400);
    window_savestate->setBaseColor(baseCol);
    window_savestate->setVisible(false);

    window_savestate->add(background_savestate);
    window_savestate->add(button_ss_load);
    window_savestate->add(button_ss_save);
    window_savestate->add(button_ss_delete);
    window_savestate->add(textField_ss);
    window_savestate->add(savestateScrollArea);
}

void menuSaveState_Exit()
{
    delete background_savestate;
    delete background_image_savestate;

    delete button_ss_load;
    delete button_ss_save;
    delete button_ss_delete;
    delete textField_ss;
    delete savestatelistBox;
    delete savestateScrollArea;

    delete window_savestate;
}

void show_settings_SaveState()
{

}

}
