#include "HelpState.h"
#include "../Core/Game.h"
#include <memory>

// ============================================================
// HelpState - Màn hình hướng dẫn chơi game
// ============================================================

void HelpState::init() {
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");

    // Nền tối
    m_background.setSize(sf::Vector2f(800.f, 600.f));
    m_background.setFillColor(sf::Color(25, 30, 45));

    // Tiêu đề
    m_titleText.setFont(m_font);
    m_titleText.setString("HOW TO PLAY");
    m_titleText.setCharacterSize(42);
    m_titleText.setFillColor(sf::Color(100, 200, 255));
    m_titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                          titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(400.f, 50.f);

    // Nội dung hướng dẫn
    std::string helpContent =
        "  CONTROLS:\n"
        "    Moveset 1:  W / A / S / D  to move\n"
        "    Moveset 2:  Arrow Keys  to move\n"
        "\n"
        "  GAMEPLAY:\n"
        "    - Avoid vehicles on the roads!\n"
        "      Getting hit means GAME OVER.\n"
        "    - Use wood logs to cross rivers.\n"
        "      Falling into water means GAME OVER.\n"
        "    - Collect 'Superhero-themed' items for bonus points.\n"
        "    - Reach the top of the screen to addvance to the next level.\n"
        "\n"
        "  OTHER KEYS:\n"
        "    P / Escape  -  Pause the game\n"
        "    S           -  Save your progress\n"
        "    R           -  Restart (when game over)\n"
        "    Q           -  Quit to menu";

    m_contentText.setFont(m_font);
    m_contentText.setString(helpContent);
    m_contentText.setCharacterSize(16);
    m_contentText.setFillColor(sf::Color(220, 220, 220));
    m_contentText.setLineSpacing(1.1f);
    m_contentText.setPosition(100.f, 105.f);

    // Nút BACK bằng back_text.png
    bool backLoaded = m_backTexture.loadFromFile("assets/textures/back_text.png");
    if (backLoaded) {
        m_backBtn = std::make_unique<Button>(320.f, 530.f, 160.f, 45.f, m_backTexture, []() {
            Game::instance().getStateMachine().popState();
        });
    } else {
        m_backBtn = std::make_unique<Button>(320.f, 530.f, 160.f, 45.f, "BACK", m_font, []() {
            Game::instance().getStateMachine().popState();
        });
    }
}

void HelpState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (m_backBtn) m_backBtn->handleEvent(event, window);

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void HelpState::update(float dt) {
    if (m_backBtn) m_backBtn->update(dt);
}

void HelpState::draw(sf::RenderWindow& window) {
    window.draw(m_background);

    if (!m_fontLoaded) return;

    window.draw(m_titleText);

    // Đường kẻ trang trí dưới tiêu đề
    sf::RectangleShape divider(sf::Vector2f(400.f, 2.f));
    divider.setPosition(200.f, 85.f);
    divider.setFillColor(sf::Color(100, 200, 255, 80));
    window.draw(divider);

    // Nền cho phần nội dung
    sf::RectangleShape contentBg(sf::Vector2f(620.f, 430.f));
    contentBg.setPosition(90.f, 95.f);
    contentBg.setFillColor(sf::Color(35, 40, 55, 200));
    contentBg.setOutlineColor(sf::Color(60, 70, 100));
    contentBg.setOutlineThickness(1.f);
    window.draw(contentBg);

    window.draw(m_contentText);

    // Biểu tượng phím nhỏ trang trí
    // Phím W
    sf::RectangleShape keyIcon(sf::Vector2f(25.f, 25.f));
    keyIcon.setFillColor(sf::Color(80, 80, 120));
    keyIcon.setOutlineColor(sf::Color(140, 140, 180));
    keyIcon.setOutlineThickness(1.f);

    // Vẽ minh họa phím WASD ở góc phải
    float iconX = 610.f, iconY = 130.f;
    // W
    keyIcon.setPosition(iconX + 27.f, iconY);
    window.draw(keyIcon);
    sf::Text keyW;
    keyW.setFont(m_font);
    keyW.setString("W");
    keyW.setCharacterSize(14);
    keyW.setFillColor(sf::Color::White);
    keyW.setPosition(iconX + 33.f, iconY + 3.f);
    window.draw(keyW);

    // A
    keyIcon.setPosition(iconX, iconY + 28.f);
    window.draw(keyIcon);
    sf::Text keyA;
    keyA.setFont(m_font);
    keyA.setString("A");
    keyA.setCharacterSize(14);
    keyA.setFillColor(sf::Color::White);
    keyA.setPosition(iconX + 7.f, iconY + 31.f);
    window.draw(keyA);

    // S
    keyIcon.setPosition(iconX + 27.f, iconY + 28.f);
    window.draw(keyIcon);
    sf::Text keyS;
    keyS.setFont(m_font);
    keyS.setString("S");
    keyS.setCharacterSize(14);
    keyS.setFillColor(sf::Color::White);
    keyS.setPosition(iconX + 34.f, iconY + 31.f);
    window.draw(keyS);

    // D
    keyIcon.setPosition(iconX + 54.f, iconY + 28.f);
    window.draw(keyIcon);
    sf::Text keyD;
    keyD.setFont(m_font);
    keyD.setString("D");
    keyD.setCharacterSize(14);
    keyD.setFillColor(sf::Color::White);
    keyD.setPosition(iconX + 61.f, iconY + 31.f);
    window.draw(keyD);

    if (m_backBtn) window.draw(*m_backBtn);
}
