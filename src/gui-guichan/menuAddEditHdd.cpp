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

extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "custom.h"
#include "filesys.h"
}

enum hdlist_cols {
    HDLIST_DEVICE,
    HDLIST_VOLUME,
    HDLIST_PATH,
    HDLIST_READONLY,
    HDLIST_HEADS,
    HDLIST_CYLS,
    HDLIST_SECS,
    HDLIST_RSRVD,
    HDLIST_SIZE,
    HDLIST_BLKSIZE,
    HDLIST_BOOTPRI,
//    HDLIST_FILESYSDIR,
    HDLIST_MAX_COLS
};

int hdd_nr;
extern int menu_load_type;
int add_or_set=0;

namespace widgets
{
static void raise_addEditHdd();
static void unraise_addEditHdd();
void setHddUnit(int nr);
void addHddUnit();
extern void run_menuLoad_guichan(char *curr_path, int aLoadType);
extern void showWarning(const char *msg, const char *msg2 = "");
extern void readVolumeInfo(void);
extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Widget* activateAfterClose;
gcn::Window *window_addEditHdd;
gcn::Button* button_hdd_ok;
gcn::Button* button_hdd_cancel;

gcn::Label* label_deviceName;
gcn::Container* backgrd_deviceName;
gcn::TextField* textField_deviceName;
gcn::Label* label_volumeName;
gcn::Container* backgrd_volumeName;
gcn::TextField* textField_volumeName;
gcn::Button* button_path;
gcn::TextField* textField_path;
gcn::CheckBox* checkBox_readonly;
gcn::Label* label_bootPriority;
gcn::Container* backgrd_bootPriority;
gcn::TextField* textField_bootPriority;
gcn::Label* label_surfaces;
gcn::Container* backgrd_surfaces;
gcn::TextField* textField_surfaces;
gcn::Label* label_reserved;
gcn::Container* backgrd_reserved;
gcn::TextField* textField_reserved;
gcn::Label* label_sectors;
gcn::Container* backgrd_sectors;
gcn::TextField* textField_sectors;
gcn::Label* label_blocksize;
gcn::Container* backgrd_blocksize;
gcn::TextField* textField_blocksize;

class OkHddButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (add_or_set==0)
            setHddUnit(hdd_nr);
        else
            addHddUnit();
        unraise_addEditHdd();
        readVolumeInfo();
    }
};
OkHddButtonActionListener* okHddButtonActionListener;

class CancelHddButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        unraise_addEditHdd();
    }
};
CancelHddButtonActionListener* cancelHddButtonActionListener;

class PathButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        char tempPath[300];
        strcpy(tempPath, prefs_get_attr ("hardfile_path"));
        strcpy(currentDir,tempPath);
        if (menu_load_type == MENU_LOAD_DIR) {
            activateAfterClose = button_path;
            window_addEditHdd->releaseModalFocus();
            run_menuLoad_guichan(currentDir, MENU_LOAD_DIR);

        }
        if (menu_load_type == MENU_LOAD_HDF) {
            activateAfterClose = button_path;
            window_addEditHdd->releaseModalFocus();
            run_menuLoad_guichan(currentDir, MENU_LOAD_HDF);
        }
    }
};
PathButtonActionListener* pathButtonActionListener;

void addEditHdd_Init()
{
    window_addEditHdd = new gcn::Window("Add/Edit HardDrive");
    window_addEditHdd->setSize(400,300);
    window_addEditHdd->setBaseColor(baseCol);

    label_deviceName = new gcn::Label("device");
    label_deviceName->setPosition(4, 2);
    backgrd_deviceName = new gcn::Container();
    backgrd_deviceName->setOpaque(true);
    backgrd_deviceName->setBaseColor(baseColLabel);
    backgrd_deviceName->setPosition(10, 10);
    backgrd_deviceName->setSize(55, 21);
    backgrd_deviceName->add(label_deviceName);
    textField_deviceName = new gcn::TextField("          ");
    textField_deviceName->setSize(110,22);
    textField_deviceName->setPosition(70,10);
    textField_deviceName->setEnabled(true);
    textField_deviceName->setBaseColor(baseCol);
    label_volumeName = new gcn::Label("volume");
    label_volumeName->setPosition(4, 2);
    backgrd_volumeName = new gcn::Container();
    backgrd_volumeName->setOpaque(true);
    backgrd_volumeName->setBaseColor(baseColLabel);
    backgrd_volumeName->setPosition(200, 10);
    backgrd_volumeName->setSize(55, 21);
    backgrd_volumeName->add(label_volumeName);
    backgrd_volumeName->setVisible(false);
    textField_volumeName = new gcn::TextField("          ");
    textField_volumeName->setSize(110,22);
    textField_volumeName->setPosition(260,10);
    textField_volumeName->setEnabled(true);
    textField_volumeName->setBaseColor(baseCol);
    textField_volumeName->setVisible(false);
    button_path = new gcn::Button("path");
    button_path->setPosition(335,50);
    button_path->setSize(50, 26);
    button_path->setBaseColor(baseCol);
    pathButtonActionListener = new PathButtonActionListener();
    button_path->addActionListener(pathButtonActionListener);
    textField_path = new gcn::TextField("          ");
    textField_path->setSize(320,22);
    textField_path->setPosition(10,51);
    textField_path->setEnabled(false);
    textField_path->setBaseColor(baseCol);
    checkBox_readonly = new gcn::CheckBox("Read/Write");
    checkBox_readonly->setPosition(10,90);
    checkBox_readonly->setId("readOnly");
    label_bootPriority = new gcn::Label("Boot priority");
    label_bootPriority->setPosition(4, 2);
    backgrd_bootPriority = new gcn::Container();
    backgrd_bootPriority->setOpaque(true);
    backgrd_bootPriority->setBaseColor(baseColLabel);
    backgrd_bootPriority->setPosition(200, 90);
    backgrd_bootPriority->setSize(95, 21);
    backgrd_bootPriority->add(label_bootPriority);
    textField_bootPriority = new gcn::TextField("          ");
    textField_bootPriority->setSize(70,22);
    textField_bootPriority->setPosition(300,90);
    textField_bootPriority->setEnabled(true);
    textField_bootPriority->setBaseColor(baseCol);
    label_surfaces = new gcn::Label("surfaces");
    label_surfaces->setPosition(4, 2);
    backgrd_surfaces = new gcn::Container();
    backgrd_surfaces->setOpaque(true);
    backgrd_surfaces->setBaseColor(baseColLabel);
    backgrd_surfaces->setPosition(20, 130);
    backgrd_surfaces->setSize(65, 21);
    backgrd_surfaces->add(label_surfaces);
    backgrd_surfaces->setVisible(false);
    textField_surfaces = new gcn::TextField("          ");
    textField_surfaces->setSize(80,22);
    textField_surfaces->setPosition(90,130);
    textField_surfaces->setEnabled(true);
    textField_surfaces->setBaseColor(baseCol);
    textField_surfaces->setVisible(false);
    label_reserved = new gcn::Label("reserved");
    label_reserved->setPosition(4, 2);
    backgrd_reserved = new gcn::Container();
    backgrd_reserved->setOpaque(true);
    backgrd_reserved->setBaseColor(baseColLabel);
    backgrd_reserved->setPosition(198, 130);
    backgrd_reserved->setSize(65, 21);
    backgrd_reserved->add(label_reserved);
    backgrd_reserved->setVisible(false);
    textField_reserved = new gcn::TextField("          ");
    textField_reserved->setSize(80,22);
    textField_reserved->setPosition(270,130);
    textField_reserved->setEnabled(true);
    textField_reserved->setBaseColor(baseCol);
    textField_reserved->setVisible(false);
    label_sectors = new gcn::Label("sectors");
    label_sectors->setPosition(4, 2);
    backgrd_sectors = new gcn::Container();
    backgrd_sectors->setOpaque(true);
    backgrd_sectors->setBaseColor(baseColLabel);
    backgrd_sectors->setPosition(28, 170);
    backgrd_sectors->setSize(65, 21);
    backgrd_sectors->add(label_sectors);
    backgrd_sectors->setVisible(false);
    textField_sectors = new gcn::TextField("          ");
    textField_sectors->setSize(80,22);
    textField_sectors->setPosition(90,170);
    textField_sectors->setEnabled(true);
    textField_sectors->setBaseColor(baseCol);
    textField_sectors->setVisible(false);
    label_blocksize = new gcn::Label("block size");
    label_blocksize->setPosition(4, 2);
    backgrd_blocksize = new gcn::Container();
    backgrd_blocksize->setOpaque(true);
    backgrd_blocksize->setBaseColor(baseColLabel);
    backgrd_blocksize->setPosition(191, 170);
    backgrd_blocksize->setSize(75, 21);
    backgrd_blocksize->add(label_blocksize);
    backgrd_blocksize->setVisible(false);
    textField_blocksize = new gcn::TextField("          ");
    textField_blocksize->setSize(80,22);
    textField_blocksize->setPosition(270,170);
    textField_blocksize->setEnabled(true);
    textField_blocksize->setBaseColor(baseCol);
    textField_blocksize->setVisible(false);

    button_hdd_ok = new gcn::Button("Ok");
    button_hdd_ok->setPosition(231,248);
    button_hdd_ok->setSize(70, 26);
    button_hdd_ok->setBaseColor(baseCol);
    okHddButtonActionListener = new OkHddButtonActionListener();
    button_hdd_ok->addActionListener(okHddButtonActionListener);
    button_hdd_cancel = new gcn::Button("Cancel");
    button_hdd_cancel->setPosition(316,248);
    button_hdd_cancel->setSize(70, 26);
    button_hdd_cancel->setBaseColor(baseCol);
    cancelHddButtonActionListener = new CancelHddButtonActionListener();
    button_hdd_cancel->addActionListener(cancelHddButtonActionListener);

    window_addEditHdd->add(backgrd_deviceName);
    window_addEditHdd->add(textField_deviceName);
    window_addEditHdd->add(backgrd_volumeName);
    window_addEditHdd->add(textField_volumeName);
    window_addEditHdd->add(button_path);
    window_addEditHdd->add(textField_path);
    window_addEditHdd->add(checkBox_readonly);
    window_addEditHdd->add(backgrd_bootPriority);
    window_addEditHdd->add(textField_bootPriority);
    window_addEditHdd->add(backgrd_surfaces);
    window_addEditHdd->add(textField_surfaces);
    window_addEditHdd->add(backgrd_reserved);
    window_addEditHdd->add(textField_reserved);
    window_addEditHdd->add(backgrd_sectors);
    window_addEditHdd->add(textField_sectors);
    window_addEditHdd->add(backgrd_blocksize);
    window_addEditHdd->add(textField_blocksize);
    window_addEditHdd->add(button_hdd_ok);
    window_addEditHdd->add(button_hdd_cancel);
    window_addEditHdd->setVisible(false);
}

void addEditHdd_Exit()
{
    delete okHddButtonActionListener;
    delete cancelHddButtonActionListener;
    delete pathButtonActionListener;
    delete button_hdd_ok;
    delete button_hdd_cancel;

    delete label_deviceName;
    delete textField_deviceName;
    delete backgrd_deviceName;
    delete label_volumeName;
    delete textField_volumeName;
    delete backgrd_volumeName;
    delete button_path;
    delete textField_path;
    delete checkBox_readonly;
    delete label_bootPriority;
    delete textField_bootPriority;
    delete backgrd_bootPriority;
    delete label_surfaces;
    delete backgrd_surfaces;
    delete textField_surfaces;
    delete label_reserved;
    delete backgrd_reserved;
    delete textField_reserved;
    delete label_sectors;
    delete backgrd_sectors;
    delete textField_sectors;
    delete label_blocksize;
    delete backgrd_blocksize;
    delete textField_blocksize;

    delete window_addEditHdd;
}

static void unraise_addEditHdd()
{
    window_addEditHdd->releaseModalFocus();
    window_addEditHdd->setVisible(false);
}


static void raise_addEditHdd()
{
    if (menu_load_type == MENU_LOAD_DIR) {
        backgrd_volumeName->setVisible(true);
        textField_volumeName->setVisible(true);
        backgrd_surfaces->setVisible(false);
        textField_surfaces->setVisible(false);
        backgrd_reserved->setVisible(false);
        textField_reserved->setVisible(false);
        backgrd_sectors->setVisible(false);
        textField_sectors->setVisible(false);
        backgrd_blocksize->setVisible(false);
        textField_blocksize->setVisible(false);
    }
    if (menu_load_type == MENU_LOAD_HDF) {
        backgrd_volumeName->setVisible(false);
        textField_volumeName->setVisible(false);
        backgrd_surfaces->setVisible(true);
        textField_surfaces->setVisible(true);
        backgrd_reserved->setVisible(true);
        textField_reserved->setVisible(true);
        backgrd_sectors->setVisible(true);
        textField_sectors->setVisible(true);
        backgrd_blocksize->setVisible(true);
        textField_blocksize->setVisible(true);
    }
    window_addEditHdd->setVisible(true);
    window_addEditHdd->requestModalFocus();
    button_path->requestFocus();
}

void readHddUnit(int nr)
{
    int     secspertrack, surfaces, reserved, blocksize, bootpri;
    uae_u64 size;
    int     cylinders, readonly, flags;
    char   *devname, *volname, *rootdir, *filesysdir;
    const char *failure;
    char tempStr[256];
    /* We always use currprefs.mountinfo for the GUI.  The filesystem
    code makes a private copy which is updated every reset.  */
    failure = get_filesys_unit (currprefs.mountinfo, nr,
                                &devname, &volname, &rootdir, &readonly,
                                &secspertrack, &surfaces, &reserved,
                                &cylinders, &size, &blocksize, &bootpri,
                                &filesysdir, &flags);
    if (is_hardfile (currprefs.mountinfo, nr)) {
        if (secspertrack == 0)
            textField_deviceName->setText("");
        else
            textField_deviceName->setText(devname);
        textField_volumeName->setText("");
        sprintf(tempStr, "%d", surfaces);
        textField_surfaces->setText(tempStr);
        sprintf(tempStr, "%d", reserved);
        textField_reserved->setText(tempStr);
        sprintf(tempStr, "%d", secspertrack);
        textField_sectors->setText(tempStr);
        sprintf(tempStr, "%d", blocksize);
        textField_blocksize->setText(tempStr);
    } else {
        textField_deviceName->setText(devname);
        textField_volumeName->setText(volname);
        textField_surfaces->setText("");
        textField_reserved->setText("");
        textField_sectors->setText("");
        textField_blocksize->setText("");
    }
    textField_path->setText(rootdir);
    if (!readonly)
        checkBox_readonly->setSelected(true);
    else
        checkBox_readonly->setSelected(false);
    sprintf(tempStr, "%d", bootpri);
    textField_bootPriority->setText(tempStr);
}

void setHddUnit(int nr)
{
    int     secspertrack, surfaces, reserved, blocksize, bootpri;
    uae_u64 size;
    int     cylinders, readonly, flags;
    char   *devname, *volname, *rootdir, *filesysdir;
    const char *failure;
    char devnameStr[256],volnameStr[256],rootdirStr[256];
    int n;
    const char *buff;

    strcpy(devnameStr,textField_deviceName->getText().c_str());
    devname=devnameStr;
    strcpy(volnameStr,textField_volumeName->getText().c_str());
    volname=volnameStr;
    strcpy(rootdirStr,textField_path->getText().c_str());
    rootdir=rootdirStr;
    bootpri=atoi(textField_bootPriority->getText().c_str());
    secspertrack=atoi(textField_sectors->getText().c_str());
    surfaces=atoi(textField_surfaces->getText().c_str());
    reserved=atoi(textField_reserved->getText().c_str());
    blocksize=atoi(textField_blocksize->getText().c_str());
    if (checkBox_readonly->isSelected())
        readonly=0;
    else
        readonly=1;
    if (menu_load_type==MENU_LOAD_DIR)
        buff=set_filesys_unit (currprefs.mountinfo, nr, devname, volname, rootdir, readonly, 0, 0, 0, 0, bootpri, 0, 0);
    if (menu_load_type==MENU_LOAD_HDF)
        buff=set_filesys_unit (currprefs.mountinfo, nr, devname, 0, rootdir, readonly, secspertrack, surfaces, reserved, blocksize, bootpri, "", 0);
}

void addHddUnit()
{
    int     secspertrack, surfaces, reserved, blocksize, bootpri;
    uae_u64 size;
    int     cylinders, readonly, flags;
    char   *devname, *volname, *rootdir, *filesysdir;
    const char *failure;
    char devnameStr[256],volnameStr[256],rootdirStr[256];
    int n;
    const char *buff;

    strcpy(devnameStr,textField_deviceName->getText().c_str());
    devname=devnameStr;
    strcpy(volnameStr,textField_volumeName->getText().c_str());
    volname=volnameStr;
    strcpy(rootdirStr,textField_path->getText().c_str());
    rootdir=rootdirStr;
    bootpri=atoi(textField_bootPriority->getText().c_str());
    secspertrack=atoi(textField_sectors->getText().c_str());
    surfaces=atoi(textField_surfaces->getText().c_str());
    reserved=atoi(textField_reserved->getText().c_str());
    blocksize=atoi(textField_blocksize->getText().c_str());
    if (checkBox_readonly->isSelected())
        readonly=0;
    else
        readonly=1;
    printf("devname=%s, volname=%s, roodir=%s\n",devname,volname,rootdir);
    printf("secspertrack=%d, surfaces=%d, reserved=%d, blocksize=%d\n",secspertrack,surfaces,reserved,blocksize);
    if (menu_load_type==MENU_LOAD_DIR) {
        buff=add_filesys_unit (currprefs.mountinfo, devname, volname, rootdir, readonly, 0, 0, 0, 0, bootpri, 0, 0);
    }
    if (menu_load_type==MENU_LOAD_HDF) {
        buff=add_filesys_unit (currprefs.mountinfo, devname, 0, rootdir, readonly, secspertrack, surfaces, reserved, blocksize, bootpri, "", 0);
    }
}

void clearHddUnit()
{
    textField_deviceName->setText("");
    textField_volumeName->setText("");
    textField_path->setText("");
    textField_bootPriority->setText("0");
    checkBox_readonly->setSelected(true);
    textField_surfaces->setText("1");
    textField_reserved->setText("2");
    textField_sectors->setText("32");
    textField_blocksize->setText("512");
}

void EditHdd_GUI(int nr)
{
    if (is_hardfile (currprefs.mountinfo, nr))
        menu_load_type=MENU_LOAD_HDF;
    else
        menu_load_type=MENU_LOAD_DIR;
    raise_addEditHdd();
    readHddUnit(nr);
    hdd_nr=nr;
    add_or_set=0;
}

void AddDirFs_GUI()
{
    menu_load_type=MENU_LOAD_DIR;
    clearHddUnit();
    raise_addEditHdd();
    add_or_set=1;
}

void AddHdf_GUI()
{
    menu_load_type=MENU_LOAD_HDF;
    clearHddUnit();
    raise_addEditHdd();
    add_or_set=1;
}

}
