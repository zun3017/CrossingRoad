#include "Player.h"
#include "../Core/Game.h"  // Cho WINDOW_WIDTH, WINDOW_HEIGHT

Player::Player(CharacterType type)
    : m_type(type)
    , m_score(0)
    , m_lives(3)
    , m_isDead(false)
    , m_frameIndex(0)
    , m_animTimer(0.f)
{
    // Đặt tên theo loại nhân vật
    switch (m_type) {
    case CharacterType::Pikachu: m_name = "Pikachu"; break;
    case CharacterType::Kirby:   m_name = "Kirby";   break;
    case CharacterType::Mario:   m_name = "Mario";    break;
    }

    setupFallback();

    // Vị trí khởi đầu: giữa dưới màn hình, căn vào lưới
    // Tính ô giữa theo chiều ngang
    int gridCenterX = (WINDOW_WIDTH / GRID_SIZE) / 2;
    int gridBottomY = (WINDOW_HEIGHT / GRID_SIZE) - 1;

    // Đặt vị trí, căn giữa nhân vật trong ô lưới
    float offsetX = (GRID_SIZE - PLAYER_SIZE) / 2.f; // 4px padding mỗi bên
    float offsetY = (GRID_SIZE - PLAYER_SIZE) / 2.f;

    setPosition(
        static_cast<float>(gridCenterX * GRID_SIZE) + offsetX,
        static_cast<float>(gridBottomY * GRID_SIZE) + offsetY
    );
}

void Player::setupFallback() {
    // Kích thước nhỏ hơn ô lưới để có padding đẹp
    setFallbackSize({ PLAYER_SIZE, PLAYER_SIZE });

    // Màu khác nhau cho từng loại nhân vật
    switch (m_type) {
    case CharacterType::Pikachu:
        setFallbackColor(sf::Color::Yellow);
        break;
    case CharacterType::Kirby:
        setFallbackColor(sf::Color(255, 105, 180)); // Hồng
        break;
    case CharacterType::Mario:
        setFallbackColor(sf::Color::Red);
        break;
    }

    // Viền đen để dễ nhìn trên mọi nền
    m_fallbackShape.setOutlineThickness(2.f);
    m_fallbackShape.setOutlineColor(sf::Color::Black);
}

void Player::update(float dt) {
    if (m_isDead) return;

    // Cập nhật animation timer
    m_animTimer += dt;
    if (m_animTimer >= ANIM_FRAME_TIME) {
        m_animTimer = 0.f;
        m_frameIndex = (m_frameIndex + 1) % ANIM_FRAME_COUNT;
        // Khi có sprite sheet: m_currentFrame = sf::IntRect(m_frameIndex * 48, 0, 48, 48);
    }
}

sf::FloatRect Player::getBounds() const {
    sf::Vector2f pos = getPosition();
    return sf::FloatRect(pos.x, pos.y, PLAYER_SIZE, PLAYER_SIZE);
}

void Player::moveUp() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    // Cho phép đi lên trên màn hình (để hoàn thành level)
    pos.y -= static_cast<float>(GRID_SIZE);
    setPosition(pos);
}

void Player::moveDown() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newY = pos.y + static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi xuống dưới màn hình
    if (newY + PLAYER_SIZE <= static_cast<float>(WINDOW_HEIGHT)) {
        setPosition(pos.x, newY);
    }
}

void Player::moveLeft() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newX = pos.x - static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi ra ngoài bên trái
    if (newX >= 0.f) {
        setPosition(newX, pos.y);
    }
}

void Player::moveRight() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newX = pos.x + static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi ra ngoài bên phải
    if (newX + PLAYER_SIZE <= static_cast<float>(WINDOW_WIDTH)) {
        setPosition(newX, pos.y);
    }
}

void Player::reset(float x, float y) {
    setPosition(x, y);
    m_isDead = false;
    m_animTimer = 0.f;
    m_frameIndex = 0;
}

void Player::die() {
    if (!m_isDead) {
        m_isDead = true;
        m_lives--;
    }
}

bool Player::isAlive() const {
    return !m_isDead && m_lives > 0;
}
