#include "CCAR.h"
#include "../Core/ResourceManager.h"

CCAR::CCAR(float x, float y, float speed, int direction, bool isCrazy)
    : CVEHICLE(x, y, speed, direction)
{
    m_isCrazy = isCrazy;

    try {
        std::string carFiles[] = { "assets/textures/car_blue.png", "assets/textures/car_red.png", "assets/textures/car_yellow.png" };
        // Xe điên ưu tiên màu đỏ thể thao nổi bật
        int colorIdx = m_isCrazy ? 1 : (std::rand() % 3);
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

        // Tải texture khói cho xe điên
        if (m_isCrazy) {
            auto& smokeTex = ResourceManager<sf::Texture>::getInstance().get("assets/textures/smoke.png");
            if (smokeTex.getSize().x > 0) {
                const_cast<sf::Sprite&>(m_smokeSprite).setTexture(smokeTex);
                m_hasSmokeTexture = true;
                float smokeScale = 28.f / static_cast<float>(smokeTex.getSize().y);
                const_cast<sf::Sprite&>(m_smokeSprite).setScale(smokeScale, smokeScale);
            }
        }
    } catch (...) {}

    setupFallback();
}

void CCAR::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    // Nếu là xe điên, vẽ khói bốc ra ở bánh xe sau
    if (m_isCrazy && m_hasSmokeTexture && !m_stopped) {
        sf::RenderStates smokeStates = states;
        if (m_direction > 0) {
            // Xe chạy sang phải -> đuôi xe và bánh sau nằm ở bên trái
            smokeStates.transform.translate(-24.f, 10.f);
        } else {
            // Xe chạy sang trái -> đuôi xe và bánh sau nằm ở bên phải
            smokeStates.transform.translate(56.f, 10.f);
        }
        target.draw(m_smokeSprite, smokeStates);
    }

    if (!m_usesFallback && m_sprite.getTexture()) {
        target.draw(m_sprite, states);
    } else {
        target.draw(m_fallbackShape, states);
    }
}

void CCAR::setupFallback() {
    setFallbackSize({ 60.f, 40.f });
    static const sf::Color colors[] = {
        sf::Color::Red,
        sf::Color(50, 100, 220),
        sf::Color(255, 165, 0),
        sf::Color(240, 240, 240)
    };
    int c = std::rand() % 4;
    sf::Color chosenColor = colors[c];
    setFallbackColor(chosenColor);
    m_fallbackShape.setOutlineThickness(2.f);
    m_fallbackShape.setOutlineColor(sf::Color(40, 40, 40));
}
