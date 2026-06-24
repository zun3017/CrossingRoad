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
        "    Player 1:  W / A / S / D  to move\n"
        "    Player 2:  Arrow Keys  to move\n"
        "\n"
        "  GAMEPLAY:\n"
        "    - Avoid vehicles on the roads!\n"
        "      Getting hit means GAME OVER.\n"
        "    - Use lilypads to cross rivers.\n"
        "      Falling into water means GAME OVER.\n"
        "    - Collect gold items for bonus points.\n"
        "    - Reach the top of the screen to advance to the next level.\n"
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

    // Nút BACK
    float backW = 160.f, backH = 45.f;
    m_backBtnBg.setSize(sf::Vector2f(backW, backH));
    m_backBtnBg.setOrigin(backW / 2.f, backH / 2.f);
    m_backBtnBg.setPosition(400.f, 550.f);
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
    m_backBtnText.setPosition(400.f, 550.f);
}

void HelpState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                              static_cast<float>(event.mouseButton.y));

        if (m_backBtnBg.getGlobalBounds().contains(mousePos)) {
            Game::instance().getStateMachine().popState();
        }
    }

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void HelpState::update(float dt) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(Game::instance().getWindow());
    sf::Vector2f mousePos(static_cast<float>(mousePixel.x),
                          static_cast<float>(mousePixel.y));

    m_backHovered = m_backBtnBg.getGlobalBounds().contains(mousePos);
    m_backBtnBg.setFillColor(m_backHovered
        ? sf::Color(120, 120, 180)
        : sf::Color(80, 80, 100));
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

    window.draw(m_backBtnBg);
    window.draw(m_backBtnText);
}
