#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "../Core/State.h"

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

    // Decorative top bar
    sf::RectangleShape m_topBar;

    // Title
    sf::Text m_titleText;

    // Content lines
    sf::Text m_line1; // Game name
    sf::Text m_line2; // Built with
    sf::Text m_line3; // Developed by
    sf::Text m_line4; // Teacher
    sf::Text m_line5; // School + year

    // Decorative logo placeholder
    sf::RectangleShape m_logoBg;
    sf::Text m_logoText;

    // Back button
    sf::RectangleShape m_backBtnBg;
    sf::Text m_backBtnText;
    bool m_backHovered = false;
};
