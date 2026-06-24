#include "MainMenuState.h"
#include "../Core/Game.h"
#include "GameState.h"
#include "LoadGameState.h"
#include "RankingState.h"
#include "HelpState.h"
#include "AboutState.h"
#include "SettingState.h"
#include <memory>

// ============================================================
// MainMenuState - Màn hình menu chính của game
// ============================================================

void MainMenuState::init() {
    // Tải font chữ
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");

    m_buttonLabels = { "PLAY", "LOAD GAME", "RANKING", "HELP", "ABOUT", "SETTINGS", "EXIT" };

    initBackground();
    initClouds();
    initButtons();

    // Tiêu đề game
    m_titleText.setFont(m_font);
    m_titleText.setString("CROSSING ROAD");
    m_titleText.setCharacterSize(48);
    m_titleText.setFillColor(sf::Color(255, 215, 0)); // Màu vàng gold
    m_titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                          titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(400.f, 60.f);

    // Phụ đề nhấp nháy
    m_subtitleText.setFont(m_font);
    m_subtitleText.setString("Press any button to start");
    m_subtitleText.setCharacterSize(18);
    m_subtitleText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect subBounds = m_subtitleText.getLocalBounds();
    m_subtitleText.setOrigin(subBounds.left + subBounds.width / 2.f,
                             subBounds.top + subBounds.height / 2.f);
    m_subtitleText.setPosition(400.f, 110.f);
}

void MainMenuState::initBackground() {
    // Gradient nền: phần trên sáng hơn, phần dưới tối hơn
    m_bgTop.setSize(sf::Vector2f(800.f, 300.f));
    m_bgTop.setPosition(0.f, 0.f);
    m_bgTop.setFillColor(sf::Color(25, 25, 80)); // Xanh đậm nhạt

    m_bgBottom.setSize(sf::Vector2f(800.f, 300.f));
    m_bgBottom.setPosition(0.f, 300.f);
    m_bgBottom.setFillColor(sf::Color(10, 10, 50)); // Xanh đậm tối
}

void MainMenuState::initClouds() {
    // Tạo 4 đám mây trang trí bay ngang phần trên màn hình
    float cloudData[][4] = {
        // x, y, width, speed
        { 50.f,  30.f, 120.f, 30.f },
        { 300.f, 55.f,  90.f, 20.f },
        { 550.f, 20.f, 110.f, 40.f },
        { 700.f, 70.f,  80.f, 25.f }
    };

    for (int i = 0; i < 4; i++) {
        Cloud cloud;
        cloud.shape.setSize(sf::Vector2f(cloudData[i][2], 25.f));
        cloud.shape.setPosition(cloudData[i][0], cloudData[i][1]);
        cloud.shape.setFillColor(sf::Color(255, 255, 255, 80)); // Trắng mờ
        cloud.shape.setOutlineThickness(0.f);
        cloud.speed = cloudData[i][3];
        m_clouds.push_back(cloud);
    }
}

void MainMenuState::initButtons() {
    // Tạo các nút menu xếp dọc chính giữa màn hình
    float startY = 150.f;
    float buttonWidth = 220.f;
    float buttonHeight = 40.f;
    float spacing = 8.f;

    for (size_t i = 0; i < m_buttonLabels.size(); i++) {
        MenuButton btn;

        // Nền nút
        btn.background.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        btn.background.setOrigin(buttonWidth / 2.f, buttonHeight / 2.f);
        float yPos = startY + i * (buttonHeight + spacing);
        btn.background.setPosition(400.f, yPos);
        btn.background.setFillColor(sf::Color(60, 60, 120));
        btn.background.setOutlineColor(sf::Color(100, 100, 180));
        btn.background.setOutlineThickness(2.f);

        // Chữ trên nút
        btn.label.setFont(m_font);
        btn.label.setString(m_buttonLabels[i]);
        btn.label.setCharacterSize(20);
        btn.label.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = btn.label.getLocalBounds();
        btn.label.setOrigin(textBounds.left + textBounds.width / 2.f,
                            textBounds.top + textBounds.height / 2.f);
        btn.label.setPosition(400.f, yPos);

        btn.hovered = false;
        m_buttons.push_back(btn);
    }
}

void MainMenuState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = sf::Vector2f(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)
        );

        for (size_t i = 0; i < m_buttons.size(); i++) {
            if (m_buttons[i].background.getGlobalBounds().contains(mousePos)) {
                onButtonClick(static_cast<int>(i));
                break;
            }
        }
    }
}

void MainMenuState::update(float dt) {
    // Cập nhật hiệu ứng nhấp nháy phụ đề
    m_blinkTimer += dt;
    if (m_blinkTimer >= 0.6f) {
        m_blinkTimer = 0.f;
        m_subtitleVisible = !m_subtitleVisible;
        m_subtitleText.setFillColor(m_subtitleVisible
            ? sf::Color(200, 200, 200)
            : sf::Color::Transparent);
    }

    // Cập nhật đám mây bay
    for (auto& cloud : m_clouds) {
        cloud.shape.move(cloud.speed * dt, 0.f);
        // Nếu bay ra khỏi bên phải, quay lại bên trái
        if (cloud.shape.getPosition().x > 820.f) {
            cloud.shape.setPosition(-cloud.shape.getSize().x, cloud.shape.getPosition().y);
        }
    }

    // Cập nhật hover cho nút menu
    sf::Vector2i mousePixel = sf::Mouse::getPosition(Game::instance().getWindow());
    sf::Vector2f mousePos = sf::Vector2f(static_cast<float>(mousePixel.x),
                                          static_cast<float>(mousePixel.y));

    for (auto& btn : m_buttons) {
        btn.hovered = btn.background.getGlobalBounds().contains(mousePos);
        if (btn.hovered) {
            btn.background.setFillColor(sf::Color(100, 100, 200));
            btn.label.setFillColor(sf::Color(255, 215, 0)); // Gold khi hover
        } else {
            btn.background.setFillColor(sf::Color(60, 60, 120));
            btn.label.setFillColor(sf::Color::White);
        }
    }
}

void MainMenuState::draw(sf::RenderWindow& window) {
    // Vẽ nền gradient
    window.draw(m_bgTop);
    window.draw(m_bgBottom);

    // Vẽ đám mây
    for (auto& cloud : m_clouds) {
        window.draw(cloud.shape);
    }

    // Vẽ tiêu đề và phụ đề
    if (m_fontLoaded) {
        window.draw(m_titleText);
        window.draw(m_subtitleText);
    }

    // Vẽ nút menu
    for (auto& btn : m_buttons) {
        window.draw(btn.background);
        if (m_fontLoaded) {
            window.draw(btn.label);
        }
    }
}

void MainMenuState::onButtonClick(int index) {
    switch (index) {
    case 0: // PLAY -> đẩy GameState vào stack
        Game::instance().getStateMachine().pushState(std::make_unique<GameState>());
        break;
    case 1: // LOAD GAME
        Game::instance().getStateMachine().pushState(std::make_unique<LoadGameState>());
        break;
    case 2: // RANKING
        Game::instance().getStateMachine().pushState(std::make_unique<RankingState>());
        break;
    case 3: // HELP
        Game::instance().getStateMachine().pushState(std::make_unique<HelpState>());
        break;
    case 4: // ABOUT
        Game::instance().getStateMachine().pushState(std::make_unique<AboutState>());
        break;
    case 5: // SETTINGS
        Game::instance().getStateMachine().pushState(std::make_unique<SettingState>());
        break;
    case 6: // EXIT -> đóng cửa sổ game
        Game::instance().getWindow().close();
        break;
    default:
        break;
    }
}
