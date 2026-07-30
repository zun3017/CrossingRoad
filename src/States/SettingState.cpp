#include "SettingState.h"
#include "../Core/Game.h"
#include <memory>

// ============================================================
// SettingState - Màn hình cài đặt game
// ============================================================

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

void SettingState::init() {
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");
    m_onLoaded = m_onTexture.loadFromFile("assets/textures/on_button.png");
    m_offLoaded = m_offTexture.loadFromFile("assets/textures/off_button.png");

    // Nền xám đậm
    m_background.setSize(sf::Vector2f(800.f, 600.f));
    m_background.setFillColor(sf::Color(40, 40, 50));

    // Tiêu đề
    m_titleText.setFont(m_font);
    m_titleText.setString("SETTINGS");
    m_titleText.setCharacterSize(42);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                          titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(400.f, 60.f);

    // Khởi tạo các toggle
    initToggle(m_soundToggle, "Sound Effects", 180.f, SettingId::Sound);
    initToggle(m_musicToggle, "Music", 260.f, SettingId::Music);
    initToggle(m_motionToggle, "Motion Effects", 340.f, SettingId::Motion);

    // Nút BACK bằng back_text.png
    bool backLoaded = m_backTexture.loadFromFile("assets/textures/back_text.png");
    if (backLoaded) {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, m_backTexture, []() {
            Game::instance().getStateMachine().popState();
        });
    } else {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, "BACK", m_font, []() {
            Game::instance().getStateMachine().popState();
        });
    }
}

void SettingState::initToggle(SettingToggle& toggle, const std::string& labelStr,
                               float y, SettingId id)
{
    toggle.settingId = id;

    // Label bên trái
    toggle.label.setFont(m_font);
    toggle.label.setString(labelStr);
    toggle.label.setCharacterSize(24);
    toggle.label.setFillColor(sf::Color::White);
    toggle.label.setPosition(200.f, y);

    // Checkbox bên phải
    float checkSize = 35.f;
    toggle.checkbox.setSize(sf::Vector2f(checkSize, checkSize));
    toggle.checkbox.setPosition(550.f, y);
    toggle.checkbox.setOutlineColor(sf::Color::White);
    toggle.checkbox.setOutlineThickness(2.f);

    // Status text
    toggle.statusText.setFont(m_font);
    toggle.statusText.setCharacterSize(16);
    toggle.statusText.setPosition(595.f, y + 8.f);

    updateToggleVisual(toggle);
}

void SettingState::updateToggleVisual(SettingToggle& toggle) {
    bool value = getSettingValue(toggle.settingId);
    if (value) {
        toggle.checkbox.setFillColor(sf::Color(0, 180, 0));   // Xanh lá = bật
        toggle.statusText.setString("ON");
        toggle.statusText.setFillColor(sf::Color(0, 200, 0));
        if (m_onLoaded) {
            toggle.statusSprite.setTexture(m_onTexture);
            sf::Vector2u size = m_onTexture.getSize();
            if (size.x > 0 && size.y > 0) {
                // Tỷ lệ vuông hơn (35x35 px)
                toggle.statusSprite.setScale(35.f / static_cast<float>(size.x), 35.f / static_cast<float>(size.y));
            }
            toggle.statusSprite.setPosition(550.f, toggle.label.getPosition().y);
        }
    } else {
        toggle.checkbox.setFillColor(sf::Color(180, 0, 0));   // Đỏ = tắt
        toggle.statusText.setString("OFF");
        toggle.statusText.setFillColor(sf::Color(200, 0, 0));
        if (m_offLoaded) {
            toggle.statusSprite.setTexture(m_offTexture);
            sf::Vector2u size = m_offTexture.getSize();
            if (size.x > 0 && size.y > 0) {
                // Tỷ lệ vuông hơn (35x35 px)
                toggle.statusSprite.setScale(35.f / static_cast<float>(size.x), 35.f / static_cast<float>(size.y));
            }
            toggle.statusSprite.setPosition(550.f, toggle.label.getPosition().y);
        }
    }
}

void SettingState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (m_backBtn) m_backBtn->handleEvent(event, window);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

        // Kiểm tra click vào checkbox hoặc statusSprite
        auto tryToggle = [&](SettingToggle& toggle) {
            bool clicked = toggle.checkbox.getGlobalBounds().contains(mousePos);
            if ((m_onLoaded || m_offLoaded) && toggle.statusSprite.getGlobalBounds().contains(mousePos)) {
                clicked = true;
            }
            if (clicked) {
                toggleSetting(toggle.settingId);
                updateToggleVisual(toggle);
            }
        };

        tryToggle(m_soundToggle);
        tryToggle(m_musicToggle);
        tryToggle(m_motionToggle);
    }
}

void SettingState::update(float dt) {
    if (m_backBtn) m_backBtn->update(dt);

    sf::Vector2f mousePos = Game::instance().getWindow().mapPixelToCoords(sf::Mouse::getPosition(Game::instance().getWindow()));

    // Cập nhật hover cho checkbox
    auto updateHover = [&](SettingToggle& toggle) {
        toggle.hovered = toggle.checkbox.getGlobalBounds().contains(mousePos) ||
                         ((m_onLoaded || m_offLoaded) && toggle.statusSprite.getGlobalBounds().contains(mousePos));
        if (toggle.hovered) {
            toggle.checkbox.setOutlineColor(sf::Color(255, 215, 0)); // Viền vàng khi hover
        } else {
            toggle.checkbox.setOutlineColor(sf::Color::White);
        }
    };

    updateHover(m_soundToggle);
    updateHover(m_musicToggle);
    updateHover(m_motionToggle);

    // Cập nhật visual để phản ánh trạng thái hiện tại
    updateToggleVisual(m_soundToggle);
    updateToggleVisual(m_musicToggle);
    updateToggleVisual(m_motionToggle);
}

void SettingState::draw(sf::RenderWindow& window) {
    window.draw(m_background);

    if (m_fontLoaded) {
        window.draw(m_titleText);

        // Vẽ đường kẻ trang trí dưới tiêu đề
        sf::RectangleShape divider(sf::Vector2f(400.f, 2.f));
        divider.setPosition(200.f, 105.f);
        divider.setFillColor(sf::Color(100, 100, 140));
        window.draw(divider);

        auto drawToggle = [&](const SettingToggle& toggle) {
            window.draw(toggle.label);
            if (m_onLoaded || m_offLoaded) {
                window.draw(toggle.statusSprite);
            } else {
                window.draw(toggle.checkbox);
                window.draw(toggle.statusText);
            }
        };

        drawToggle(m_soundToggle);
        drawToggle(m_musicToggle);
        drawToggle(m_motionToggle);

        if (m_backBtn) window.draw(*m_backBtn);
    }
}
