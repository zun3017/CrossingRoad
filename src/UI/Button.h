#pragma once

// Button.h - Nút bấm có hiệu ứng hover và click
// Hỗ trợ callback function khi được nhấn

#include "Component.h"
#include <functional>
#include <string>

class Button : public Component {
public:
    // Tạo nút bấm với vị trí, kích thước, text, font và callback
    Button(float x, float y, float width, float height,
           const std::string& text, sf::Font& font,
           std::function<void()> onClick = nullptr);

    ~Button() override = default;

    // Xử lý sự kiện chuột (hover, click)
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;

    // Cập nhật animation (scale khi hover)
    void update(float dt) override;

    // Vẽ nút bấm (hình chữ nhật + text)
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // Đặt callback khi nhấn nút
    void setOnClick(std::function<void()> onClick) { m_onClick = std::move(onClick); }

    // Đặt màu cho các trạng thái
    void setNormalColor(const sf::Color& color) { m_normalColor = color; }
    void setHoverColor(const sf::Color& color) { m_hoverColor = color; }
    void setClickColor(const sf::Color& color) { m_clickColor = color; }
    void setTextColor(const sf::Color& color) { m_text.setFillColor(color); }

    // Đặt lại text hiển thị
    void setText(const std::string& text);

    // Lấy bounds để kiểm tra va chạm
    sf::FloatRect getGlobalBounds() const;

private:
    // Căn giữa text trong nút
    void centerText();

    sf::RectangleShape m_shape;          // Hình dạng nút bấm
    sf::Text m_text;                      // Text hiển thị trên nút

    std::function<void()> m_onClick;      // Hàm callback khi nhấn

    sf::Color m_normalColor;              // Màu bình thường
    sf::Color m_hoverColor;               // Màu khi hover
    sf::Color m_clickColor;               // Màu khi nhấn

    bool m_isHovered = false;             // Chuột đang ở trên nút?
    bool m_isPressed = false;             // Nút đang được nhấn?

    float m_currentScale = 1.0f;          // Scale hiện tại (cho animation)
    float m_targetScale = 1.0f;           // Scale đích (1.0 hoặc 1.05)
};
