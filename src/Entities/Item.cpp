#include "Item.h"

Item::Item(float x, float y, int points)
    : m_points(points)
    , m_isCollected(false)
{
    setPosition(x, y);

    // Kích thước nhỏ (24x24 pixel)
    setFallbackSize({ 24.f, 24.f });
    // Màu cam nổi bật
    setFallbackColor(sf::Color(255, 165, 0));

    // Viền cam đậm để dễ nhìn
    m_fallbackShape.setOutlineThickness(1.f);
    m_fallbackShape.setOutlineColor(sf::Color(200, 120, 0));
}

void Item::update(float dt) {
    // Vật phẩm tĩnh, không di chuyển
    (void)dt;
}

sf::FloatRect Item::getBounds() const {
    sf::Vector2f pos = getPosition();
    return sf::FloatRect(pos.x, pos.y, 24.f, 24.f);
}
