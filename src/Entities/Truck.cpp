#include "Truck.h"

Truck::Truck(float x, float y, float speed, int direction)
    : Vehicle(x, y, speed, direction)
{
    setupFallback();
}

void Truck::setupFallback() {
    // Xe tải lớn hơn xe ô tô: 120x44 pixel
    setFallbackSize({ 120.f, 44.f });
    // Màu nâu
    setFallbackColor(sf::Color(139, 69, 19));

    // Viền tối
    m_fallbackShape.setOutlineThickness(2.f);
    m_fallbackShape.setOutlineColor(sf::Color(80, 40, 10));
}
