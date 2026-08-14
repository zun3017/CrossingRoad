#include "HelpState.h"
#include "../Core/Game.h"
#include "../Core/ResourceManager.h"
#include <memory>
#include <vector>

void HelpState::init() {
    auto& font = FontManager::getInstance().get("assets/fonts/arial.ttf");
    m_font = font;
    m_fontLoaded = true;

    // Load menu.png background from cache
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
        // Title: HOW TO PLAY
        m_titleShadow.setFont(m_font);
        m_titleShadow.setString("HOW TO PLAY");
        m_titleShadow.setCharacterSize(36);
        m_titleShadow.setFillColor(sf::Color::Black);
        m_titleShadow.setStyle(sf::Text::Bold);
        sf::FloatRect tb = m_titleShadow.getLocalBounds();
        m_titleShadow.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleShadow.setPosition(403.f, 61.f);

        m_titleText.setFont(m_font);
        m_titleText.setString("HOW TO PLAY");
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
    }

    // Back Button (Wooden pixel style)
    auto& backTex = TextureManager::getInstance().get("assets/textures/back_text.png");
    if (backTex.getSize().x > 0) {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, backTex, []() {
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
    if (m_bgLoaded) {
        window.draw(m_bgSprite);
    } else {
        window.draw(m_background);
    }

    // Main window container
    window.draw(m_mainContainer);
    window.draw(m_topRibbon);
    window.draw(m_containerBorder);

    if (!m_fontLoaded) return;

    // Header
    window.draw(m_titleShadow);
    window.draw(m_titleText);
    window.draw(m_accentBar);
    window.draw(m_accentBarBorder);

    // Content Box
    window.draw(m_contentBox);

    // Helper lambda for section title
    auto drawSectionHeader = [this, &window](const std::string& title, float y) {
        sf::Text headerText;
        headerText.setFont(m_font);
        headerText.setString(title);
        headerText.setCharacterSize(16);
        headerText.setFillColor(sf::Color(253, 216, 53)); // #fdd835
        headerText.setStyle(sf::Text::Bold);
        headerText.setPosition(90.f, y);
        window.draw(headerText);
    };

    // --- SECTION 1: CONTROLS ---
    drawSectionHeader("CONTROLS", 124.f);

    sf::Text ctrl1, ctrl2;
    ctrl1.setFont(m_font);
    ctrl1.setString("Moveset 1:   W / A / S / D   to move");
    ctrl1.setCharacterSize(13);
    ctrl1.setFillColor(sf::Color(230, 230, 230));
    ctrl1.setPosition(90.f, 150.f);
    window.draw(ctrl1);

    ctrl2.setFont(m_font);
    ctrl2.setString("Moveset 2:   Arrow Keys   to move");
    ctrl2.setCharacterSize(13);
    ctrl2.setFillColor(sf::Color(230, 230, 230));
    ctrl2.setPosition(90.f, 175.f);
    window.draw(ctrl2);

    // Visual WASD Keys Box
    sf::RectangleShape keyBox(sf::Vector2f(106.f, 64.f));
    keyBox.setPosition(580.f, 130.f);
    keyBox.setFillColor(sf::Color(0, 0, 0, 100));
    keyBox.setOutlineColor(sf::Color::Black);
    keyBox.setOutlineThickness(2.f);
    window.draw(keyBox);

    auto drawKeyBtn = [this, &window](const std::string& label, float x, float y) {
        sf::RectangleShape key(sf::Vector2f(26.f, 26.f));
        key.setPosition(x, y);
        key.setFillColor(sf::Color(220, 220, 220));
        key.setOutlineColor(sf::Color::Black);
        key.setOutlineThickness(2.f);
        window.draw(key);

        sf::Text txt;
        txt.setFont(m_font);
        txt.setString(label);
        txt.setCharacterSize(12);
        txt.setFillColor(sf::Color::Black);
        txt.setStyle(sf::Text::Bold);
        sf::FloatRect kb = txt.getLocalBounds();
        txt.setOrigin(kb.left + kb.width / 2.f, kb.top + kb.height / 2.f);
        txt.setPosition(x + 13.f, y + 13.f);
        window.draw(txt);
    };

    drawKeyBtn("W", 620.f, 134.f);
    drawKeyBtn("A", 590.f, 163.f);
    drawKeyBtn("S", 620.f, 163.f);
    drawKeyBtn("D", 650.f, 163.f);

    // Section 1 Divider
    sf::RectangleShape div1(sf::Vector2f(620.f, 2.f));
    div1.setPosition(90.f, 204.f);
    div1.setFillColor(sf::Color(80, 80, 80));
    window.draw(div1);

    // --- SECTION 2: GAMEPLAY ---
    drawSectionHeader("GAMEPLAY", 212.f);

    auto drawBullet = [this, &window](const std::string& mainTxt, const std::string& subTxt, sf::Color subColor, float y) {
        sf::Text bulletText;
        bulletText.setFont(m_font);
        bulletText.setString("- " + mainTxt);
        bulletText.setCharacterSize(13);
        bulletText.setFillColor(sf::Color(230, 230, 230));
        bulletText.setPosition(100.f, y);
        window.draw(bulletText);

        if (!subTxt.empty()) {
            sf::Text noteText;
            noteText.setFont(m_font);
            noteText.setString(subTxt);
            noteText.setCharacterSize(12);
            noteText.setFillColor(subColor);
            noteText.setStyle(sf::Text::Bold);
            noteText.setPosition(120.f, y + 18.f);
            window.draw(noteText);
        }
    };

    drawBullet("Avoid vehicles on the roads!", "Getting hit means GAME OVER.", sf::Color(229, 57, 53), 236.f);
    drawBullet("Use wood logs to cross rivers.", "Falling into water means GAME OVER.", sf::Color(229, 57, 53), 276.f);
    drawBullet("Collect 'Superhero-figured' items for bonus points.", "", sf::Color(67, 160, 71), 316.f);
    drawBullet("Reach the top of the screen to advance to next level.", "", sf::Color::White, 338.f);

    // Section 2 Divider
    sf::RectangleShape div2(sf::Vector2f(620.f, 2.f));
    div2.setPosition(90.f, 362.f);
    div2.setFillColor(sf::Color(80, 80, 80));
    window.draw(div2);

    // --- SECTION 3: OTHER KEYS ---
    drawSectionHeader("OTHER KEYS", 370.f);

    auto drawShortcut = [this, &window](const std::string& keyStr, const std::string& descStr, float x, float y) {
        sf::Text kText;
        kText.setFont(m_font);
        kText.setString(keyStr);
        kText.setCharacterSize(13);
        kText.setFillColor(sf::Color(253, 216, 53)); // #fdd835
        kText.setStyle(sf::Text::Bold);
        kText.setPosition(x, y);
        window.draw(kText);

        sf::Text dText;
        dText.setFont(m_font);
        dText.setString("-  " + descStr);
        dText.setCharacterSize(13);
        dText.setFillColor(sf::Color(220, 220, 220));
        dText.setPosition(x + 95.f, y);
        window.draw(dText);
    };

    drawShortcut("P / Esc", "Pause the game", 100.f, 396.f);
    drawShortcut("S", "Save your progress", 400.f, 396.f);
    drawShortcut("R", "Restart (when game over)", 100.f, 422.f);
    drawShortcut("Q", "Quit to menu", 400.f, 422.f);

    // Back Button
    if (m_backBtn) window.draw(*m_backBtn);
}

