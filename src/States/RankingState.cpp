#include "RankingState.h"
#include "../Core/Game.h"
#include "../Managers/SaveManager.h"
#include <memory>
#include <algorithm>

// ============================================================
// RankingState - Bảng xếp hạng điểm cao
// ============================================================

void RankingState::init() {
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");

    // Nền tối
    m_background.setSize(sf::Vector2f(800.f, 600.f));
    m_background.setFillColor(sf::Color(20, 20, 35));

    // Tiêu đề vàng gold
    m_titleText.setFont(m_font);
    m_titleText.setString("LEADERBOARD");
    m_titleText.setCharacterSize(42);
    m_titleText.setFillColor(sf::Color(255, 215, 0));
    m_titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                          titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(400.f, 45.f);

    // Trophy placeholder - hình chữ nhật vàng ở góc trên
    m_trophy.setSize(sf::Vector2f(40.f, 50.f));
    m_trophy.setFillColor(sf::Color(255, 215, 0));
    m_trophy.setOutlineColor(sf::Color(200, 170, 0));
    m_trophy.setOutlineThickness(2.f);
    m_trophy.setPosition(380.f, 80.f);

    // Header bảng
    float tableTop = 150.f;
    m_headerRank.setFont(m_font);
    m_headerRank.setString("Rank");
    m_headerRank.setCharacterSize(20);
    m_headerRank.setFillColor(sf::Color(255, 215, 0));
    m_headerRank.setStyle(sf::Text::Bold);
    m_headerRank.setPosition(150.f, tableTop);

    m_headerName.setFont(m_font);
    m_headerName.setString("Name");
    m_headerName.setCharacterSize(20);
    m_headerName.setFillColor(sf::Color(255, 215, 0));
    m_headerName.setStyle(sf::Text::Bold);
    m_headerName.setPosition(300.f, tableTop);

    m_headerScore.setFont(m_font);
    m_headerScore.setString("Score");
    m_headerScore.setCharacterSize(20);
    m_headerScore.setFillColor(sf::Color(255, 215, 0));
    m_headerScore.setStyle(sf::Text::Bold);
    m_headerScore.setPosition(550.f, tableTop);

    // Không có dữ liệu
    m_noDataText.setFont(m_font);
    m_noDataText.setString("No high scores yet");
    m_noDataText.setCharacterSize(22);
    m_noDataText.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect noBounds = m_noDataText.getLocalBounds();
    m_noDataText.setOrigin(noBounds.left + noBounds.width / 2.f,
                            noBounds.top + noBounds.height / 2.f);
    m_noDataText.setPosition(400.f, 320.f);

    // Load dữ liệu highscore
    auto highscores = SaveManager::loadHighscores();

    float entryY = tableTop + 40.f;
    float entrySpacing = 35.f;

    for (size_t i = 0; i < highscores.size() && i < 10; i++) {
        RankEntry entry;
        float y = entryY + static_cast<float>(i) * entrySpacing;

        // Màu theo thứ hạng
        sf::Color rankColor = sf::Color::White;
        if (i == 0) rankColor = sf::Color(255, 215, 0);   // Vàng cho hạng 1
        else if (i == 1) rankColor = sf::Color(192, 192, 192); // Bạc cho hạng 2
        else if (i == 2) rankColor = sf::Color(205, 127, 50);  // Đồng cho hạng 3

        entry.rankText.setFont(m_font);
        entry.rankText.setString("#" + std::to_string(i + 1));
        entry.rankText.setCharacterSize(18);
        entry.rankText.setFillColor(rankColor);
        entry.rankText.setPosition(160.f, y);

        entry.nameText.setFont(m_font);
        entry.nameText.setString(highscores[i].name);
        entry.nameText.setCharacterSize(18);
        entry.nameText.setFillColor(sf::Color::White);
        entry.nameText.setPosition(300.f, y);

        entry.scoreText.setFont(m_font);
        entry.scoreText.setString(std::to_string(highscores[i].score));
        entry.scoreText.setCharacterSize(18);
        entry.scoreText.setFillColor(sf::Color(100, 255, 100));
        entry.scoreText.setPosition(560.f, y);

        m_entries.push_back(entry);
    }

    // Nút BACK
    float backW = 160.f, backH = 45.f;
    m_backBtnBg.setSize(sf::Vector2f(backW, backH));
    m_backBtnBg.setOrigin(backW / 2.f, backH / 2.f);
    m_backBtnBg.setPosition(400.f, 550.f);
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
    m_backBtnText.setPosition(400.f, 550.f);
}

void RankingState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                              static_cast<float>(event.mouseButton.y));

        if (m_backBtnBg.getGlobalBounds().contains(mousePos)) {
            Game::instance().getStateMachine().popState();
        }
    }

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void RankingState::update(float dt) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(Game::instance().getWindow());
    sf::Vector2f mousePos(static_cast<float>(mousePixel.x),
                          static_cast<float>(mousePixel.y));

    m_backHovered = m_backBtnBg.getGlobalBounds().contains(mousePos);
    m_backBtnBg.setFillColor(m_backHovered
        ? sf::Color(120, 120, 180)
        : sf::Color(80, 80, 100));
}

void RankingState::draw(sf::RenderWindow& window) {
    window.draw(m_background);

    if (!m_fontLoaded) return;

    window.draw(m_titleText);

    // Vẽ trophy placeholder
    window.draw(m_trophy);
    // Vẽ chân trophy
    sf::RectangleShape trophyBase(sf::Vector2f(50.f, 8.f));
    trophyBase.setPosition(375.f, 130.f);
    trophyBase.setFillColor(sf::Color(200, 170, 0));
    window.draw(trophyBase);
    // Vẽ thân trophy nhỏ
    sf::RectangleShape trophyStem(sf::Vector2f(12.f, 15.f));
    trophyStem.setPosition(394.f, 115.f);
    trophyStem.setFillColor(sf::Color(200, 170, 0));
    window.draw(trophyStem);

    // Đường kẻ dưới tiêu đề
    sf::RectangleShape divider(sf::Vector2f(500.f, 2.f));
    divider.setPosition(150.f, 145.f);
    divider.setFillColor(sf::Color(255, 215, 0, 100));
    window.draw(divider);

    // Header bảng
    window.draw(m_headerRank);
    window.draw(m_headerName);
    window.draw(m_headerScore);

    // Đường kẻ dưới header
    sf::RectangleShape headerLine(sf::Vector2f(500.f, 1.f));
    headerLine.setPosition(150.f, 178.f);
    headerLine.setFillColor(sf::Color(100, 100, 140));
    window.draw(headerLine);

    if (m_entries.empty()) {
        window.draw(m_noDataText);
    } else {
        for (size_t i = 0; i < m_entries.size(); i++) {
            // Nền xen kẽ cho hàng
            if (i % 2 == 0) {
                float y = m_entries[i].rankText.getPosition().y - 3.f;
                sf::RectangleShape rowBg(sf::Vector2f(500.f, 30.f));
                rowBg.setPosition(150.f, y);
                rowBg.setFillColor(sf::Color(40, 40, 60, 100));
                window.draw(rowBg);
            }

            window.draw(m_entries[i].rankText);
            window.draw(m_entries[i].nameText);
            window.draw(m_entries[i].scoreText);
        }
    }

    window.draw(m_backBtnBg);
    window.draw(m_backBtnText);
}
