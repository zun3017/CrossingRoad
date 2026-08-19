#include "RankingState.h"
#include "../Core/Game.h"
#include "../Core/ResourceManager.h"
#include "../Managers/SaveManager.h"
#include <memory>
#include <algorithm>

void RankingState::init() {
    auto& font = FontManager::getInstance().get("assets/fonts/arial.ttf");
    m_font = font;
    m_fontLoaded = true;

    auto& cupTex = TextureManager::getInstance().get("assets/textures/cup.png");
    if (cupTex.getSize().x > 0) {
        m_cupLoaded = true;
        m_cupTexture = cupTex;
    }

    auto& goldTex = TextureManager::getInstance().get("assets/textures/gold_medal.png");
    auto& silverTex = TextureManager::getInstance().get("assets/textures/silver_medal.png");
    auto& bronzeTex = TextureManager::getInstance().get("assets/textures/bronze_medal.png");
    if (goldTex.getSize().x > 0 && silverTex.getSize().x > 0 && bronzeTex.getSize().x > 0) {
        m_medalsLoaded = true;
        m_goldMedalTex = goldTex;
        m_silverMedalTex = silverTex;
        m_bronzeMedalTex = bronzeTex;
    }

    // Load menu.png background from cache
    auto& bgTex = TextureManager::getInstance().get("assets/textures/menu.png");
    if (bgTex.getSize().x > 0) {
        m_bgLoaded = true;
        m_bgSprite.setTexture(bgTex);
        sf::Vector2u size = bgTex.getSize();
        m_bgSprite.setScale(
            800.f / static_cast<float>(size.x),
            600.f / static_cast<float>(size.y));
    } else {
        m_background.setSize(sf::Vector2f(800.f, 600.f));
        m_background.setFillColor(sf::Color(124, 179, 66));
    }

    // Main window container (Sky blue)
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
        // Title: LEADERBOARD
        m_titleShadow.setFont(m_font);
        m_titleShadow.setString("LEADERBOARD");
        m_titleShadow.setCharacterSize(36);
        m_titleShadow.setFillColor(sf::Color::Black);
        m_titleShadow.setStyle(sf::Text::Bold);
        sf::FloatRect tb = m_titleShadow.getLocalBounds();
        m_titleShadow.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleShadow.setPosition(403.f, 61.f);

        m_titleText.setFont(m_font);
        m_titleText.setString("LEADERBOARD");
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

        // Table Box Container
        m_tableBox.setSize(sf::Vector2f(660.f, 365.f));
        m_tableBox.setPosition(70.f, 110.f);
        m_tableBox.setFillColor(sf::Color(0, 0, 0, 180));
        m_tableBox.setOutlineColor(sf::Color::Black);
        m_tableBox.setOutlineThickness(3.f);

        // Table Column Headers
        m_headerRank.setFont(m_font);
        m_headerRank.setString("RANK");
        m_headerRank.setCharacterSize(15);
        m_headerRank.setFillColor(sf::Color(253, 216, 53));
        m_headerRank.setStyle(sf::Text::Bold);
        m_headerRank.setPosition(110.f, 122.f);

        m_headerName.setFont(m_font);
        m_headerName.setString("PLAYER");
        m_headerName.setCharacterSize(15);
        m_headerName.setFillColor(sf::Color(253, 216, 53));
        m_headerName.setStyle(sf::Text::Bold);
        m_headerName.setPosition(240.f, 122.f);

        m_headerScore.setFont(m_font);
        m_headerScore.setString("SCORE");
        m_headerScore.setCharacterSize(15);
        m_headerScore.setFillColor(sf::Color(253, 216, 53));
        m_headerScore.setStyle(sf::Text::Bold);
        m_headerScore.setPosition(610.f, 122.f);

        m_headerDivider.setSize(sf::Vector2f(640.f, 3.f));
        m_headerDivider.setPosition(80.f, 148.f);
        m_headerDivider.setFillColor(sf::Color::Black);

        // No Data text
        m_noDataText.setFont(m_font);
        m_noDataText.setString("NO HIGH SCORES YET");
        m_noDataText.setCharacterSize(20);
        m_noDataText.setFillColor(sf::Color(253, 216, 53));
        m_noDataText.setStyle(sf::Text::Bold);
        sf::FloatRect nb = m_noDataText.getLocalBounds();
        m_noDataText.setOrigin(nb.left + nb.width / 2.f, nb.top + nb.height / 2.f);
        m_noDataText.setPosition(400.f, 280.f);
    }

    // Cup sprite position (2 cup đối xứng 2 bên tiêu đề LEADERBOARD)
    if (m_cupLoaded) {
        m_cupSprite.setTexture(m_cupTexture);
        m_cupRightSprite.setTexture(m_cupTexture);
        sf::Vector2u size = m_cupTexture.getSize();
        if (size.x > 0 && size.y > 0) {
            m_cupSprite.setOrigin(size.x / 2.f, size.y / 2.f);
            m_cupSprite.setScale(36.f / static_cast<float>(size.x), 36.f / static_cast<float>(size.y));

            m_cupRightSprite.setOrigin(size.x / 2.f, size.y / 2.f);
            m_cupRightSprite.setScale(36.f / static_cast<float>(size.x), 36.f / static_cast<float>(size.y));
        }
        m_cupSprite.setPosition(150.f, 58.f);
        m_cupRightSprite.setPosition(650.f, 58.f);
    }

    // Load Highscores
    auto highscores = SaveManager::loadHighscores();

    for (size_t i = 0; i < highscores.size() && i < 8; i++) {
        RankEntry entry;
        entry.rank = static_cast<int>(i + 1);
        entry.name = highscores[i].name;
        entry.score = highscores[i].score;

        sf::Color textColor = sf::Color::White;
        if (i == 0) textColor = sf::Color(255, 215, 0);       // Gold
        else if (i == 1) textColor = sf::Color(192, 192, 192); // Silver
        else if (i == 2) textColor = sf::Color(205, 127, 50);  // Bronze

        if (m_medalsLoaded && i < 3) {
            entry.isMedal = true;
            if (i == 0) entry.medalSprite.setTexture(m_goldMedalTex);
            else if (i == 1) entry.medalSprite.setTexture(m_silverMedalTex);
            else if (i == 2) entry.medalSprite.setTexture(m_bronzeMedalTex);

            sf::Vector2u size = entry.medalSprite.getTexture()->getSize();
            if (size.x > 0 && size.y > 0) {
                entry.medalSprite.setScale(24.f / static_cast<float>(size.x), 24.f / static_cast<float>(size.y));
            }
        } else {
            entry.isMedal = false;
            entry.rankText.setFont(m_font);
            entry.rankText.setString("#" + std::to_string(i + 1));
            entry.rankText.setCharacterSize(14);
            entry.rankText.setFillColor(textColor);
            entry.rankText.setStyle(sf::Text::Bold);
        }

        entry.nameText.setFont(m_font);
        entry.nameText.setString(entry.name);
        entry.nameText.setCharacterSize(14);
        entry.nameText.setFillColor(textColor);
        entry.nameText.setStyle(sf::Text::Bold);

        entry.scoreText.setFont(m_font);
        entry.scoreText.setString(std::to_string(entry.score));
        entry.scoreText.setCharacterSize(14);
        entry.scoreText.setFillColor((i < 3) ? textColor : sf::Color(67, 160, 71)); // Green for 4+
        entry.scoreText.setStyle(sf::Text::Bold);

        entry.ptsText.setFont(m_font);
        entry.ptsText.setString("PTS");
        entry.ptsText.setCharacterSize(10);
        entry.ptsText.setFillColor(sf::Color(180, 180, 180));

        m_entries.push_back(entry);
    }

    // Back Button (Wooden pixel style)
    auto& backTex = TextureManager::getInstance().get("assets/textures/back_text.png");
    if (backTex.getSize().x > 0) {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, backTex, []() {
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

    if (m_cupLoaded) {
        window.draw(m_cupSprite);
        window.draw(m_cupRightSprite);
    }

    // Table Box & Header
    window.draw(m_tableBox);
    window.draw(m_headerRank);
    window.draw(m_headerName);
    window.draw(m_headerScore);

    sf::RectangleShape hDivider(sf::Vector2f(640.f, 2.f));
    hDivider.setPosition(80.f, 148.f);
    hDivider.setFillColor(sf::Color::Black);
    window.draw(hDivider);

    if (m_entries.empty()) {
        window.draw(m_noDataText);
    } else {
        float startY = 156.f;
        float rowHeight = 38.f;

        for (size_t i = 0; i < m_entries.size(); i++) {
            float y = startY + static_cast<float>(i) * rowHeight;

            // Row Container
            sf::RectangleShape rowBg(sf::Vector2f(640.f, 34.f));
            rowBg.setPosition(80.f, y);

            sf::RectangleShape leftBorder(sf::Vector2f(6.f, 34.f));
            leftBorder.setPosition(80.f, y);

            if (i == 0) {
                rowBg.setFillColor(sf::Color(255, 215, 0, 35));
                leftBorder.setFillColor(sf::Color(255, 215, 0));
            } else if (i == 1) {
                rowBg.setFillColor(sf::Color(192, 192, 192, 35));
                leftBorder.setFillColor(sf::Color(192, 192, 192));
            } else if (i == 2) {
                rowBg.setFillColor(sf::Color(205, 127, 50, 35));
                leftBorder.setFillColor(sf::Color(205, 127, 50));
            } else {
                rowBg.setFillColor(sf::Color(0, 0, 0, 80));
                leftBorder.setFillColor(sf::Color(80, 80, 80));
                leftBorder.setSize(sf::Vector2f(3.f, 34.f));
            }

            rowBg.setOutlineColor(sf::Color::Black);
            rowBg.setOutlineThickness(2.f);

            window.draw(rowBg);
            window.draw(leftBorder);

            // Rank Badge / Text
            if (m_entries[i].isMedal) {
                m_entries[i].medalSprite.setPosition(115.f, y + 5.f);
                window.draw(m_entries[i].medalSprite);
            } else {
                m_entries[i].rankText.setPosition(115.f, y + 8.f);
                window.draw(m_entries[i].rankText);
            }

            // Name
            m_entries[i].nameText.setPosition(240.f, y + 8.f);
            window.draw(m_entries[i].nameText);

            // Score & PTS
            sf::FloatRect sb = m_entries[i].scoreText.getLocalBounds();
            float scoreX = 665.f - sb.width;
            m_entries[i].scoreText.setPosition(scoreX, y + 8.f);
            window.draw(m_entries[i].scoreText);

            m_entries[i].ptsText.setPosition(672.f, y + 12.f);
            window.draw(m_entries[i].ptsText);
        }
    }

    // Back Button
    if (m_backBtn) window.draw(*m_backBtn);
}

