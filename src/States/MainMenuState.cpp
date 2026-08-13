#include "MainMenuState.h"

#include "../Core/Game.h"
#include "GameState.h"
#include "LoadGameState.h"
#include "RankingState.h"
#include "HelpState.h"
#include "AboutState.h"
#include "SettingState.h"

#include <memory>

// ============================================================
// MainMenuState - Main Menu
// ============================================================

void MainMenuState::init()
{
    // Phát nhạc nền Menu
    Game::instance().playBackgroundMusic("assets/audio/bgm_menu.ogg");

    // ========================================================
    // LOAD FONT
    // ========================================================

    m_fontLoaded = m_font.loadFromFile(
        "assets/fonts/arial.ttf");

    if (!m_fontLoaded)
    {
        // Không crash game nếu font không load được
    }

    // ========================================================
    // LOAD BACKGROUND
    // ========================================================

    // Attempt to load video frames (video_000.jpg to video_034.jpg)
    for (int i = 0; i <= 34; ++i) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "assets/textures/menu_video/video_%03d.jpg", i);
        std::string filename = buffer;
        sf::Texture tex;
        if (tex.loadFromFile(filename)) {
            m_menuVideoTextures.push_back(tex);
        } else {
            filename = "CrossingRoad/" + filename;
            if (tex.loadFromFile(filename)) {
                m_menuVideoTextures.push_back(tex);
            }
        }
    }

    if (!m_menuVideoTextures.empty()) {
        m_menuBgLoaded = true;
        m_menuBgSprite.setTexture(m_menuVideoTextures[0]);
        sf::Vector2u size = m_menuVideoTextures[0].getSize();
        if (size.x > 0 && size.y > 0) {
            m_menuBgSprite.setScale(800.f / static_cast<float>(size.x), 600.f / static_cast<float>(size.y));
            m_menuBgSprite.setOrigin(0.f, 0.f);
            m_menuBgSprite.setPosition(0.f, 0.f);
        }
    } else {
        // Fallback to static menu.png
        m_menuBgLoaded = m_menuBgTexture.loadFromFile("assets/textures/menu.png");
        if (!m_menuBgLoaded) {
            m_menuBgLoaded = m_menuBgTexture.loadFromFile("CrossingRoad/assets/textures/menu.png");
        }
        if (m_menuBgLoaded) {
            m_menuBgSprite.setTexture(m_menuBgTexture);
            sf::Vector2u size = m_menuBgTexture.getSize();
            if (size.x > 0 && size.y > 0) {
                m_menuBgSprite.setScale(800.f / static_cast<float>(size.x), 600.f / static_cast<float>(size.y));
                m_menuBgSprite.setOrigin(0.f, 0.f);
                m_menuBgSprite.setPosition(0.f, 0.f);
            }
        }
    }

    // ========================================================
    // LOAD CLOUD
    // ========================================================

    m_cloudLoaded = m_cloudTexture.loadFromFile(
        "assets/textures/cloud.png");

    // ========================================================
    // LOAD 7 BUTTON SPRITE SHEETS
    //
    // 0 = PLAY
    // 1 = LOAD
    // 2 = RANK
    // 3 = HELP
    // 4 = CREDIT
    // 5 = SETTINGS
    // 6 = EXIT
    // ========================================================

    const char *buttonFiles[BUTTON_COUNT] =
        {
            "assets/textures/play.png",
            "assets/textures/load.png",
            "assets/textures/rank.png",
            "assets/textures/help.png",
            "assets/textures/credit.png",
            "assets/textures/settings.png",
            "assets/textures/exit.png"};

    const char *textFiles[BUTTON_COUNT] =
        {
            "assets/textures/play_text.png",
            "assets/textures/load_text.png",
            "assets/textures/rank_text.png",
            "assets/textures/help_text.png",
            "assets/textures/credit_text.png",
            "assets/textures/settings_text.png",
            "assets/textures/exit_text.png"};

    for (int i = 0; i < BUTTON_COUNT; ++i)
    {
        m_buttonLoaded[i] =
            m_buttonTextures[i].loadFromFile(buttonFiles[i]);

        m_hoverTextLoaded[i] = m_hoverTextTextures[i].loadFromFile(textFiles[i]);

        if (!m_buttonLoaded[i])
        {
            // Không crash game nếu một texture bị lỗi.
            // Button đó sẽ không được vẽ.
        }
    }

    // ========================================================
    // BACKGROUND
    // ========================================================

    initBackground();

    // ========================================================
    // CLOUDS
    // ========================================================

    initClouds();

    // ========================================================
    // BUTTONS
    // ========================================================

    initButtons();

    // ========================================================
    // TITLE
    // ========================================================

    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("CROSSING ROAD");
        m_titleText.setCharacterSize(48);
        m_titleText.setFillColor(
            sf::Color(255, 215, 0));
        m_titleText.setStyle(sf::Text::Bold);

        sf::FloatRect titleBounds =
            m_titleText.getLocalBounds();

        m_titleText.setOrigin(
            titleBounds.left + titleBounds.width / 2.f,
            titleBounds.top + titleBounds.height / 2.f);

        m_titleText.setPosition(
            400.f,
            60.f);

        // ====================================================
        // SUBTITLE
        // ====================================================

        m_subtitleText.setFont(m_font);
        m_subtitleText.setString(
            "Welcome to our project!!!");

        m_subtitleText.setCharacterSize(18);

        m_subtitleText.setFillColor(
            sf::Color(200, 200, 200));

        sf::FloatRect subBounds =
            m_subtitleText.getLocalBounds();

        m_subtitleText.setOrigin(
            subBounds.left + subBounds.width / 2.f,
            subBounds.top + subBounds.height / 2.f);

        m_subtitleText.setPosition(
            400.f,
            110.f);
    }
}

// ============================================================
// INIT BACKGROUND
// ============================================================

void MainMenuState::initBackground()
{
    // Phần trên
    m_bgTop.setSize(
        sf::Vector2f(800.f, 300.f));

    m_bgTop.setPosition(
        0.f,
        0.f);

    m_bgTop.setFillColor(
        sf::Color(25, 25, 80));

    // Phần dưới
    m_bgBottom.setSize(
        sf::Vector2f(800.f, 300.f));

    m_bgBottom.setPosition(
        0.f,
        300.f);

    m_bgBottom.setFillColor(
        sf::Color(10, 10, 50));
}

// ============================================================
// INIT CLOUDS
// ============================================================

void MainMenuState::initClouds()
{
    float cloudData[][4] =
        {
            // x,     y,     width, speed
            {50.f, 30.f, 120.f, 30.f},
            {300.f, 55.f, 90.f, 20.f},
            {550.f, 20.f, 110.f, 40.f},
            {700.f, 70.f, 80.f, 25.f}};

    for (int i = 0; i < 4; ++i)
    {
        Cloud cloud;

        // ====================================================
        // Fallback rectangle
        // ====================================================

        cloud.shape.setSize(
            sf::Vector2f(
                cloudData[i][2],
                25.f));

        cloud.shape.setPosition(
            cloudData[i][0],
            cloudData[i][1]);

        cloud.shape.setFillColor(
            sf::Color(255, 255, 255, 80));

        cloud.shape.setOutlineThickness(0.f);

        cloud.speed = cloudData[i][3];

        // ====================================================
        // Cloud sprite
        // ====================================================

        if (m_cloudLoaded)
        {
            cloud.sprite.setTexture(
                m_cloudTexture);

            sf::Vector2u texSize =
                m_cloudTexture.getSize();

            if (texSize.x > 0 && texSize.y > 0)
            {
                float scaleX =
                    cloudData[i][2] /
                    static_cast<float>(texSize.x);

                float scaleY = scaleX;

                cloud.sprite.setScale(
                    scaleX,
                    scaleY);
            }

            cloud.sprite.setColor(
                sf::Color(255, 255, 255, 200));

            cloud.sprite.setPosition(
                cloudData[i][0],
                cloudData[i][1]);
        }

        m_clouds.push_back(cloud);
    }
}

// ============================================================
// INIT BUTTONS
// ============================================================

void MainMenuState::initButtons()
{
    m_buttons.clear();

    // ========================================================
    // Tổng chiều rộng của 7 button
    // ========================================================

    float totalWidth =
        BUTTON_COUNT * BUTTON_SIZE +
        (BUTTON_COUNT - 1) * BUTTON_SPACING;

    // ========================================================
    // Đặt cả cụm button ở góc dưới bên phải
    // ========================================================

    float startX =
        800.f -
        20.f -
        totalWidth +
        BUTTON_SIZE / 2.f;

    float startY =
        600.f -
        20.f -
        BUTTON_SIZE / 2.f;

    // ========================================================
    // Tạo 7 button
    // ========================================================

    for (int i = 0; i < BUTTON_COUNT; ++i)
    {
        MenuButton button;

        float xPos =
            startX +
            i * (BUTTON_SIZE + BUTTON_SPACING);

        // ====================================================
        // HITBOX
        // ====================================================

        button.hitbox.setSize(
            sf::Vector2f(
                BUTTON_SIZE,
                BUTTON_SIZE));

        button.hitbox.setOrigin(
            BUTTON_SIZE / 2.f,
            BUTTON_SIZE / 2.f);

        button.hitbox.setPosition(
            xPos,
            startY);

        button.hitbox.setFillColor(
            sf::Color::Transparent);

        // ====================================================
        // VỊ TRÍ GỐC
        // ====================================================

        button.basePosition =
            sf::Vector2f(
                xPos,
                startY);

        button.width = BUTTON_SIZE;
        button.height = BUTTON_SIZE;

        button.hovered = false;
        button.pressed = false;
        button.currentFrame = 0;

        // ====================================================
        // LOAD SPRITE
        // ====================================================

        if (m_buttonLoaded[i])
        {
            button.sprite.setTexture(
                m_buttonTextures[i]);

            sf::Vector2u textureSize =
                m_buttonTextures[i].getSize();

            if (textureSize.x > 0 &&
                textureSize.y > 0)
            {
                // =================================================
                // Sprite sheet:
                //
                // +----------+----------+----------+
                // | NORMAL   | HOVER    | PRESSED  |
                // +----------+----------+----------+
                //
                // Chia texture thành 3 frame theo chiều ngang.
                // =================================================

                int frameWidth =
                    static_cast<int>(
                        textureSize.x / 3);

                int frameHeight =
                    static_cast<int>(
                        textureSize.y);

                // =================================================
                // QUAN TRỌNG:
                //
                // Không scale frame 512x1024 thành 50x50.
                //
                // Thay vào đó lấy một vùng VUÔNG bên trong frame.
                // =================================================

                int squareSize =
                    std::min(
                        frameWidth,
                        frameHeight);

                // =================================================
                // Căn giữa vùng vuông trong frame.
                // =================================================

                int offsetX =
                    (frameWidth - squareSize) / 2;

                int offsetY =
                    (frameHeight - squareSize) / 2;

                // =================================================
                // Frame NORMAL
                // =================================================

                button.sprite.setTextureRect(
                    sf::IntRect(
                        offsetX,
                        offsetY,
                        squareSize,
                        squareSize));

                // =================================================
                // Origin chính giữa icon
                // =================================================

                button.sprite.setOrigin(
                    squareSize / 2.f,
                    squareSize / 2.f);

                // =================================================
                // SCALE ĐỒNG ĐỀU
                //
                // X = Y
                //
                // => hình tròn KHÔNG BAO GIỜ bị méo.
                // =================================================

                float scale =
                    BUTTON_SIZE /
                    static_cast<float>(squareSize);

                button.sprite.setScale(
                    scale,
                    scale);

                // =================================================
                // Vị trí
                // =================================================

                button.sprite.setPosition(
                    xPos,
                    startY);
            }
        }

        // Setup Hover Text Sprite
        if (m_hoverTextLoaded[i])
        {
            m_hoverTextSprites[i].setTexture(m_hoverTextTextures[i]);
            sf::Vector2u textTexSize = m_hoverTextTextures[i].getSize();
            if (textTexSize.x > 0 && textTexSize.y > 0)
            {
                // Scale sao cho chữ có kích thước cố định mà không méo
                float textScale = 25.f / static_cast<float>(textTexSize.y);
                m_hoverTextSprites[i].setScale(textScale, textScale);
                
                m_hoverTextSprites[i].setOrigin(
                    static_cast<float>(textTexSize.x) / 2.f,
                    static_cast<float>(textTexSize.y) / 2.f
                );
                
                // Đặt text ở phía trên nút
                m_hoverTextSprites[i].setPosition(
                    xPos,
                    startY - BUTTON_SIZE / 2.f - 20.f
                );
            }
        }

        m_buttons.push_back(button);
    }
}

// ============================================================
// SET BUTTON FRAME
// ============================================================

void MainMenuState::setButtonFrame(
    MenuButton &button,
    int frame)
{
    // ========================================================
    // Giới hạn frame
    // ========================================================

    if (frame < 0)
        frame = 0;

    if (frame > 2)
        frame = 2;

    // ========================================================
    // Kiểm tra texture
    // ========================================================

    if (button.sprite.getTexture() == nullptr)
        return;

    sf::Vector2u textureSize =
        button.sprite.getTexture()->getSize();

    if (textureSize.x == 0 ||
        textureSize.y == 0)
    {
        return;
    }

    // ========================================================
    // Chia sprite sheet thành 3 frame
    // ========================================================

    int frameWidth =
        static_cast<int>(
            textureSize.x / 3);

    int frameHeight =
        static_cast<int>(
            textureSize.y);

    if (frameWidth <= 0 ||
        frameHeight <= 0)
    {
        return;
    }

    // ========================================================
    // Lấy vùng vuông lớn nhất trong frame
    // ========================================================

    int squareSize =
        std::min(
            frameWidth,
            frameHeight);

    // ========================================================
    // Căn giữa vùng vuông
    // ========================================================

    int offsetX =
        (frameWidth - squareSize) / 2;

    int offsetY =
        (frameHeight - squareSize) / 2;

    // ========================================================
    // Chọn frame
    //
    // frame 0 = NORMAL
    // frame 1 = HOVER
    // frame 2 = PRESSED
    // ========================================================

    button.sprite.setTextureRect(
        sf::IntRect(
            frame * frameWidth + offsetX,
            offsetY,
            squareSize,
            squareSize));

    // ========================================================
    // Origin luôn ở chính giữa icon
    // ========================================================

    button.sprite.setOrigin(
        squareSize / 2.f,
        squareSize / 2.f);

    // ========================================================
    // SCALE ĐỒNG ĐỀU
    //
    // Đây là phần sửa lỗi hình tròn bị méo.
    // ========================================================

    float scale =
        BUTTON_SIZE /
        static_cast<float>(squareSize);

    button.sprite.setScale(
        scale,
        scale);

    // ========================================================
    // Lưu frame hiện tại
    // ========================================================

    button.currentFrame = frame;
}

// ============================================================
// HANDLE INPUT
// ============================================================

void MainMenuState::handleInput(
    sf::RenderWindow &window,
    sf::Event &event)
{
    // ========================================================
    // MOUSE PRESSED
    // ========================================================

    if (event.type ==
        sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button ==
            sf::Mouse::Left)
        {
            sf::Vector2i mousePixel =
                sf::Mouse::getPosition(window);

            sf::Vector2f mousePos =
                window.mapPixelToCoords(
                    mousePixel);

            for (int i = 0;
                 i < BUTTON_COUNT;
                 ++i)
            {
                if (!m_buttonLoaded[i])
                    continue;

                if (m_buttons[i]
                        .hitbox
                        .getGlobalBounds()
                        .contains(mousePos))
                {
                    m_buttons[i].pressed = true;

                    // Frame PRESSED
                    setButtonFrame(
                        m_buttons[i],
                        2);

                    // Di chuyển lên một chút
                    m_buttons[i].sprite.setPosition(
                        m_buttons[i].basePosition.x,
                        m_buttons[i].basePosition.y -
                            PRESSED_OFFSET);

                    break;
                }
            }
        }
    }

    // ========================================================
    // MOUSE RELEASED
    // ========================================================

    if (event.type ==
        sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button ==
            sf::Mouse::Left)
        {
            sf::Vector2i mousePixel =
                sf::Mouse::getPosition(window);

            sf::Vector2f mousePos =
                window.mapPixelToCoords(
                    mousePixel);

            for (int i = 0;
                 i < BUTTON_COUNT;
                 ++i)
            {
                if (!m_buttonLoaded[i])
                    continue;

                bool inside =
                    m_buttons[i]
                        .hitbox
                        .getGlobalBounds()
                        .contains(mousePos);

                if (m_buttons[i].pressed)
                {
                    m_buttons[i].pressed = false;

                    if (inside)
                    {
                        Game::instance().playSound("assets/audio/sfx_click.wav");
                        onButtonClick(i);
                    }
                }
            }
        }
    }
}

// ============================================================
// UPDATE
// ============================================================

void MainMenuState::update(float dt)
{
    // Đảm bảo nhạc nền Menu luôn được phát khi trở về MainMenuState
    Game::instance().playBackgroundMusic("assets/audio/bgm_menu.ogg");

    // ========================================================
    // Background Video Animation
    // ========================================================
    if (!m_menuVideoTextures.empty()) {
        if (Game::instance().isMotionEnabled()) {
            m_videoAnimTimer += dt;
            // Phát video ở tốc độ ~12 fps (chậm lại)
            if (m_videoAnimTimer >= 1.0f / 12.0f) {
                m_videoAnimTimer = 0.f;
                m_currentVideoFrame = (m_currentVideoFrame + 1) % m_menuVideoTextures.size();
                m_menuBgSprite.setTexture(m_menuVideoTextures[m_currentVideoFrame]);
            }
        } else {
            // Khi cài đặt Motion bị tắt -> Hiển thị ảnh đầu tiên
            m_currentVideoFrame = 0;
            m_videoAnimTimer = 0.f;
            m_menuBgSprite.setTexture(m_menuVideoTextures[0]);
        }
    }

    // ========================================================
    // Subtitle blinking
    // ========================================================

    m_blinkTimer += dt;

    if (m_blinkTimer >= 0.6f)
    {
        m_blinkTimer = 0.f;

        m_subtitleVisible =
            !m_subtitleVisible;

        if (m_fontLoaded)
        {
            m_subtitleText.setFillColor(
                m_subtitleVisible
                    ? sf::Color(200, 200, 200)
                    : sf::Color::Transparent);
        }
    }

    // ========================================================
    // Clouds
    // ========================================================

    if (Game::instance().isMotionEnabled())
    {
        for (auto &cloud : m_clouds)
        {
            cloud.shape.move(
                cloud.speed * dt,
                0.f);

            if (m_cloudLoaded)
            {
                cloud.sprite.move(
                    cloud.speed * dt,
                    0.f);
            }

            // Nếu ra ngoài màn hình
            if (cloud.shape.getPosition().x > 820.f)
            {
                float newX =
                    -cloud.shape.getSize().x;

                float y =
                    cloud.shape.getPosition().y;

                cloud.shape.setPosition(
                    newX,
                    y);

                if (m_cloudLoaded)
                {
                    cloud.sprite.setPosition(
                        newX,
                        y);
                }
            }
        }
    }

    // ========================================================
    // BUTTON HOVER
    // ========================================================

    updateButtonHover();
}

// ============================================================
// UPDATE BUTTON HOVER
// ============================================================

void MainMenuState::updateButtonHover()
{
    sf::RenderWindow &window =
        Game::instance().getWindow();

    sf::Vector2i mousePixel =
        sf::Mouse::getPosition(window);

    sf::Vector2f mousePos =
        window.mapPixelToCoords(
            mousePixel);

    // ========================================================
    // Duyệt 7 button
    // ========================================================

    for (int i = 0;
         i < BUTTON_COUNT;
         ++i)
    {
        MenuButton &button =
            m_buttons[i];

        if (!m_buttonLoaded[i])
            continue;

        // ====================================================
        // Kiểm tra hover
        // ====================================================

        bool mouseOver =
            button.hitbox
                .getGlobalBounds()
                .contains(mousePos);

        bool wasHovered = button.hovered;
        button.hovered = mouseOver;

        if (button.hovered && !wasHovered)
        {
            Game::instance().playSound("assets/audio/sfx_hovering.wav");
        }

        // ====================================================
        // Nếu đang nhấn chuột
        // ====================================================

        if (button.pressed)
        {
            setButtonFrame(
                button,
                2);

            button.sprite.setPosition(
                button.basePosition.x,
                button.basePosition.y - PRESSED_OFFSET);

            continue;
        }

        // ====================================================
        // HOVER
        // ====================================================

        if (button.hovered)
        {
            // Frame 1 = HOVER
            setButtonFrame(
                button,
                1);

            // Di chuyển lên 4 px
            button.sprite.setPosition(
                button.basePosition.x,
                button.basePosition.y - HOVER_OFFSET);
        }

        // ====================================================
        // NORMAL
        // ====================================================

        else
        {
            // Frame 0 = NORMAL
            setButtonFrame(
                button,
                0);

            // Trở lại vị trí ban đầu
            button.sprite.setPosition(
                button.basePosition);
        }
    }
}

// ============================================================
// DRAW
// ============================================================

void MainMenuState::draw(
    sf::RenderWindow &window)
{
    // ========================================================
    // BACKGROUND
    // ========================================================

    if (m_menuBgLoaded)
    {
        window.draw(
            m_menuBgSprite);
    }
    else
    {
        window.draw(
            m_bgTop);

        window.draw(
            m_bgBottom);
    }

    // ========================================================
    // CLOUDS
    // ========================================================

    for (auto &cloud : m_clouds)
    {
        if (m_cloudLoaded)
        {
            window.draw(
                cloud.sprite);
        }
        else
        {
            window.draw(
                cloud.shape);
        }
    }

    // ========================================================
    // TITLE & SUBTITLE
    // ========================================================

    if (m_fontLoaded)
    {
        window.draw(
            m_titleText);

        window.draw(
            m_subtitleText);
    }

    // ========================================================
    // BUTTONS
    // ========================================================

    for (int i = 0;
         i < BUTTON_COUNT;
         ++i)
    {
        if (m_buttonLoaded[i])
        {
            window.draw(
                m_buttons[i].sprite);
                
            // Draw hover text if hovered
            if (m_buttons[i].hovered && m_hoverTextLoaded[i])
            {
                window.draw(m_hoverTextSprites[i]);
            }
        }
    }
}

// ============================================================
// BUTTON CLICK
// ============================================================

void MainMenuState::onButtonClick(
    int index)
{
    switch (index)
    {
        // ====================================================
        // 0 - PLAY
        // ====================================================

    case 0:
    {
        Game::instance()
            .getStateMachine()
            .pushState(
                std::make_unique<GameState>());

        break;
    }

        // ====================================================
        // 1 - LOAD GAME
        // ====================================================

    case 1:
    {
        Game::instance()
            .getStateMachine()
            .pushState(
                std::make_unique<LoadGameState>());

        break;
    }

        // ====================================================
        // 2 - RANKING
        // ====================================================

    case 2:
    {
        Game::instance()
            .getStateMachine()
            .pushState(
                std::make_unique<RankingState>());

        break;
    }

        // ====================================================
        // 3 - HELP
        // ====================================================

    case 3:
    {
        Game::instance()
            .getStateMachine()
            .pushState(
                std::make_unique<HelpState>());

        break;
    }

        // ====================================================
        // 4 - CREDIT / ABOUT
        // ====================================================

    case 4:
    {
        Game::instance()
            .getStateMachine()
            .pushState(
                std::make_unique<AboutState>());

        break;
    }

        // ====================================================
        // 5 - SETTINGS
        // ====================================================

    case 5:
    {
        Game::instance()
            .getStateMachine()
            .pushState(
                std::make_unique<SettingState>());

        break;
    }

        // ====================================================
        // 6 - EXIT
        // ====================================================

    case 6:
    {
        Game::instance()
            .getWindow()
            .close();

        break;
    }

    default:
        break;
    }
}