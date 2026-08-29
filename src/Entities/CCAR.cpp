#include "CCAR.h"
#include "../Core/ResourceManager.h"
#include "../Core/Game.h"

CCAR::CCAR(float x, float y, float speed, int direction, bool isCrazy)
    : CVEHICLE(x, y, speed, direction)
{
    m_isCrazy = isCrazy;

    try {
        std::string bluePath = Game::instance().getCarSkinPath();
        std::string carFiles[] = { bluePath, "assets/textures/car_red.png", "assets/textures/car_yellow.png" };
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

        // Luôn tải sẵn texture khói để khi xe bất ngờ hóa điên có thể xả khói ngay
        auto& smokeTex = ResourceManager<sf::Texture>::getInstance().get("assets/textures/smoke.png");
        if (smokeTex.getSize().x > 0) {
            const_cast<sf::Sprite&>(m_smokeSprite).setTexture(smokeTex);
            m_hasSmokeTexture = true;
            // Khói to 38px (to hơn đáng kể so với bánh xe ~16px)
            float smokeScale = 38.f / static_cast<float>(smokeTex.getSize().y);
            const_cast<sf::Sprite&>(m_smokeSprite).setScale(smokeScale, smokeScale);
        }
    } catch (...) {}

    setupFallback();
}

void CCAR::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();

    // 1. Nếu là xe điên, vẽ khói bốc ra ở dưới bánh xe sau (vẽ trước để khói nằm dưới thân xe)
    if (m_isCrazy && m_hasSmokeTexture && !m_stopped) {
        sf::RenderStates smokeStates = states;
        if (m_direction > 0) {
            // Xe chạy sang phải -> khói nằm dưới bánh sau ở bên trái
            smokeStates.transform.translate(-30.f, 14.f);
        } else {
            // Xe chạy sang trái -> khói nằm dưới bánh sau ở bên phải
            smokeStates.transform.translate(52.f, 14.f);
        }
        target.draw(m_smokeSprite, smokeStates);
    }

    // 2. Vẽ thân xe đè lên khói
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
