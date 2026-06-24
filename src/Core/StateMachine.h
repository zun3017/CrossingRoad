#pragma once

// =============================================================================
// StateMachine.h - Quản lý ngăn xếp (stack) các State
// State trên cùng là state đang hoạt động
// Hỗ trợ overlay: nếu state trên cùng transparent, state bên dưới cũng được vẽ
// =============================================================================

#include "State.h"
#include <stack>
#include <vector>
#include <memory>
#include <functional>

class StateMachine {
public:
    StateMachine() = default;
    ~StateMachine() = default;

    // Đẩy state mới lên đỉnh stack (state cũ vẫn giữ bên dưới)
    void pushState(std::unique_ptr<State> state);

    // Lấy state trên cùng ra khỏi stack
    void popState();

    // Thay thế state trên cùng (pop rồi push)
    void changeState(std::unique_ptr<State> state);

    // Lấy con trỏ tới state đang hoạt động (trên cùng stack)
    State* getCurrentState() const;

    // Ủy thác (delegate) các phương thức xuống state hiện tại
    void handleInput(sf::RenderWindow& window, sf::Event& event);
    void update(float dt);
    void draw(sf::RenderWindow& window);

    // Kiểm tra stack có rỗng không
    bool isEmpty() const;

    // Lấy số lượng state trong stack
    std::size_t size() const;

    // Xử lý các thay đổi pending (push/pop/change được gọi trong update/handleInput)
    void processStateChanges();

private:
    // Dùng vector thay vì stack để có thể duyệt từ dưới lên (vẽ transparent states)
    std::vector<std::unique_ptr<State>> m_states;

    // Hàng đợi các thao tác chờ xử lý (tránh thay đổi stack trong khi đang iterate)
    enum class Action { Push, Pop, Change };
    struct PendingChange {
        Action action;
        std::unique_ptr<State> state; // chỉ dùng cho Push và Change
    };
    std::vector<PendingChange> m_pendingChanges;
};
