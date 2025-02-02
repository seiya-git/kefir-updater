#include "main_frame.hpp"

#include <fstream>
#include <json.hpp>

#include "about_tab.hpp"
#include "ams_tab.hpp"
#include "download.hpp"
#include "fs.hpp"
#include "list_download_tab.hpp"
#include "tools_tab.hpp"
#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
using json = nlohmann::ordered_json;

namespace {
    constexpr const char AppTitle[] = APP_TITLE;
    constexpr const char AppVersion[] = APP_VERSION;
}  // namespace

MainFrame::MainFrame() : TabFrame()
{
    this->setIcon("romfs:/gui_icon.png");
    this->setTitle(AppTitle);

    s64 freeStorage;
    std::string tag = util::getLatestTag(TAGS_INFO);
    this->setFooterText(fmt::format("menus/main/footer_text"_i18n, AppVersion,
                                    R_SUCCEEDED(fs::getFreeStorageSD(freeStorage)) ? (float)freeStorage / 0x40000000 : -1));

    json hideStatus = fs::parseJsonFile(HIDE_TABS_JSON);
    nlohmann::ordered_json nxlinks;
    download::getRequest(NXLINKS_URL, nxlinks);

    bool erista = util::isErista();

    if (!util::getBoolValue(hideStatus, "atmosphere"))
        this->addTab("menus/main/update_ams"_i18n, new AmsTab_Regular(nxlinks, erista));

    if (!util::getBoolValue(hideStatus, "cfw"))
        this->addTab("menus/main/update_bootloaders"_i18n, new ListDownloadTab(contentType::bootloaders, nxlinks));

    if (!util::getBoolValue(hideStatus, "firmwares"))
        this->addTab("menus/main/download_firmware"_i18n, new ListDownloadTab(contentType::fw, nxlinks));

    if (!util::getBoolValue(hideStatus, "cheats"))
        this->addTab("menus/main/download_cheats"_i18n, new ListDownloadTab(contentType::cheats));

    if (!util::getBoolValue(hideStatus, "custom"))
        this->addTab("menus/main/custom_downloads"_i18n, new AmsTab_Custom(nxlinks, erista));

    if (!util::getBoolValue(hideStatus, "tools"))
        this->addTab("menus/main/tools"_i18n, new ToolsTab(tag, util::getValueFromKey(nxlinks, "payloads"), erista, hideStatus));

    if (!util::getBoolValue(hideStatus, "about"))
        this->addTab("menus/main/about"_i18n, new AboutTab());

    this->registerAction("", brls::Key::B, [this] { return true; });
}
