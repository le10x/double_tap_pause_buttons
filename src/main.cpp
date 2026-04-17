#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        int m_clickCount = 0;
        CCObject* m_lastButton = nullptr; // Para saber qué botón se tocó
        std::chrono::system_clock::time_point m_lastClickTime;
    };

    // Función genérica para manejar la lógica de "doble clic"
    void handleSafeClick(CCObject* sender, std::string_view settingKey, std::function<void(CCObject*)> originalFunc) {
        // 1. Verificar si la protección está activada para este botón
        if (!Mod::get()->getSettingValue<bool>(std::string(settingKey))) {
            originalFunc(sender);
            return;
        }

        auto ahora = std::chrono::system_clock::now();
        auto tiempoTranscurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClickTime).count();

        // 2. Corregir BUG: Reiniciar si es un botón distinto o pasó mucho tiempo (>500ms)
        if (m_fields->m_lastButton != sender || tiempoTranscurrido > 500) {
            m_fields->m_clickCount = 0;
        }

        m_fields->m_clickCount++;
        m_fields->m_lastButton = sender;
        m_fields->m_lastClickTime = ahora;

        if (m_fields->m_clickCount >= 2) {
            originalFunc(sender);
        } else {
            // Texto en pantalla (Inglés)
            auto label = CCLabelBMFont::create("Click again to confirm", "bigFont.fnt");
            label->setPosition(CCDirector::get()->getWinSize() / 2);
            label->setScale(0.5f);
            label->setTag(69420); // Tag único para evitar duplicados
            
            this->removeChildByTag(69420); // Borrar el anterior si existe
            this->addChild(label);
            
            label->runAction(CCSequence::create(
                CCDelayTime::create(0.5f),
                CCFadeOut::create(0.2f),
                CCRemoveSelf::create(),
                nullptr
            ));
        }
    }

    // Hooks para cada botón
    void onResume(CCObject* s) { handleSafeClick(s, "lock-play", [this](CCObject* o) { PauseLayer::onResume(o); }); }
    void onEdit(CCObject* s) { handleSafeClick(s, "lock-editor", [this](CCObject* o) { PauseLayer::onEdit(o); }); }
    void onPracticeMode(CCObject* s) { handleSafeClick(s, "lock-practice", [this](CCObject* o) { PauseLayer::onPracticeMode(o); }); }
    void onQuit(CCObject* s) { handleSafeClick(s, "lock-exit", [this](CCObject* o) { PauseLayer::onQuit(o); }); }
    void onRestart(CCObject* s) { handleSafeClick(s, "lock-reset", [this](CCObject* o) { PauseLayer::onRestart(o); }); }
    void onNormalMode(CCObject* s) { handleSafeClick(s, "lock-exit-practice", [this](CCObject* o) { PauseLayer::onNormalMode(o); }); }
    void onRestartFull(CCObject* s) { handleSafeClick(s, "lock-reset-plat", [this](CCObject* o) { PauseLayer::onRestartFull(o); }); }
};
