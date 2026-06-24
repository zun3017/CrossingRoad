#pragma once
#include <SFML/Graphics.hpp>

// Trạng thái đèn giao thông
enum class LightState {
    Green,  // Xe được đi
    Red     // Xe phải dừng
};

// Đèn giao thông - điều khiển lưu lượng xe trên đường
// Tự động chuyển đổi giữa Xanh và Đỏ theo bộ đếm thời gian
class TrafficLight {
public:
    TrafficLight();
    TrafficLight(float posX, float posY);

    // Cập nhật bộ đếm thời gian và chuyển trạng thái
    void update(float dt);

    // Vẽ đèn giao thông
    void draw(sf::RenderTarget& target) const;

    // Kiểm tra trạng thái
    LightState getState() const { return m_state; }
    bool isRed() const { return m_state == LightState::Red; }
    bool isGreen() const { return m_state == LightState::Green; }

    // Đặt vị trí đèn
    void setPosition(float x, float y);

private:
    // Ngẫu nhiên thời gian xanh/đỏ
    void randomizeDuration();

    LightState m_state;
    float m_timer;           // Bộ đếm thời gian hiện tại
    float m_greenDuration;   // Thời gian xanh (3-6 giây)
    float m_redDuration;     // Thời gian đỏ (2-4 giây)

    sf::RectangleShape m_shape;    // Bóng đèn (12x30px, đổi màu)
    sf::RectangleShape m_housing;  // Vỏ đèn (nền đen)
};
