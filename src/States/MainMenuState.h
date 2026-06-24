#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../Core/State.h"

// Cấu trúc đám mây trang trí nền
struct Cloud {
    sf::RectangleShape shape;
    float speed;
};

// Cấu trúc nút menu
struct MenuButton {
    sf::RectangleShape background;
    sf::Text label;
    bool hovered = false;
};

class MainMenuState : public State {
public:
    MainMenuState() = default;

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    bool m_fontLoaded = false;

    // Background gradient
    sf::RectangleShape m_bgTop;
    sf::RectangleShape m_bgBottom;

    // Title & subtitle
    sf::Text m_titleText;
    sf::Text m_subtitleText;
    float m_blinkTimer = 0.f;
    bool m_subtitleVisible = true;

    // Animated clouds
    std::vector<Cloud> m_clouds;

    // Menu buttons
    std::vector<MenuButton> m_buttons;
    std::vector<std::string> m_buttonLabels;

    void initBackground();
    void initClouds();
    void initButtons();
    void onButtonClick(int index);
};
