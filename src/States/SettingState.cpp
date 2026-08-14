#include "SettingState.h"
#include "../Core/Game.h"
#include <memory>

bool SettingState::getSettingValue(SettingId id) const {
    switch (id) {
    case SettingId::Sound:  return Game::instance().isSoundEnabled();
    case SettingId::Music:  return Game::instance().isMusicEnabled();
    case SettingId::Motion: return Game::instance().isMotionEnabled();
    }
    return false;
}

void SettingState::toggleSetting(SettingId id) {
    switch (id) {
    case SettingId::Sound:
        Game::instance().setSoundEnabled(!Game::instance().isSoundEnabled());
        break;
    case SettingId::Music:
        Game::instance().setMusicEnabled(!Game::instance().isMusicEnabled());
        break;
    case SettingId::Motion:
        Game::instance().setMotionEnabled(!Game::instance().isMotionEnabled());
        break;
    }
}

void SettingState::resetDefaults() {
    Game::instance().setSoundEnabled(true);
    Game::instance().setMusicEnabled(true);
    Game::instance().setMotionEnabled(true);

    updateToggleVisual(m_soundToggle);
    updateToggleVisual(m_musicToggle);
    updateToggleVisual(m_motionToggle);
}

void SettingState::init() {
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");
    m_onLoaded = m_onTexture.loadFromFile("assets/textures/on_button.png");
    m_offLoaded = m_offTexture.loadFromFile("assets/textures/off_button.png");

    // Load menu.png background
    m_bgLoaded = m_bgTexture.loadFromFile("assets/textures/menu.png");
    if (m_bgLoaded) {
        m_bgSprite.setTexture(m_bgTexture);
        sf::Vector2u size = m_bgTexture.getSize();
        if (size.x > 0 && size.y > 0) {
            m_bgSprite.setScale(
                800.f / static_cast<float>(size.x),
                600.f / static_cast<float>(size.y));
        }
    } else {
        m_background.setSize(sf::Vector2f(800.f, 600.f));
        m_background.setFillColor(sf::Color(124, 179, 66));
    }

    // Main window container (Sky Blue)
    m_mainContainer.setSize(sf::Vector2f(720.f, 540.f));
    m_mainContainer.setOrigin(360.f, 270.f);
    m_mainContainer.setPosition(400.f, 300.f);
    m_mainContainer.setFillColor(sf::Color(74, 144, 226, 242)); // #4a90e2 with 95% opacity

    m_containerBorder.setSize(sf::Vector2f(720.f, 540.f));
    m_containerBorder.setOrigin(360.f, 270.f);
    m_containerBorder.setPosition(400.f, 300.f);
    m_containerBorder.setFillColor(sf::Color::Transparent);
    m_containerBorder.setOutlineColor(sf::Color::Black);
    m_containerBorder.setOutlineThickness(4.f);

    // Top ribbon
    m_topRibbon.setSize(sf::Vector2f(720.f, 8.f));
    m_topRibbon.setPosition(40.f, 30.f);
    m_topRibbon.setFillColor(sf::Color(255, 255, 255, 60));

    if (m_fontLoaded) {
        // Title: SETTINGS
        m_titleShadow.setFont(m_font);
        m_titleShadow.setString("SETTINGS");
        m_titleShadow.setCharacterSize(36);
        m_titleShadow.setFillColor(sf::Color::Black);
        m_titleShadow.setStyle(sf::Text::Bold);
        sf::FloatRect tb = m_titleShadow.getLocalBounds();
        m_titleShadow.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleShadow.setPosition(403.f, 61.f);

        m_titleText.setFont(m_font);
        m_titleText.setString("SETTINGS");
        m_titleText.setCharacterSize(36);
        m_titleText.setFillColor(sf::Color(253, 216, 53)); // #fdd835 yellow
        m_titleText.setStyle(sf::Text::Bold);
        m_titleText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleText.setPosition(400.f, 58.f);

        // Accent Bar
        m_accentBar.setSize(sf::Vector2f(440.f, 6.f));
        m_accentBar.setOrigin(220.f, 0.f);
        m_accentBar.setPosition(400.f, 88.f);
        m_accentBar.setFillColor(sf::Color(253, 216, 53));

        m_accentBarBorder.setSize(sf::Vector2f(440.f, 3.f));
        m_accentBarBorder.setOrigin(220.f, 0.f);
        m_accentBarBorder.setPosition(400.f, 94.f);
        m_accentBarBorder.setFillColor(sf::Color::Black);

        // Content Box Container
        m_contentBox.setSize(sf::Vector2f(660.f, 365.f));
        m_contentBox.setPosition(70.f, 110.f);
        m_contentBox.setFillColor(sf::Color(0, 0, 0, 180));
        m_contentBox.setOutlineColor(sf::Color::Black);
        m_contentBox.setOutlineThickness(3.f);

        // Bottom Preset Info Bar
        m_infoBarBg.setSize(sf::Vector2f(630.f, 32.f));
        m_infoBarBg.setPosition(85.f, 430.f);
        m_infoBarBg.setFillColor(sf::Color(0, 0, 0, 140));
        m_infoBarBg.setOutlineColor(sf::Color::Black);
        m_infoBarBg.setOutlineThickness(2.f);

        m_infoBarText.setFont(m_font);
        m_infoBarText.setString("Auto-saves changes to local config");
        m_infoBarText.setCharacterSize(12);
        m_infoBarText.setFillColor(sf::Color(41, 182, 246)); // #29b6f6
        m_infoBarText.setPosition(95.f, 438.f);

        m_resetDefaultsBtnText.setFont(m_font);
        m_resetDefaultsBtnText.setString("Reset Defaults");
        m_resetDefaultsBtnText.setCharacterSize(12);
        m_resetDefaultsBtnText.setFillColor(sf::Color(253, 216, 53)); // #fdd835
        m_resetDefaultsBtnText.setStyle(sf::Text::Underlined | sf::Text::Bold);
        sf::FloatRect rb = m_resetDefaultsBtnText.getLocalBounds();
        m_resetDefaultsBtnText.setPosition(700.f - rb.width, 438.f);
        m_resetBtnBounds = m_resetDefaultsBtnText.getGlobalBounds();
    }

    // Init 3 setting toggles
    initToggle(m_soundToggle, "SFX", "Sound Effects", 125.f, SettingId::Sound);
    initToggle(m_musicToggle, "BGM", "Music", 215.f, SettingId::Music);
    initToggle(m_motionToggle, "FX", "Motion Effects", 305.f, SettingId::Motion);

    // Back Button (Wooden pixel style)
    bool backLoaded = m_backTexture.loadFromFile("assets/textures/back_text.png");
    if (backLoaded) {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, m_backTexture, []() {
            Game::instance().getStateMachine().popState();
        });
    } else {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, "< BACK", m_font, []() {
            Game::instance().getStateMachine().popState();
        });
        m_backBtn->setNormalColor(sf::Color(141, 110, 99)); // #8d6e63
        m_backBtn->setHoverColor(sf::Color(161, 136, 127));  // #a1887f
        m_backBtn->setClickColor(sf::Color(93, 64, 55));     // #5d4037
        m_backBtn->setTextColor(sf::Color(253, 216, 53));
    }
}

void SettingState::initToggle(SettingToggle& toggle, const std::string& iconStr, const std::string& labelStr,
                               float y, SettingId id)
{
    toggle.settingId = id;

    // Card background
    float cardHeight = (id == SettingId::Motion) ? 55.f : 78.f;
    toggle.cardBg.setSize(sf::Vector2f(630.f, cardHeight));
    toggle.cardBg.setPosition(85.f, y);
    toggle.cardBg.setFillColor(sf::Color(0, 0, 0, 140));
    toggle.cardBg.setOutlineColor(sf::Color::Black);
    toggle.cardBg.setOutlineThickness(2.f);

    // Icon / Prefix
    toggle.iconText.setFont(m_font);
    toggle.iconText.setString("[" + iconStr + "]");
    toggle.iconText.setCharacterSize(14);
    toggle.iconText.setFillColor(sf::Color(253, 216, 53));
    toggle.iconText.setStyle(sf::Text::Bold);
    toggle.iconText.setPosition(98.f, y + 12.f);

    // Label
    toggle.label.setFont(m_font);
    toggle.label.setString(labelStr);
    toggle.label.setCharacterSize(15);
    toggle.label.setFillColor(sf::Color::White);
    toggle.label.setStyle(sf::Text::Bold);
    toggle.label.setPosition(155.f, y + 12.f);

    // Retro Checkbox (right side)
    float checkSize = 32.f;
    toggle.checkbox.setSize(sf::Vector2f(checkSize, checkSize));
    toggle.checkbox.setPosition(670.f - checkSize, y + 10.f);
    toggle.checkbox.setOutlineColor(sf::Color::Black);
    toggle.checkbox.setOutlineThickness(2.f);

    // Status text
    toggle.statusText.setFont(m_font);
    toggle.statusText.setCharacterSize(13);
    toggle.statusText.setStyle(sf::Text::Bold);

    // Volume Slider/Indicator (Sound & Music)
    if (id != SettingId::Motion) {
        toggle.volumeLabel.setFont(m_font);
        toggle.volumeLabel.setString("Volume");
        toggle.volumeLabel.setCharacterSize(11);
        toggle.volumeLabel.setFillColor(sf::Color(180, 180, 180));
        toggle.volumeLabel.setPosition(155.f, y + 44.f);

        toggle.volumeBarBg.setSize(sf::Vector2f(380.f, 10.f));
        toggle.volumeBarBg.setPosition(225.f, y + 47.f);
        toggle.volumeBarBg.setFillColor(sf::Color(26, 26, 26));
        toggle.volumeBarBg.setOutlineColor(sf::Color::Black);
        toggle.volumeBarBg.setOutlineThickness(2.f);

        float pct = (id == SettingId::Sound) ? 0.80f : 0.65f;
        toggle.volumeBarFill.setSize(sf::Vector2f(380.f * pct, 10.f));
        toggle.volumeBarFill.setPosition(225.f, y + 47.f);
        toggle.volumeBarFill.setFillColor(sf::Color(253, 216, 53)); // #fdd835 yellow

        toggle.volumeValueText.setFont(m_font);
        toggle.volumeValueText.setString(std::to_string(static_cast<int>(pct * 100)) + "%");
        toggle.volumeValueText.setCharacterSize(11);
        toggle.volumeValueText.setFillColor(sf::Color(253, 216, 53));
        toggle.volumeValueText.setStyle(sf::Text::Bold);
        toggle.volumeValueText.setPosition(615.f, y + 44.f);
    }

    updateToggleVisual(toggle);
}

void SettingState::updateToggleVisual(SettingToggle& toggle) {
    bool value = getSettingValue(toggle.settingId);
    float checkSize = 32.f;
    float x = 670.f - checkSize;
    float y = toggle.cardBg.getPosition().y + 10.f;

    if (value) {
        toggle.checkbox.setFillColor(sf::Color(67, 160, 71));   // Green #43a047
        toggle.statusText.setFont(m_font);
        toggle.statusText.setString("v"); // Retro Checkmark
        toggle.statusText.setCharacterSize(18);
        toggle.statusText.setFillColor(sf::Color::White);
        sf::FloatRect sb = toggle.statusText.getLocalBounds();
        toggle.statusText.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
        toggle.statusText.setPosition(x + checkSize / 2.f, y + checkSize / 2.f);

        if (m_onLoaded) {
            toggle.statusSprite.setTexture(m_onTexture);
            sf::Vector2u size = m_onTexture.getSize();
            if (size.x > 0 && size.y > 0) {
                toggle.statusSprite.setScale(checkSize / static_cast<float>(size.x), checkSize / static_cast<float>(size.y));
            }
            toggle.statusSprite.setPosition(x, y);
        }
    } else {
        toggle.checkbox.setFillColor(sf::Color(229, 57, 53));   // Red #e53935
        toggle.statusText.setFont(m_font);
        toggle.statusText.setString("x");
        toggle.statusText.setCharacterSize(16);
        toggle.statusText.setFillColor(sf::Color::White);
        sf::FloatRect sb = toggle.statusText.getLocalBounds();
        toggle.statusText.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
        toggle.statusText.setPosition(x + checkSize / 2.f, y + checkSize / 2.f);

        if (m_offLoaded) {
            toggle.statusSprite.setTexture(m_offTexture);
            sf::Vector2u size = m_offTexture.getSize();
            if (size.x > 0 && size.y > 0) {
                toggle.statusSprite.setScale(checkSize / static_cast<float>(size.x), checkSize / static_cast<float>(size.y));
            }
            toggle.statusSprite.setPosition(x, y);
        }
    }

    // Dim volume bar if toggled off
    if (toggle.settingId != SettingId::Motion) {
        sf::Color fillCol = value ? sf::Color(253, 216, 53) : sf::Color(100, 100, 100);
        toggle.volumeBarFill.setFillColor(fillCol);
    }
}

void SettingState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (m_backBtn) m_backBtn->handleEvent(event, window);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

        auto tryToggle = [&](SettingToggle& toggle) {
            bool clicked = toggle.checkbox.getGlobalBounds().contains(mousePos) || toggle.cardBg.getGlobalBounds().contains(mousePos);
            if (clicked) {
                Game::instance().playSound("assets/audio/sfx_click.wav");
                toggleSetting(toggle.settingId);
                updateToggleVisual(toggle);
            }
        };

        tryToggle(m_soundToggle);
        tryToggle(m_musicToggle);
        tryToggle(m_motionToggle);

        if (m_resetBtnBounds.contains(mousePos)) {
            Game::instance().playSound("assets/audio/sfx_click.wav");
            resetDefaults();
        }
    }

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void SettingState::update(float dt) {
    if (m_backBtn) m_backBtn->update(dt);

    sf::Vector2f mousePos = Game::instance().getWindow().mapPixelToCoords(sf::Mouse::getPosition(Game::instance().getWindow()));

    auto updateHover = [&](SettingToggle& toggle) {
        toggle.hovered = toggle.checkbox.getGlobalBounds().contains(mousePos) || toggle.cardBg.getGlobalBounds().contains(mousePos);
        if (toggle.hovered) {
            toggle.checkbox.setOutlineColor(sf::Color(253, 216, 53)); // Yellow hover outline
        } else {
            toggle.checkbox.setOutlineColor(sf::Color::Black);
        }
    };

    updateHover(m_soundToggle);
    updateHover(m_musicToggle);
    updateHover(m_motionToggle);

    updateToggleVisual(m_soundToggle);
    updateToggleVisual(m_musicToggle);
    updateToggleVisual(m_motionToggle);
}

void SettingState::draw(sf::RenderWindow& window) {
    if (m_bgLoaded) {
        window.draw(m_bgSprite);
    } else {
        window.draw(m_background);
    }

    // Main window container
    window.draw(m_mainContainer);
    window.draw(m_topRibbon);
    window.draw(m_containerBorder);

    if (m_fontLoaded) {
        // Header
        window.draw(m_titleShadow);
        window.draw(m_titleText);
        window.draw(m_accentBar);
        window.draw(m_accentBarBorder);

        // Content Box
        window.draw(m_contentBox);

        // Helper lambda to draw a setting card
        auto drawToggle = [&](const SettingToggle& toggle) {
            window.draw(toggle.cardBg);
            window.draw(toggle.iconText);
            window.draw(toggle.label);

            if (m_onLoaded || m_offLoaded) {
                window.draw(toggle.statusSprite);
            } else {
                window.draw(toggle.checkbox);
                window.draw(toggle.statusText);
            }

            if (toggle.settingId != SettingId::Motion) {
                window.draw(toggle.volumeLabel);
                window.draw(toggle.volumeBarBg);
                window.draw(toggle.volumeBarFill);
                window.draw(toggle.volumeValueText);
            }
        };

        drawToggle(m_soundToggle);
        drawToggle(m_musicToggle);
        drawToggle(m_motionToggle);

        // Bottom Info Bar
        window.draw(m_infoBarBg);
        window.draw(m_infoBarText);
        window.draw(m_resetDefaultsBtnText);

        if (m_backBtn) window.draw(*m_backBtn);
    }
}
