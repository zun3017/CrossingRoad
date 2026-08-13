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
    sf::RectangleShape checkbox;
    sf::Text label;
    sf::Text statusText;
    sf::Sprite statusSprite;
    SettingId settingId = SettingId::Sound;
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

    // Textures cho nút ON/OFF
    sf::Texture m_onTexture;
    sf::Texture m_offTexture;
    bool m_onLoaded = false;
    bool m_offLoaded = false;

    // Background
    sf::RectangleShape m_background;

    // Title
    sf::Text m_titleText;

    // Toggle rows
    SettingToggle m_soundToggle;
    SettingToggle m_musicToggle;
    SettingToggle m_motionToggle;

    // Back button
    sf::Texture m_backTexture;
    std::unique_ptr<Button> m_backBtn;

    // Lấy giá trị hiện tại của một setting
    bool getSettingValue(SettingId id) const;
    // Đảo giá trị một setting
    void toggleSetting(SettingId id);

    void initToggle(SettingToggle& toggle, const std::string& labelStr,
                    float y, SettingId id);
    void updateToggleVisual(SettingToggle& toggle);
};
