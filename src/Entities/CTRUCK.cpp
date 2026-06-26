#include "CTRUCK.h"
#include "../Core/ResourceManager.h"

CTRUCK::CTRUCK(float x, float y, float speed, int direction)
    : CVEHICLE(x, y, speed, direction)
{
    try {
        auto& tex = ResourceManager<sf::Texture>::getInstance().get("assets/textures/car_red.png");
        if (tex.getSize().x > 0) {
            m_sprite.setTexture(tex);
            m_usesFallback = false;
            m_sprite.setTextureRect(sf::IntRect(0, 0, tex.getSize().x, tex.getSize().y));
            float scaleX = 120.f / tex.getSize().x;
            float scaleY = 40.f / tex.getSize().y;
            if (m_direction < 0) {
                m_sprite.setScale(-scaleX, scaleY);
                m_sprite.setOrigin(tex.getSize().x, 0.f);
            } else {
                m_sprite.setScale(scaleX, scaleY);
                m_sprite.setOrigin(0.f, 0.f);
            }
        }
    } catch (...) {}

    setupFallback();
}

void CTRUCK::setupFallback() {
    setFallbackSize({ 120.f, 44.f });
    setFallbackColor(sf::Color(139, 69, 19));
    m_fallbackShape.setOutlineThickness(2.f);
    m_fallbackShape.setOutlineColor(sf::Color(80, 40, 10));
}

