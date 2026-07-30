#include "CCAR.h"
#include "../Core/ResourceManager.h"
CCAR::CCAR(float x, float y, float speed, int direction)
    : CVEHICLE(x, y, speed, direction)
{
    try {
        std::string carFiles[] = { "assets/textures/car_blue.png", "assets/textures/car_red.png", "assets/textures/car_yellow.png" };
        int colorIdx = std::rand() % 3;
        auto& tex = ResourceManager<sf::Texture>::getInstance().get(carFiles[colorIdx]);
        if (tex.getSize().x > 0) {
            m_sprite.setTexture(tex);
            m_usesFallback = false;
            m_sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(tex.getSize().x), static_cast<int>(tex.getSize().y)));
            float scaleY = 40.f / static_cast<float>(tex.getSize().y);
            float scaleX = scaleY; // Maintain aspect ratio!
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

void CCAR::setupFallback() {
    setFallbackSize({ 60.f, 40.f });
    static const sf::Color colors[] = {
        sf::Color::Red,
        sf::Color(50, 100, 220),
        sf::Color(255, 165, 0),
        sf::Color(240, 240, 240)
    };
    std::uniform_int_distribution<int> colorDist(0, 3);
    sf::Color chosenColor = colors[colorDist(getCarRNG())];
    setFallbackColor(chosenColor);
    m_fallbackShape.setOutlineThickness(2.f);
    m_fallbackShape.setOutlineColor(sf::Color(40, 40, 40));
}
