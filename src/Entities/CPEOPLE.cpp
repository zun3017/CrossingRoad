#include "CPEOPLE.h"
// (Game.h đã được include qua CPEOPLE.h)

CPEOPLE::CPEOPLE()
    : m_score(0)
    , m_lives(3)
    , m_isDead(false)
    , m_frameIndex(0)
    , m_animTimer(0.f)
{
    setupFallback();
    
    // Load hình ảnh
    if (loadTexture("assets/textures/player.png")) {
        m_texturesLoaded = true;
        // CPEOPLE scale
        auto texSize = m_sprite.getTexture()->getSize();
        int frameW = texSize.x / 4;
        int frameH = texSize.y / 4;
        m_sprite.setScale(PLAYER_SIZE / frameW, PLAYER_SIZE / frameH);
        m_sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
    }

    // Vị trí khởi đầu: giữa dưới màn hình, căn vào lưới
    int gridCenterX = (WINDOW_WIDTH / GRID_SIZE) / 2;
    int gridBottomY = (WINDOW_HEIGHT / GRID_SIZE) - 1;

    float offsetX = (GRID_SIZE - PLAYER_SIZE) / 2.f;
    float offsetY = (GRID_SIZE - PLAYER_SIZE) / 2.f;

    setPosition(
        static_cast<float>(gridCenterX * GRID_SIZE) + offsetX,
        static_cast<float>(gridBottomY * GRID_SIZE) + offsetY
    );
}

void CPEOPLE::setupFallback() {
    setFallbackSize({ PLAYER_SIZE, PLAYER_SIZE });
    setFallbackColor(sf::Color::Yellow); // Màu vàng mặc định
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
    if (m_isDead) {
        // Cập nhật animation khi chết (như chìm sông player_drown.png)
        if (m_sprite.getTexture() != nullptr && !m_isDrowned) {
            m_animTimer += dt;
            int currentFrame = static_cast<int>(m_animTimer / 0.12f) % 4;
            auto tex = m_sprite.getTexture();
            int frameW = static_cast<int>(tex->getSize().x) / 4;
            int frameH = static_cast<int>(tex->getSize().y) / 4;
            m_sprite.setTextureRect(sf::IntRect(currentFrame * frameW, m_animRow * frameH, frameW, frameH));
        }
        return;
    }

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
    // Cho phép đi lên trên màn hình (để hoàn thành level)
    startMove(0.f, -static_cast<float>(GRID_SIZE));
}

void CPEOPLE::moveDown() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newY = pos.y + static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi xuống dưới màn hình
    if (newY + PLAYER_SIZE <= static_cast<float>(WINDOW_HEIGHT)) {
        startMove(0.f, static_cast<float>(GRID_SIZE)); // Dùng LERP animation nhất quán
    }
}

void CPEOPLE::moveLeft() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newX = pos.x - static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi ra ngoài bên trái
    if (newX >= 0.f) {
        startMove(-static_cast<float>(GRID_SIZE), 0.f); // Dùng LERP animation nhất quán
    }
}

void CPEOPLE::moveRight() {
    if (m_isDead) return;
    sf::Vector2f pos = getPosition();
    float newX = pos.x + static_cast<float>(GRID_SIZE);
    // Giới hạn: không đi ra ngoài bên phải
    if (newX + PLAYER_SIZE <= static_cast<float>(WINDOW_WIDTH)) {
        startMove(static_cast<float>(GRID_SIZE), 0.f); // Dùng LERP animation nhất quán
    }
}

void CPEOPLE::reset(float x, float y) {
    setPosition(x, y);
    m_isDead = false;
    m_isDrowned = false;
    m_animTimer = 0.f;
    m_frameIndex = 0;

    // Khôi phục texture player từ ResourceManager (không dùng m_texture vì nó luôn rỗng)
    if (m_texturesLoaded && !m_texturePath.empty()) {
        loadTexture(m_texturePath);  // Reload lại đúng texture player
        if (!m_usesFallback && m_sprite.getTexture() != nullptr) {
            auto texSize = m_sprite.getTexture()->getSize();
            int frameW = static_cast<int>(texSize.x) / 4;
            int frameH = static_cast<int>(texSize.y) / 4;
            m_sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
            m_sprite.setScale(PLAYER_SIZE / static_cast<float>(frameW),
                              PLAYER_SIZE / static_cast<float>(frameH));
            m_sprite.setOrigin(0.f, 0.f);
        }
    } else {
        // Không có texture → dùng fallback và reset màu
        setupFallback();
    }
    m_sprite.setColor(sf::Color::White);
}

void CPEOPLE::die(DeathType type, const sf::Texture* deathTexture) {
    if (!m_isDead) {
        m_isDead = true;
        m_lives--;
        
        if (type == DeathType::HitByCar && deathTexture != nullptr) {
            m_sprite.setTexture(*deathTexture, true);
            auto texSize = deathTexture->getSize();
            m_sprite.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
            m_sprite.setScale(PLAYER_SIZE * 1.5f / texSize.x, PLAYER_SIZE * 1.5f / texSize.y);
            m_sprite.setOrigin(texSize.x * 0.166f, texSize.y * 0.166f);
        } else if (type == DeathType::Drowned) {
            if (deathTexture != nullptr) {
                m_sprite.setTexture(*deathTexture, true);
                auto texSize = deathTexture->getSize();
                int frameW = static_cast<int>(texSize.x) / 4;
                int frameH = static_cast<int>(texSize.y) / 4;
                m_sprite.setTextureRect(sf::IntRect(0, m_animRow * frameH, frameW, frameH));
                m_sprite.setScale(PLAYER_SIZE / static_cast<float>(frameW), PLAYER_SIZE / static_cast<float>(frameH));
                m_sprite.setOrigin(0.f, 0.f);
                m_isDrowned = false;
            } else {
                m_isDrowned = true;
            }
        } else {
            m_fallbackShape.setFillColor(sf::Color::Red);
        }
    }
}

void CPEOPLE::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (m_isDrowned) return; // Không vẽ nếu chết chìm
    Entity::draw(target, states);
}

bool CPEOPLE::isAlive() const {
    return !m_isDead && m_lives > 0;
}



