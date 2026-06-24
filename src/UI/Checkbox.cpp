// Checkbox.cpp - Triển khai ô đánh dấu toggle

#include "Checkbox.h"

Checkbox::Checkbox(float x, float y, const std::string& label, sf::Font& font, bool checked)
    : m_isChecked(checked)
{
    // Thiết lập ô vuông bên ngoài (24x24 px)
    m_box.setSize(sf::Vector2f(24.0f, 24.0f));
    m_box.setPosition(x, y);
    m_box.setFillColor(sf::Color(50, 50, 50));
    m_box.setOutlineThickness(2.0f);
    m_box.setOutlineColor(sf::Color(180, 180, 180));

    // Thiết lập ô vuông bên trong - dấu check (16x16 px)
    // Đặt ở giữa ô ngoài: offset = (24 - 16) / 2 = 4
    m_checkmark.setSize(sf::Vector2f(16.0f, 16.0f));
    m_checkmark.setPosition(x + 4.0f, y + 4.0f);

    // Cập nhật màu dấu check theo trạng thái
    if (m_isChecked) {
        m_checkmark.setFillColor(sf::Color(50, 205, 50)); // Xanh lá khi checked
    } else {
        m_checkmark.setFillColor(sf::Color(205, 50, 50)); // Đỏ khi unchecked
    }

    // Thiết lập nhãn - đặt bên phải ô vuông
    m_label.setFont(font);
    m_label.setString(label);
    m_label.setCharacterSize(16);
    m_label.setFillColor(sf::Color::White);

    // Căn dọc giữa label với ô vuông
    // Ô vuông cao 24px, text cao khoảng 16-18px
    sf::FloatRect textBounds = m_label.getLocalBounds();
    float labelX = x + 24.0f + 10.0f; // Cách ô vuông 10px
    float labelY = y + (24.0f - textBounds.height) / 2.0f - textBounds.top;
    m_label.setPosition(labelX, labelY);
}

void Checkbox::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (!m_isVisible) return;

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Lấy vị trí chuột
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

            // Kiểm tra click trong vùng checkbox (ô vuông + nhãn)
            sf::FloatRect clickArea = getClickBounds();
            if (clickArea.contains(mousePos)) {
                // Toggle trạng thái
                setChecked(!m_isChecked);
            }
        }
    }
}

void Checkbox::update(float dt) {
    // Checkbox không cần animation đặc biệt
    (void)dt;
}

void Checkbox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_isVisible) return;

    // Vẽ ô vuông bên ngoài
    target.draw(m_box, states);

    // Vẽ dấu check hoặc dấu X bên trong
    if (m_isChecked) {
        // Vẽ ô vuông xanh lá (biểu thị ON)
        target.draw(m_checkmark, states);
    } else {
        // Vẽ ô vuông đỏ nhỏ (biểu thị OFF)
        // Tạo 2 thanh chéo để tạo dấu X
        sf::Vector2f boxPos = m_box.getPosition();
        float centerX = boxPos.x + 12.0f;
        float centerY = boxPos.y + 12.0f;

        // Thanh chéo 1: \ (từ trên-trái xuống dưới-phải)
        sf::RectangleShape line1(sf::Vector2f(20.0f, 3.0f));
        line1.setOrigin(10.0f, 1.5f);
        line1.setPosition(centerX, centerY);
        line1.setRotation(45.0f);
        line1.setFillColor(sf::Color(205, 50, 50));

        // Thanh chéo 2: / (từ trên-phải xuống dưới-trái)
        sf::RectangleShape line2(sf::Vector2f(20.0f, 3.0f));
        line2.setOrigin(10.0f, 1.5f);
        line2.setPosition(centerX, centerY);
        line2.setRotation(-45.0f);
        line2.setFillColor(sf::Color(205, 50, 50));

        target.draw(line1, states);
        target.draw(line2, states);
    }

    // Vẽ nhãn
    target.draw(m_label, states);
}

void Checkbox::setChecked(bool checked) {
    m_isChecked = checked;

    if (m_isChecked) {
        m_checkmark.setFillColor(sf::Color(50, 205, 50)); // Xanh lá
    } else {
        m_checkmark.setFillColor(sf::Color(205, 50, 50)); // Đỏ
    }
}

void Checkbox::setLabel(const std::string& label) {
    m_label.setString(label);
}

sf::FloatRect Checkbox::getClickBounds() const {
    // Vùng click bao gồm cả ô vuông và nhãn
    sf::FloatRect boxBounds = m_box.getGlobalBounds();
    sf::FloatRect labelBounds = m_label.getGlobalBounds();

    // Tính bounding box chung
    float left = boxBounds.left;
    float top = boxBounds.top;
    float right = labelBounds.left + labelBounds.width;
    float bottom = std::max(boxBounds.top + boxBounds.height,
                           labelBounds.top + labelBounds.height);

    return sf::FloatRect(left, top, right - left, bottom - top);
}
