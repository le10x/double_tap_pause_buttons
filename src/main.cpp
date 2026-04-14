#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
    void updatePlayerFrame(int frame) {
        PlayerObject::updatePlayerFrame(frame);

        if (this->m_isSwing) {
            bool upsideDown = this->m_isUpsideDown;
            
            // 1. Invertimos el sprite base del icono
            if (this->m_iconSprite) {
                this->m_iconSprite->setFlipY(upsideDown);
            }
            
            // 2. Invertimos el brillo (Glow) para que no se duplique/desfase
            if (this->m_iconGlow) {
                this->m_iconGlow->setFlipY(upsideDown);
            }

            // 3. Invertimos el contenedor del vehículo (el Swing en sí)
            if (this->m_vehicleSprite) {
                this->m_vehicleSprite->setFlipY(upsideDown);
            }

            // Si notas que el icono se "hunde", ajustamos el offset visual
            if (upsideDown) {
                // Ajuste fino por si el punto de anclaje lo mueve
                // this->m_iconSprite->setPositionY(-1.0f); 
            }
        }
    }
};
