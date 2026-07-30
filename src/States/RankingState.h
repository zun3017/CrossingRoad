#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "../Core/State.h"
#include "../UI/Button.h"

// Mỗi hàng bảng xếp hạng
struct RankEntry {
    sf::Text rankText;
    sf::Sprite medalSprite;
    bool isMedal = false;
    sf::Text nameText;
    sf::Text scoreText;
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

    // Title
    sf::Text m_titleText;

    // Cup sprite
    sf::Texture m_cupTexture;
    sf::Sprite m_cupSprite;
    bool m_cupLoaded = false;

    // Medal textures (Gold, Silver, Bronze for Top 3)
    sf::Texture m_goldMedalTex;
    sf::Texture m_silverMedalTex;
    sf::Texture m_bronzeMedalTex;
    bool m_medalsLoaded = false;

    // Table header
    sf::Text m_headerRank;
    sf::Text m_headerName;
    sf::Text m_headerScore;

    // Rank entries
    std::vector<RankEntry> m_entries;
    sf::Text m_noDataText;

    // Back button
    sf::Texture m_backTexture;
    std::unique_ptr<Button> m_backBtn;
};
