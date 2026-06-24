#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "../Core/State.h"

class HelpState : public State {
public:
    HelpState() = default;

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

    // Content text
    sf::Text m_contentText;

    // Back button
    sf::RectangleShape m_backBtnBg;
    sf::Text m_backBtnText;
    bool m_backHovered = false;
};
