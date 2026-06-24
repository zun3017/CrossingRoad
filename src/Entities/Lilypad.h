#pragma once
#include "Entity.h"

// Lá sen trên sông - người chơi phải nhảy lên để qua sông an toàn
// Di chuyển ngang, quấn vòng khi ra ngoài màn hình
class Lilypad : public Entity {
public:
    Lilypad(float x, float y, float speed, int direction);
    ~Lilypad() override = default;

    void update(float dt) override;
    sf::FloatRect getBounds() const override;

    float getSpeed() const { return m_speed; }
    int getDirection() const { return m_direction; }

private:
    float m_speed;       // Tốc độ di chuyển (pixel/giây)
    int m_direction;     // +1 = sang phải, -1 = sang trái

    static constexpr float LILYPAD_WIDTH = 56.f;
    static constexpr float LILYPAD_HEIGHT = 40.f;
    static constexpr float SCREEN_WIDTH = 800.f;
};
