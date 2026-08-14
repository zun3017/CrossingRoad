#include "SettingState.h"
#include "../Core/Game.h"
#include "../Core/ResourceManager.h"
#include <memory>
#include <iostream>

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
    Game::instance().setSoundVolume(80.0f);
    Game::instance().setMusicEnabled(true);
    Game::instance().setMusicVolume(65.0f);
    Game::instance().setMotionEnabled(true);
    updateToggleVisual(m_soundToggle);
    updateToggleVisual(m_musicToggle);
    updateToggleVisual(m_motionToggle);
}

void SettingState::updateSliderFromMouse(SettingToggle& toggle, float mouseX) {
    if (toggle.settingId == SettingId::Motion) return;

    float barX = toggle.volumeBarBg.getPosition().x;
    float barW = toggle.volumeBarBg.getSize().x;
    float pct = (mouseX - barX) / barW;
    pct = std::max(0.0f, std::min(1.0f, pct));
    float vol = pct * 100.0f;

    if (toggle.settingId == SettingId::Sound) {
        Game::instance().setSoundVolume(vol);
        if (vol > 0.0f && !Game::instance().isSoundEnabled()) {
            Game::instance().setSoundEnabled(true);
        }
    } else if (toggle.settingId == SettingId::Music) {
        Game::instance().setMusicVolume(vol);
        if (vol > 0.0f && !Game::instance().isMusicEnabled()) {
            Game::instance().setMusicEnabled(true);
        }
    }
    updateToggleVisual(toggle);
}

void SettingState::init() {
    auto& font = FontManager::getInstance().get("assets/fonts/arial.ttf");
    m_font = font;
    m_fontLoaded = true;

    auto& onTex = TextureManager::getInstance().get("assets/textures/on_button.png");
    if (onTex.getSize().x > 0) {
        m_onLoaded = true;
        m_onTexture = onTex;
    }

    auto& offTex = TextureManager::getInstance().get("assets/textures/off_button.png");
    if (offTex.getSize().x > 0) {
        m_offLoaded = true;
        m_offTexture = offTex;
    }

    auto& bgTex = TextureManager::getInstance().get("assets/textures/menu.png");
    if (bgTex.getSize().x > 0) {
        m_bgLoaded = true;
        m_bgSprite.setTexture(bgTex);
        sf::Vector2u size = bgTex.getSize();
        m_bgSprite.setScale(
            800.f / static_cast<float>(size.x),
            600.f / static_cast<float>(size.y));
    } else {
        m_background.setSize(sf::Vector2f(800.f, 600.f));
        m_background.setFillColor(sf::Color(124, 179, 66));
    }

    m_mainContainer.setSize(sf::Vector2f(720.f, 540.f));
    m_mainContainer.setOrigin(360.f, 270.f);
    m_mainContainer.setPosition(400.f, 300.f);
    m_mainContainer.setFillColor(sf::Color(74, 144, 226, 242));

    m_containerBorder.setSize(sf::Vector2f(720.f, 540.f));
    m_containerBorder.setOrigin(360.f, 270.f);
    m_containerBorder.setPosition(400.f, 300.f);
    m_containerBorder.setFillColor(sf::Color::Transparent);
    m_containerBorder.setOutlineColor(sf::Color::Black);
    m_containerBorder.setOutlineThickness(4.f);

    m_topRibbon.setSize(sf::Vector2f(720.f, 8.f));
    m_topRibbon.setPosition(40.f, 30.f);
    m_topRibbon.setFillColor(sf::Color(255, 255, 255, 60));

    if (m_fontLoaded) {
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
        m_titleText.setFillColor(sf::Color(253, 216, 53));
        m_titleText.setStyle(sf::Text::Bold);
        m_titleText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleText.setPosition(400.f, 58.f);

        m_accentBar.setSize(sf::Vector2f(440.f, 6.f));
        m_accentBar.setOrigin(220.f, 0.f);
        m_accentBar.setPosition(400.f, 88.f);
        m_accentBar.setFillColor(sf::Color(253, 216, 53));

        m_accentBarBorder.setSize(sf::Vector2f(440.f, 3.f));
        m_accentBarBorder.setOrigin(220.f, 0.f);
        m_accentBarBorder.setPosition(400.f, 94.f);
        m_accentBarBorder.setFillColor(sf::Color::Black);

        m_contentBox.setSize(sf::Vector2f(660.f, 365.f));
        m_contentBox.setPosition(70.f, 110.f);
        m_contentBox.setFillColor(sf::Color(0, 0, 0, 180));
        m_contentBox.setOutlineColor(sf::Color::Black);
        m_contentBox.setOutlineThickness(3.f);

        m_infoBarBg.setSize(sf::Vector2f(630.f, 32.f));
        m_infoBarBg.setPosition(85.f, 430.f);
        m_infoBarBg.setFillColor(sf::Color(0, 0, 0, 140));
        m_infoBarBg.setOutlineColor(sf::Color::Black);
        m_infoBarBg.setOutlineThickness(2.f);

        m_infoBarText.setFont(m_font);
        m_infoBarText.setString("Auto-saves changes to local config");
        m_infoBarText.setCharacterSize(12);
        m_infoBarText.setFillColor(sf::Color(41, 182, 246));
        m_infoBarText.setPosition(95.f, 438.f);

        m_resetDefaultsBtnText.setFont(m_font);
        m_resetDefaultsBtnText.setString("Reset Defaults");
        m_resetDefaultsBtnText.setCharacterSize(12);
        m_resetDefaultsBtnText.setFillColor(sf::Color(253, 216, 53));
        m_resetDefaultsBtnText.setStyle(sf::Text::Underlined | sf::Text::Bold);
        sf::FloatRect rb = m_resetDefaultsBtnText.getLocalBounds();
        m_resetDefaultsBtnText.setPosition(700.f - rb.width, 438.f);
        m_resetBtnBounds = m_resetDefaultsBtnText.getGlobalBounds();
    }

    initToggle(m_soundToggle, "SFX", "Sound Effects", 125.f, SettingId::Sound);
    initToggle(m_musicToggle, "BGM", "Music", 215.f, SettingId::Music);
    initToggle(m_motionToggle, "FX", "Motion Effects", 305.f, SettingId::Motion);

    auto& backTex = TextureManager::getInstance().get("assets/textures/back_text.png");
    if (backTex.getSize().x > 0) {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, backTex, []() {
            Game::instance().getStateMachine().popState();
        });
    } else {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, "< BACK", m_font, []() {
            Game::instance().getStateMachine().popState();
        });
        m_backBtn->setNormalColor(sf::Color(141, 110, 99));
        m_backBtn->setHoverColor(sf::Color(161, 136, 127));
        m_backBtn->setClickColor(sf::Color(93, 64, 55));
        m_backBtn->setTextColor(sf::Color(253, 216, 53));
    }
}

void SettingState::initToggle(SettingToggle& toggle, const std::string& iconStr, const std::string& labelStr,
                               float y, SettingId id)
{
    toggle.settingId = id;

    float cardHeight = (id == SettingId::Motion) ? 55.f : 78.f;
    toggle.cardBg.setSize(sf::Vector2f(630.f, cardHeight));
    toggle.cardBg.setPosition(85.f, y);
    toggle.cardBg.setFillColor(sf::Color(0, 0, 0, 140));
    toggle.cardBg.setOutlineColor(sf::Color::Black);
    toggle.cardBg.setOutlineThickness(2.f);

    toggle.iconText.setFont(m_font);
    toggle.iconText.setString("[" + iconStr + "]");
    toggle.iconText.setCharacterSize(14);
    toggle.iconText.setFillColor(sf::Color(253, 216, 53));
    toggle.iconText.setStyle(sf::Text::Bold);
    toggle.iconText.setPosition(98.f, y + 12.f);

    toggle.label.setFont(m_font);
    toggle.label.setString(labelStr);
    toggle.label.setCharacterSize(15);
    toggle.label.setFillColor(sf::Color::White);
    toggle.label.setStyle(sf::Text::Bold);
    toggle.label.setPosition(155.f, y + 12.f);

    float checkSize = 32.f;
    toggle.checkbox.setSize(sf::Vector2f(checkSize, checkSize));
    toggle.checkbox.setPosition(670.f - checkSize, y + 10.f);
    toggle.checkbox.setOutlineColor(sf::Color::Black);
    toggle.checkbox.setOutlineThickness(2.f);

    toggle.statusText.setFont(m_font);
    toggle.statusText.setCharacterSize(13);
    toggle.statusText.setStyle(sf::Text::Bold);

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

        float vol = (id == SettingId::Sound) ? Game::instance().getSoundVolume() : Game::instance().getMusicVolume();
        float pct = vol / 100.0f;
        toggle.volumeBarFill.setSize(sf::Vector2f(380.f * pct, 10.f));
        toggle.volumeBarFill.setPosition(225.f, y + 47.f);
        toggle.volumeBarFill.setFillColor(sf::Color(253, 216, 53));

        toggle.sliderKnob.setRadius(8.f);
        toggle.sliderKnob.setOrigin(8.f, 8.f);
        toggle.sliderKnob.setPosition(225.f + 380.f * pct, y + 52.f);
        toggle.sliderKnob.setFillColor(sf::Color(255, 235, 59));
        toggle.sliderKnob.setOutlineColor(sf::Color::Black);
        toggle.sliderKnob.setOutlineThickness(2.f);

        toggle.volumeValueText.setFont(m_font);
        toggle.volumeValueText.setString(std::to_string(static_cast<int>(std::round(vol))) + "%");
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
        toggle.checkbox.setFillColor(sf::Color(67, 160, 71));
        toggle.statusText.setFont(m_font);
        toggle.statusText.setString("v");
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
        toggle.checkbox.setFillColor(sf::Color(229, 57, 53));
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

    if (toggle.settingId != SettingId::Motion) {
        float vol = (toggle.settingId == SettingId::Sound) ? Game::instance().getSoundVolume() : Game::instance().getMusicVolume();
        float pct = vol / 100.0f;
        float barX = toggle.volumeBarBg.getPosition().x;
        float barY = toggle.volumeBarBg.getPosition().y;
        float barW = toggle.volumeBarBg.getSize().x;

        toggle.volumeBarFill.setSize(sf::Vector2f(barW * pct, 10.f));
        toggle.sliderKnob.setPosition(barX + barW * pct, barY + 5.f);

        if (value) {
            toggle.volumeBarFill.setFillColor(sf::Color(253, 216, 53));
            toggle.sliderKnob.setFillColor(sf::Color(255, 235, 59));
            toggle.volumeValueText.setFillColor(sf::Color(253, 216, 53));
            toggle.volumeValueText.setString(std::to_string(static_cast<int>(std::round(vol))) + "%");
        } else {
            toggle.volumeBarFill.setFillColor(sf::Color(100, 100, 100));
            toggle.sliderKnob.setFillColor(sf::Color(120, 120, 120));
            toggle.volumeValueText.setFillColor(sf::Color(140, 140, 140));
            toggle.volumeValueText.setString("OFF");
        }
    }
}

void SettingState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (m_backBtn) m_backBtn->handleEvent(event, window);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

        auto checkVolumeSlider = [&](SettingToggle& toggle) -> bool {
            if (toggle.settingId == SettingId::Motion) return false;
            sf::FloatRect barBounds = toggle.volumeBarBg.getGlobalBounds();
            barBounds.left -= 12.f;
            barBounds.top -= 12.f;
            barBounds.width += 24.f;
            barBounds.height += 24.f;
            if (barBounds.contains(mousePos)) {
                m_activeSlider = &toggle;
                updateSliderFromMouse(toggle, mousePos.x);
                return true;
            }
            return false;
        };

        if (checkVolumeSlider(m_soundToggle) || checkVolumeSlider(m_musicToggle)) {
            return;
        }

        auto tryToggle = [&](SettingToggle& toggle) {
            sf::FloatRect checkBounds = toggle.checkbox.getGlobalBounds();
            checkBounds.left -= 10.f;
            checkBounds.top -= 10.f;
            checkBounds.width += 20.f;
            checkBounds.height += 20.f;

            sf::FloatRect headerBounds = toggle.cardBg.getGlobalBounds();
            headerBounds.height = 40.f;

            bool clicked = checkBounds.contains(mousePos) || headerBounds.contains(mousePos) ||
                           ((m_onLoaded || m_offLoaded) && toggle.statusSprite.getGlobalBounds().contains(mousePos));

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
    else if (event.type == sf::Event::MouseMoved) {
        if (m_activeSlider != nullptr && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            updateSliderFromMouse(*m_activeSlider, mousePos.x);
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased &&
             event.mouseButton.button == sf::Mouse::Left)
    {
        m_activeSlider = nullptr;
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
            toggle.checkbox.setOutlineColor(sf::Color(253, 216, 53));
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

    window.draw(m_mainContainer);
    window.draw(m_topRibbon);
    window.draw(m_containerBorder);

    if (m_fontLoaded) {
        window.draw(m_titleShadow);
        window.draw(m_titleText);
        window.draw(m_accentBar);
        window.draw(m_accentBarBorder);

        window.draw(m_contentBox);

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
                window.draw(toggle.sliderKnob);
                window.draw(toggle.volumeValueText);
            }
        };

        drawToggle(m_soundToggle);
        drawToggle(m_musicToggle);
        drawToggle(m_motionToggle);

        window.draw(m_infoBarBg);
        window.draw(m_infoBarText);
        window.draw(m_resetDefaultsBtnText);

        if (m_backBtn) window.draw(*m_backBtn);
    }
}
