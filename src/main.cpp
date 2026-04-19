#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>
#include <algorithm>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        int m_clickCount = 0;
        CCObject* m_lastButton = nullptr;
        std::chrono::system_clock::time_point m_lastClickTime;
    };

    bool init(bool unfocused) {
        if (!PauseLayer::init(unfocused)) return false;

        auto winSize = CCDirector::get()->getWinSize();
        int64_t posMode = Mod::get()->getSettingValue<int64_t>("settings-btn-pos");

        // 0: Disabled, 1: Left, 2: Right
        if (posMode != 0) { 
            auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
            sprite->setScale(0.7f);
            
            auto btn = CCMenuItemSpriteExtra::create(
                sprite, this, menu_selector(MyPauseLayer::onOpenMySettings)
            );

            auto menu = CCMenu::create();
            if (posMode == 1) {
                menu->setPosition({30, 30});
            } else {
                menu->setPosition({winSize.width - 30, 30});
            }

            menu->addChild(btn);
            menu->setID("settings-shortcut-menu");
            this->addChild(menu);
        }

        return true;
    }

    // Corregido para usar la API compatible de Geode
    void onOpenMySettings(CCObject*) {
        geode::openSettings(Mod::get());
    }

    void handleSafeClick(CCObject* sender, std::string_view settingKey, std::function<void(CCObject*)> originalFunc) {
        if (!Mod::get()->getSettingValue<bool>(std::string(settingKey))) {
            originalFunc(sender);
            return;
        }

        auto playLayer = PlayLayer::get();
        if (playLayer && !playLayer->m_isPlatformer) {
            int64_t rawMinP = Mod::get()->getSettingValue<int64_t>("min-percent");
            int minP = std::clamp(static_cast<int>(rawMinP), 0, 100);
            if (static_cast<int>(playLayer->getCurrentPercent()) < minP) {
                originalFunc(sender);
                return;
            }
        }

        auto ahora = std::chrono::system_clock::now();
        int64_t rawSpeed = Mod::get()->getSettingValue<int64_t>("click-speed");
        int speedLimit = std::clamp(static_cast<int>(rawSpeed), 100, 1000);
        auto tiempo = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClickTime).count();

        if (m_fields->m_lastButton != sender || tiempo > speedLimit) {
            m_fields->m_clickCount = 0;
            this->removeChildByTag(69420);
        }

        m_fields->m_clickCount++;
        m_fields->m_lastButton = sender;
        m_fields->m_lastClickTime = ahora;

        if (m_fields->m_clickCount >= 2) {
            m_fields->m_clickCount = 0;
            originalFunc(sender);
        } else {
            if (Mod::get()->getSettingValue<bool>("show-message")) {
                std::string texto = Mod::get()->getSettingValue<std::string>("custom-text");
                bool gold = Mod::get()->getSettingValue<bool>("use-gold-font");
                int64_t rawOpc = Mod::get()->getSettingValue<int64_t>("message-opacity");
                int opc = std::clamp(static_cast<int>(rawOpc), 0, 100);
                
                auto label = CCLabelBMFont::create(texto.c_str(), gold ? "goldFont.fnt" : "bigFont.fnt");
                auto winSize = CCDirector::get()->getWinSize();
                label->setPosition({winSize.width / 2, winSize.height / 2 - 60});
                label->setScale(0.5f);
                label->setTag(69420);
                label->setOpacity(static_cast<GLubyte>((opc * 255) / 100));
                
                this->removeChildByTag(69420);
                this->addChild(label);
                
                label->runAction(CCSequence::create(
                    CCEaseExponentialOut::create(CCScaleTo::create(0.2f, 0.55f)),
                    CCDelayTime::create(0.4f),
                    CCFadeOut::create(0.2f),
                    CCRemoveSelf::create(),
                    nullptr
                ));
            }
        }
    }

    void onQuit(CCObject* s) { handleSafeClick(s, "lock-exit", [this](CCObject* o) { PauseLayer::onQuit(o); }); }
    void onRestart(CCObject* s) { handleSafeClick(s, "lock-reset", [this](CCObject* o) { PauseLayer::onRestart(o); }); }
    void onRestartFull(CCObject* s) { handleSafeClick(s, "lock-reset-plat", [this](CCObject* o) { PauseLayer::onRestartFull(o); }); }
    void onPracticeMode(CCObject* s) { handleSafeClick(s, "lock-practice-all", [this](CCObject* o) { PauseLayer::onPracticeMode(o); }); }
    void onNormalMode(CCObject* s) { handleSafeClick(s, "lock-practice-all", [this](CCObject* o) { PauseLayer::onNormalMode(o); }); }
};
