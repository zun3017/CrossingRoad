#pragma once

// =============================================================================
// State.h - Lớp cơ sở trừu tượng cho tất cả màn hình game
// Mỗi màn hình (Menu, Gameplay, Pause, GameOver...) kế thừa từ class này
// =============================================================================

#include <SFML/Graphics.hpp>

class State {
public:
    virtual ~State() = default;

    // Khởi tạo state (gọi sau khi push vào StateMachine)
    virtual void init() = 0;

    // Xử lý input từ người chơi
    virtual void handleInput(sf::RenderWindow& window, sf::Event& event) = 0;

    // Cập nhật logic game mỗi frame
    // dt = delta time (thời gian giữa 2 frame, tính bằng giây)
    virtual void update(float dt) = 0;

    // Vẽ mọi thứ lên cửa sổ
    virtual void draw(sf::RenderWindow& window) = 0;

    // State có trong suốt không? (ví dụ: màn hình Pause vẽ đè lên Gameplay)
    // Nếu true, state bên dưới cũng sẽ được vẽ
    virtual bool isTransparent() const { return false; }
};
