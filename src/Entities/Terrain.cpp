#include "Terrain.h"

Terrain::Terrain(float posY, float width, float height)
    : m_posY(posY)
    , m_width(width)
    , m_height(height)
{
    m_background.setSize({ m_width, m_height });
    m_background.setPosition(0.f, m_posY);
    // Màu mặc định, lớp con sẽ thay đổi
    m_background.setFillColor(sf::Color::Green);
}

sf::FloatRect Terrain::getBounds() const {
    return sf::FloatRect(0.f, m_posY, m_width, m_height);
}

void Terrain::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_background, states);
}
