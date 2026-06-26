#pragma once
#include "Entity.h"
#include <string>

// Hằng số riêng cho CPEOPLE (WINDOW_WIDTH, WINDOW_HEIGHT đã khai báo trong CGAME.h)
constexpr int GRID_SIZE = 48;
constexpr float PLAYER_SIZE = 40.f;

// Loại nhân vật người chơi
enum class CharacterType {
    Pikachu,  // Vàng
    Kirby,    // Hồng
    Mario     // Đỏ
};

// Nhân vật chính - di chuyển theo lưới, có mạng sống và điểm
class CPEOPLE : public Entity {
public:
    explicit CPEOPLE(CharacterType type = CharacterType::Pikachu);
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
    void die();
    bool isAlive() const;

    // Hồi sinh (bỏ trạng thái chết, KHÔNG tăng mạng)
    void revive() { m_isDead = false; }

    // Getters & setters
    int getScore() const { return m_score; }
    void addScore(int points) { m_score += points; }
    void setScore(int score) { m_score = score; }
    int getLives() const { return m_lives; }
    void setLives(int lives) { m_lives = lives; }
    CharacterType getCharacterType() const { return m_type; }
    std::string getName() const { return m_name; }
    bool isDead() const { return m_isDead; }

private:
    // Thiết lập màu và kích thước fallback theo loại nhân vật
    void setupFallback();

    CharacterType m_type;
    std::string m_name;
    int m_score = 0;
    int m_lives = 3;
    bool m_isDead = false;

    // Animation (chuẩn bị cho khi có sprite sheet)
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


