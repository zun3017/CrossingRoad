#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "../Core/State.h"
#include "../UI/Button.h"

// Định danh cho từng setting toggle
enum class SettingId { Sound, Music, Motion };

// Toggle row cho Settings
struct SettingToggle {
    sf::RectangleShape cardBg;
    sf::RectangleShape checkbox;
    sf::Text iconText;
    sf::Text label;
    sf::Text statusText;
    sf::Sprite statusSprite;
    sf::RectangleShape volumeBarBg;
    sf::RectangleShape volumeBarFill;
    sf::CircleShape sliderKnob;
    sf::Text volumeLabel;
    sf::Text volumeValueText;
    SettingId settingId = SettingId::Sound;
    bool hovered = false;
    bool isDraggingVolume = false;
};

class SettingState : public State {
public:
    SettingState() = default;

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    bool m_fontLoaded = false;

    // Textures cho nút ON/OFF
    sf::Texture m_onTexture;
    sf::Texture m_offTexture;
    bool m_onLoaded = false;
    bool m_offLoaded = false;

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

    // Bottom Preset Info Bar
    sf::RectangleShape m_infoBarBg;
    sf::Text m_infoBarText;
    sf::Text m_resetDefaultsBtnText;
    sf::FloatRect m_resetBtnBounds;

    // Toggle rows
    SettingToggle m_soundToggle;
    SettingToggle m_musicToggle;
    SettingToggle m_motionToggle;

    // Back button
    sf::Texture m_backTexture;
    std::unique_ptr<Button> m_backBtn;

    SettingToggle* m_activeSlider = nullptr;

    // Lấy giá trị hiện tại của một setting
    bool getSettingValue(SettingId id) const;
    // Đảo giá trị một setting
    void toggleSetting(SettingId id);
    // Reset defaults
    void resetDefaults();

    void initToggle(SettingToggle& toggle, const std::string& iconStr, const std::string& labelStr,
                    float y, SettingId id);
    void updateToggleVisual(SettingToggle& toggle);
    void updateSliderFromMouse(SettingToggle& toggle, float mouseX);
};
