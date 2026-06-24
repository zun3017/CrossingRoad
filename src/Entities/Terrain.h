#pragma once
#include <SFML/Graphics.hpp>

// Lớp cơ sở cho các hàng địa hình (cỏ, đường, sông)
// Mỗi terrain là 1 hàng ngang rộng 800px, cao 48px (1 ô lưới)
class Terrain : public sf::Drawable {
public:
    Terrain(float posY, float width = 800.f, float height = 48.f);
    virtual ~Terrain() = default;

    // Cập nhật trạng thái mỗi frame (pure virtual)
    virtual void update(float dt) = 0;

    // Lấy vị trí Y của hàng
    float getPosY() const { return m_posY; }

    // Lấy chiều rộng và cao
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }

    // Lấy hình chữ nhật bao quanh toàn bộ hàng
    sf::FloatRect getBounds() const;

protected:
    // Vẽ nền hàng
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    float m_posY;    // Vị trí Y (đỉnh hàng)
    float m_width;   // Chiều rộng (800px)
    float m_height;  // Chiều cao (48px = 1 ô lưới)
    sf::RectangleShape m_background;  // Nền màu của hàng
};
