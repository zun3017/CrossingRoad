#pragma once
#include "Entity.h"

// Lớp cơ sở cho các phương tiện giao thông (xe hơi, xe tải)
// Di chuyển ngang qua màn hình, có thể dừng khi đèn đỏ
class CVEHICLE : public Entity {
public:
    CVEHICLE(float x, float y, float speed, int direction);
    ~CVEHICLE() override = default;

    void update(float dt) override;
    sf::FloatRect getBounds() const override;
    
    // Di chuyển đa hình 
    virtual void Move(float dt);

    // Kiểm tra xe đã đi ra ngoài màn hình chưa (x < -200 hoặc x > 1000)
    bool isOffScreen() const;

    // Getter/setter
    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }
    int getDirection() const { return m_direction; }

    // Dừng/tiếp tục khi đèn giao thông thay đổi
    void stop() { m_stopped = true; }
    void resume() { m_stopped = false; }
    bool isStopped() const { return m_stopped; }

    // Xe điên (Crazy Car / Speeding Car)
    bool isCrazy() const { return m_isCrazy; }
    void setCrazy(bool crazy) { m_isCrazy = crazy; }
    bool hasHonked() const { return m_hasHonked; }
    void setHonked(bool honked) { m_hasHonked = honked; }

protected:
    float m_speed;       // Tốc độ (pixel/giây)
    int m_direction;     // +1 = sang phải, -1 = sang trái
    bool m_stopped = false;  // Đang dừng vì đèn đỏ
    bool m_isCrazy = false;  // Cờ xe điên chạy nhanh xả khói
    bool m_hasHonked = false; // Đã bóp còi cảnh báo trong lượt chạy này chưa
};
