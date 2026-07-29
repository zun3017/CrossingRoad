#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../Core/State.h"

// ============================================================
// Cloud
// ============================================================

struct Cloud
{
    sf::RectangleShape shape;
    sf::Sprite sprite;
    float speed = 0.f;
};

// ============================================================
// Menu Button
// ============================================================

struct MenuButton
{
    // Vùng click
    sf::RectangleShape hitbox;

    // Sprite sheet của button
    sf::Sprite sprite;

    // Vị trí gốc của button
    sf::Vector2f basePosition;

    // Kích thước hiển thị
    float width = 50.f;
    float height = 50.f;

    // Trạng thái
    bool hovered = false;
    bool pressed = false;

    // Frame hiện tại
    int currentFrame = 0;
};

// ============================================================
// MainMenuState
// ============================================================

class MainMenuState : public State
{
public:
    MainMenuState() = default;

    void init() override;
    void handleInput(sf::RenderWindow &window, sf::Event &event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow &window) override;

private:
    // ========================================================
    // Font
    // ========================================================

    sf::Font m_font;
    bool m_fontLoaded = false;

    // ========================================================
    // Background
    // ========================================================

    sf::Texture m_menuBgTexture;
    sf::Sprite m_menuBgSprite;
    bool m_menuBgLoaded = false;

    sf::RectangleShape m_bgTop;
    sf::RectangleShape m_bgBottom;

    // ========================================================
    // Clouds
    // ========================================================

    sf::Texture m_cloudTexture;
    bool m_cloudLoaded = false;

    std::vector<Cloud> m_clouds;

    // ========================================================
    // Title & Subtitle
    // ========================================================

    sf::Text m_titleText;
    sf::Text m_subtitleText;

    float m_blinkTimer = 0.f;
    bool m_subtitleVisible = true;

    // ========================================================
    // Button textures
    // ========================================================

    static const int BUTTON_COUNT = 7;

    sf::Texture m_buttonTextures[BUTTON_COUNT];
    bool m_buttonLoaded[BUTTON_COUNT];

    std::vector<MenuButton> m_buttons;

    // ========================================================
    // Button configuration
    // ========================================================

    const float BUTTON_SIZE = 50.f;
    const float BUTTON_SPACING = 10.f;

    // Khoảng cách button di chuyển lên khi hover
    const float HOVER_OFFSET = 4.f;

    // Khoảng cách button di chuyển lên khi click
    const float PRESSED_OFFSET = 2.f;

    // ========================================================
    // Functions
    // ========================================================

    void initBackground();
    void initClouds();
    void initButtons();

    void setButtonFrame(MenuButton &button, int frame);
    void updateButtonHover();

    void onButtonClick(int index);
};