#include "CCAR.h"
#include "../Core/ResourceManager.h"
#include <random>

static std::mt19937& getCarRNG() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

CCAR::CCAR(float x, float y, float speed, int direction)
    : CVEHICLE(x, y, speed, direction)
{
    try {
        std::string carFiles[] = { "assets/textures/car_blue.png", "assets/textures/car_red.png", "assets/textures/car_yellow.png" };
        std::uniform_int_distribution<int> colorDist(0, 2);
        auto& tex = ResourceManager<sf::Texture>::getInstance().get(carFiles[colorDist(getCarRNG())]);
        if (tex.getSize().x > 0) {
            m_sprite.setTexture(tex);
            m_usesFallback = false;
            m_sprite.setTextureRect(sf::IntRect(0, 0, tex.getSize().x, tex.getSize().y));
            float scaleX = 60.f / tex.getSize().x;
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
