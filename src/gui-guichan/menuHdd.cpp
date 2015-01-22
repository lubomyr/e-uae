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
#include "filesys.h"
}

std::string HDD_Volume[8];

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

namespace widgets
{
void show_settings_Hdd(void);
void readVolumeInfo(void);

extern void EditHdd_GUI(int);
extern void AddDirFs_GUI(void);
extern void AddHdf_GUI();
extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

class VolumeListModel : public gcn::ListModel
{
public:
    int getNumberOfElements() {
        int nr = nr_units (currprefs.mountinfo);
        return nr;
    }

    std::string getElementAt(int i) {
        return HDD_Volume[i];
    }
};
VolumeListModel volumeListModel;

// Tab HDD main area
gcn::Window *window_hdd;
gcn::Image *background_image_hdd;
gcn::Icon* background_hdd;
gcn::ListBox* volumeListBox;
gcn::ScrollArea* volumeScrollArea;
gcn::Button* button_addhdf;
gcn::Button* button_addfs;
gcn::Button* button_edithd;
gcn::Button* button_create;
gcn::Button* button_remove;

class EditHdButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int nr = nr_units (currprefs.mountinfo);
        int selected_item = volumeListBox->getSelected();
        if ((selected_item>=0) && (selected_item<=nr))
            EditHdd_GUI(selected_item);
    }
};
EditHdButtonActionListener* editHdButtonActionListener;

class AddfsButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        AddDirFs_GUI();
    }
};
AddfsButtonActionListener* addfsButtonActionListener;

class AddhdfButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        AddHdf_GUI();
    }
};
AddhdfButtonActionListener* addhdfButtonActionListener;

class RemoveButtonActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        int nr = nr_units (currprefs.mountinfo);
        int selected_item = volumeListBox->getSelected();
        if ((selected_item>=0) && (selected_item<=nr)) {
            kill_filesys_unit (currprefs.mountinfo, selected_item);
        }
        readVolumeInfo();
    }
};
RemoveButtonActionListener* removeButtonActionListener;

void menuHdd_Init()
{
    background_image_hdd = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_hdd = new gcn::Icon(background_image_hdd);

    volumeListBox = new gcn::ListBox(&volumeListModel);
    volumeListBox->setId("volumeList");
    volumeListBox->setSize(650,150);
    volumeListBox->setBaseColor(baseCol);
    volumeListBox->setWrappingEnabled(true);
    volumeScrollArea = new gcn::ScrollArea(volumeListBox);
    volumeScrollArea->setFrameSize(1);
    volumeScrollArea->setPosition(10,10);
    volumeScrollArea->setSize(480,100);
    volumeScrollArea->setScrollbarWidth(20);
    volumeScrollArea->setBaseColor(baseCol);

    button_addfs = new gcn::Button("Add FS");
    button_addfs->setSize(70,30);
    button_addfs->setPosition(10,125);
    button_addfs->setBaseColor(baseCol);
    button_addfs->setId("AddFS");
    addfsButtonActionListener = new AddfsButtonActionListener();
    button_addfs->addActionListener(addfsButtonActionListener);
    button_addhdf = new gcn::Button("Add HDF");
    button_addhdf->setSize(70,30);
    button_addhdf->setPosition(110,125);
    button_addhdf->setBaseColor(baseCol);
    button_addhdf->setId("AddHDF");
    addhdfButtonActionListener = new AddhdfButtonActionListener();
    button_addhdf->addActionListener(addhdfButtonActionListener);

    button_edithd = new gcn::Button("Edit");
    button_edithd->setSize(70,30);
    button_edithd->setPosition(210,125);
    button_edithd->setBaseColor(baseCol);
    button_edithd->setId("EditHD");
    editHdButtonActionListener = new EditHdButtonActionListener();
    button_edithd->addActionListener(editHdButtonActionListener);

    button_create = new gcn::Button("Create");
    button_create->setSize(70,30);
    button_create->setPosition(310,125);
    button_create->setBaseColor(baseCol);
    button_create->setId("Create");
    button_remove = new gcn::Button("Remove");
    button_remove->setSize(70,30);
    button_remove->setPosition(410,125);
    button_remove->setBaseColor(baseCol);
    button_remove->setId("Remove");
    removeButtonActionListener = new RemoveButtonActionListener();
    button_remove->addActionListener(removeButtonActionListener);

    window_hdd = new gcn::Window("Hard Drives");
    window_hdd->setPosition(137,20);
    window_hdd->setMovable(false);
    window_hdd->setSize(500,400);
    window_hdd->setBaseColor(baseCol);
    window_hdd->setVisible(false);

    window_hdd->add(background_hdd);
    window_hdd->add(volumeScrollArea);
    window_hdd->add(button_addfs);
    window_hdd->add(button_addhdf);
    window_hdd->add(button_edithd);
    window_hdd->add(button_create);
    window_hdd->add(button_remove);

}

void menuHdd_Exit()
{
    delete addfsButtonActionListener;
    delete addhdfButtonActionListener;
    delete editHdButtonActionListener;
    delete removeButtonActionListener;

    delete background_hdd;
    delete background_image_hdd;
    delete volumeListBox;
    delete volumeScrollArea;
    delete button_addhdf;
    delete button_addfs;
    delete button_edithd;
    delete button_create;
    delete button_remove;

    delete window_hdd;
}

void show_settings_Hdd()
{
    readVolumeInfo();
}

void readVolumeInfo()
{
    char  texts[HDLIST_MAX_COLS][256];
    char *tptrs[HDLIST_MAX_COLS];
    std::string tempStr;
    int nr = nr_units (currprefs.mountinfo);

    for (int i=0; i<HDLIST_MAX_COLS; i++)
        tptrs[i] = texts[i];

    for (int i = 0; i < nr; i++) {
        int     secspertrack, surfaces, reserved, blocksize, bootpri;
        uae_u64 size;
        int     cylinders, readonly, flags;
        char   *devname, *volname, *rootdir, *filesysdir;
        const char *failure;

        /* We always use currprefs.mountinfo for the GUI.  The filesystem
        code makes a private copy which is updated every reset.  */
        failure = get_filesys_unit (currprefs.mountinfo, i,
                                    &devname, &volname, &rootdir, &readonly,
                                    &secspertrack, &surfaces, &reserved,
                                    &cylinders, &size, &blocksize, &bootpri,
                                    &filesysdir, &flags);

        if (is_hardfile (currprefs.mountinfo, i)) {
            if (secspertrack == 0)
                strcpy (texts[HDLIST_DEVICE], "N/A" );
            else
                strncpy (texts[HDLIST_DEVICE], devname, 255);
            sprintf (texts[HDLIST_VOLUME],  "N/A" );
            sprintf (texts[HDLIST_HEADS],   "%d", surfaces);
            sprintf (texts[HDLIST_CYLS],    "%d", cylinders);
            sprintf (texts[HDLIST_SECS],    "%d", secspertrack);
            sprintf (texts[HDLIST_RSRVD],   "%d", reserved);
            sprintf (texts[HDLIST_SIZE],    "%d", size);
            sprintf (texts[HDLIST_BLKSIZE], "%d", blocksize);
        } else {
            strncpy (texts[HDLIST_DEVICE], devname, 255);
            strncpy (texts[HDLIST_VOLUME], volname, 255);
            strcpy (texts[HDLIST_HEADS],   "N/A");
            strcpy (texts[HDLIST_CYLS],    "N/A");
            strcpy (texts[HDLIST_SECS],    "N/A");
            strcpy (texts[HDLIST_RSRVD],   "N/A");
            strcpy (texts[HDLIST_SIZE],    "N/A");
            strcpy (texts[HDLIST_BLKSIZE], "N/A");
        }
        strcpy  (texts[HDLIST_PATH],     rootdir);
        strcpy  (texts[HDLIST_READONLY], readonly ? "R/O" : "R/W");
        sprintf (texts[HDLIST_BOOTPRI], "%d", bootpri);
        for (int j=0; j<HDLIST_MAX_COLS; j++)
            tempStr=tempStr+tptrs[j]+" ";
        HDD_Volume[i]=tempStr;
        tempStr="";
    }
}

}
