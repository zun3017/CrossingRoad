#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../Core/State.h"

// Mỗi hàng bảng xếp hạng
struct RankEntry {
    sf::Text rankText;
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

    // Trophy placeholder
    sf::RectangleShape m_trophy;

    // Table header
    sf::Text m_headerRank;
    sf::Text m_headerName;
    sf::Text m_headerScore;

    // Rank entries
    std::vector<RankEntry> m_entries;
    sf::Text m_noDataText;

    // Back button
    sf::RectangleShape m_backBtnBg;
    sf::Text m_backBtnText;
    bool m_backHovered = false;
};
