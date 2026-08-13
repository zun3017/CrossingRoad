#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "../Core/State.h"
#include "../UI/Button.h"

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
    sf::Texture m_backTexture;
    std::unique_ptr<Button> m_backBtn;
};
