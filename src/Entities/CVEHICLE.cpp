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
    if (!m_usesFallback && m_sprite.getTexture() != nullptr) {
        m_animTimer += dt;
        if (m_animTimer >= 0.1f) {
            m_animTimer = 0.f;
            m_frameIndex = (m_frameIndex + 1) % m_totalFrames;
            
            auto texSize = m_sprite.getTexture()->getSize();
            int frameW = texSize.x / m_totalFrames;
            int frameH = m_sprite.getTextureRect().height;
            int rowY = m_sprite.getTextureRect().top;
            
            // Lật ngược hình ảnh xe nếu xe đang đi sang trái
            if (m_direction < 0) {
                m_sprite.setTextureRect(sf::IntRect(m_frameIndex * frameW + frameW, rowY, -frameW, frameH));
            } else {
                m_sprite.setTextureRect(sf::IntRect(m_frameIndex * frameW, rowY, frameW, frameH));
            }
        }
    }
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

