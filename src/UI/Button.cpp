// Button.cpp - Triển khai nút bấm UI với hiệu ứng hover/click

#include "Button.h"
#include <cmath>

Button::Button(float x, float y, float width, float height,
               const std::string& text, sf::Font& font,
               std::function<void()> onClick)
    : m_onClick(std::move(onClick))
    , m_normalColor(sf::Color(70, 70, 70))
    , m_hoverColor(sf::Color(100, 100, 100))
    , m_clickColor(sf::Color(40, 40, 40))
{
    // Thiết lập hình dạng nút
    m_shape.setSize(sf::Vector2f(width, height));
    m_shape.setPosition(x, y);
    m_shape.setFillColor(m_normalColor);
    m_shape.setOutlineThickness(2.0f);
    m_shape.setOutlineColor(sf::Color(200, 200, 200));

    // Đặt origin ở giữa để scale từ tâm
    m_shape.setOrigin(width / 2.0f, height / 2.0f);
    m_shape.setPosition(x + width / 2.0f, y + height / 2.0f);

    // Thiết lập text
    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::White);

    // Căn giữa text trong nút
    centerText();
}

void Button::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (!m_isVisible) return;

    // Lấy vị trí chuột theo tọa độ thế giới (hỗ trợ view)
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    // Tính bounds thực tế (sau khi đã scale)
    sf::FloatRect bounds = getGlobalBounds();

    // Kiểm tra chuột có nằm trong nút không
    bool mouseOver = bounds.contains(mousePos);

    if (event.type == sf::Event::MouseMoved) {
        // Cập nhật trạng thái hover
        m_isHovered = mouseOver;

        if (m_isHovered) {
            m_shape.setFillColor(m_isPressed ? m_clickColor : m_hoverColor);
            m_targetScale = 1.05f;
        } else {
            m_shape.setFillColor(m_normalColor);
            m_isPressed = false;
            m_targetScale = 1.0f;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left && mouseOver) {
            m_isPressed = true;
            m_shape.setFillColor(m_clickColor);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (m_isPressed && mouseOver) {
                // Gọi callback khi nhả chuột trên nút
                if (m_onClick) {
                    m_onClick();
                }
            }
            m_isPressed = false;
            m_shape.setFillColor(mouseOver ? m_hoverColor : m_normalColor);
        }
    }
}

void Button::update(float dt) {
    if (!m_isVisible) return;

    // Animation scale mượt khi hover
    // Nội suy tuyến tính (lerp) scale hiện tại đến scale đích
    float lerpSpeed = 10.0f; // Tốc độ nội suy
    m_currentScale += (m_targetScale - m_currentScale) * lerpSpeed * dt;

    // Clamp để tránh dao động
    if (std::abs(m_currentScale - m_targetScale) < 0.001f) {
        m_currentScale = m_targetScale;
    }

    m_shape.setScale(m_currentScale, m_currentScale);

    // Cập nhật vị trí text theo scale
    centerText();
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_isVisible) return;

    target.draw(m_shape, states);
    target.draw(m_text, states);
}

void Button::setText(const std::string& text) {
    m_text.setString(text);
    centerText();
}

sf::FloatRect Button::getGlobalBounds() const {
    return m_shape.getGlobalBounds();
}

void Button::centerText() {
    // Căn giữa text theo bounds của nút
    sf::FloatRect shapeBounds = m_shape.getGlobalBounds();
    sf::FloatRect textBounds = m_text.getLocalBounds();

    // Đặt origin của text ở giữa chính nó
    m_text.setOrigin(
        textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f
    );

    // Đặt vị trí text ở giữa nút
    m_text.setPosition(
        shapeBounds.left + shapeBounds.width / 2.0f,
        shapeBounds.top + shapeBounds.height / 2.0f
    );
}
