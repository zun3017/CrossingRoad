#include "LoadGameState.h"
#include "../Core/Game.h"
#include "../Managers/SaveManager.h"
#include "GameState.h"
#include <memory>
#include <iostream>
#include <filesystem>
#include <algorithm>

// ============================================================
// LoadGameState - Màn hình load game đã lưu
// ============================================================

void LoadGameState::init() {
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");

    // Nền tối
    m_background.setSize(sf::Vector2f(800.f, 600.f));
    m_background.setFillColor(sf::Color(30, 30, 45));

    // Tiêu đề
    m_titleText.setFont(m_font);
    m_titleText.setString("LOAD GAME");
    m_titleText.setCharacterSize(42);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                          titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(400.f, 50.f);

    // Thông báo không có save
    m_noSavesText.setFont(m_font);
    m_noSavesText.setString("No saved games found");
    m_noSavesText.setCharacterSize(24);
    m_noSavesText.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect noBounds = m_noSavesText.getLocalBounds();
    m_noSavesText.setOrigin(noBounds.left + noBounds.width / 2.f,
                             noBounds.top + noBounds.height / 2.f);
    m_noSavesText.setPosition(400.f, 280.f);

    // Nút BACK
    float backW = 160.f, backH = 45.f;
    m_backBtnBg.setSize(sf::Vector2f(backW, backH));
    m_backBtnBg.setOrigin(backW / 2.f, backH / 2.f);
    m_backBtnBg.setPosition(400.f, 540.f);
    m_backBtnBg.setFillColor(sf::Color(80, 80, 100));
    m_backBtnBg.setOutlineColor(sf::Color(120, 120, 160));
    m_backBtnBg.setOutlineThickness(2.f);

    m_backBtnText.setFont(m_font);
    m_backBtnText.setString("BACK");
    m_backBtnText.setCharacterSize(22);
    m_backBtnText.setFillColor(sf::Color::White);
    sf::FloatRect backBounds = m_backBtnText.getLocalBounds();
    m_backBtnText.setOrigin(backBounds.left + backBounds.width / 2.f,
                             backBounds.top + backBounds.height / 2.f);
    m_backBtnText.setPosition(400.f, 540.f);

    refreshSaveList();
}

void LoadGameState::refreshSaveList() {
    m_entries.clear();

    std::vector<SaveFileInfo> saveFiles = SaveManager::getSaveList();
    float startY = 120.f;
    float rowHeight = 65.f; // Tăng chiều cao để chứa 2 dòng text

    for (size_t i = 0; i < saveFiles.size() && i < 6; i++) { // Hiển thị 6 dòng thay vì 7
        SaveEntry entry;
        entry.filename = saveFiles[i].filename;
        float y = startY + static_cast<float>(i) * rowHeight;

        // Tên file save
        entry.nameText.setFont(m_font);
        entry.nameText.setString(saveFiles[i].filename);
        entry.nameText.setCharacterSize(20);
        entry.nameText.setFillColor(sf::Color::White);
        entry.nameText.setPosition(100.f, y + 5.f);

        // Chi tiết
        int mins = saveFiles[i].playTimeSeconds / 60;
        int secs = saveFiles[i].playTimeSeconds % 60;
        std::string detailStr = "Playtime: " + std::to_string(mins) + "m " + std::to_string(secs) + "s  |  " + saveFiles[i].timestamp;
        
        entry.detailText.setFont(m_font);
        entry.detailText.setString(detailStr);
        entry.detailText.setCharacterSize(14);
        entry.detailText.setFillColor(sf::Color(180, 180, 180));
        entry.detailText.setPosition(100.f, y + 30.f);

        // Nút LOAD
        float loadW = 80.f, loadH = 35.f;
        entry.loadBtnBg.setSize(sf::Vector2f(loadW, loadH));
        entry.loadBtnBg.setPosition(520.f, y + 10.f);
        entry.loadBtnBg.setFillColor(sf::Color(0, 120, 0));
        entry.loadBtnBg.setOutlineColor(sf::Color(0, 180, 0));
        entry.loadBtnBg.setOutlineThickness(1.f);

        entry.loadBtnText.setFont(m_font);
        entry.loadBtnText.setString("LOAD");
        entry.loadBtnText.setCharacterSize(16);
        entry.loadBtnText.setFillColor(sf::Color::White);
        sf::FloatRect lb = entry.loadBtnText.getLocalBounds();
        entry.loadBtnText.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
        entry.loadBtnText.setPosition(520.f + loadW / 2.f, y + 10.f + loadH / 2.f);

        // Nút DELETE
        float delW = 80.f, delH = 35.f;
        entry.deleteBtnBg.setSize(sf::Vector2f(delW, delH));
        entry.deleteBtnBg.setPosition(620.f, y + 10.f);
        entry.deleteBtnBg.setFillColor(sf::Color(150, 0, 0));
        entry.deleteBtnBg.setOutlineColor(sf::Color(200, 0, 0));
        entry.deleteBtnBg.setOutlineThickness(1.f);

        entry.deleteBtnText.setFont(m_font);
        entry.deleteBtnText.setString("DELETE");
        entry.deleteBtnText.setCharacterSize(16);
        entry.deleteBtnText.setFillColor(sf::Color::White);
        sf::FloatRect db = entry.deleteBtnText.getLocalBounds();
        entry.deleteBtnText.setOrigin(db.left + db.width / 2.f, db.top + db.height / 2.f);
        entry.deleteBtnText.setPosition(620.f + delW / 2.f, y + 10.f + delH / 2.f);

        m_entries.push_back(entry);
    }
}

void LoadGameState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                              static_cast<float>(event.mouseButton.y));

        // Kiểm tra nút LOAD và DELETE cho từng entry
        for (size_t i = 0; i < m_entries.size(); i++) {
            if (m_entries[i].loadBtnBg.getGlobalBounds().contains(mousePos)) {
                // Load game
                SaveData data;
                if (SaveManager::loadGame(m_entries[i].filename, data)) {
                    Game::instance().getStateMachine().popState();
                    auto gameState = std::make_unique<GameState>();
                    gameState->loadGame(m_entries[i].filename, data);
                    Game::instance().getStateMachine().pushState(std::move(gameState));
                } else {
                    std::cerr << "Failed to load save file!" << std::endl;
                }
                return;
            }
            if (m_entries[i].deleteBtnBg.getGlobalBounds().contains(mousePos)) {
                // Xóa file save và refresh danh sách
                SaveManager::deleteGame(m_entries[i].filename);
                refreshSaveList();
                return;
            }
        }

        // Nút BACK
        if (m_backBtnBg.getGlobalBounds().contains(mousePos)) {
            Game::instance().getStateMachine().popState();
        }
    }

    // Phím ESC để quay lại
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void LoadGameState::update(float dt) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(Game::instance().getWindow());
    sf::Vector2f mousePos(static_cast<float>(mousePixel.x),
                          static_cast<float>(mousePixel.y));

    // Cập nhật hover cho nút BACK
    m_backHovered = m_backBtnBg.getGlobalBounds().contains(mousePos);
    m_backBtnBg.setFillColor(m_backHovered
        ? sf::Color(120, 120, 180)
        : sf::Color(80, 80, 100));

    // Cập nhật hover cho các entry
    for (auto& entry : m_entries) {
        entry.loadHovered = entry.loadBtnBg.getGlobalBounds().contains(mousePos);
        entry.deleteHovered = entry.deleteBtnBg.getGlobalBounds().contains(mousePos);

        entry.loadBtnBg.setFillColor(entry.loadHovered
            ? sf::Color(0, 180, 0)
            : sf::Color(0, 120, 0));
        entry.deleteBtnBg.setFillColor(entry.deleteHovered
            ? sf::Color(220, 0, 0)
            : sf::Color(150, 0, 0));
    }
}

void LoadGameState::draw(sf::RenderWindow& window) {
    window.draw(m_background);

    if (!m_fontLoaded) return;

    window.draw(m_titleText);

    // Đường kẻ trang trí
    sf::RectangleShape divider(sf::Vector2f(600.f, 2.f));
    divider.setPosition(100.f, 90.f);
    divider.setFillColor(sf::Color(80, 80, 120));
    window.draw(divider);

    if (m_entries.empty()) {
        window.draw(m_noSavesText);
    } else {
        for (auto& entry : m_entries) {
            // Nền hàng
            sf::RectangleShape rowBg(sf::Vector2f(620.f, 55.f));
            rowBg.setPosition(90.f, entry.nameText.getPosition().y - 3.f);
            rowBg.setFillColor(sf::Color(50, 50, 70, 150));
            rowBg.setOutlineColor(sf::Color(70, 70, 100));
            rowBg.setOutlineThickness(1.f);
            window.draw(rowBg);

            window.draw(entry.nameText);
            window.draw(entry.detailText);
            window.draw(entry.loadBtnBg);
            window.draw(entry.loadBtnText);
            window.draw(entry.deleteBtnBg);
            window.draw(entry.deleteBtnText);
        }
    }

    window.draw(m_backBtnBg);
    window.draw(m_backBtnText);
}
