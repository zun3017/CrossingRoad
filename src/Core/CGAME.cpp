// =============================================================================
// CGAME.cpp - Implementation của lớp CGAME
// =============================================================================

#include "CGAME.h"
#include "../States/MainMenuState.h"
#include <iostream>

// Khởi tạo con trỏ static
CGAME* CGAME::s_instance = nullptr;

CGAME::CGAME()
    : m_window(sf::VideoMode(1024, 768), "Crossing Road",
               sf::Style::Default)
{
    // Thiết lập singleton instance
    s_instance = this;

    // Giới hạn FPS
    m_window.setFramerateLimit(FRAME_RATE);

    // Tắt tính năng lặp phím khi giữ phím (chống lỗi nhân vật chạy quá nhanh dẫn đến chết đột ngột)
    m_window.setKeyRepeatEnabled(false);

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
    
    // Khởi tạo View cố định tỷ lệ
    sf::View view(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    m_window.setView(view);
}

CGAME::~CGAME() {
    // Dừng nhạc nền
    m_backgroundMusic.stop();

    // Xóa singleton reference
    if (s_instance == this) {
        s_instance = nullptr;
    }

    std::cout << "CGAME ket thuc. Tam biet!" << std::endl;
}

void CGAME::run() {
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
            
            // Xử lý resize cửa sổ (giữ nguyên view để tự động scale nội dung)
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                m_window.setView(sf::View(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)));
            }

            // Map pixel to coords cho input chuột
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f worldPos = m_window.mapPixelToCoords(pixelPos);
                event.mouseButton.x = static_cast<int>(worldPos.x);
                event.mouseButton.y = static_cast<int>(worldPos.y);
            }

            // Ủy thác input cho state hiện tại
            m_stateMachine.handleInput(m_window, event);
        }

        // === 2. Xử lý các thay đổi state pending ===
        m_stateMachine.processStateChanges();

        // Nếu không còn state nào -> thoát CGAME
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
CGAME& CGAME::instance() {
    // Đảm bảo CGAME đã được tạo
    if (s_instance == nullptr) {
        std::cerr << "[CGAME] LOI: CGAME::instance() goi truoc khi CGAME duoc tao!" << std::endl;
        // Tạo static fallback (không nên xảy ra trong thực tế)
        static CGAME fallback;
        return fallback;
    }
    return *s_instance;
}

// === Quản lý State - Chuyển tiếp tới StateMachine ===
void CGAME::pushState(std::unique_ptr<State> state) {
    m_stateMachine.pushState(std::move(state));
}

void CGAME::popState() {
    m_stateMachine.popState();
}

void CGAME::changeState(std::unique_ptr<State> state) {
    m_stateMachine.changeState(std::move(state));
}

// === Cửa sổ ===
sf::RenderWindow& CGAME::getWindow() {
    return m_window;
}

const sf::RenderWindow& CGAME::getWindow() const {
    return m_window;
}

// === Cài đặt ===
bool CGAME::isSoundEnabled() const {
    return m_soundEnabled;
}

void CGAME::setSoundEnabled(bool enabled) {
    m_soundEnabled = enabled;
    std::cout << "[CGAME] Am thanh: " << (enabled ? "BAT" : "TAT") << std::endl;
}

bool CGAME::isMusicEnabled() const {
    return m_musicEnabled;
}

void CGAME::setMusicEnabled(bool enabled) {
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
    std::cout << "[CGAME] Nhac nen: " << (enabled ? "BAT" : "TAT") << std::endl;
}

bool CGAME::isMotionEnabled() const {
    return m_motionEnabled;
}

void CGAME::setMotionEnabled(bool enabled) {
    m_motionEnabled = enabled;
    std::cout << "[CGAME] Hieu ung chuyen dong: " << (enabled ? "BAT" : "TAT") << std::endl;
}

// === Nhạc nền ===
sf::Music& CGAME::getBackgroundMusic() {
    return m_backgroundMusic;
}

void CGAME::playBackgroundMusic(const std::string& filename) {
    // Dừng nhạc cũ
    m_backgroundMusic.stop();

    if (!m_musicEnabled) {
        std::cout << "[CGAME] Nhac da tat, khong phat: " << filename << std::endl;
        return;
    }

    if (!m_backgroundMusic.openFromFile(filename)) {
        std::cerr << "[CGAME] Khong the mo file nhac: " << filename << std::endl;
        return;
    }

    m_backgroundMusic.setLoop(true);
    m_backgroundMusic.setVolume(50.0f); // Âm lượng 50%
    m_backgroundMusic.play();
    std::cout << "[CGAME] Dang phat nhac: " << filename << std::endl;
}

void CGAME::stopBackgroundMusic() {
    m_backgroundMusic.stop();
}

// === StateMachine ===
StateMachine& CGAME::getStateMachine() {
    return m_stateMachine;
}
