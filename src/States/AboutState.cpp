#include "AboutState.h"
#include "../Core/Game.h"
#include "../Core/ResourceManager.h"
#include <memory>

void AboutState::init()
{
    auto &font = FontManager::getInstance().get("assets/fonts/arial.ttf");
    m_font = font;
    m_fontLoaded = true;

    // Load menu.png background from cache
    auto &bgTex = TextureManager::getInstance().get("assets/textures/menu.png");
    if (bgTex.getSize().x > 0)
    {
        m_bgLoaded = true;
        m_bgSprite.setTexture(bgTex);
        sf::Vector2u size = bgTex.getSize();
        m_bgSprite.setScale(
            800.f / static_cast<float>(size.x),
            600.f / static_cast<float>(size.y));
    }
    else
    {
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

    if (m_fontLoaded)
    {
        // Title: ABOUT
        m_titleShadow.setFont(m_font);
        m_titleShadow.setString("ABOUT");
        m_titleShadow.setCharacterSize(38);
        m_titleShadow.setFillColor(sf::Color::Black);
        m_titleShadow.setStyle(sf::Text::Bold);
        sf::FloatRect tb = m_titleShadow.getLocalBounds();
        m_titleShadow.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleShadow.setPosition(403.f, 63.f);

        m_titleText.setFont(m_font);
        m_titleText.setString("ABOUT");
        m_titleText.setCharacterSize(38);
        m_titleText.setFillColor(sf::Color(253, 216, 53)); // #fdd835 yellow
        m_titleText.setStyle(sf::Text::Bold);
        m_titleText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleText.setPosition(400.f, 60.f);

        // Accent Bar
        m_accentBar.setSize(sf::Vector2f(400.f, 6.f));
        m_accentBar.setOrigin(200.f, 0.f);
        m_accentBar.setPosition(400.f, 92.f);
        m_accentBar.setFillColor(sf::Color(253, 216, 53));

        m_accentBarBorder.setSize(sf::Vector2f(400.f, 3.f));
        m_accentBarBorder.setOrigin(200.f, 0.f);
        m_accentBarBorder.setPosition(400.f, 98.f);
        m_accentBarBorder.setFillColor(sf::Color::Black);

        // Game Info Box
        m_infoBox.setSize(sf::Vector2f(660.f, 120.f));
        m_infoBox.setPosition(70.f, 115.f);
        m_infoBox.setFillColor(sf::Color(0, 0, 0, 160));
        m_infoBox.setOutlineColor(sf::Color::Black);
        m_infoBox.setOutlineThickness(3.f);

        // "CR" Badge
        m_crBadge.setSize(sf::Vector2f(54.f, 54.f));
        m_crBadge.setPosition(88.f, 130.f);
        m_crBadge.setFillColor(sf::Color(74, 144, 226));
        m_crBadge.setOutlineColor(sf::Color::Black);
        m_crBadge.setOutlineThickness(3.f);

        m_crBadgeText.setFont(m_font);
        m_crBadgeText.setString("CR");
        m_crBadgeText.setCharacterSize(26);
        m_crBadgeText.setFillColor(sf::Color(253, 216, 53));
        m_crBadgeText.setStyle(sf::Text::Bold);
        sf::FloatRect crb = m_crBadgeText.getLocalBounds();
        m_crBadgeText.setOrigin(crb.left + crb.width / 2.f, crb.top + crb.height / 2.f);
        m_crBadgeText.setPosition(115.f, 157.f);

        // Game Title
        m_gameTitle.setFont(m_font);
        m_gameTitle.setString("Crossing Road - OOP Project");
        m_gameTitle.setCharacterSize(20);
        m_gameTitle.setFillColor(sf::Color(253, 216, 53));
        m_gameTitle.setStyle(sf::Text::Bold);
        m_gameTitle.setPosition(158.f, 128.f);

        // Subtitle
        m_gameSubtitle.setFont(m_font);
        m_gameSubtitle.setString("Built with C++ & SFML 2.5");
        m_gameSubtitle.setCharacterSize(14);
        m_gameSubtitle.setFillColor(sf::Color(41, 182, 246)); // #29b6f6
        m_gameSubtitle.setPosition(158.f, 156.f);

        // School Tag
        m_schoolTagBg.setSize(sf::Vector2f(270.f, 24.f));
        m_schoolTagBg.setPosition(158.f, 182.f);
        m_schoolTagBg.setFillColor(sf::Color(93, 64, 55, 200)); // #5d4037
        m_schoolTagBg.setOutlineColor(sf::Color::Black);
        m_schoolTagBg.setOutlineThickness(2.f);

        m_schoolTagText.setFont(m_font);
        m_schoolTagText.setString("HCMUS - University of Science");
        m_schoolTagText.setCharacterSize(12);
        m_schoolTagText.setFillColor(sf::Color(254, 240, 138));
        m_schoolTagText.setPosition(166.f, 185.f);

        // Developed By Header & Badge
        m_devHeader.setFont(m_font);
        m_devHeader.setString("DEVELOPED BY: GROUP 08");
        m_devHeader.setCharacterSize(17);
        m_devHeader.setFillColor(sf::Color(253, 216, 53));
        m_devHeader.setStyle(sf::Text::Bold);
        m_devHeader.setPosition(70.f, 252.f);

        m_memberCountBadge.setFont(m_font);
        m_memberCountBadge.setString("5 MEMBERS");
        m_memberCountBadge.setCharacterSize(13);
        m_memberCountBadge.setFillColor(sf::Color(67, 160, 71)); // #43a047
        m_memberCountBadge.setStyle(sf::Text::Bold);
        m_memberCountBadge.setPosition(610.f, 254.f);
    }

    // Initialize 5 team members data
    m_members = {
        {"01", "Truong Tan Phuc", "ID: 25127467", sf::Color(67, 160, 71)},        // Green
        {"02", "Nguyen Hoang Danh", "ID: 25127031", sf::Color(41, 182, 246)},     // Blue
        {"03", "Pham Thanh Lam", "ID: 20127086", sf::Color(253, 216, 53)},        // Yellow
        {"04", "Ho Hoang Tien Dung", "ID: 25127304", sf::Color(229, 57, 53)},     // Red
        {"05", "Nguyen Bach Thanh Nhan", "ID: 25127111", sf::Color(168, 85, 247)} // Purple
    };

    // Back Button (Wooden pixel style)
    auto &backTex = TextureManager::getInstance().get("assets/textures/back_text.png");
    if (backTex.getSize().x > 0)
    {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, backTex, []()
                                             { Game::instance().getStateMachine().popState(); });
    }
    else
    {
        m_backBtn = std::make_unique<Button>(320.f, 490.f, 160.f, 45.f, "< BACK", m_font, []()
                                             { Game::instance().getStateMachine().popState(); });
        m_backBtn->setNormalColor(sf::Color(141, 110, 99)); // #8d6e63
        m_backBtn->setHoverColor(sf::Color(161, 136, 127)); // #a1887f
        m_backBtn->setClickColor(sf::Color(93, 64, 55));    // #5d4037
        m_backBtn->setTextColor(sf::Color(253, 216, 53));
    }
}

void AboutState::handleInput(sf::RenderWindow &window, sf::Event &event)
{
    if (m_backBtn)
        m_backBtn->handleEvent(event, window);

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape)
    {
        Game::instance().getStateMachine().popState();
    }
}

void AboutState::update(float dt)
{
    if (m_backBtn)
        m_backBtn->update(dt);
}

void AboutState::draw(sf::RenderWindow &window)
{
    if (m_bgLoaded)
    {
        window.draw(m_bgSprite);
    }
    else
    {
        window.draw(m_background);
    }

    // Main window container
    window.draw(m_mainContainer);
    window.draw(m_topRibbon);
    window.draw(m_containerBorder);

    if (!m_fontLoaded)
        return;

    // Header
    window.draw(m_titleShadow);
    window.draw(m_titleText);
    window.draw(m_accentBar);
    window.draw(m_accentBarBorder);

    // Game Info Box
    window.draw(m_infoBox);
    window.draw(m_crBadge);
    window.draw(m_crBadgeText);
    window.draw(m_gameTitle);
    window.draw(m_gameSubtitle);
    window.draw(m_schoolTagBg);
    window.draw(m_schoolTagText);

    // Section Header
    window.draw(m_devHeader);
    window.draw(m_memberCountBadge);

    sf::RectangleShape sectionDivider(sf::Vector2f(660.f, 2.f));
    sectionDivider.setPosition(70.f, 278.f);
    sectionDivider.setFillColor(sf::Color(100, 100, 100));
    window.draw(sectionDivider);

    // Render Member Cards Grid (2x2 + 1 bottom centered)
    float cardW = 320.f;
    float cardH = 48.f;
    float startY = 290.f;

    for (size_t i = 0; i < m_members.size(); ++i)
    {
        float x, y, width;
        if (i < 4)
        {
            x = (i % 2 == 0) ? 70.f : 410.f;
            y = startY + static_cast<float>(i / 2) * 56.f;
            width = cardW;
        }
        else
        {
            x = 70.f;
            y = startY + 2.f * 56.f;
            width = 660.f;
        }

        // Card Container
        sf::RectangleShape card(sf::Vector2f(width, cardH));
        card.setPosition(x, y);
        card.setFillColor(sf::Color(0, 0, 0, 140));
        card.setOutlineColor(sf::Color::Black);
        card.setOutlineThickness(2.f);
        window.draw(card);

        // Number Badge
        sf::RectangleShape numBadge(sf::Vector2f(32.f, 32.f));
        numBadge.setPosition(x + 8.f, y + 8.f);
        numBadge.setFillColor(m_members[i].badgeColor);
        numBadge.setOutlineColor(sf::Color::Black);
        numBadge.setOutlineThickness(2.f);
        window.draw(numBadge);

        // Number Text
        sf::Text numText;
        numText.setFont(m_font);
        numText.setString(m_members[i].num);
        numText.setCharacterSize(13);
        numText.setFillColor((m_members[i].badgeColor == sf::Color(253, 216, 53) || m_members[i].badgeColor == sf::Color(41, 182, 246)) ? sf::Color::Black : sf::Color::White);
        numText.setStyle(sf::Text::Bold);
        sf::FloatRect nb = numText.getLocalBounds();
        numText.setOrigin(nb.left + nb.width / 2.f, nb.top + nb.height / 2.f);
        numText.setPosition(x + 24.f, y + 24.f);
        window.draw(numText);

        // Member Name
        sf::Text nameText;
        nameText.setFont(m_font);
        nameText.setString(m_members[i].name);
        nameText.setCharacterSize(14);
        nameText.setFillColor(sf::Color::White);
        nameText.setStyle(sf::Text::Bold);
        nameText.setPosition(x + 48.f, y + 6.f);
        window.draw(nameText);

        // Member ID
        sf::Text idText;
        idText.setFont(m_font);
        idText.setString(m_members[i].id);
        idText.setCharacterSize(12);
        idText.setFillColor(sf::Color(253, 216, 53)); // #fdd835
        idText.setPosition(x + 48.f, y + 26.f);
        window.draw(idText);
    }

    // Back Button
    if (m_backBtn)
        window.draw(*m_backBtn);
}
