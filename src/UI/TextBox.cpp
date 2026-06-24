// TextBox.cpp - Triển khai ô nhập liệu text

#include "TextBox.h"

TextBox::TextBox(float x, float y, float width, float height, sf::Font& font)
    : m_fontPtr(&font)
{
    // Thiết lập khung ô nhập
    m_box.setSize(sf::Vector2f(width, height));
    m_box.setPosition(x, y);
    m_box.setFillColor(sf::Color(40, 40, 40));
    m_box.setOutlineThickness(2.0f);
    m_box.setOutlineColor(sf::Color(128, 128, 128)); // Xám khi inactive

    // Thiết lập text hiển thị
    m_displayText.setFont(font);
    m_displayText.setCharacterSize(18);
    m_displayText.setFillColor(sf::Color::White);

    // Đặt vị trí text bên trong ô (có padding)
    float padding = 8.0f;
    m_displayText.setPosition(x + padding, y + (height - 22.0f) / 2.0f);
}

void TextBox::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (!m_isVisible) return;

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Lấy vị trí chuột
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

            // Kiểm tra click trong ô
            if (getGlobalBounds().contains(mousePos)) {
                setActive(true);
            } else {
                setActive(false);
            }
        }
    }

    // Chỉ xử lý nhập liệu khi ô đang active
    if (!m_isActive) return;

    if (event.type == sf::Event::TextEntered) {
        sf::Uint32 unicode = event.text.unicode;

        if (unicode == 8) {
            // Backspace - xóa ký tự cuối
            if (!m_inputString.empty()) {
                m_inputString.pop_back();
                updateDisplayText();
            }
        } else if (unicode == 13 || unicode == 10) {
            // Enter - tắt ô nhập
            setActive(false);
        } else if (unicode == 27) {
            // Escape - tắt ô nhập
            setActive(false);
        } else if (unicode >= 32 && unicode < 127) {
            // Ký tự ASCII in được (từ space đến ~)
            if (static_cast<int>(m_inputString.size()) < m_maxLength) {
                m_inputString += static_cast<char>(unicode);
                updateDisplayText();
            }
        }
    }

    // Xử lý phím Delete
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Delete) {
            // Xóa toàn bộ (đơn giản hóa - không hỗ trợ cursor position)
            // Trong phiên bản đầy đủ có thể thêm vị trí cursor
        }
    }
}

void TextBox::update(float dt) {
    if (!m_isVisible) return;

    if (m_isActive) {
        // Nhấp nháy cursor
        m_cursorBlinkTimer += dt;
        if (m_cursorBlinkTimer >= CURSOR_BLINK_INTERVAL) {
            m_cursorBlinkTimer -= CURSOR_BLINK_INTERVAL;
            m_showCursor = !m_showCursor;
            updateDisplayText();
        }
    }
}

void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_isVisible) return;

    target.draw(m_box, states);
    target.draw(m_displayText, states);
}

void TextBox::clear() {
    m_inputString.clear();
    updateDisplayText();
}

void TextBox::setString(const std::string& str) {
    m_inputString = str;
    // Cắt nếu vượt quá giới hạn
    if (static_cast<int>(m_inputString.size()) > m_maxLength) {
        m_inputString.resize(static_cast<size_t>(m_maxLength));
    }
    updateDisplayText();
}

void TextBox::setActive(bool active) {
    m_isActive = active;
    m_isFocused = active;

    if (active) {
        // Viền trắng sáng khi active
        m_box.setOutlineColor(sf::Color::White);
        m_showCursor = true;
        m_cursorBlinkTimer = 0.0f;
    } else {
        // Viền xám khi inactive
        m_box.setOutlineColor(sf::Color(128, 128, 128));
        m_showCursor = false;
    }

    updateDisplayText();
}

sf::FloatRect TextBox::getGlobalBounds() const {
    return m_box.getGlobalBounds();
}

void TextBox::updateDisplayText() {
    // Hiển thị chuỗi nhập + cursor nếu đang active và cursor đang bật
    if (m_isActive && m_showCursor) {
        m_displayText.setString(m_inputString + "|");
    } else {
        m_displayText.setString(m_inputString);
    }
}
