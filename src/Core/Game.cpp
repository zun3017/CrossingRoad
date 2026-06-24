// =============================================================================
// Game.cpp - Implementation của lớp Game
// =============================================================================

#include "Game.h"
#include "../States/MainMenuState.h"
#include <iostream>

// Khởi tạo con trỏ static
Game* Game::s_instance = nullptr;

Game::Game()
    : m_window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Crossing Road",
               sf::Style::Titlebar | sf::Style::Close)
{
    // Thiết lập singleton instance
    s_instance = this;

    // Giới hạn FPS
    m_window.setFramerateLimit(FRAME_RATE);

    // Đặt icon cửa sổ (nếu có) - bỏ qua nếu file không tồn tại
    // sf::Image icon;
    // if (icon.loadFromFile("assets/icon.png")) {
    //     m_window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    // }

    std::cout << "=== Crossing Road ===" << std::endl;
    std::cout << "Cua so: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;
    std::cout << "FPS: " << FRAME_RATE << std::endl;
    std::cout << "Kich thuoc o: " << CELL_SIZE << "px" << std::endl;
    std::cout << "Luoi: " << GRID_COLS << " cot x " << GRID_ROWS << " hang" << std::endl;

    // Đẩy MainMenuState làm state đầu tiên
    m_stateMachine.pushState(std::make_unique<MainMenuState>());
    // Xử lý ngay để state được init
    m_stateMachine.processStateChanges();
}

Game::~Game() {
    // Dừng nhạc nền
    m_backgroundMusic.stop();

    // Xóa singleton reference
    if (s_instance == this) {
        s_instance = nullptr;
    }

    std::cout << "Game ket thuc. Tam biet!" << std::endl;
}

void Game::run() {
    // Reset đồng hồ trước khi vào vòng lặp
    m_clock.restart();

    while (m_running && m_window.isOpen()) {
        // Tính delta time
        float dt = m_clock.restart().asSeconds();

        // Giới hạn dt tránh spike (ví dụ khi kéo cửa sổ)
        if (dt > 0.1f) {
            dt = 0.1f;
        }

        // === 1. Xử lý sự kiện (Event Polling) ===
        sf::Event event;
        while (m_window.pollEvent(event)) {
            // Đóng cửa sổ
            if (event.type == sf::Event::Closed) {
                m_window.close();
                m_running = false;
                return;
            }

            // Ủy thác input cho state hiện tại
            m_stateMachine.handleInput(m_window, event);
        }

        // === 2. Xử lý các thay đổi state pending ===
        m_stateMachine.processStateChanges();

        // Nếu không còn state nào -> thoát game
        if (m_stateMachine.isEmpty()) {
            m_window.close();
            m_running = false;
            return;
        }

        // === 3. Cập nhật logic ===
        m_stateMachine.update(dt);

        // === 4. Xử lý state changes sau update ===
        m_stateMachine.processStateChanges();

        // Kiểm tra lại sau update
        if (m_stateMachine.isEmpty()) {
            m_window.close();
            m_running = false;
            return;
        }

        // === 5. Vẽ ===
        m_window.clear(sf::Color(50, 50, 50)); // Màu nền xám đậm
        m_stateMachine.draw(m_window);
        m_window.display();
    }
}

// === Singleton ===
Game& Game::instance() {
    // Đảm bảo Game đã được tạo
    if (s_instance == nullptr) {
        std::cerr << "[Game] LOI: Game::instance() goi truoc khi Game duoc tao!" << std::endl;
        // Tạo static fallback (không nên xảy ra trong thực tế)
        static Game fallback;
        return fallback;
    }
    return *s_instance;
}

// === Quản lý State - Chuyển tiếp tới StateMachine ===
void Game::pushState(std::unique_ptr<State> state) {
    m_stateMachine.pushState(std::move(state));
}

void Game::popState() {
    m_stateMachine.popState();
}

void Game::changeState(std::unique_ptr<State> state) {
    m_stateMachine.changeState(std::move(state));
}

// === Cửa sổ ===
sf::RenderWindow& Game::getWindow() {
    return m_window;
}

const sf::RenderWindow& Game::getWindow() const {
    return m_window;
}

// === Cài đặt ===
bool Game::isSoundEnabled() const {
    return m_soundEnabled;
}

void Game::setSoundEnabled(bool enabled) {
    m_soundEnabled = enabled;
    std::cout << "[Game] Am thanh: " << (enabled ? "BAT" : "TAT") << std::endl;
}

bool Game::isMusicEnabled() const {
    return m_musicEnabled;
}

void Game::setMusicEnabled(bool enabled) {
    m_musicEnabled = enabled;
    if (enabled) {
        // Nếu bật lại nhạc và đang có nhạc tải sẵn
        if (m_backgroundMusic.getStatus() == sf::Music::Paused) {
            m_backgroundMusic.play();
        }
    } else {
        // Tạm dừng nhạc nền
        if (m_backgroundMusic.getStatus() == sf::Music::Playing) {
            m_backgroundMusic.pause();
        }
    }
    std::cout << "[Game] Nhac nen: " << (enabled ? "BAT" : "TAT") << std::endl;
}

bool Game::isMotionEnabled() const {
    return m_motionEnabled;
}

void Game::setMotionEnabled(bool enabled) {
    m_motionEnabled = enabled;
    std::cout << "[Game] Hieu ung chuyen dong: " << (enabled ? "BAT" : "TAT") << std::endl;
}

// === Nhạc nền ===
sf::Music& Game::getBackgroundMusic() {
    return m_backgroundMusic;
}

void Game::playBackgroundMusic(const std::string& filename) {
    // Dừng nhạc cũ
    m_backgroundMusic.stop();

    if (!m_musicEnabled) {
        std::cout << "[Game] Nhac da tat, khong phat: " << filename << std::endl;
        return;
    }

    if (!m_backgroundMusic.openFromFile(filename)) {
        std::cerr << "[Game] Khong the mo file nhac: " << filename << std::endl;
        return;
    }

    m_backgroundMusic.setLoop(true);
    m_backgroundMusic.setVolume(50.0f); // Âm lượng 50%
    m_backgroundMusic.play();
    std::cout << "[Game] Dang phat nhac: " << filename << std::endl;
}

void Game::stopBackgroundMusic() {
    m_backgroundMusic.stop();
}

// === StateMachine ===
StateMachine& Game::getStateMachine() {
    return m_stateMachine;
}
