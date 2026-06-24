#pragma once

// Component.h - Lớp cơ sở cho tất cả thành phần UI
// Kế thừa sf::Drawable và sf::Transformable để hỗ trợ vẽ và biến đổi

#include <SFML/Graphics.hpp>

class Component : public sf::Drawable, public sf::Transformable {
public:
    Component() = default;
    virtual ~Component() = default;

    // Xử lý sự kiện (chuột, bàn phím, ...) - bắt buộc override
    virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;

    // Cập nhật logic mỗi frame (mặc định không làm gì)
    virtual void update(float dt) {}

    // Vẽ component lên màn hình - bắt buộc override
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;

    // Hiển thị / ẩn component
    void setVisible(bool visible) { m_isVisible = visible; }
    bool isVisible() const { return m_isVisible; }

    // Focus / unfocus component (dùng cho điều hướng bàn phím)
    void setFocused(bool focused) { m_isFocused = focused; }
    bool isFocused() const { return m_isFocused; }

protected:
    bool m_isVisible = true;   // Component có đang hiển thị không
    bool m_isFocused = false;  // Component có đang được focus không
};
