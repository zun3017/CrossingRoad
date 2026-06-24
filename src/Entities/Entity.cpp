#include "Entity.h"
#include <iostream>

Entity::Entity()
    : m_velocity(0.f, 0.f)
    , m_usesFallback(true)
{
    // Kích thước mặc định = 1 ô lưới
    m_fallbackShape.setSize({ 48.f, 48.f });
    m_fallbackShape.setFillColor(sf::Color::White);
}

bool Entity::loadTexture(const std::string& path) {
    if (m_texture.loadFromFile(path)) {
        m_sprite.setTexture(m_texture);
        m_usesFallback = false;
        return true;
    }
    // Không tìm thấy texture -> dùng hình dạng dự phòng
    std::cerr << "[Entity] Khong the tai texture: " << path
              << " -> su dung hinh du phong.\n";
    m_usesFallback = true;
    return false;
}

void Entity::setFallbackColor(sf::Color color) {
    m_fallbackShape.setFillColor(color);
}

void Entity::setFallbackSize(sf::Vector2f size) {
    m_fallbackShape.setSize(size);
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Áp dụng transform (vị trí, xoay, scale) của Transformable
    states.transform *= getTransform();

    if (m_usesFallback) {
        target.draw(m_fallbackShape, states);
    }
    else {
        target.draw(m_sprite, states);
    }
}
