#include "AboutState.h"
#include "../Core/Game.h"
#include <memory>

// ============================================================
// AboutState - Màn hình giới thiệu về game và nhóm phát triển
// ============================================================

void AboutState::init() {
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");

    // Nền tối gradient
    m_background.setSize(sf::Vector2f(800.f, 600.f));
    m_background.setFillColor(sf::Color(20, 20, 40));

    // Thanh trang trí phía trên
    m_topBar.setSize(sf::Vector2f(800.f, 5.f));
    m_topBar.setPosition(0.f, 0.f);
    m_topBar.setFillColor(sf::Color(100, 150, 255));

    // Tiêu đề
    m_titleText.setFont(m_font);
    m_titleText.setString("ABOUT");
    m_titleText.setCharacterSize(42);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                          titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(400.f, 50.f);

    // Logo placeholder
    m_logoBg.setSize(sf::Vector2f(80.f, 80.f));
    m_logoBg.setOrigin(40.f, 40.f);
    m_logoBg.setPosition(400.f, 140.f);
    m_logoBg.setFillColor(sf::Color(60, 80, 140));
    m_logoBg.setOutlineColor(sf::Color(100, 150, 255));
    m_logoBg.setOutlineThickness(2.f);

    m_logoText.setFont(m_font);
    m_logoText.setString("CR");
    m_logoText.setCharacterSize(32);
    m_logoText.setFillColor(sf::Color(255, 215, 0));
    m_logoText.setStyle(sf::Text::Bold);
    sf::FloatRect logoBounds = m_logoText.getLocalBounds();
    m_logoText.setOrigin(logoBounds.left + logoBounds.width / 2.f,
                          logoBounds.top + logoBounds.height / 2.f);
    m_logoText.setPosition(400.f, 140.f);

    // Nội dung giới thiệu - căn giữa từng dòng
    auto centerText = [this](sf::Text& text, const std::string& str,
                             unsigned int size, sf::Color color, float y)
    {
        text.setFont(m_font);
        text.setString(str);
        text.setCharacterSize(size);
        text.setFillColor(color);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f,
                       bounds.top + bounds.height / 2.f);
        text.setPosition(400.f, y);
    };

    centerText(m_line1, "Crossing Road - OOP Project",
               26, sf::Color(255, 215, 0), 230.f);

    centerText(m_line2, "Built with C++ & SFML 2.5",
               20, sf::Color(180, 200, 255), 280.f);

    centerText(m_line3, "Developed by: Group 8",
               22, sf::Color::White, 330.f);

    centerText(m_line5, "HCMUS",
               24, sf::Color(100, 200, 255), 380.f);

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

void AboutState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (m_backBtn) m_backBtn->handleEvent(event, window);

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void AboutState::update(float dt) {
    if (m_backBtn) m_backBtn->update(dt);
}

void AboutState::draw(sf::RenderWindow& window) {
    window.draw(m_background);
    window.draw(m_topBar);

    if (!m_fontLoaded) return;

    window.draw(m_titleText);

    // Logo placeholder
    window.draw(m_logoBg);
    window.draw(m_logoText);

    // Đường kẻ trang trí
    sf::RectangleShape divider(sf::Vector2f(300.f, 1.f));
    divider.setPosition(250.f, 195.f);
    divider.setFillColor(sf::Color(100, 150, 255, 80));
    window.draw(divider);

    // Nền nội dung
    sf::RectangleShape contentBg(sf::Vector2f(500.f, 220.f));
    contentBg.setOrigin(250.f, 0.f);
    contentBg.setPosition(400.f, 200.f);
    contentBg.setFillColor(sf::Color(30, 30, 55, 180));
    contentBg.setOutlineColor(sf::Color(60, 70, 100));
    contentBg.setOutlineThickness(1.f);
    window.draw(contentBg);

    // Nội dung
    window.draw(m_line1);
    window.draw(m_line2);
    window.draw(m_line3);
    window.draw(m_line5);

    // Đường kẻ dưới nội dung
    sf::RectangleShape bottomDivider(sf::Vector2f(300.f, 1.f));
    bottomDivider.setPosition(250.f, 435.f);
    bottomDivider.setFillColor(sf::Color(100, 150, 255, 80));
    window.draw(bottomDivider);

    // Nút BACK
    if (m_backBtn) window.draw(*m_backBtn);
}
