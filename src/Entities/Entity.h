#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Lớp cơ sở cho tất cả đối tượng trong game
// Kế thừa sf::Drawable để vẽ được, sf::Transformable để có vị trí/xoay/scale
class Entity : public sf::Drawable, public sf::Transformable {
public:
    Entity();
    virtual ~Entity() = default;

    // Cập nhật trạng thái mỗi frame (pure virtual)
    virtual void update(float dt) = 0;

    // Lấy hình chữ nhật bao quanh để kiểm tra va chạm (pure virtual)
    virtual sf::FloatRect getBounds() const = 0;

    // Tải texture từ file, trả về false nếu không tìm thấy file
    bool loadTexture(const std::string& path);

    // Thiết lập hình dạng dự phòng khi không có texture
    void setFallbackColor(sf::Color color);
    void setFallbackSize(sf::Vector2f size);

    // Getter/setter cho vận tốc
    sf::Vector2f getVelocity() const { return m_velocity; }
    void setVelocity(sf::Vector2f vel) { m_velocity = vel; }
    void setVelocity(float vx, float vy) { m_velocity = { vx, vy }; }

    // Kiểm tra xem entity có đang dùng fallback shape không
    bool isUsingFallback() const { return m_usesFallback; }

protected:
    // Override sf::Drawable - vẽ sprite hoặc fallback shape
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Sprite m_sprite;
    sf::Texture m_texture;
    sf::Vector2f m_velocity;

    // Hình dạng dự phòng khi không có texture
    sf::RectangleShape m_fallbackShape;
    bool m_usesFallback = true;
};
