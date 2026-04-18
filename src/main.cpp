#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>
#include <functional>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields {
        int m_clickCount = 0;
        CCObject* m_lastButton = nullptr;
        std::chrono::system_clock::time_point m_lastClickTime;
    };

    void handleSafeClick(CCObject* sender, std::string_view settingKey, std::function<void(CCObject*)> originalFunc) {
        // Si la configuración está apagada, ejecutar original y salir
        if (!Mod::get()->getSettingValue<bool>(std::string(settingKey))) {
            originalFunc(sender);
            return;
        }

        auto ahora = std::chrono::system_clock::now();
        auto tiempoTranscurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClickTime).count();

        // Corrección del bug de botones mezclados:
        // Reiniciamos si el botón físico es diferente O si pasó más de 500ms
        if (m_fields->m_lastButton != sender || tiempoTranscurrido > 500) {
            m_fields->m_clickCount = 0;
            // Borrar cualquier mensaje previo para no acumular etiquetas
            this->removeChildByTag(69420);
        }

        m_fields->m_clickCount++;
        m_fields->m_lastButton = sender;
        m_fields->m_lastClickTime = ahora;

        if (m_fields->m_clickCount >= 2) {
            // Resetear contador antes de ejecutar para evitar bucles en botones de toggle
            m_fields->m_clickCount = 0;
            originalFunc(sender);
        } else {
            auto label = CCLabelBMFont::create("Click again to confirm", "bigFont.fnt");
            label->setPosition(CCDirector::get()->getWinSize() / 2);
            label->setScale(0.5f);
            label->setTag(69420);
            
            this->addChild(label);
            
            label->runAction(CCSequence::create(
                CCFadeIn::create(0.1f),
                CCDelayTime::create(0.4f),
                CCFadeOut::create(0.2f),
                CCRemoveSelf::create(),
                nullptr
            ));
        }
    }

    // --- Hooks Actualizados ---
    
    // Cambiado de onEdit a onGoToEditor para compatibilidad 2.2081
    void onGoToEditor(CCObject* s) { 
        handleSafeClick(s, "lock-editor", [this](CCObject* o) { PauseLayer::onGoToEditor(o); }); 
    }

    void onResume(CCObject* s) { 
        handleSafeClick(s, "lock-play", [this](CCObject* o) { PauseLayer::onResume(o); }); 
    }

    void onPracticeMode(CCObject* s) { 
        handleSafeClick(s, "lock-practice", [this](CCObject* o) { PauseLayer::onPracticeMode(o); }); 
    }

    void onNormalMode(CCObject* s) { 
        handleSafeClick(s, "lock-exit-practice", [this](CCObject* o) { PauseLayer::onNormalMode(o); }); 
    }

    void onQuit(CCObject* s) { 
        handleSafeClick(s, "lock-exit", [this](CCObject* o) { PauseLayer::onQuit(o); }); 
    }

    void onRestart(CCObject* s) { 
        handleSafeClick(s, "lock-reset", [this](CCObject* o) { PauseLayer::onRestart(o); }); 
    }

    void onRestartFull(CCObject* s) { 
        handleSafeClick(s, "lock-reset-plat", [this](CCObject* o) { PauseLayer::onRestartFull(o); }); 
    }
};
