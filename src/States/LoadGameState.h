#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../Core/State.h"

// Mỗi entry hiển thị 1 save file
struct SaveEntry {
    std::string filename;
    sf::Text nameText;
    sf::Text detailText;
    sf::RectangleShape loadBtnBg;
    sf::Text loadBtnText;
    sf::RectangleShape deleteBtnBg;
    sf::Text deleteBtnText;
    bool loadHovered = false;
    bool deleteHovered = false;
};

class LoadGameState : public State {
public:
    LoadGameState() = default;

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

    // Save entries
    std::vector<SaveEntry> m_entries;
    sf::Text m_noSavesText;

    // Back button
    sf::RectangleShape m_backBtnBg;
    sf::Text m_backBtnText;
    bool m_backHovered = false;

    void refreshSaveList();
};
