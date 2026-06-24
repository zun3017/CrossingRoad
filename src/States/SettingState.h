#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "../Core/State.h"

// Định danh cho từng setting toggle
enum class SettingId { Sound, Music, Motion };

// Toggle row cho Settings
struct SettingToggle {
    sf::RectangleShape checkbox;
    sf::Text label;
    sf::Text statusText;
    SettingId settingId;
    bool hovered = false;
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

    // Background
    sf::RectangleShape m_background;

    // Title
    sf::Text m_titleText;

    // Toggle rows
    SettingToggle m_soundToggle;
    SettingToggle m_musicToggle;
    SettingToggle m_motionToggle;

    // Back button
    sf::RectangleShape m_backBtnBg;
    sf::Text m_backBtnText;
    bool m_backHovered = false;

    // Lấy giá trị hiện tại của một setting
    bool getSettingValue(SettingId id) const;
    // Đảo giá trị một setting
    void toggleSetting(SettingId id);

    void initToggle(SettingToggle& toggle, const std::string& labelStr,
                    float y, SettingId id);
    void updateToggleVisual(SettingToggle& toggle);
};
