#include "CVEHICLE.h"

CVEHICLE::CVEHICLE(float x, float y, float speed, int direction)
    : m_speed(speed)
    , m_direction(direction)
    , m_stopped(false)
{
    setPosition(x, y);
}

void CVEHICLE::update(float dt) {
    // Nếu đang dừng (đèn đỏ) thì không di chuyển
    if (m_stopped) return;

    Move(dt);
    updateAnimation(dt);
}

void CVEHICLE::Move(float dt) {
    // Di chuyển ngang: vị trí += tốc độ * hướng * delta_time
    sf::Vector2f pos = getPosition();
    pos.x += m_speed * static_cast<float>(m_direction) * dt;
    setPosition(pos);
}

void CVEHICLE::updateAnimation(float dt) {
    // Không làm gì cả vì xe cộ trong game này là ảnh đơn (không phải spritesheet)
    // Việc lật ảnh đã được xử lý trong constructor của CCAR và CTRUCK.
}

sf::FloatRect CVEHICLE::getBounds() const {
    sf::Vector2f pos = getPosition();
    sf::Vector2f size = m_fallbackShape.getSize();
    return sf::FloatRect(pos.x, pos.y, size.x, size.y);
}

bool CVEHICLE::isOffScreen() const {
    float x = getPosition().x;
    // Xe ra ngoài quá xa bên trái hoặc bên phải
    return (x < -200.f || x > 1000.f);
}

