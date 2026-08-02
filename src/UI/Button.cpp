// Button.cpp - Triển khai nút bấm UI với hiệu ứng hover/click

#include "Button.h"
#include "../Core/Game.h"
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

Button::Button(float x, float y, float width, float height,
               const sf::Texture& texture,
               std::function<void()> onClick)
    : m_onClick(std::move(onClick))
    , m_hasTexture(true)
    , m_normalColor(sf::Color(255, 255, 255))
    , m_hoverColor(sf::Color(240, 240, 240))
    , m_clickColor(sf::Color(180, 180, 180))
{
    m_sprite.setTexture(texture);
    sf::Vector2u texSize = texture.getSize();
    if (texSize.x > 0 && texSize.y > 0) {
        m_baseScale = sf::Vector2f(width / static_cast<float>(texSize.x), height / static_cast<float>(texSize.y));
        m_sprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
        m_sprite.setScale(m_baseScale);
    }
    m_sprite.setPosition(x + width / 2.0f, y + height / 2.0f);
}

void Button::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (!m_isVisible) return;

    // Lấy vị trí chuột theo tọa độ thế giới (hỗ trợ view ảo 800x600)
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

    // Tính bounds thực tế (sau khi đã scale)
    sf::FloatRect bounds = getGlobalBounds();

    // Kiểm tra chuột có nằm trong nút không
    bool mouseOver = bounds.contains(mousePos);

    if (event.type == sf::Event::MouseMoved) {
        bool wasHovered = m_isHovered;
        m_isHovered = mouseOver;

        if (m_isHovered) {
            if (!wasHovered) {
                Game::instance().playSound("assets/audio/sfx_hovering.wav");
            }
            sf::Color c = m_isPressed ? m_clickColor : m_hoverColor;
            if (m_hasTexture) m_sprite.setColor(c);
            else m_shape.setFillColor(c);
            m_targetScale = 1.05f;
        } else {
            if (m_hasTexture) m_sprite.setColor(m_normalColor);
            else m_shape.setFillColor(m_normalColor);
            m_isPressed = false;
            m_targetScale = 1.0f;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left && mouseOver) {
            m_isPressed = true;
            if (m_hasTexture) m_sprite.setColor(m_clickColor);
            else m_shape.setFillColor(m_clickColor);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (m_isPressed && mouseOver) {
                Game::instance().playSound("assets/audio/sfx_click.wav");
                // Gọi callback khi nhả chuột trên nút
                if (m_onClick) {
                    m_onClick();
                }
            }
            m_isPressed = false;
            sf::Color c = mouseOver ? m_hoverColor : m_normalColor;
            if (m_hasTexture) m_sprite.setColor(c);
            else m_shape.setFillColor(c);
        }
    }
}

void Button::update(float dt) {
    if (!m_isVisible) return;

    // Animation scale mượt khi hover
    float lerpSpeed = 10.0f; // Tốc độ nội suy
    m_currentScale += (m_targetScale - m_currentScale) * lerpSpeed * dt;

    if (std::abs(m_currentScale - m_targetScale) < 0.001f) {
        m_currentScale = m_targetScale;
    }

    if (m_hasTexture) {
        m_sprite.setScale(m_baseScale.x * m_currentScale, m_baseScale.y * m_currentScale);
    } else {
        m_shape.setScale(m_currentScale, m_currentScale);
        centerText();
    }
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_isVisible) return;

    if (m_hasTexture) {
        target.draw(m_sprite, states);
    } else {
        target.draw(m_shape, states);
        target.draw(m_text, states);
    }
}

void Button::setText(const std::string& text) {
    m_text.setString(text);
    centerText();
}

sf::FloatRect Button::getGlobalBounds() const {
    if (m_hasTexture) {
        return m_sprite.getGlobalBounds();
    }
    return m_shape.getGlobalBounds();
}

void Button::centerText() {
    if (m_hasTexture) return;
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
