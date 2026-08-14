// =============================================================================
// Game.cpp - Implementation của lớp Game
// =============================================================================

#include "Game.h"
#include "ResourceManager.h"
#include "../States/MainMenuState.h"
#include <iostream>

// Khởi tạo con trỏ static
Game* Game::s_instance = nullptr;

Game::Game()
    // Tạo cửa sổ 1024×768 (vật lý) - nội dung game được vẽ trong viewport 800×600 và scale tự động
    : m_window(sf::VideoMode(1024, 768), "Crossing Road",
               sf::Style::Default)
{
    // Thiết lập singleton instance
    s_instance = this;

    // Giới hạn FPS
    m_window.setFramerateLimit(FRAME_RATE);

    // Tắt tính năng lặp phím khi giữ phím (chống lỗi nhân vật chạy quá nhanh dẫn đến chết đột ngột)
    m_window.setKeyRepeatEnabled(false);

    // Preload các texture và font dùng chung để chuyển cảnh không bị khựng (0ms lag)
    TextureManager::getInstance().preload("assets/textures/menu.png");
    TextureManager::getInstance().preload("assets/textures/back_text.png");
    TextureManager::getInstance().preload("assets/textures/on_button.png");
    TextureManager::getInstance().preload("assets/textures/off_button.png");
    TextureManager::getInstance().preload("assets/textures/watch.png");
    TextureManager::getInstance().preload("assets/textures/smoke.png");
    TextureManager::getInstance().preload("assets/textures/hand_lamp.png");
    TextureManager::getInstance().preload("assets/textures/street_lamp.png");
    TextureManager::getInstance().preload("assets/textures/lotus_lamp.png");
    FontManager::getInstance().preload("assets/fonts/arial.ttf");

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
            
            // Xử lý resize cửa sổ (giữ nguyên view để tự động scale nội dung)
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                m_window.setView(sf::View(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)));
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

float Game::getSoundVolume() const {
    return m_soundVolume;
}

void Game::setSoundVolume(float volume) {
    m_soundVolume = std::max(0.0f, std::min(100.0f, volume));
    for (auto& sound : m_sounds) {
        sound.setVolume(m_soundVolume);
    }
}

bool Game::isMusicEnabled() const {
    return m_musicEnabled;
}

void Game::setMusicEnabled(bool enabled) {
    m_musicEnabled = enabled;
    if (enabled) {
        m_backgroundMusic.setVolume(m_musicVolume);
        if (!m_currentMusicFile.empty()) {
            std::string fileToPlay = m_currentMusicFile;
            m_currentMusicFile.clear(); // Force reload
            playBackgroundMusic(fileToPlay);
        }
    } else {
        m_backgroundMusic.stop();
    }
    std::cout << "[Game] Nhac nen: " << (enabled ? "BAT" : "TAT") << std::endl;
}

float Game::getMusicVolume() const {
    return m_musicVolume;
}

void Game::setMusicVolume(float volume) {
    m_musicVolume = std::max(0.0f, std::min(100.0f, volume));
    if (m_musicEnabled) {
        m_backgroundMusic.setVolume(m_musicVolume);
    }
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
    if (m_currentMusicFile == filename) {
        if (!m_musicEnabled) {
            if (m_backgroundMusic.getStatus() == sf::Music::Playing) {
                m_backgroundMusic.stop();
            }
            return;
        }
        if (m_backgroundMusic.getStatus() == sf::Music::Playing) {
            m_backgroundMusic.setVolume(m_musicVolume);
            return; // Đã đang phát cùng bài nhạc -> giữ nguyên không ngắt đoạn
        }
    }

    m_backgroundMusic.stop();
    m_currentMusicFile = filename;

    if (!m_musicEnabled) {
        return; // Tắt nhạc -> không in log lặp lại
    }

    if (!m_backgroundMusic.openFromFile(filename)) {
        std::cerr << "[Game] Khong the mo file nhac: " << filename << std::endl;
        return;
    }

    m_backgroundMusic.setLoop(true);
    m_backgroundMusic.setVolume(m_musicVolume);
    m_backgroundMusic.play();
    std::cout << "[Game] Dang phat nhac: " << filename << " (Volume: " << m_musicVolume << "%)" << std::endl;
}

void Game::stopBackgroundMusic() {
    m_backgroundMusic.stop();
    m_currentMusicFile.clear();
}

// === Hiệu ứng âm thanh SFX ===
void Game::playSound(const std::string& filename) {
    if (!m_soundEnabled || m_soundVolume <= 0.0f) return;

    // Nạp SoundBuffer nếu chưa có trong cache
    auto it = m_soundBuffers.find(filename);
    if (it == m_soundBuffers.end()) {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename)) {
            std::cerr << "[Game] Khong the mo file am thanh SFX: " << filename << std::endl;
            return;
        }
        m_soundBuffers[filename] = std::move(buffer);
        it = m_soundBuffers.find(filename);
    }

    // Tìm kênh sf::Sound đang rảnh trong pool
    for (auto& sound : m_sounds) {
        if (sound.getStatus() != sf::Sound::Playing) {
            sound.setBuffer(it->second);
            sound.setVolume(m_soundVolume);
            sound.play();
            return;
        }
    }

    // Nếu các kênh đều bận và chưa vượt giới hạn 16 kênh, tạo thêm kênh mới
    if (m_sounds.size() < 16) {
        m_sounds.emplace_back();
        m_sounds.back().setBuffer(it->second);
        m_sounds.back().setVolume(m_soundVolume);
        m_sounds.back().play();
    }
}

// === StateMachine ===
StateMachine& Game::getStateMachine() {
    return m_stateMachine;
}

// === Skin Quản Lý ===
PlayerSkin Game::getPlayerSkin() const {
    return m_playerSkin;
}

void Game::setPlayerSkin(PlayerSkin skin) {
    m_playerSkin = skin;
}

std::string Game::getPlayerSkinPath() const {
    switch (m_playerSkin) {
    case PlayerSkin::Beach:
        return "assets/textures/skins nv/player_beach.png";
    case PlayerSkin::Mafia:
        return "assets/textures/skins nv/player_mafia.png";
    default:
        return "assets/textures/skins nv/player.png";
    }
}

std::string Game::getPlayerDrownPath() const {
    switch (m_playerSkin) {
    case PlayerSkin::Beach:
        return "assets/textures/skins nv/player_beach_drown.png";
    case PlayerSkin::Mafia:
        return "assets/textures/skins nv/player_mafia_drown.png";
    default:
        return "assets/textures/skins nv/player_drown.png";
    }
}

std::string Game::getPlayerHitPath() const {
    switch (m_playerSkin) {
    case PlayerSkin::Beach:
        return "assets/textures/skins nv/player_beach_hit.png";
    case PlayerSkin::Mafia:
        return "assets/textures/skins nv/player_mafia_hit.png";
    default:
        return "assets/textures/skins nv/hitbycar.png";
    }
}
