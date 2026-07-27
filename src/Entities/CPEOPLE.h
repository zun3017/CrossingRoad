#pragma once
#include "Entity.h"

// Hằng số riêng cho CPEOPLE (WINDOW_WIDTH, WINDOW_HEIGHT đã khai báo trong CGAME.h)
constexpr int GRID_SIZE = 48;
constexpr float PLAYER_SIZE = 40.f;

enum class DeathType {
    HitByCar,
    Drowned
};

// Nhân vật chính - di chuyển theo lưới, có mạng sống và điểm
class CPEOPLE : public Entity {
public:
    CPEOPLE();
    ~CPEOPLE() override = default;

    void update(float dt) override;
    sf::FloatRect getBounds() const override;

    // Di chuyển theo lưới (mỗi bước = GRID_SIZE pixel)
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    // Đặt lại vị trí về tọa độ cho trước
    void reset(float x, float y);

    // Xử lý khi nhân vật chết
    void die(DeathType type = DeathType::HitByCar, const sf::Texture* deathTexture = nullptr);
    bool isAlive() const;

    // Hồi sinh (bỏ trạng thái chết, KHÔNG tăng mạng)
    void revive() { m_isDead = false; }

    // Getters & setters
    int getScore() const { return m_score; }
    void addScore(int points) { m_score += points; }
    void setScore(int score) { m_score = score; }
    int getLives() const { return m_lives; }
    void setLives(int lives) { m_lives = lives; }
    bool isDead() const { return m_isDead; }

private:
    void setupFallback();

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    int m_score = 0;
    int m_lives = 3;
    bool m_isDead = false;
    bool m_isDrowned = false;

    // Animation
    sf::IntRect m_currentFrame;
    
public:
    // LERP & Animation
    sf::Vector2f m_startPos;
    sf::Vector2f m_targetPos;
    bool m_isAnimating = false;
    float m_animTimer = 0.f;
    int m_animRow = 0;
    bool m_texturesLoaded = false;
    
    void startMove(float dx, float dy);
    void setTextureLoaded(bool loaded) { m_texturesLoaded = loaded; }
    void forcePosition(float x, float y);
    int m_frameIndex = 0;
    static constexpr float ANIM_FRAME_TIME = 0.15f;
    static constexpr int ANIM_FRAME_COUNT = 4;
};
