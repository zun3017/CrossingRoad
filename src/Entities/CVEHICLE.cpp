#include "CVEHICLE.h"
#include <cmath>

CVEHICLE::CVEHICLE(float x, float y, float speed, int direction)
    : m_speed(speed)
    , m_direction(direction)
    , m_stopped(false)
{
    setPosition(x, y);
}

void CVEHICLE::update(float dt) {
    if (m_stopped) return;
    Move(dt);
    // (updateAnimation đã xóa - xe dùng ảnh đơn, không có spritesheet animation)
}

void CVEHICLE::Move(float dt) {
    // Di chuyển ngang: vị trí += tốc độ * hướng * delta_time
    sf::Vector2f pos = getPosition();
    pos.x += m_speed * static_cast<float>(m_direction) * dt;
    setPosition(pos);
}


sf::FloatRect CVEHICLE::getBounds() const {
    sf::Vector2f pos = getPosition();
    if (!m_usesFallback && m_sprite.getTexture() != nullptr) {
        // Dùng bounds thực của sprite (đã tính scale và origin)
        sf::FloatRect spriteBounds = m_sprite.getGlobalBounds();
        // Tính lại theo vị trí transform của entity (vì sprite dùng origin để flip)
        return sf::FloatRect(
            pos.x,
            pos.y,
            std::abs(spriteBounds.width),
            std::abs(spriteBounds.height)
        );
    }
    // Dùng fallback shape khi không có texture
    sf::Vector2f size = m_fallbackShape.getSize();
    return sf::FloatRect(pos.x, pos.y, size.x, size.y);
}

bool CVEHICLE::isOffScreen() const {
    float x = getPosition().x;
    // Xe ra ngoài quá xa bên trái hoặc bên phải
    return (x < -200.f || x > 1000.f);
}

