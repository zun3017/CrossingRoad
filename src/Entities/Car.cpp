#include "Car.h"
#include <random>

// Bộ sinh số ngẫu nhiên cho xe ô tô
static std::mt19937& getCarRNG() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

Car::Car(float x, float y, float speed, int direction)
    : Vehicle(x, y, speed, direction)
{
    setupFallback();
}

void Car::setupFallback() {
    // Kích thước xe: 60x40 pixel
    setFallbackSize({ 60.f, 40.f });

    // Chọn màu ngẫu nhiên từ 4 lựa chọn
    static const sf::Color colors[] = {
        sf::Color::Red,                // Đỏ
        sf::Color(50, 100, 220),       // Xanh dương
        sf::Color(255, 165, 0),        // Cam
        sf::Color(240, 240, 240)       // Trắng
    };

    std::uniform_int_distribution<int> colorDist(0, 3);
    sf::Color chosenColor = colors[colorDist(getCarRNG())];
    setFallbackColor(chosenColor);

    // Viền tối để tạo cảm giác 3D
    m_fallbackShape.setOutlineThickness(2.f);
    m_fallbackShape.setOutlineColor(sf::Color(40, 40, 40));
}
