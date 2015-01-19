#include <algorithm>
#ifdef ANDROIDSDL
#include <android/log.h>
#include <SDL_screenkeyboard.h>
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
#include "disk.h"
#include "filesys.h"
}

#define extterms files[q].size()>=4 && files[q].substr(files[q].size()-4)
#define configterms extterms!=".uae" && extterms!=".UAE" && extterms!=".Uae"
#define ext2terms configstring.size()>=4 && configstring.substr(configstring.size()-4)
#define config2terms ext2terms!=".uae" && ext2terms!=".UAE" && ext2terms!=".Uae"

#ifdef WIN23
static char config_filename_default[255]= {
    '\\', 't', 'm', 'p', '\\', 'n', 'u', 'l', 'l', '.', 'u', 'a', 'e', '\0'
};
#else
static char config_filename_default[255]= {
    '/', 't', 'm', 'p', '/', 'n', 'u', 'l', 'l', '.', 'u', 'a', 'e', '\0'
};
#endif
char *config_filename=(char *)&config_filename_default[0];

extern char launchDir[300];

static void BuildBaseDir(char *filename)
{
    strcpy(filename, "");
    strcat(filename, launchDir);
#ifdef WIN23
    strcat(filename, "\configurations");
#else
    strcat(filename, "/configurations");
#endif
}

namespace widgets
{
void show_settings_Config(void);
void showWarning(const char *msg, const char *msg2 = "");
void showInfo(const char *msg, const char *msg2 = "");

extern void run_menuLoad_guichan(char *curr_path, int aLoadType);
extern void show_settings(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Config
gcn::Window *window_config;
gcn::Image *background_image_config;
gcn::Icon* background_config;
gcn::Button* button_cfg_load;
gcn::Button* button_cfg_save;
gcn::Button* button_cfg_save_global;
gcn::Button* button_cfg_delete;
gcn::TextField* textField_config;
gcn::ListBox* configlistBox;
gcn::ScrollArea* configScrollArea;

class ConfigListModel : public gcn::ListModel
{
    std::vector<std::string> files;

public:
    ConfigListModel(const char * path) {
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
#ifdef ANDROIDSDL
        if (menuLoad_extfilter==1)
#endif
            for (int q=0; q<files.size(); q++) {
                if (configterms) {
                    files.erase(files.begin()+q);
                    q--;
                }
            }
    }
};
ConfigListModel configList("configurations");

class CfgLoadButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        selected_item = configlistBox->getSelected();
        BuildBaseDir(config_filename);
#ifdef WIN23
        strcat(config_filename, "\\");
#else
        strcat(config_filename, "/");
#endif
        strcat(config_filename, configList.getElementAt(selected_item).c_str());
        free_mountinfo (currprefs.mountinfo);
        cfgfile_load(&changed_prefs, config_filename, 0);
        showInfo("Config file loaded.");
        show_settings();
    }
};
CfgLoadButtonActionListener* cfgloadButtonActionListener;


class CfgSaveButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        char filename[256]="";
        std::string configstring;

        BuildBaseDir(config_filename);
#ifdef WIN23
        strcat(config_filename, "\\");
#else
        strcat(config_filename, "/");
#endif
        configstring = textField_config->getText().c_str();
        strcat(config_filename, textField_config->getText().c_str());
        // check extension of editable name
        if (config2terms || configstring.size()<5)
            strcat(config_filename, ".uae");
        cfgfile_save(&changed_prefs, config_filename, 0);
        showInfo("Config file saved.");
        BuildBaseDir(config_filename);
        configList = config_filename;

    }
};
CfgSaveButtonActionListener* cfgsaveButtonActionListener;

class CfgSaveGlobalButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        strcpy(config_filename, "");
        strcat(config_filename, launchDir);
#ifdef WIN32
        strcat(config_filename, "/uae.rc");
#else
        strcat(config_filename, "/.uaerc");
#endif
        cfgfile_save(&changed_prefs, config_filename, 0);
        showInfo("Global Config saved.");
    }
};
CfgSaveGlobalButtonActionListener* cfgsaveGlobalButtonActionListener;

class CfgDeleteButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        selected_item = configlistBox->getSelected();
        BuildBaseDir(config_filename);
#ifdef WIN23
        strcat(config_filename, "\\");
#else
        strcat(config_filename, "/");
#endif
        strcat(config_filename, configList.getElementAt(selected_item).c_str());
        if(unlink(config_filename)) {
            showWarning("Failed to delete config.");
        } else {
            BuildBaseDir(config_filename);
            configList = config_filename;
        }
    }
};
CfgDeleteButtonActionListener* cfgdeleteButtonActionListener;

class ConfiglistBoxActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        selected_item = configlistBox->getSelected();
        textField_config->setText(configList.getElementAt(selected_item).c_str());
    }
};
ConfiglistBoxActionListener* configlistBoxActionListener;

void menuConfig_Init()
{
    background_image_config= gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_config = new gcn::Icon(background_image_config);

    button_cfg_load = new gcn::Button("Load");
    button_cfg_load->setId("cfgLoad");
    button_cfg_load->setPosition(416,10);
    button_cfg_load->setSize(70, 26);
    button_cfg_load->setBaseColor(baseCol);
    cfgloadButtonActionListener = new CfgLoadButtonActionListener();
    button_cfg_load->addActionListener(cfgloadButtonActionListener);
    button_cfg_save = new gcn::Button("Save");
    button_cfg_save->setId("cfgSave");
    button_cfg_save->setPosition(416,55);
    button_cfg_save->setSize(70, 26);
    button_cfg_save->setBaseColor(baseCol);
    cfgsaveButtonActionListener = new CfgSaveButtonActionListener();
    button_cfg_save->addActionListener(cfgsaveButtonActionListener);
    button_cfg_delete = new gcn::Button("Delete");
    button_cfg_delete->setId("cfgDelete");
    button_cfg_delete->setPosition(416,100);
    button_cfg_delete->setSize(70, 26);
    button_cfg_delete->setBaseColor(baseCol);
    cfgdeleteButtonActionListener = new CfgDeleteButtonActionListener();
    button_cfg_delete->addActionListener(cfgdeleteButtonActionListener);
    button_cfg_save_global = new gcn::Button("Save Global Config");
    button_cfg_save_global->setId("cfgSave");
    button_cfg_save_global->setPosition(346,315);
    button_cfg_save_global->setSize(140, 26);
    button_cfg_save_global->setBaseColor(baseCol);
    cfgsaveGlobalButtonActionListener = new CfgSaveGlobalButtonActionListener();
    button_cfg_save_global->addActionListener(cfgsaveGlobalButtonActionListener);

    textField_config = new gcn::TextField("");
    textField_config->setId("cfgText");
    textField_config->setPosition(10,10);
    textField_config->setSize(395,22);
    textField_config->setBaseColor(baseCol);

    configlistBox = new gcn::ListBox(&configList);
    configlistBox->setId("configList");
    configlistBox->setSize(650,225);
    configlistBox->setBaseColor(baseCol);
    configlistBox->setWrappingEnabled(true);
    configlistBoxActionListener = new ConfiglistBoxActionListener();
    configlistBox->addActionListener(configlistBoxActionListener);
    configScrollArea = new gcn::ScrollArea(configlistBox);
    configScrollArea->setFrameSize(1);
    configScrollArea->setPosition(10,40);
    configScrollArea->setSize(395,263);
    configScrollArea->setScrollbarWidth(20);
    configScrollArea->setBaseColor(baseCol);

    window_config = new gcn::Window("Config");
    window_config->setPosition(137,20);
    window_config->setMovable(false);
    window_config->setSize(500,400);
    window_config->setBaseColor(baseCol);
    window_config->setVisible(true);

    window_config->add(background_config);
    window_config->add(button_cfg_load);
    window_config->add(button_cfg_save);
    window_config->add(button_cfg_delete);
    window_config->add(button_cfg_save_global);
    window_config->add(textField_config);
    window_config->add(configScrollArea);
}

void menuConfig_Exit()
{
    delete cfgloadButtonActionListener;
    delete cfgsaveButtonActionListener;
    delete cfgdeleteButtonActionListener;
    delete configlistBoxActionListener;

    delete background_config;
    delete background_image_config;

    delete configlistBox;
    delete configScrollArea;

    delete button_cfg_load;
    delete button_cfg_save;
    delete button_cfg_delete;
    delete button_cfg_save_global;
    delete textField_config;
    delete window_config;
}

void show_settings_Config()
{

}

}
