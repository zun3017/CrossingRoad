#include "Terrain.h"
#include "../Core/ResourceManager.h"
#include <iostream>

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

bool Terrain::loadTexture(const std::string& path) {
    try {
        auto& tex = ResourceManager<sf::Texture>::getInstance().get(path);
        if (tex.getSize().x > 0) {
            m_sprite.setTexture(tex);
            // Scale sprite to fit the width and height
            m_sprite.setScale(m_width / tex.getSize().x, m_height / tex.getSize().y);
            m_sprite.setPosition(0.f, m_posY);
            m_hasTexture = true;
            return true;
        }
    } catch (...) {}
    
    m_hasTexture = false;
    return false;
}

void Terrain::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (m_hasTexture) {
        target.draw(m_sprite, states);
    } else {
        target.draw(m_background, states);
    }
}
