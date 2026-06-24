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

    // Nút BACK
    float backW = 160.f, backH = 45.f;
    m_backBtnBg.setSize(sf::Vector2f(backW, backH));
    m_backBtnBg.setOrigin(backW / 2.f, backH / 2.f);
    m_backBtnBg.setPosition(400.f, 500.f);
    m_backBtnBg.setFillColor(sf::Color(80, 80, 100));
    m_backBtnBg.setOutlineColor(sf::Color(120, 120, 160));
    m_backBtnBg.setOutlineThickness(2.f);

    m_backBtnText.setFont(m_font);
    m_backBtnText.setString("BACK");
    m_backBtnText.setCharacterSize(22);
    m_backBtnText.setFillColor(sf::Color::White);
    sf::FloatRect backBounds = m_backBtnText.getLocalBounds();
    m_backBtnText.setOrigin(backBounds.left + backBounds.width / 2.f,
                             backBounds.top + backBounds.height / 2.f);
    m_backBtnText.setPosition(400.f, 500.f);
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
    } else {
        toggle.checkbox.setFillColor(sf::Color(180, 0, 0));   // Đỏ = tắt
        toggle.statusText.setString("OFF");
        toggle.statusText.setFillColor(sf::Color(200, 0, 0));
    }
}

void SettingState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                              static_cast<float>(event.mouseButton.y));

        // Kiểm tra click vào checkbox
        auto tryToggle = [&](SettingToggle& toggle) {
            if (toggle.checkbox.getGlobalBounds().contains(mousePos)) {
                toggleSetting(toggle.settingId);
                updateToggleVisual(toggle);
            }
        };

        tryToggle(m_soundToggle);
        tryToggle(m_musicToggle);
        tryToggle(m_motionToggle);

        // Nút BACK
        if (m_backBtnBg.getGlobalBounds().contains(mousePos)) {
            Game::instance().getStateMachine().popState();
        }
    }
}

void SettingState::update(float dt) {
    // Cập nhật hover cho nút BACK
    sf::Vector2i mousePixel = sf::Mouse::getPosition(Game::instance().getWindow());
    sf::Vector2f mousePos(static_cast<float>(mousePixel.x),
                          static_cast<float>(mousePixel.y));

    m_backHovered = m_backBtnBg.getGlobalBounds().contains(mousePos);
    m_backBtnBg.setFillColor(m_backHovered
        ? sf::Color(120, 120, 180)
        : sf::Color(80, 80, 100));

    // Cập nhật hover cho checkbox
    auto updateHover = [&](SettingToggle& toggle) {
        toggle.hovered = toggle.checkbox.getGlobalBounds().contains(mousePos);
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

        // Vẽ toggle Sound
        window.draw(m_soundToggle.label);
        window.draw(m_soundToggle.checkbox);
        window.draw(m_soundToggle.statusText);

        // Vẽ toggle Music
        window.draw(m_musicToggle.label);
        window.draw(m_musicToggle.checkbox);
        window.draw(m_musicToggle.statusText);

        // Vẽ toggle Motion
        window.draw(m_motionToggle.label);
        window.draw(m_motionToggle.checkbox);
        window.draw(m_motionToggle.statusText);

        // Vẽ nút BACK
        window.draw(m_backBtnBg);
        window.draw(m_backBtnText);
    }
}
