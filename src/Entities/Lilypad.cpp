#include "Lilypad.h"

Lilypad::Lilypad(float x, float y, float speed, int direction)
    : m_speed(speed)
    , m_direction(direction)
{
    setPosition(x, y);

    // Kích thước lá sen: 56x40 pixel
    setFallbackSize({ LILYPAD_WIDTH, LILYPAD_HEIGHT });
    // Xanh lá đậm
    setFallbackColor(sf::Color(0, 100, 0));

    // Viền xanh đậm hơn
    m_fallbackShape.setOutlineThickness(2.f);
    m_fallbackShape.setOutlineColor(sf::Color(0, 80, 0));
    
    if (loadTexture("assets/textures/log.png")) {
        auto texSize = m_sprite.getTexture()->getSize();
        m_sprite.setScale(LILYPAD_WIDTH / texSize.x, LILYPAD_HEIGHT / texSize.y);
    }
}

void Lilypad::update(float dt) {
    sf::Vector2f pos = getPosition();
    pos.x += m_speed * static_cast<float>(m_direction) * dt;

    // Quấn vòng: khi lá sen đi ra ngoài một bên, xuất hiện lại ở bên kia
    if (m_direction > 0 && pos.x > SCREEN_WIDTH) {
        // Đi sang phải, ra ngoài bên phải -> quay lại bên trái
        pos.x = -LILYPAD_WIDTH;
    }
    else if (m_direction < 0 && pos.x + LILYPAD_WIDTH < 0.f) {
        // Đi sang trái, ra ngoài bên trái -> quay lại bên phải
        pos.x = SCREEN_WIDTH;
    }

    setPosition(pos);
}

sf::FloatRect Lilypad::getBounds() const {
    sf::Vector2f pos = getPosition();
    return sf::FloatRect(pos.x, pos.y, LILYPAD_WIDTH, LILYPAD_HEIGHT);
}
