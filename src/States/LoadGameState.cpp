#include "LoadGameState.h"
#include "../Core/Game.h"
#include "../Managers/SaveManager.h"
#include "GameState.h"
#include <memory>
#include <iostream>
#include <algorithm>

void LoadGameState::init() {
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");

    // Load menu.png background
    m_bgLoaded = m_bgTexture.loadFromFile("assets/textures/menu.png");
    if (m_bgLoaded) {
        m_bgSprite.setTexture(m_bgTexture);
        sf::Vector2u size = m_bgTexture.getSize();
        if (size.x > 0 && size.y > 0) {
            m_bgSprite.setScale(
                800.f / static_cast<float>(size.x),
                600.f / static_cast<float>(size.y));
        }
    } else {
        m_background.setSize(sf::Vector2f(800.f, 600.f));
        m_background.setFillColor(sf::Color(124, 179, 66));
    }

    // Main window container (Sky Blue)
    m_mainContainer.setSize(sf::Vector2f(720.f, 540.f));
    m_mainContainer.setOrigin(360.f, 270.f);
    m_mainContainer.setPosition(400.f, 300.f);
    m_mainContainer.setFillColor(sf::Color(74, 144, 226, 242)); // #4a90e2 with 95% opacity

    m_containerBorder.setSize(sf::Vector2f(720.f, 540.f));
    m_containerBorder.setOrigin(360.f, 270.f);
    m_containerBorder.setPosition(400.f, 300.f);
    m_containerBorder.setFillColor(sf::Color::Transparent);
    m_containerBorder.setOutlineColor(sf::Color::Black);
    m_containerBorder.setOutlineThickness(4.f);

    // Top ribbon
    m_topRibbon.setSize(sf::Vector2f(720.f, 8.f));
    m_topRibbon.setPosition(40.f, 30.f);
    m_topRibbon.setFillColor(sf::Color(255, 255, 255, 60));

    if (m_fontLoaded) {
        // Title: LOAD GAME
        m_titleShadow.setFont(m_font);
        m_titleShadow.setString("LOAD GAME");
        m_titleShadow.setCharacterSize(36);
        m_titleShadow.setFillColor(sf::Color::Black);
        m_titleShadow.setStyle(sf::Text::Bold);
        sf::FloatRect tb = m_titleShadow.getLocalBounds();
        m_titleShadow.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleShadow.setPosition(403.f, 61.f);

        m_titleText.setFont(m_font);
        m_titleText.setString("LOAD GAME");
        m_titleText.setCharacterSize(36);
        m_titleText.setFillColor(sf::Color(253, 216, 53)); // #fdd835 yellow
        m_titleText.setStyle(sf::Text::Bold);
        m_titleText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleText.setPosition(400.f, 58.f);

        // Accent Bar
        m_accentBar.setSize(sf::Vector2f(440.f, 6.f));
        m_accentBar.setOrigin(220.f, 0.f);
        m_accentBar.setPosition(400.f, 88.f);
        m_accentBar.setFillColor(sf::Color(253, 216, 53));

        m_accentBarBorder.setSize(sf::Vector2f(440.f, 3.f));
        m_accentBarBorder.setOrigin(220.f, 0.f);
        m_accentBarBorder.setPosition(400.f, 94.f);
        m_accentBarBorder.setFillColor(sf::Color::Black);

        // Content Box Container
        m_contentBox.setSize(sf::Vector2f(660.f, 365.f));
        m_contentBox.setPosition(70.f, 110.f);
        m_contentBox.setFillColor(sf::Color(0, 0, 0, 180));
        m_contentBox.setOutlineColor(sf::Color::Black);
        m_contentBox.setOutlineThickness(3.f);

        // No Saves Text
        m_noSavesText.setFont(m_font);
        m_noSavesText.setString("NO SAVED GAMES FOUND");
        m_noSavesText.setCharacterSize(20);
        m_noSavesText.setFillColor(sf::Color(253, 216, 53)); // #fdd835
        m_noSavesText.setStyle(sf::Text::Bold);
        sf::FloatRect noBounds = m_noSavesText.getLocalBounds();
        m_noSavesText.setOrigin(noBounds.left + noBounds.width / 2.f, noBounds.top + noBounds.height / 2.f);
        m_noSavesText.setPosition(400.f, 280.f);
    }

    // Back Button (Wooden pixel style)
    bool backLoaded = m_backTexture.loadFromFile("assets/textures/back_text.png");
    if (backLoaded) {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, m_backTexture, []() {
            Game::instance().getStateMachine().popState();
        });
    } else {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, "< BACK", m_font, []() {
            Game::instance().getStateMachine().popState();
        });
        m_backBtn->setNormalColor(sf::Color(141, 110, 99)); // #8d6e63
        m_backBtn->setHoverColor(sf::Color(161, 136, 127));  // #a1887f
        m_backBtn->setClickColor(sf::Color(93, 64, 55));     // #5d4037
        m_backBtn->setTextColor(sf::Color(253, 216, 53));
    }

    refreshSaveList();
}

void LoadGameState::refreshSaveList() {
    m_entries.clear();

    std::vector<SaveFileInfo> saveFiles = SaveManager::getSaveList();
    float startY = 122.f;
    float rowHeight = 60.f;

    for (size_t i = 0; i < saveFiles.size() && i < 5; i++) {
        SaveEntry entry;
        entry.filename = saveFiles[i].filename;
        float y = startY + static_cast<float>(i) * rowHeight;

        // Save File Name
        entry.nameText.setFont(m_font);
        entry.nameText.setString(saveFiles[i].filename);
        entry.nameText.setCharacterSize(16);
        entry.nameText.setFillColor(sf::Color(253, 216, 53)); // #fdd835
        entry.nameText.setStyle(sf::Text::Bold);
        entry.nameText.setPosition(96.f, y + 6.f);

        // Details
        int mins = saveFiles[i].playTimeSeconds / 60;
        int secs = saveFiles[i].playTimeSeconds % 60;
        std::string detailStr = "Playtime: " + std::to_string(mins) + "m " + std::to_string(secs) + "s   |   Date: " + saveFiles[i].timestamp;
        
        entry.detailText.setFont(m_font);
        entry.detailText.setString(detailStr);
        entry.detailText.setCharacterSize(12);
        entry.detailText.setFillColor(sf::Color(200, 200, 200));
        entry.detailText.setPosition(96.f, y + 28.f);

        // LOAD Button
        float loadW = 75.f, loadH = 32.f;
        entry.loadBtnBg.setSize(sf::Vector2f(loadW, loadH));
        entry.loadBtnBg.setPosition(525.f, y + 11.f);
        entry.loadBtnBg.setFillColor(sf::Color(67, 160, 71)); // #43a047
        entry.loadBtnBg.setOutlineColor(sf::Color::Black);
        entry.loadBtnBg.setOutlineThickness(2.f);

        entry.loadBtnText.setFont(m_font);
        entry.loadBtnText.setString("LOAD");
        entry.loadBtnText.setCharacterSize(13);
        entry.loadBtnText.setFillColor(sf::Color::White);
        entry.loadBtnText.setStyle(sf::Text::Bold);
        sf::FloatRect lb = entry.loadBtnText.getLocalBounds();
        entry.loadBtnText.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
        entry.loadBtnText.setPosition(525.f + loadW / 2.f, y + 11.f + loadH / 2.f);

        // DELETE Button
        float delW = 75.f, delH = 32.f;
        entry.deleteBtnBg.setSize(sf::Vector2f(delW, delH));
        entry.deleteBtnBg.setPosition(615.f, y + 11.f);
        entry.deleteBtnBg.setFillColor(sf::Color(229, 57, 53)); // #e53935
        entry.deleteBtnBg.setOutlineColor(sf::Color::Black);
        entry.deleteBtnBg.setOutlineThickness(2.f);

        entry.deleteBtnText.setFont(m_font);
        entry.deleteBtnText.setString("DELETE");
        entry.deleteBtnText.setCharacterSize(11);
        entry.deleteBtnText.setFillColor(sf::Color::White);
        entry.deleteBtnText.setStyle(sf::Text::Bold);
        sf::FloatRect db = entry.deleteBtnText.getLocalBounds();
        entry.deleteBtnText.setOrigin(db.left + db.width / 2.f, db.top + db.height / 2.f);
        entry.deleteBtnText.setPosition(615.f + delW / 2.f, y + 11.f + delH / 2.f);

        m_entries.push_back(entry);
    }
}

void LoadGameState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (m_backBtn) m_backBtn->handleEvent(event, window);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

        for (size_t i = 0; i < m_entries.size(); i++) {
            if (m_entries[i].loadBtnBg.getGlobalBounds().contains(mousePos)) {
                Game::instance().playSound("assets/audio/sfx_click.wav");
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
                Game::instance().playSound("assets/audio/sfx_click.wav");
                SaveManager::deleteGame(m_entries[i].filename);
                refreshSaveList();
                return;
            }
        }
    }

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void LoadGameState::update(float dt) {
    if (m_backBtn) m_backBtn->update(dt);

    sf::Vector2f mousePos = Game::instance().getWindow().mapPixelToCoords(sf::Mouse::getPosition(Game::instance().getWindow()));

    for (auto& entry : m_entries) {
        entry.loadHovered = entry.loadBtnBg.getGlobalBounds().contains(mousePos);
        entry.deleteHovered = entry.deleteBtnBg.getGlobalBounds().contains(mousePos);

        entry.loadBtnBg.setFillColor(entry.loadHovered
            ? sf::Color(46, 125, 50)   // Darker green hover
            : sf::Color(67, 160, 71)); // #43a047
        entry.deleteBtnBg.setFillColor(entry.deleteHovered
            ? sf::Color(198, 40, 40)   // Darker red hover
            : sf::Color(229, 57, 53)); // #e53935
    }
}

void LoadGameState::draw(sf::RenderWindow& window) {
    if (m_bgLoaded) {
        window.draw(m_bgSprite);
    } else {
        window.draw(m_background);
    }

    // Main window container
    window.draw(m_mainContainer);
    window.draw(m_topRibbon);
    window.draw(m_containerBorder);

    if (!m_fontLoaded) return;

    // Header
    window.draw(m_titleShadow);
    window.draw(m_titleText);
    window.draw(m_accentBar);
    window.draw(m_accentBarBorder);

    // Content Box
    window.draw(m_contentBox);

    if (m_entries.empty()) {
        window.draw(m_noSavesText);
    } else {
        for (auto& entry : m_entries) {
            // Row Container
            sf::RectangleShape rowBg(sf::Vector2f(630.f, 52.f));
            rowBg.setPosition(85.f, entry.nameText.getPosition().y - 6.f);
            rowBg.setFillColor(sf::Color(0, 0, 0, 140));
            rowBg.setOutlineColor(sf::Color::Black);
            rowBg.setOutlineThickness(2.f);
            window.draw(rowBg);

            window.draw(entry.nameText);
            window.draw(entry.detailText);
            window.draw(entry.loadBtnBg);
            window.draw(entry.loadBtnText);
            window.draw(entry.deleteBtnBg);
            window.draw(entry.deleteBtnText);
        }
    }

    if (m_backBtn) window.draw(*m_backBtn);
}

