#pragma once

// TextBox.h - Ô nhập liệu text
// Cho phép người dùng nhập tên hoặc dữ liệu text

#include "Component.h"
#include <string>

class TextBox : public Component {
public:
    // Tạo ô nhập liệu tại vị trí (x, y) với kích thước (width, height)
    TextBox(float x, float y, float width, float height, sf::Font& font);

    ~TextBox() override = default;

    // Xử lý sự kiện: click để activate, nhập ký tự, Backspace, Enter
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;

    // Cập nhật cursor nhấp nháy
    void update(float dt) override;

    // Vẽ ô text (khung + text + cursor)
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // Lấy chuỗi đã nhập
    const std::string& getString() const { return m_inputString; }

    // Xóa toàn bộ nội dung
    void clear();

    // Đặt nội dung (dùng khi load dữ liệu)
    void setString(const std::string& str);

    // Đặt số ký tự tối đa
    void setMaxLength(int maxLen) { m_maxLength = maxLen; }

    // Kích hoạt / hủy kích hoạt ô nhập
    void setActive(bool active);
    bool isActive() const { return m_isActive; }

    // Lấy bounds để kiểm tra va chạm
    sf::FloatRect getGlobalBounds() const;

private:
    // Cập nhật text hiển thị (thêm cursor nếu đang active)
    void updateDisplayText();

    sf::RectangleShape m_box;           // Khung ô nhập
    sf::Text m_displayText;             // Text hiển thị trên màn hình
    std::string m_inputString;          // Chuỗi ký tự đã nhập

    sf::Font* m_fontPtr = nullptr;      // Con trỏ đến font (không sở hữu)

    int m_maxLength = 20;               // Số ký tự tối đa
    bool m_isActive = false;            // Ô đang được kích hoạt (đang nhập)

    float m_cursorBlinkTimer = 0.0f;    // Đồng hồ nhấp nháy cursor
    bool m_showCursor = true;           // Cursor đang hiển thị hay ẩn
    static constexpr float CURSOR_BLINK_INTERVAL = 0.5f; // Chu kỳ nhấp nháy (giây)
};
