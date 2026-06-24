#pragma once

// Checkbox.h - Ô đánh dấu (toggle) cho cài đặt
// Hiển thị ô vuông + nhãn, nhấn để bật/tắt

#include "Component.h"
#include <string>

class Checkbox : public Component {
public:
    // Tạo checkbox tại vị trí (x, y) với nhãn và trạng thái ban đầu
    Checkbox(float x, float y, const std::string& label, sf::Font& font, bool checked = true);

    ~Checkbox() override = default;

    // Xử lý sự kiện: click để toggle
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;

    // Không cần update đặc biệt
    void update(float dt) override;

    // Vẽ checkbox (ô vuông + dấu check/X + nhãn)
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // Kiểm tra trạng thái
    bool isChecked() const { return m_isChecked; }

    // Đặt trạng thái
    void setChecked(bool checked);

    // Đặt nhãn
    void setLabel(const std::string& label);

    // Lấy bounds khu vực click được (bao gồm cả nhãn)
    sf::FloatRect getClickBounds() const;

private:
    sf::RectangleShape m_box;         // Ô vuông bên ngoài (24x24)
    sf::RectangleShape m_checkmark;   // Ô vuông bên trong (16x16, hiển thị khi checked)
    sf::Text m_label;                  // Nhãn text bên phải

    bool m_isChecked = true;           // Trạng thái bật/tắt
};
