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

    sf::Texture m_menuBgTexture; // Static fallback
    sf::Sprite m_menuBgSprite;
    bool m_menuBgLoaded = false;
    
    std::vector<sf::Texture> m_menuVideoTextures;
    float m_videoAnimTimer = 0.f;
    int m_currentVideoFrame = 0;

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
    bool m_buttonLoaded[BUTTON_COUNT] = {false};
    
    sf::Texture m_hoverTextTextures[BUTTON_COUNT];
    bool m_hoverTextLoaded[BUTTON_COUNT] = {false};
    sf::Sprite m_hoverTextSprites[BUTTON_COUNT];

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

    void initBackground();
    void initClouds();
    void initButtons();

    void setButtonFrame(MenuButton &button, int frame);
    void updateButtonHover();

    void onButtonClick(int index);

    // ========================================================
    // Skins Button (Góc dưới trái màn hình)
    // ========================================================
    sf::Texture m_skinsButtonTexture;
    bool m_skinsButtonLoaded = false;
    sf::Texture m_skinsTextTexture;
    bool m_skinsTextLoaded = false;
    sf::Sprite m_skinsTextSprite;
    MenuButton m_skinsButton;

    // ========================================================
    // Skin Selection UI Popup (3 Danh Mục: Nhân vật, Khúc gỗ, Xe hơi)
    // ========================================================
    bool m_showSkinsUI = false;
    int m_viewingCategory = 0; // 0 = Nhân vật, 1 = Khúc gỗ, 2 = Xe ô tô
    int m_viewingSkinIndex = 0;
    static const int TOTAL_SKINS = 3;

    // Textures preview cho từng danh mục
    sf::Texture m_playerPreviewTextures[TOTAL_SKINS];
    bool m_playerPreviewLoaded[TOTAL_SKINS] = {false};

    sf::Texture m_logPreviewTextures[TOTAL_SKINS];
    bool m_logPreviewLoaded[TOTAL_SKINS] = {false};

    sf::Texture m_carPreviewTextures[TOTAL_SKINS];
    bool m_carPreviewLoaded[TOTAL_SKINS] = {false};

    sf::Sprite m_skinPreviewSprite;

    sf::Texture m_selectTextTexture;
    sf::Texture m_selectedTextTexture;
    bool m_selectTextLoaded = false;
    bool m_selectedTextLoaded = false;
    sf::Sprite m_selectBtnSprite;

    sf::Text m_skinNameText;
    sf::Text m_skinDescText;
    sf::Text m_skinTitleText;
    sf::Text m_arrowLeftText;
    sf::Text m_arrowRightText;
    sf::Text m_skinCloseBtn;

    // 3 Tab chuyển đổi danh mục
    sf::RectangleShape m_tabBgs[3];
    sf::Text m_tabTexts[3];

    sf::RectangleShape m_skinOverlayDim;
    sf::RectangleShape m_skinOuterFrame;
    sf::RectangleShape m_skinInnerFrame;

    bool m_arrowLeftHovered = false;
    bool m_arrowRightHovered = false;
    bool m_selectBtnHovered = false;
    bool m_skinCloseHovered = false;

    void initSkinSelectionUI();
    void updateSkinPreview();
};