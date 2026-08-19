#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "../Core/State.h"
#include "../UI/Button.h"

// Mỗi hàng bảng xếp hạng
struct RankEntry {
    int rank = 0;
    std::string name;
    int score = 0;
    sf::Text rankText;
    sf::Sprite medalSprite;
    bool isMedal = false;
    sf::Text nameText;
    sf::Text scoreText;
    sf::Text ptsText;
};

class RankingState : public State {
public:
    RankingState() = default;

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    bool m_fontLoaded = false;

    // Background
    sf::RectangleShape m_background;
    sf::Texture m_bgTexture;
    sf::Sprite m_bgSprite;
    bool m_bgLoaded = false;

    // Main window container (Sky blue)
    sf::RectangleShape m_mainContainer;
    sf::RectangleShape m_containerBorder;
    sf::RectangleShape m_topRibbon;

    // Header Title & Accent Bar
    sf::Text m_titleShadow;
    sf::Text m_titleText;
    sf::RectangleShape m_accentBar;
    sf::RectangleShape m_accentBarBorder;

    // Cup sprite (2 cup đối xứng 2 bên tiêu đề)
    sf::Texture m_cupTexture;
    sf::Sprite m_cupSprite;
    sf::Sprite m_cupRightSprite;
    bool m_cupLoaded = false;

    // Medal textures (Gold, Silver, Bronze for Top 3)
    sf::Texture m_goldMedalTex;
    sf::Texture m_silverMedalTex;
    sf::Texture m_bronzeMedalTex;
    bool m_medalsLoaded = false;

    // Table container & Header
    sf::RectangleShape m_tableBox;
    sf::RectangleShape m_tableBoxBorder;
    sf::Text m_headerRank;
    sf::Text m_headerName;
    sf::Text m_headerScore;
    sf::RectangleShape m_headerDivider;

    // Rank entries
    std::vector<RankEntry> m_entries;
    sf::Text m_noDataText;

    // Back button
    sf::Texture m_backTexture;
    std::unique_ptr<Button> m_backBtn;
};
