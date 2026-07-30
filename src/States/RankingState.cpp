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
    m_cupLoaded = m_cupTexture.loadFromFile("assets/textures/cup.png");
    bool gL = m_goldMedalTex.loadFromFile("assets/textures/gold_medal.png");
    bool sL = m_silverMedalTex.loadFromFile("assets/textures/silver_medal.png");
    bool bL = m_bronzeMedalTex.loadFromFile("assets/textures/bronze_medal.png");
    m_medalsLoaded = gL && sL && bL;

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

    // Cup sprite
    if (m_cupLoaded) {
        m_cupSprite.setTexture(m_cupTexture);
        sf::Vector2u size = m_cupTexture.getSize();
        if (size.x > 0 && size.y > 0) {
            m_cupSprite.setOrigin(size.x / 2.f, size.y / 2.f);
            m_cupSprite.setScale(50.f / static_cast<float>(size.x), 50.f / static_cast<float>(size.y));
        }
        m_cupSprite.setPosition(400.f, 105.f);
    }

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

        if (m_medalsLoaded && i < 3) {
            entry.isMedal = true;
            if (i == 0) entry.medalSprite.setTexture(m_goldMedalTex);
            else if (i == 1) entry.medalSprite.setTexture(m_silverMedalTex);
            else if (i == 2) entry.medalSprite.setTexture(m_bronzeMedalTex);

            sf::Vector2u size = entry.medalSprite.getTexture()->getSize();
            if (size.x > 0 && size.y > 0) {
                entry.medalSprite.setScale(26.f / static_cast<float>(size.x), 26.f / static_cast<float>(size.y));
            }
            entry.medalSprite.setPosition(165.f, y - 1.f);
        } else {
            entry.isMedal = false;
            entry.rankText.setFont(m_font);
            entry.rankText.setString("#" + std::to_string(i + 1));
            entry.rankText.setCharacterSize(18);
            entry.rankText.setFillColor(rankColor);
            entry.rankText.setPosition(160.f, y);
        }

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

    // Nút BACK bằng back_text.png
    bool backLoaded = m_backTexture.loadFromFile("assets/textures/back_text.png");
    if (backLoaded) {
        m_backBtn = std::make_unique<Button>(320.f, 530.f, 160.f, 45.f, m_backTexture, []() {
            Game::instance().getStateMachine().popState();
        });
    } else {
        m_backBtn = std::make_unique<Button>(320.f, 530.f, 160.f, 45.f, "BACK", m_font, []() {
            Game::instance().getStateMachine().popState();
        });
    }
}

void RankingState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (m_backBtn) m_backBtn->handleEvent(event, window);

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void RankingState::update(float dt) {
    if (m_backBtn) m_backBtn->update(dt);
}

void RankingState::draw(sf::RenderWindow& window) {
    window.draw(m_background);

    if (!m_fontLoaded) return;

    window.draw(m_titleText);

    // Vẽ cup sprite nếu loaded thành công
    if (m_cupLoaded) {
        window.draw(m_cupSprite);
    }

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
                float y = (m_entries[i].isMedal ? m_entries[i].medalSprite.getPosition().y + 1.f : m_entries[i].rankText.getPosition().y) - 3.f;
                sf::RectangleShape rowBg(sf::Vector2f(500.f, 30.f));
                rowBg.setPosition(150.f, y);
                rowBg.setFillColor(sf::Color(40, 40, 60, 100));
                window.draw(rowBg);
            }

            if (m_entries[i].isMedal) {
                window.draw(m_entries[i].medalSprite);
            } else {
                window.draw(m_entries[i].rankText);
            }
            window.draw(m_entries[i].nameText);
            window.draw(m_entries[i].scoreText);
        }
    }

    if (m_backBtn) window.draw(*m_backBtn);
}
