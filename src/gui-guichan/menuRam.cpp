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
#include "memory-uae.h"
#include "custom.h"
}

namespace widgets
{
void show_settings_Ram(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::contrib::SDLTrueTypeFont* font2;

// Tab Main
gcn::Window *window_ram;
gcn::Image *background_image_ram;
gcn::Icon* background_ram;
gcn::Label* label_chipmem;
gcn::Label* label_slowmem;
gcn::Label* label_fastmem;
gcn::Label* label_z3fastmem;
gcn::Label* label_z3rtgmem;
gcn::Container* backgrd_chipmem;
gcn::Container* backgrd_slowmem;
gcn::Container* backgrd_fastmem;
gcn::Container* backgrd_z3fastmem;
gcn::Container* backgrd_z3rtgmem;
gcn::Label* label_chipsize;
gcn::Label* label_slowsize;
gcn::Label* label_fastsize;
gcn::Label* label_z3fastsize;
gcn::Label* label_z3rtgsize;
gcn::Container* backgrd_chipsize;
gcn::Container* backgrd_slowsize;
gcn::Container* backgrd_fastsize;
gcn::Container* backgrd_z3fastsize;
gcn::Container* backgrd_z3rtgsize;
gcn::Slider* slider_chipmem;
gcn::Slider* slider_slowmem;
gcn::Slider* slider_fastmem;
gcn::Slider* slider_z3fastmem;
gcn::Slider* slider_z3rtgmem;

class MemorySliderActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == slider_chipmem) {
            int t = (int)(slider_chipmem->getValue());
            changed_prefs.chipmem_size = 0x80000 << t;
            if (changed_prefs.chipmem_size > 0x200000)
                changed_prefs.fastmem_size = 0;
        }

        if (actionEvent.getSource() == slider_slowmem) {
            int t = (int)(slider_slowmem->getValue());
            switch (t) {
            case 0:
                changed_prefs.bogomem_size = 0;
                break;
            case 1:
                changed_prefs.bogomem_size = 0x080000;
                break;
            case 2:
                changed_prefs.bogomem_size = 0x100000;
                break;
            case 3:
                changed_prefs.bogomem_size = 0x1C0000;
                break;
            case 4:
                changed_prefs.bogomem_size = 0x1E0000;
                break;
            }
        }
        if (actionEvent.getSource() == slider_fastmem) {
            int t = (int)(slider_fastmem->getValue());
            if (t>0 && changed_prefs.chipmem_size>(0x80000 << 2))
                changed_prefs.chipmem_size=0x80000 << 2;
            changed_prefs.fastmem_size = (0x80000 << t) & ~0x80000;
        }
        if (actionEvent.getSource() == slider_z3fastmem) {
            int t = (int)(slider_z3fastmem->getValue());
            changed_prefs.z3fastmem_size = (0x80000 << t) & ~0x80000;
        }
        if (actionEvent.getSource() == slider_z3rtgmem) {
            int t = (int)(slider_z3rtgmem->getValue());
            changed_prefs.gfxmem_size = (0x80000 << t) & ~0x80000;
        }
        show_settings_Ram();
    }
};
MemorySliderActionListener* memorySliderActionListener;

void menuRam_Init()
{
    background_image_ram = gcn::Image::load("data/amigawallpaper500x390.jpg");
    background_ram = new gcn::Icon(background_image_ram);

    // Select memory
    label_chipmem = new gcn::Label("Chip");
    label_chipmem->setPosition(4, 2);
    label_slowmem = new gcn::Label("Slow");
    label_slowmem->setPosition(4, 2);
    label_fastmem = new gcn::Label("Z2 Fast");
    label_fastmem->setPosition(4, 2);
    label_z3fastmem = new gcn::Label("Z3 Fast");
    label_z3fastmem->setPosition(4, 2);
    label_z3rtgmem = new gcn::Label("RTG");
    label_z3rtgmem->setPosition(4, 2);
    backgrd_chipmem = new gcn::Container();
    backgrd_chipmem->setOpaque(true);
    backgrd_chipmem->setBaseColor(baseColLabel);
    backgrd_chipmem->setPosition(15, 20);
    backgrd_chipmem->setSize(65, 21);
    backgrd_chipmem->add(label_chipmem);
    backgrd_slowmem = new gcn::Container();
    backgrd_slowmem->setOpaque(true);
    backgrd_slowmem->setBaseColor(baseColLabel);
    backgrd_slowmem->setPosition(15, 70);
    backgrd_slowmem->setSize(65, 21);
    backgrd_slowmem->add(label_slowmem);
    backgrd_fastmem = new gcn::Container();
    backgrd_fastmem->setOpaque(true);
    backgrd_fastmem->setBaseColor(baseColLabel);
    backgrd_fastmem->setPosition(15, 120);
    backgrd_fastmem->setSize(65, 21);
    backgrd_fastmem->add(label_fastmem);
    backgrd_z3fastmem = new gcn::Container();
    backgrd_z3fastmem->setOpaque(true);
    backgrd_z3fastmem->setBaseColor(baseColLabel);
    backgrd_z3fastmem->setPosition(15, 170);
    backgrd_z3fastmem->setSize(65, 21);
    backgrd_z3fastmem->add(label_z3fastmem);
    backgrd_z3rtgmem = new gcn::Container();
    backgrd_z3rtgmem->setOpaque(true);
    backgrd_z3rtgmem->setBaseColor(baseColLabel);
    backgrd_z3rtgmem->setPosition(15, 220);
    backgrd_z3rtgmem->setSize(65, 21);
    backgrd_z3rtgmem->add(label_z3rtgmem);
    slider_chipmem = new gcn::Slider(0, 4);
    slider_chipmem->setPosition(95,20);
    slider_chipmem->setSize(180, 21);
    slider_chipmem->setMarkerLength(30);
    slider_chipmem->setStepLength(1);
    slider_chipmem->setId("ChipMem");
    slider_slowmem = new gcn::Slider(0, 4);
    slider_slowmem->setPosition(95,70);
    slider_slowmem->setSize(180, 21);
    slider_slowmem->setMarkerLength(30);
    slider_slowmem->setStepLength(1);
    slider_slowmem->setId("SlowMem");
    slider_fastmem = new gcn::Slider(0, 4);
    slider_fastmem->setPosition(95,120);
    slider_fastmem->setSize(180, 21);
    slider_fastmem->setMarkerLength(30);
    slider_fastmem->setStepLength(1);
    slider_fastmem->setId("FastMem");
    slider_z3fastmem = new gcn::Slider(0, 9);
    slider_z3fastmem->setPosition(95,170);
    slider_z3fastmem->setSize(180, 21);
    slider_z3fastmem->setMarkerLength(30);
    slider_z3fastmem->setStepLength(1);
    slider_z3fastmem->setId("Z3FastMem");
    slider_z3rtgmem = new gcn::Slider(0, 6);
    slider_z3rtgmem->setPosition(95,220);
    slider_z3rtgmem->setSize(180, 21);
    slider_z3rtgmem->setMarkerLength(30);
    slider_z3rtgmem->setStepLength(1);
    slider_z3rtgmem->setId("Z3RtgMem");
    memorySliderActionListener = new MemorySliderActionListener();
    slider_chipmem->addActionListener(memorySliderActionListener);
    slider_slowmem->addActionListener(memorySliderActionListener);
    slider_fastmem->addActionListener(memorySliderActionListener);
    slider_z3fastmem->addActionListener(memorySliderActionListener);
    slider_z3rtgmem->addActionListener(memorySliderActionListener);
    label_chipsize = new gcn::Label("None     ");
    label_chipsize->setPosition(4, 2);
    label_slowsize = new gcn::Label("None     ");
    label_slowsize->setPosition(4, 2);
    label_fastsize = new gcn::Label("None     ");
    label_fastsize->setPosition(4, 2);
    label_z3fastsize = new gcn::Label("None     ");
    label_z3fastsize->setPosition(4, 2);
    label_z3rtgsize = new gcn::Label("None     ");
    label_z3rtgsize->setPosition(4, 2);
    backgrd_chipsize = new gcn::Container();
    backgrd_chipsize->setOpaque(true);
    backgrd_chipsize->setBaseColor(baseColLabel);
    backgrd_chipsize->setPosition(285, 20);
    backgrd_chipsize->setSize(56, 21);
    backgrd_chipsize->add(label_chipsize);
    backgrd_slowsize = new gcn::Container();
    backgrd_slowsize->setOpaque(true);
    backgrd_slowsize->setBaseColor(baseColLabel);
    backgrd_slowsize->setPosition(285, 70);
    backgrd_slowsize->setSize(56, 21);
    backgrd_slowsize->add(label_slowsize);
    backgrd_fastsize = new gcn::Container();
    backgrd_fastsize->setOpaque(true);
    backgrd_fastsize->setBaseColor(baseColLabel);
    backgrd_fastsize->setPosition(285, 120);
    backgrd_fastsize->setSize(56, 21);
    backgrd_fastsize->add(label_fastsize);
    backgrd_z3fastsize = new gcn::Container();
    backgrd_z3fastsize->setOpaque(true);
    backgrd_z3fastsize->setBaseColor(baseColLabel);
    backgrd_z3fastsize->setPosition(285, 170);
    backgrd_z3fastsize->setSize(56, 21);
    backgrd_z3fastsize->add(label_z3fastsize);
    backgrd_z3rtgsize = new gcn::Container();
    backgrd_z3rtgsize->setOpaque(true);
    backgrd_z3rtgsize->setBaseColor(baseColLabel);
    backgrd_z3rtgsize->setPosition(285, 220);
    backgrd_z3rtgsize->setSize(56, 21);
    backgrd_z3rtgsize->add(label_z3rtgsize);

    window_ram = new gcn::Window("Memory");
    window_ram->setPosition(137,20);
    window_ram->setMovable(false);
    window_ram->setSize(500,400);
    window_ram->setBaseColor(baseCol);
    window_ram->setVisible(false);

    window_ram->add(background_ram);
    window_ram->add(backgrd_chipmem);
    window_ram->add(backgrd_slowmem);
    window_ram->add(backgrd_fastmem);
    window_ram->add(backgrd_z3fastmem);
    window_ram->add(backgrd_z3rtgmem);
    window_ram->add(slider_chipmem);
    window_ram->add(slider_slowmem);
    window_ram->add(slider_fastmem);
    window_ram->add(slider_z3fastmem);
    window_ram->add(slider_z3rtgmem);
    window_ram->add(backgrd_chipsize);
    window_ram->add(backgrd_slowsize);
    window_ram->add(backgrd_fastsize);
    window_ram->add(backgrd_z3fastsize);
    window_ram->add(backgrd_z3rtgsize);
}

void menuRam_Exit()
{
    delete memorySliderActionListener;

    delete background_ram;
    delete background_image_ram;

    delete label_chipmem;
    delete label_slowmem;
    delete label_fastmem;
    delete label_z3fastmem;
    delete label_z3rtgmem;
    delete backgrd_chipmem;
    delete backgrd_slowmem;
    delete backgrd_fastmem;
    delete backgrd_z3fastmem;
    delete backgrd_z3rtgmem;
    delete label_chipsize;
    delete label_slowsize;
    delete label_fastsize;
    delete label_z3fastsize;
    delete label_z3rtgsize;
    delete backgrd_chipsize;
    delete backgrd_slowsize;
    delete backgrd_fastsize;
    delete backgrd_z3fastsize;
    delete backgrd_z3rtgsize;
    delete slider_chipmem;
    delete slider_slowmem;
    delete slider_fastmem;
    delete slider_z3fastmem;
    delete slider_z3rtgmem;

    delete window_ram;
}

void show_settings_Ram()
{
    const char *ChipMem_list[] = { "512Kb", "1Mb", "2Mb", "4Mb", "8Mb" };
    const char *SlowMem_list[] = { "None", "512Kb", "1Mb", "1.5Mb", "1.8Mb" };
    const char *FastMem_list[] = { "None", "1Mb", "2Mb", "4Mb", "8Mb" };
    const char *z3FastMem_list[] = { "None", "1Mb", "2Mb", "4Mb", "8Mb", "16Mb", "32Mb", "64Mb", "128Mb", "256Mb"};
    const char *z3RtgMem_list[] = { "None", "1Mb", "2Mb", "4Mb", "8Mb", "16Mb", "32Mb", "64Mb", "128Mb", "256Mb"};
    int t, t2;
    t = 0;
    t2 = changed_prefs.chipmem_size;
    while (t < 4 && t2 > 0x80000)
        t++, t2 >>= 1;
    slider_chipmem->setValue(t);
    label_chipsize->setCaption(ChipMem_list[t]);

    t = 0;
    t2 = changed_prefs.bogomem_size;
    while (t < 3 && t2 >= 0x80000)
        t++, t2 >>= 1;
    slider_slowmem->setValue(t);
    label_slowsize->setCaption(SlowMem_list[t]);

    t = 0;
    t2 = changed_prefs.fastmem_size;
    while (t < 4 && t2 >= 0x100000)
        t++, t2 >>= 1;
    slider_fastmem->setValue(t);
    label_fastsize->setCaption(FastMem_list[t]);

    t = 0;
    t2 = changed_prefs.z3fastmem_size;
    while (t < 9 && t2 >= 0x100000)
        t++, t2 >>= 1;
    slider_z3fastmem->setValue(t);
    label_z3fastsize->setCaption(z3FastMem_list[t]);

    t = 0;
    t2 = changed_prefs.gfxmem_size;
    while (t < 9 && t2 >= 0x100000)
        t++, t2 >>= 1;
    slider_z3rtgmem->setValue(t);
    label_z3rtgsize->setCaption(z3RtgMem_list[t]);
}

}
