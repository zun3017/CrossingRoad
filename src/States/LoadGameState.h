#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "../Core/State.h"
#include "../UI/Button.h"

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

    // Content Box
    sf::RectangleShape m_contentBox;
    sf::RectangleShape m_contentBoxBorder;

    // Save entries
    std::vector<SaveEntry> m_entries;
    sf::Text m_noSavesText;

    // Back button
    sf::Texture m_backTexture;
    std::unique_ptr<Button> m_backBtn;

    void refreshSaveList();
};
