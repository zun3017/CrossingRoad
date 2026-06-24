#pragma once
#include "Terrain.h"
#include "Lilypad.h"
#include <vector>
#include <memory>

// Hàng sông - nguy hiểm! Người chơi phải nhảy lên lá sen
// Rơi xuống nước = chết
class River : public Terrain {
public:
    // lilypadSpeed: tốc độ lá sen, direction: +1 phải, -1 trái
    River(float posY, float lilypadSpeed, int direction);
    ~River() override = default;

    void update(float dt) override;

    // Sinh lá sen mới
    void spawnLilypad();

    // Truy cập danh sách lá sen
    const std::vector<std::unique_ptr<Lilypad>>& getLilypads() const { return m_lilypads; }

    // Kiểm tra xem người chơi có đang ở trên mặt nước không
    // Trả về true nếu playerBounds nằm trong sông VÀ KHÔNG trên lá sen nào
    bool isOnWater(sf::FloatRect playerBounds) const;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<std::unique_ptr<Lilypad>> m_lilypads;
    float m_lilypadSpeed;    // Tốc độ lá sen
    int m_direction;         // Hướng trôi
    float m_spawnTimer;      // Bộ đếm spawn
    float m_spawnInterval;   // Khoảng cách spawn (1.5-3.5s)
};
