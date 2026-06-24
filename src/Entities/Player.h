#pragma once
#include "Entity.h"
#include <string>

// Hằng số riêng cho Player (WINDOW_WIDTH, WINDOW_HEIGHT đã khai báo trong Game.h)
constexpr int GRID_SIZE = 48;
constexpr float PLAYER_SIZE = 40.f;

// Loại nhân vật người chơi
enum class CharacterType {
    Pikachu,  // Vàng
    Kirby,    // Hồng
    Mario     // Đỏ
};

// Nhân vật chính - di chuyển theo lưới, có mạng sống và điểm
class Player : public Entity {
public:
    explicit Player(CharacterType type = CharacterType::Pikachu);
    ~Player() override = default;

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
    int m_frameIndex = 0;
    float m_animTimer = 0.f;
    static constexpr float ANIM_FRAME_TIME = 0.15f;
    static constexpr int ANIM_FRAME_COUNT = 4;
};
