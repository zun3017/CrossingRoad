// =============================================================================
// StateMachine.cpp - Implementation của StateMachine
// =============================================================================

#include "StateMachine.h"
#include <iostream>
#include <algorithm>

void StateMachine::pushState(std::unique_ptr<State> state) {
    // Thêm vào hàng đợi pending, xử lý sau trong processStateChanges()
    PendingChange change;
    change.action = Action::Push;
    change.state = std::move(state);
    m_pendingChanges.push_back(std::move(change));
}

void StateMachine::popState() {
    PendingChange change;
    change.action = Action::Pop;
    change.state = nullptr;
    m_pendingChanges.push_back(std::move(change));
}

void StateMachine::changeState(std::unique_ptr<State> state) {
    PendingChange change;
    change.action = Action::Change;
    change.state = std::move(state);
    m_pendingChanges.push_back(std::move(change));
}

State* StateMachine::getCurrentState() const {
    if (m_states.empty()) {
        return nullptr;
    }
    return m_states.back().get();
}

void StateMachine::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // Chỉ state trên cùng nhận input
    if (!m_states.empty()) {
        m_states.back()->handleInput(window, event);
    }
}

void StateMachine::update(float dt) {
    // Chỉ state trên cùng được update
    if (!m_states.empty()) {
        m_states.back()->update(dt);
    }
}

void StateMachine::draw(sf::RenderWindow& window) {
    if (m_states.empty()) {
        return;
    }

    // Tìm state thấp nhất cần vẽ
    // Duyệt từ trên xuống: nếu state transparent thì tiếp tục xuống
    std::size_t startIndex = m_states.size() - 1;
    while (startIndex > 0 && m_states[startIndex]->isTransparent()) {
        --startIndex;
    }

    // Vẽ từ state thấp nhất lên state trên cùng
    for (std::size_t i = startIndex; i < m_states.size(); ++i) {
        m_states[i]->draw(window);
    }
}

bool StateMachine::isEmpty() const {
    return m_states.empty();
}

std::size_t StateMachine::size() const {
    return m_states.size();
}

void StateMachine::processStateChanges() {
    // Xử lý tất cả thao tác đang chờ
    for (auto& change : m_pendingChanges) {
        switch (change.action) {
        case Action::Push:
            std::cout << "[StateMachine] Push state moi" << std::endl;
            m_states.push_back(std::move(change.state));
            m_states.back()->init();
            break;

        case Action::Pop:
            if (!m_states.empty()) {
                std::cout << "[StateMachine] Pop state" << std::endl;
                m_states.pop_back();
            }
            break;

        case Action::Change:
            if (!m_states.empty()) {
                std::cout << "[StateMachine] Change state" << std::endl;
                m_states.pop_back();
            }
            m_states.push_back(std::move(change.state));
            m_states.back()->init();
            break;
        }
    }

    m_pendingChanges.clear();
}
