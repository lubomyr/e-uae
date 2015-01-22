#include <algorithm>
#ifdef ANDROID
#include <android/log.h>
#endif
#include <guichan.hpp>
#include <iostream>
#include <sstream>
#include <SDL/SDL_ttf.h>
#include <guichan/sdl.hpp>
#include "sdltruetypefont.hpp"
#include "rw_config.h"
#include <sys/types.h>
#include <dirent.h>

#if defined(WIN32)
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

int menuLoad_extfilter=1;
bool confirmselection = false;

#define NOTDIR_SELECTION (menu_load_type !=MENU_LOAD_DIR && menu_load_type !=MENU_SELECT_ROMPATH && menu_load_type !=MENU_SELECT_FLOPPYPATH && menu_load_type !=MENU_SELECT_HARDFILEPATH && menu_load_type !=MENU_SELECT_SAVESTATEPATH)
#define DIR_SELECTION (menu_load_type ==MENU_LOAD_DIR || menu_load_type ==MENU_SELECT_ROMPATH || menu_load_type ==MENU_SELECT_FLOPPYPATH || menu_load_type ==MENU_SELECT_HARDFILEPATH || menu_load_type ==MENU_SELECT_SAVESTATEPATH)
#define extterms files[q].size()>=4 && files[q].substr(files[q].size()-4)
#define floppyterms extterms!=".adf" && extterms!=".ADF" && extterms!=".Adf" && extterms!=".adz" && extterms!=".ADZ" && extterms!=".Adz" && extterms!="f.gz" && extterms!="F.GZ" && extterms!=".bz2" && extterms!=".BZ2" && extterms!=".Bz2" && extterms!=".zip" && extterms!=".ZIP" && extterms!=".Zip" && extterms!=".RP9" && extterms!=".rp9"
#define hddirterms extterms!=".hdf" && extterms!=".HDF" && extterms!=".Hdf"
#define romterms extterms!=".rom" && extterms!=".ROM" && extterms!=".Rom"

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "disk.h"
}

#define _XOPEN_SOURCE 500
#if defined(AROS)
#include <limits.h>

#if defined(__GP2X__) || defined(__WIZ__) || defined(__CAANOO__) || defined(__amigaos__)
// This is a random default value ...
#define PATH_MAX 32768
#endif

static char *sep(char *path)
{
    char *tmp, c;

    tmp = strrchr(path, '/');
    if(tmp) {
        c = tmp[1];
        tmp[1] = 0;
        if (chdir(path)) {
            return NULL;
        }
        tmp[1] = c;

        return tmp + 1;
    }
    return path;
}

char *realpath(const char *_path, char *resolved_path)
{
    int fd = open(".", O_RDONLY), l;
    char current_dir_path[PATH_MAX];
    char path[PATH_MAX], lnk[PATH_MAX], *tmp = (char *)"";

    if (fd < 0) {
        return NULL;
    }
    getcwd(current_dir_path,PATH_MAX);
    strncpy(path, _path, PATH_MAX);

    if (chdir(path)) {
        if (errno == ENOTDIR) {
#if defined(__WIN32__) || defined(__MORPHOS__) || defined(__amigaos__)
            // No symbolic links and no readlink()
            l = -1;
#else
            l = readlink(path, lnk, PATH_MAX);
#endif
            if (!(tmp = sep(path))) {
                resolved_path = NULL;
                goto abort;
            }
            if (l < 0) {
                if (errno != EINVAL) {
                    resolved_path = NULL;
                    goto abort;
                }
            } else {
                lnk[l] = 0;
                if (!(tmp = sep(lnk))) {
                    resolved_path = NULL;
                    goto abort;
                }
            }
        } else {
            resolved_path = NULL;
            goto abort;
        }
    }

    if(resolved_path==NULL) // if we called realpath with null as a 2nd arg
        resolved_path = (char*) malloc( PATH_MAX );

    if (!getcwd(resolved_path, PATH_MAX)) {
        resolved_path = NULL;
        goto abort;
    }

    if(strcmp(resolved_path, "..") && *tmp) {
        strcat(resolved_path, "..");
    }

    strcat(resolved_path, tmp);
abort:
    chdir(current_dir_path);
    close(fd);
    return resolved_path;
}

#endif

/* What is being loaded, floppy/hd dir/hdf */
int menu_load_type;

namespace widgets
{
void show_settings(void);
static void unraise_loadMenu_guichan();
static void checkfilename (char *currentfilename);

extern gcn::Color baseCol;
extern gcn::TextField* textField_path;
extern gcn::TextField* textField_volumeName;
extern gcn::TextField* textField_rom_path;
extern gcn::contrib::SDLTrueTypeFont* font2;
gcn::Window *window_load;
gcn::Button* button_ok;
gcn::Button* button_select;
gcn::Button* button_open;
gcn::Button* button_cancel;
gcn::TextField* textTempRom;
gcn::TextField* textTempFloppy;
gcn::TextField* textTempHardfile;
gcn::TextField* textTempSavestate;
gcn::CheckBox* checkBox_extfilter;
gcn::ListBox* listBox;
gcn::ScrollArea* listBoxScrollArea;

gcn::Widget* activateAfterClose = NULL;

int lastSelectedIndex = 0;


class DirListModel : public gcn::ListModel
{
    std::vector<std::string> dirs, files;

public:
    DirListModel(const char * path) {
        changeDir(path);
    }

    int getNumberOfElements() {
        if (NOTDIR_SELECTION)
            return dirs.size() + files.size();
        else
            return dirs.size();
    }

    std::string getElementAt(int i) {
        if(i >= dirs.size() + files.size() || i < 0)
            return "---";
        if(i < dirs.size())
            return dirs[i];
        if (NOTDIR_SELECTION)
            return files[i - dirs.size()];
    }

    void changeDir(const char * path) {
        dirs.clear();
        files.clear();
        DIR *dir;
        struct dirent *dent;
        dir = opendir(path);
        if(dir != NULL) {
            while((dent=readdir(dir))!=NULL) {
                if(dent->d_type == DT_DIR)
                    dirs.push_back(dent->d_name);
                else if (NOTDIR_SELECTION)
                    files.push_back(dent->d_name);
            }

            if(dirs.size() > 0 && dirs[0] == ".")
                dirs.erase(dirs.begin());

            closedir(dir);
        }

        if(dirs.size() == 0)
            dirs.push_back("..");

        std::sort(dirs.begin(), dirs.end());
        if (NOTDIR_SELECTION)
            std::sort(files.begin(), files.end());

        if (menuLoad_extfilter==1)
            for (int q=0; q<files.size(); q++) {
                if (((menu_load_type == MENU_LOAD_FLOPPY) && floppyterms) || ((menu_load_type == MENU_SELECT_MAIN_ROM) && romterms) || ((menu_load_type == MENU_SELECT_EXT_ROM) && romterms) || ((menu_load_type == MENU_SELECT_KEY_ROM) && romterms)) {
                    files.erase(files.begin()+q);
                    q--;
                }
            }
    }

    bool isDir(int i) {
        if(i < dirs.size())
            return true;
        return false;
    }
};
DirListModel dirList(".");


class OkButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        char filename[256]="";

        selected_item = listBox->getSelected();
        lastSelectedIndex = selected_item;
        strcpy(filename, "");
        strcat(filename, currentDir);
#ifdef WIN32
        strcat(filename, "\\");
#else
        strcat(filename, "/");
#endif
        strcat(filename, dirList.getElementAt(selected_item).c_str());
        confirmselection=true;
        checkfilename(filename);
    }
};
OkButtonActionListener* okButtonActionListener;


class SelectButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        char filename[256]="";

        selected_item = listBox->getSelected();
        lastSelectedIndex = selected_item;
        strcpy(filename, "");
        strcat(filename, currentDir);
#ifdef WIN32
        strcat(filename, "\\");
#else
        strcat(filename, "/");
#endif
        strcat(filename, dirList.getElementAt(selected_item).c_str());
        if (menu_load_type ==MENU_LOAD_DIR) {
            textField_path->setText(filename);
            textField_volumeName->setText(dirList.getElementAt(selected_item).c_str());
        } else if (menu_load_type ==MENU_SELECT_ROMPATH) {
            textTempRom->setText(filename);
            prefs_set_attr ("rom_path", textTempRom->getText().c_str());
        } else if (menu_load_type ==MENU_SELECT_FLOPPYPATH) {
            textTempFloppy->setText(filename);
            prefs_set_attr ("floppy_path", textTempFloppy->getText().c_str());
        } else if (menu_load_type ==MENU_SELECT_HARDFILEPATH) {
            textTempHardfile->setText(filename);
            prefs_set_attr ("hardfile_path", textTempHardfile->getText().c_str());
        } else if (menu_load_type ==MENU_SELECT_SAVESTATEPATH) {
            textTempSavestate->setText(filename);
            prefs_set_attr ("savestate_path", textTempSavestate->getText().c_str());
        }
        confirmselection=true;
        unraise_loadMenu_guichan();
    }
};
SelectButtonActionListener* selectButtonActionListener;


class OpenButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int selected_item;
        char filename[256]="";

        selected_item = listBox->getSelected();
        lastSelectedIndex = selected_item;
        strcpy(filename, "");
        strcat(filename, currentDir);
#ifdef WIN32
        strcat(filename, "\\");
#else
        strcat(filename, "/");
#endif
        strcat(filename, dirList.getElementAt(selected_item).c_str());
        checkfilename(filename);
    }
};
OpenButtonActionListener* openButtonActionListener;


class CancelButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        unraise_loadMenu_guichan();
    }
};
CancelButtonActionListener* cancelButtonActionListener;


class ListBoxActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        confirmselection=false;
#if defined(WIN32) || defined(ANDROID) || defined(AROS) || defined(RASPBERRY)
        if (NOTDIR_SELECTION) {
#endif

            int selected_item;
            char filename[256]="";


            selected_item = listBox->getSelected();
            lastSelectedIndex = selected_item;
            strcpy(filename, "");
            strcat(filename, currentDir);
#ifdef WIN32
            strcat(filename, "\\");
#else
            strcat(filename, "/");
#endif
            strcat(filename, dirList.getElementAt(selected_item).c_str());
            checkfilename(filename);
#if defined(WIN32) || defined(ANDROID) || defined(AROS) || defined(RASPBERRY)
        }
#endif
    }
};
ListBoxActionListener* listBoxActionListener;


class ListBoxKeyListener : public gcn::KeyListener
{
public:
    void keyPressed(gcn::KeyEvent& keyEvent) {
        bool bHandled = false;

        Uint8 *keystate;
        gcn::Key key = keyEvent.getKey();
        if (key.getValue() == gcn::Key::UP) {
            keystate = SDL_GetKeyState(NULL);
            if(keystate[SDLK_RSHIFT]) {
                int selected = listBox->getSelected() - 10;
                if(selected < 0)
                    selected = 0;
                listBox->setSelected(selected);
                bHandled = true;
            }
        }
        if (key.getValue() == gcn::Key::DOWN) {
            keystate = SDL_GetKeyState(NULL);
            if(keystate[SDLK_RSHIFT]) {
                int selected = listBox->getSelected() + 10;
                if(selected >= dirList.getNumberOfElements())
                    selected = dirList.getNumberOfElements() - 1;
                listBox->setSelected(selected);
                bHandled = true;
            }
        }

        if(!bHandled)
            listBox->keyPressed(keyEvent);
    }
};
ListBoxKeyListener* listBoxKeyListener;

class ExtfilterActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_extfilter) {
            if (checkBox_extfilter->isSelected())
                menuLoad_extfilter=1;
            else
                menuLoad_extfilter=0;
        }
        dirList=currentDir;
    }
};
ExtfilterActionListener* extfilterActionListener;

void loadMenu_Init()
{
    window_load = new gcn::Window("Load");
    window_load->setSize(400,320);
    window_load->setBaseColor(baseCol);

    button_ok = new gcn::Button("Ok");
    button_ok->setPosition(231,268);
    button_ok->setSize(70, 26);
    button_ok->setBaseColor(baseCol);
    okButtonActionListener = new OkButtonActionListener();
    button_ok->addActionListener(okButtonActionListener);
    button_select = new gcn::Button("Select");
    button_select->setPosition(145,268);
    button_select->setSize(70, 26);
    button_select->setBaseColor(baseCol);
    selectButtonActionListener = new SelectButtonActionListener();
    button_select->addActionListener(selectButtonActionListener);
    button_open = new gcn::Button("Open");
    button_open->setPosition(231, 268);
    button_open->setSize(70, 26);
    button_open->setBaseColor(baseCol);
    openButtonActionListener = new OpenButtonActionListener();
    button_open->addActionListener(openButtonActionListener);
    button_cancel = new gcn::Button("Cancel");
    button_cancel->setPosition(316,268);
    button_cancel->setSize(70, 26);
    button_cancel->setBaseColor(baseCol);
    cancelButtonActionListener = new CancelButtonActionListener();
    button_cancel->addActionListener(cancelButtonActionListener);

    textTempRom = new gcn::TextField("");
    textTempFloppy = new gcn::TextField("");
    textTempHardfile = new gcn::TextField("");
    textTempSavestate = new gcn::TextField("");

    listBox = new gcn::ListBox(&dirList);
    listBox->setSize(650,170);
    listBox->setBaseColor(baseCol);
    listBox->setWrappingEnabled(true);
    listBox->setFont(font2);
    listBoxScrollArea = new gcn::ScrollArea(listBox);
    listBoxScrollArea->setFrameSize(1);
    listBoxScrollArea->setPosition(10,10);
    listBoxScrollArea->setSize(376,248);
    listBoxScrollArea->setScrollbarWidth(20);
    listBoxScrollArea->setBaseColor(baseCol);
    listBoxActionListener = new ListBoxActionListener();
    listBox->addActionListener(listBoxActionListener);
    listBoxKeyListener = new ListBoxKeyListener();
    listBox->removeKeyListener(listBox);
    listBox->addKeyListener(listBoxKeyListener);

    checkBox_extfilter = new gcn::CheckBox("ext. filter");
    checkBox_extfilter->setPosition(10,270);
    checkBox_extfilter->setId("extFilter");
    extfilterActionListener = new ExtfilterActionListener();
    checkBox_extfilter->addActionListener(extfilterActionListener);

    window_load->add(button_ok);
    window_load->add(button_select);
    window_load->add(button_open);
    window_load->add(button_cancel);
    window_load->add(listBoxScrollArea);
    window_load->add(checkBox_extfilter);
    window_load->setVisible(false);
}


void loadMenu_Exit()
{
    delete listBox;
    delete listBoxScrollArea;

    delete button_ok;
    delete button_select;
    delete button_open;
    delete button_cancel;
    delete textTempRom;
    delete textTempFloppy;
    delete textTempHardfile;
    delete textTempSavestate;
    delete checkBox_extfilter;

    delete okButtonActionListener;
    delete selectButtonActionListener;
    delete openButtonActionListener;
    delete cancelButtonActionListener;
    delete listBoxActionListener;
    delete listBoxKeyListener;
    delete extfilterActionListener;

    delete window_load;
}


static void unraise_loadMenu_guichan()
{
    window_load->releaseModalFocus();
    window_load->setVisible(false);
    if(activateAfterClose != NULL)
        activateAfterClose->requestFocus();
    activateAfterClose = NULL;
    show_settings();
}


static void raise_loadMenu_guichan()
{
    if (NOTDIR_SELECTION) {
        button_ok->setVisible(true);
        button_select->setVisible(false);
        button_open->setVisible(false);
        button_ok->setId("cmdOk");
        button_select->setId("cmdSelect");
        button_open->setId("cmdOpen");
        button_cancel->setId("cmdCancel1");
        listBox->setId("dirList1");
    } else {
        button_ok->setVisible(false);
        button_select->setVisible(true);
        button_open->setVisible(true);
        button_ok->setId("cmdOk");
        button_select->setId("cmdSelect");
        button_open->setId("cmdOpen");
        button_cancel->setId("cmdCancel2");
        listBox->setId("dirList2");
    }

    if (menuLoad_extfilter==0)
        checkBox_extfilter->setSelected(false);
    else if (menuLoad_extfilter==1)
        checkBox_extfilter->setSelected(true);

    window_load->setVisible(true);
//    window_load->requestModalFocus();
    listBox->requestFocus();
}


static int menuLoadLoop(char *curr_path)
{
    char *ret = NULL;
    char *fname = NULL;
    DIR *dir;

    // is this a dir or a full path?
    if ((dir = opendir(curr_path)))
        closedir(dir);
    else {
        char *p;
        for (p = curr_path + strlen(curr_path) - 1; p > curr_path && *p != '/'; p--);
        *p = 0;
        fname = p+1;
    }
    dirList = curr_path;

    if (DIR_SELECTION)
        window_load->setCaption("  Select HD-dir  ");
    else if (menu_load_type == MENU_LOAD_FLOPPY)
        window_load->setCaption(" Add Volume ");
    else
        window_load->setCaption("File Manager");

    if(lastSelectedIndex >= 0 && lastSelectedIndex < dirList.getNumberOfElements())
        listBox->setSelected(lastSelectedIndex);
}


void run_menuLoad_guichan(char *curr_path, int aLoadType)
{
    menu_load_type = aLoadType;
    raise_loadMenu_guichan();
    int ret = menuLoadLoop(curr_path);
}


void checkfilename (char *currentfilename)
{
    char *ret = NULL, *fname = NULL;
    char *ptr;
    char actualpath [PATH_MAX];
    struct dirent **namelist;
    DIR *dir;

    if ((dir = opendir(currentfilename))) {
        dirList=currentfilename;
        ptr = realpath(currentfilename, actualpath);
        strcpy(currentDir, ptr);
        closedir(dir);
    } else {
        if ((menu_load_type == MENU_LOAD_FLOPPY) && confirmselection) {
            strcpy(changed_prefs.df[current_drive],currentfilename);
        } else if ((menu_load_type == MENU_LOAD_HDF) && confirmselection) {
            textField_path->setText(currentfilename);
        } else if ((menu_load_type == MENU_SELECT_MAIN_ROM) && confirmselection) {
            strcpy(changed_prefs.romfile,currentfilename);
        } else if ((menu_load_type == MENU_SELECT_EXT_ROM) && confirmselection) {
            strcpy(changed_prefs.romextfile,currentfilename);
        } else if ((menu_load_type == MENU_SELECT_KEY_ROM) && confirmselection) {
            strcpy(changed_prefs.keyfile,currentfilename);
        }
        if (confirmselection)
            unraise_loadMenu_guichan();
    }
}
}
