#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
    void updatePlayerFrame(int frame) {
        PlayerObject::updatePlayerFrame(frame);

        // m_isSwing detecta si estamos en el modo Swing
        if (this->m_isSwing) {
            // m_mainLayer es el nodo principal que contiene los sprites del icono
            if (auto layer = this->m_mainLayer) {
                if (this->m_isUpsideDown) {
                    layer->setScaleY(-1.0f);
                } else {
                    layer->setScaleY(1.0f);
                }
            }
        }
    }
};
