#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "../Core/State.h"
#include "../UI/Button.h"

struct MemberInfo {
    std::string num;
    std::string name;
    std::string id;
    sf::Color badgeColor;
};

class AboutState : public State {
public:
    AboutState() = default;

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

    // Window frame container (Sky blue)
    sf::RectangleShape m_mainContainer;
    sf::RectangleShape m_containerBorder;
    sf::RectangleShape m_topRibbon;

    // Header Title & Accent Bar
    sf::Text m_titleShadow;
    sf::Text m_titleText;
    sf::RectangleShape m_accentBar;
    sf::RectangleShape m_accentBarBorder;

    // Game Info Box
    sf::RectangleShape m_infoBox;
    sf::RectangleShape m_infoBoxBorder;
    sf::RectangleShape m_crBadge;
    sf::RectangleShape m_crBadgeBorder;
    sf::Text m_crBadgeText;
    sf::Text m_gameTitle;
    sf::Text m_gameSubtitle;
    sf::RectangleShape m_schoolTagBg;
    sf::RectangleShape m_schoolTagBorder;
    sf::Text m_schoolTagText;

    // Developed By Header
    sf::Text m_devHeader;
    sf::Text m_memberCountBadge;

    // Team Member Cards
    std::vector<MemberInfo> m_members;

    // Back button
    sf::Texture m_backTexture;
    std::unique_ptr<Button> m_backBtn;
};
