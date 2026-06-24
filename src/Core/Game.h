#pragma once

// =============================================================================
// Game.h - Lớp chính của game, quản lý vòng lặp game và mọi thứ
// =============================================================================

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <string>
#include "StateMachine.h"

// Hằng số cửa sổ game
constexpr unsigned int WINDOW_WIDTH  = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;
constexpr unsigned int FRAME_RATE    = 60;
constexpr float CELL_SIZE            = 48.0f;

// Số cột và hàng trên lưới
constexpr int GRID_COLS = static_cast<int>(WINDOW_WIDTH / CELL_SIZE);   // 800/48 = 16 (dư 32px)
constexpr int GRID_ROWS = static_cast<int>(WINDOW_HEIGHT / CELL_SIZE);  // 600/48 = 12 (dư 24px)

class Game {
public:
    Game();
    ~Game();

    // Vòng lặp game chính
    void run();

    // === Truy cập instance (Singleton-like) ===
    // Các State dùng Game::instance() để push/pop state, truy cập setting...
    static Game& instance();

    // === Quản lý State ===
    void pushState(std::unique_ptr<State> state);
    void popState();
    void changeState(std::unique_ptr<State> state);

    // === Cửa sổ ===
    sf::RenderWindow& getWindow();
    const sf::RenderWindow& getWindow() const;

    // === Cài đặt âm thanh & hiệu ứng ===
    bool isSoundEnabled() const;
    void setSoundEnabled(bool enabled);

    bool isMusicEnabled() const;
    void setMusicEnabled(bool enabled);

    bool isMotionEnabled() const;
    void setMotionEnabled(bool enabled);

    // === Nhạc nền ===
    sf::Music& getBackgroundMusic();
    void playBackgroundMusic(const std::string& filename);
    void stopBackgroundMusic();

    // === Lấy StateMachine (cho trường hợp đặc biệt) ===
    StateMachine& getStateMachine();

private:
    // Con trỏ static tới instance duy nhất
    static Game* s_instance;

    // Cửa sổ game
    sf::RenderWindow m_window;

    // Quản lý state
    StateMachine m_stateMachine;

    // Đồng hồ đo delta time
    sf::Clock m_clock;

    // Nhạc nền
    sf::Music m_backgroundMusic;

    // Cài đặt
    bool m_soundEnabled  = true;
    bool m_musicEnabled  = true;
    bool m_motionEnabled = true;

    // Game đang chạy?
    bool m_running = true;
};
