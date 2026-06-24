#pragma once
#include "Entity.h"

// Vật phẩm có thể nhặt được trên hàng cỏ
// Nằm yên, cho điểm khi người chơi đi qua
class Item : public Entity {
public:
    // Tạo vật phẩm tại vị trí (x, y) với số điểm cho trước
    Item(float x, float y, int points = 10);
    ~Item() override = default;

    void update(float dt) override;
    sf::FloatRect getBounds() const override;

    // Lấy số điểm của vật phẩm
    int getPoints() const { return m_points; }

    // Kiểm tra/đánh dấu đã thu thập
    bool isCollected() const { return m_isCollected; }
    void collect() { m_isCollected = true; }

private:
    int m_points;            // Số điểm thưởng
    bool m_isCollected = false;  // Đã được nhặt chưa
};
