#include "CTRUCK.h"
#include "../Core/ResourceManager.h"

CTRUCK::CTRUCK(float x, float y, float speed, int direction)
    : CVEHICLE(x, y, speed, direction)
{
    // Do không có truck.png, dùng tạm ảnh car_red.png nhưng giữ nguyên tỷ lệ để không bị ảo ma
    try {
        auto& tex = ResourceManager<sf::Texture>::getInstance().get("assets/textures/car_red.png");
        if (tex.getSize().x > 0) {
            m_sprite.setTexture(tex);
            m_usesFallback = false;
            m_sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(tex.getSize().x), static_cast<int>(tex.getSize().y)));
            float scaleY = 40.f / static_cast<float>(tex.getSize().y);
            float scaleX = scaleY;
            if (m_direction > 0) {
                m_sprite.setScale(-scaleX, scaleY);
                m_sprite.setOrigin(static_cast<float>(tex.getSize().x), 0.f);
            } else {
                m_sprite.setScale(scaleX, scaleY);
                m_sprite.setOrigin(0.f, 0.f);
            }
        }
    } catch (...) {}

    setupFallback();
}

void CTRUCK::setupFallback() {
    setFallbackSize({ 70.f, 40.f });
    setFallbackColor(sf::Color::Red); 
}


