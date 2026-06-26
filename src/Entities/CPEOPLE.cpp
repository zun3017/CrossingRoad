#include "CPEOPLE.h"
#include "../Core/CGAME.h"  // Cho WINDOW_WIDTH, WINDOW_HEIGHT

CPEOPLE::CPEOPLE(CharacterType type)
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
    
    // Load hình ảnh
    if (loadTexture("assets/textures/player.png")) {
        // CPEOPLE scale
        auto texSize = m_sprite.getTexture()->getSize();
        int frameW = texSize.x / 4;
        int frameH = texSize.y / 4;
        m_sprite.setScale(PLAYER_SIZE / frameW, PLAYER_SIZE / frameH);
        m_sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
    }

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

void CPEOPLE::setupFallback() {
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


void CPEOPLE::startMove(float dx, float dy) {
    m_startPos = getPosition();
    m_targetPos = m_startPos + sf::Vector2f(dx, dy);
    m_isAnimating = true;
    m_animTimer = 0.f;
    
    if (dy < 0.f) m_animRow = 0;      // Lên
    else if (dy > 0.f) m_animRow = 1; // Xuống
    else if (dx > 0.f) m_animRow = 2; // Phải
    else if (dx < 0.f) m_animRow = 3; // Trái
}

void CPEOPLE::forcePosition(float x, float y) {
    setPosition(x, y);
    m_targetPos = sf::Vector2f(x, y);
    m_startPos = sf::Vector2f(x, y);
    m_isAnimating = false;
}

void CPEOPLE::update(float dt) {
    if (m_isDead) return;

    if (m_isAnimating) {
        m_animTimer += dt;
        
        // Trượt LERP tọa độ
        float progress = m_animTimer / 0.15f;
        if (progress > 1.0f) progress = 1.0f;
        sf::Vector2f newPos = m_startPos + (m_targetPos - m_startPos) * progress;
        setPosition(newPos);
        
        // Thay đổi sprite rect theo thời gian
        int currentFrame = 1 + int(progress * 2.f); 
        if (currentFrame >= 4) currentFrame = 0; 

        if (m_texturesLoaded && m_sprite.getTexture() != nullptr) {
            auto tex = m_sprite.getTexture();
            int frameW = tex->getSize().x / 4;
            int frameH = tex->getSize().y / 4;
            m_sprite.setTextureRect(sf::IntRect(currentFrame * frameW, m_animRow * frameH, frameW, frameH));
        }
        
        if (m_animTimer >= 0.15f) { // Kết thúc
            m_isAnimating = false;
            setPosition(m_targetPos);
            
            if (m_texturesLoaded && m_sprite.getTexture() != nullptr) {
                auto tex = m_sprite.getTexture();
                int frameW = tex->getSize().x / 4;
                int frameH = tex->getSize().y / 4;
                m_sprite.setTextureRect(sf::IntRect(0, m_animRow * frameH, frameW, frameH));
            }
        }
    }
}


sf::FloatRect CPEOPLE::getBounds() const {
    sf::Vector2f pos = getPosition();
    return sf::FloatRect(pos.x, pos.y, PLAYER_SIZE, PLAYER_SIZE);
}

void CPEOPLE::moveUp() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    // Cho phép đi lên trên màn hình (để hoàn thành level)
    startMove(0.f, -static_cast<float>(GRID_SIZE));
}

void CPEOPLE::moveDown() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newY = pos.y + static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi xuống dưới màn hình
    if (newY + PLAYER_SIZE <= static_cast<float>(WINDOW_HEIGHT)) {
        setPosition(pos.x, newY);
    }
}

void CPEOPLE::moveLeft() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newX = pos.x - static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi ra ngoài bên trái
    if (newX >= 0.f) {
        setPosition(newX, pos.y);
    }
}

void CPEOPLE::moveRight() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newX = pos.x + static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi ra ngoài bên phải
    if (newX + PLAYER_SIZE <= static_cast<float>(WINDOW_WIDTH)) {
        setPosition(newX, pos.y);
    }
}

void CPEOPLE::reset(float x, float y) {
    setPosition(x, y);
    m_isDead = false;
    m_animTimer = 0.f;
    m_frameIndex = 0;
}

void CPEOPLE::die() {
    if (!m_isDead) {
        m_isDead = true;
        m_lives--;
    }
}

bool CPEOPLE::isAlive() const {
    return !m_isDead && m_lives > 0;
}



