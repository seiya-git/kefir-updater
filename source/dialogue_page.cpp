#include "dialogue_page.hpp"
#include "utils.hpp"
#include "reboot_payload.h"
#include "main_frame.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;

DialoguePage::DialoguePage(brls::StagedAppletFrame* frame, std::string text)
{
    this->button1 = (new brls::Button(brls::ButtonStyle::REGULAR))->setLabel("menus/common/yes"_i18n);
    this->button1->setParent(this);
    this->button2 = (new brls::Button(brls::ButtonStyle::REGULAR))->setLabel("menus/common/no"_i18n);
    this->button2->setParent(this);

    this->button1->getClickEvent()->subscribe([frame, this](View* view) {
        if (!frame->isLastStage()) frame->nextStage();
        else {
            brls::Application::pushView(new MainFrame());
        }
    });

    this->button2->getClickEvent()->subscribe([frame, this](View* view) {
        reboot_to_payload(RCM_PAYLOAD_PATH);
        brls::Application::popView();
    });

    this->label = new brls::Label(brls::LabelStyle::DIALOG, "menus/ams_update/install_hekate"_i18n + "\n\n" + text, true);
    this->label->setHorizontalAlign(NVG_ALIGN_CENTER);
    this->label->setParent(this);

    this->navigationMap.add(
        this->button1,
        brls::FocusDirection::RIGHT,
        this->button2);

    this->navigationMap.add(
        this->button2,
        brls::FocusDirection::LEFT,
        this->button1);
    
    this->registerAction("", brls::Key::B, [this] { return true; });
}

void DialoguePage::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx)
{
    this->label->frame(ctx);
    this->button1->frame(ctx);
    this->button2->frame(ctx);
}

void DialoguePage::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash)
{
    this->label->setWidth(this->width);
    this->label->invalidate(true);
    this->label->setBoundaries(
        this->x + this->width / 2 - this->label->getWidth() / 2,
        this->y + (this->height - this->label->getHeight() - this->y - style->CrashFrame.buttonHeight)/2,
        this->label->getWidth(),
        this->label->getHeight());
    this->button1->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth / 2 - 200,
        this->y + (this->height-style->CrashFrame.buttonHeight*3),
        style->CrashFrame.buttonWidth,
        style->CrashFrame.buttonHeight);
    this->button1->invalidate();

    this->button2->setBoundaries(
        this->x + this->width / 2 - style->CrashFrame.buttonWidth / 2 + 200,
        this->y + (this->height-style->CrashFrame.buttonHeight*3),
        style->CrashFrame.buttonWidth,
        style->CrashFrame.buttonHeight);
    this->button2->invalidate();
}

brls::View* DialoguePage::getDefaultFocus()
{
    return this->button1;
}

brls::View* DialoguePage::getNextFocus(brls::FocusDirection direction, brls::View* currentView){
    return this->navigationMap.getNextFocus(direction, currentView);
}
