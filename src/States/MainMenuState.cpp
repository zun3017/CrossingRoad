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
    // LOAD SKINS BUTTON & TEXT
    // ========================================================
    m_skinsButtonLoaded = m_skinsButtonTexture.loadFromFile("assets/textures/skins_button.png");
    if (!m_skinsButtonLoaded) m_skinsButtonLoaded = m_skinsButtonTexture.loadFromFile("CrossingRoad/assets/textures/skins_button.png");

    m_skinsTextLoaded = m_skinsTextTexture.loadFromFile("assets/textures/skins_text.png");
    if (!m_skinsTextLoaded) m_skinsTextLoaded = m_skinsTextTexture.loadFromFile("CrossingRoad/assets/textures/skins_text.png");

    initSkinSelectionUI();

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

    // ========================================================
    // SKINS BUTTON (Góc dưới bên trái màn hình)
    // ========================================================
    float skinsX = 20.f + BUTTON_SIZE / 2.f;
    float skinsY = startY; // Ngang hàng tuyệt đối với các nút khác ở dưới

    m_skinsButton.hitbox.setSize(sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE));
    m_skinsButton.hitbox.setOrigin(BUTTON_SIZE / 2.f, BUTTON_SIZE / 2.f);
    m_skinsButton.hitbox.setPosition(skinsX, skinsY);
    m_skinsButton.hitbox.setFillColor(sf::Color::Transparent);

    m_skinsButton.basePosition = sf::Vector2f(skinsX, skinsY);
    m_skinsButton.width = BUTTON_SIZE;
    m_skinsButton.height = BUTTON_SIZE;
    m_skinsButton.hovered = false;
    m_skinsButton.pressed = false;
    m_skinsButton.currentFrame = 0;

    if (m_skinsButtonLoaded)
    {
        m_skinsButton.sprite.setTexture(m_skinsButtonTexture);
        sf::Vector2u texSize = m_skinsButtonTexture.getSize();
        if (texSize.x > 0 && texSize.y > 0)
        {
            int frameWidth = static_cast<int>(texSize.x / 3);
            int frameHeight = static_cast<int>(texSize.y);
            int squareSize = std::min(frameWidth, frameHeight);
            int offsetX = (frameWidth - squareSize) / 2;
            int offsetY = (frameHeight - squareSize) / 2;

            m_skinsButton.sprite.setTextureRect(sf::IntRect(offsetX, offsetY, squareSize, squareSize));
            m_skinsButton.sprite.setOrigin(squareSize / 2.f, squareSize / 2.f);
            float scale = BUTTON_SIZE / static_cast<float>(squareSize);
            m_skinsButton.sprite.setScale(scale, scale);
            m_skinsButton.sprite.setPosition(skinsX, skinsY);
        }
    }

    if (m_skinsTextLoaded)
    {
        m_skinsTextSprite.setTexture(m_skinsTextTexture);
        sf::Vector2u textTexSize = m_skinsTextTexture.getSize();
        if (textTexSize.x > 0 && textTexSize.y > 0)
        {
            float textScale = 25.f / static_cast<float>(textTexSize.y);
            m_skinsTextSprite.setScale(textScale, textScale);
            m_skinsTextSprite.setOrigin(
                static_cast<float>(textTexSize.x) / 2.f,
                static_cast<float>(textTexSize.y) / 2.f
            );
            m_skinsTextSprite.setPosition(skinsX, skinsY - BUTTON_SIZE / 2.f - 20.f);
        }
    }
}

// ============================================================
// INIT SKIN SELECTION UI POPUP
// ============================================================
void MainMenuState::initSkinSelectionUI()
{
    m_skinPreviewLoaded[0] = m_skinPreviewTextures[0].loadFromFile("assets/textures/skins nv/player.png");
    if (!m_skinPreviewLoaded[0]) m_skinPreviewLoaded[0] = m_skinPreviewTextures[0].loadFromFile("assets/textures/player.png");
    if (!m_skinPreviewLoaded[0]) m_skinPreviewLoaded[0] = m_skinPreviewTextures[0].loadFromFile("CrossingRoad/assets/textures/skins nv/player.png");

    m_skinPreviewLoaded[1] = m_skinPreviewTextures[1].loadFromFile("assets/textures/skins nv/player_beach.png");
    if (!m_skinPreviewLoaded[1]) m_skinPreviewLoaded[1] = m_skinPreviewTextures[1].loadFromFile("CrossingRoad/assets/textures/skins nv/player_beach.png");

    m_skinPreviewLoaded[2] = m_skinPreviewTextures[2].loadFromFile("assets/textures/skins nv/player_mafia.png");
    if (!m_skinPreviewLoaded[2]) m_skinPreviewLoaded[2] = m_skinPreviewTextures[2].loadFromFile("CrossingRoad/assets/textures/skins nv/player_mafia.png");

    m_selectTextLoaded = m_selectTextTexture.loadFromFile("assets/textures/select_text.png");
    if (!m_selectTextLoaded) m_selectTextLoaded = m_selectTextTexture.loadFromFile("CrossingRoad/assets/textures/select_text.png");

    m_selectedTextLoaded = m_selectedTextTexture.loadFromFile("assets/textures/selected_text.png");
    if (!m_selectedTextLoaded) m_selectedTextLoaded = m_selectedTextTexture.loadFromFile("CrossingRoad/assets/textures/selected_text.png");

    // Lớp phủ nền mờ
    m_skinOverlayDim.setSize(sf::Vector2f(800.f, 600.f));
    m_skinOverlayDim.setFillColor(sf::Color(0, 0, 0, 160));

    // Khung lớn
    m_skinOuterFrame.setSize(sf::Vector2f(660.f, 480.f));
    m_skinOuterFrame.setOrigin(330.f, 240.f);
    m_skinOuterFrame.setPosition(400.f, 300.f);
    m_skinOuterFrame.setFillColor(sf::Color(18, 18, 35, 235));
    m_skinOuterFrame.setOutlineColor(sf::Color(90, 90, 150, 220));
    m_skinOuterFrame.setOutlineThickness(3.f);

    // Khung nhỏ bên trong
    m_skinInnerFrame.setSize(sf::Vector2f(380.f, 370.f));
    m_skinInnerFrame.setOrigin(190.f, 185.f);
    m_skinInnerFrame.setPosition(400.f, 315.f);
    m_skinInnerFrame.setFillColor(sf::Color(30, 30, 52, 240));
    m_skinInnerFrame.setOutlineColor(sf::Color(130, 130, 190, 220));
    m_skinInnerFrame.setOutlineThickness(2.f);

    // Tiêu đề & các nút chữ
    if (m_fontLoaded)
    {
        m_skinTitleText.setFont(m_font);
        m_skinTitleText.setString("SKIN SELECTION");
        m_skinTitleText.setCharacterSize(28);
        m_skinTitleText.setFillColor(sf::Color(255, 215, 0));
        m_skinTitleText.setStyle(sf::Text::Bold);
        sf::FloatRect tb = m_skinTitleText.getLocalBounds();
        m_skinTitleText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_skinTitleText.setPosition(400.f, 88.f);

        m_skinCloseBtn.setFont(m_font);
        m_skinCloseBtn.setString("X");
        m_skinCloseBtn.setCharacterSize(26);
        m_skinCloseBtn.setFillColor(sf::Color(200, 200, 200));
        m_skinCloseBtn.setStyle(sf::Text::Bold);
        sf::FloatRect cb = m_skinCloseBtn.getLocalBounds();
        m_skinCloseBtn.setOrigin(cb.left + cb.width / 2.f, cb.top + cb.height / 2.f);
        m_skinCloseBtn.setPosition(700.f, 88.f);

        m_arrowLeftText.setFont(m_font);
        m_arrowLeftText.setString("<");
        m_arrowLeftText.setCharacterSize(52);
        m_arrowLeftText.setFillColor(sf::Color(255, 215, 0));
        m_arrowLeftText.setStyle(sf::Text::Bold);
        sf::FloatRect ab1 = m_arrowLeftText.getLocalBounds();
        m_arrowLeftText.setOrigin(ab1.left + ab1.width / 2.f, ab1.top + ab1.height / 2.f);
        m_arrowLeftText.setPosition(250.f, 220.f);

        m_arrowRightText.setFont(m_font);
        m_arrowRightText.setString(">");
        m_arrowRightText.setCharacterSize(52);
        m_arrowRightText.setFillColor(sf::Color(255, 215, 0));
        m_arrowRightText.setStyle(sf::Text::Bold);
        sf::FloatRect ab2 = m_arrowRightText.getLocalBounds();
        m_arrowRightText.setOrigin(ab2.left + ab2.width / 2.f, ab2.top + ab2.height / 2.f);
        m_arrowRightText.setPosition(550.f, 220.f);

        m_skinNameText.setFont(m_font);
        m_skinNameText.setCharacterSize(24);
        m_skinNameText.setFillColor(sf::Color::White);
        m_skinNameText.setStyle(sf::Text::Bold);
    }

    updateSkinPreview();
}

// ============================================================
// UPDATE SKIN PREVIEW
// ============================================================
void MainMenuState::updateSkinPreview()
{
    if (m_viewingSkinIndex < 0) m_viewingSkinIndex = 0;
    if (m_viewingSkinIndex >= TOTAL_SKINS) m_viewingSkinIndex = TOTAL_SKINS - 1;

    // 1. Ảnh Demo Skin
    if (m_skinPreviewLoaded[m_viewingSkinIndex])
    {
        m_skinPreviewSprite.setTexture(m_skinPreviewTextures[m_viewingSkinIndex]);
        sf::Vector2u texSize = m_skinPreviewTextures[m_viewingSkinIndex].getSize();
        if (texSize.x > 0 && texSize.y > 0)
        {
            int frameW = static_cast<int>(texSize.x / 4);
            int frameH = static_cast<int>(texSize.y / 4);
            // Frame nhìn về phía trước (hàng 1, frame 0)
            m_skinPreviewSprite.setTextureRect(sf::IntRect(0, frameH, frameW, frameH));
            m_skinPreviewSprite.setOrigin(frameW / 2.f, frameH / 2.f);
            float scale = 110.f / static_cast<float>(frameH);
            m_skinPreviewSprite.setScale(scale, scale);
            m_skinPreviewSprite.setPosition(400.f, 220.f);
        }
    }

    // 2. Tên Skin
    std::string skinNames[TOTAL_SKINS] = { "Default", "Beach", "Mafia" };
    if (m_fontLoaded)
    {
        m_skinNameText.setString(skinNames[m_viewingSkinIndex]);
        sf::FloatRect nb = m_skinNameText.getLocalBounds();
        m_skinNameText.setOrigin(nb.left + nb.width / 2.f, nb.top + nb.height / 2.f);
        m_skinNameText.setPosition(400.f, 315.f);
    }

    // 3. Nút SELECT / SELECTED (Tăng kích thước to rõ hơn)
    bool isCurrent = (m_viewingSkinIndex == static_cast<int>(Game::instance().getPlayerSkin()));
    if (isCurrent && m_selectedTextLoaded)
    {
        m_selectBtnSprite.setTexture(m_selectedTextTexture);
        sf::Vector2u sSize = m_selectedTextTexture.getSize();
        if (sSize.x > 0 && sSize.y > 0)
        {
            float btnScale = 65.f / static_cast<float>(sSize.y);
            m_selectBtnSprite.setScale(btnScale, btnScale);
            m_selectBtnSprite.setOrigin(sSize.x / 2.f, sSize.y / 2.f);
            m_selectBtnSprite.setPosition(400.f, 405.f);
            m_selectBtnSprite.setColor(sf::Color::White);
        }
    }
    else if (!isCurrent && m_selectTextLoaded)
    {
        m_selectBtnSprite.setTexture(m_selectTextTexture);
        sf::Vector2u sSize = m_selectTextTexture.getSize();
        if (sSize.x > 0 && sSize.y > 0)
        {
            float btnScale = 65.f / static_cast<float>(sSize.y);
            m_selectBtnSprite.setScale(btnScale, btnScale);
            m_selectBtnSprite.setOrigin(sSize.x / 2.f, sSize.y / 2.f);
            m_selectBtnSprite.setPosition(400.f, 405.f);
            m_selectBtnSprite.setColor(sf::Color::White);
        }
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
    // 1. Phím ESC đóng bảng Skin
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
        {
            if (m_showSkinsUI)
            {
                m_showSkinsUI = false;
                Game::instance().playSound("assets/audio/sfx_click.wav");
                return;
            }
        }
    }

    // 2. Nếu bảng chọn Skin đang mở -> Chỉ nhận tương tác trong bảng Skin
    if (m_showSkinsUI)
    {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

            // Nút đóng X (ở góc trên phải bảng)
            sf::FloatRect closeBounds(680.f, 70.f, 45.f, 45.f);
            if (closeBounds.contains(mousePos))
            {
                m_showSkinsUI = false;
                Game::instance().playSound("assets/audio/sfx_click.wav");
                return;
            }

            // Mũi tên trái < (quay lại skin trước)
            if (m_viewingSkinIndex > 0)
            {
                sf::FloatRect leftArrowBounds(220.f, 190.f, 60.f, 60.f);
                if (leftArrowBounds.contains(mousePos))
                {
                    m_viewingSkinIndex--;
                    Game::instance().playSound("assets/audio/sfx_click.wav");
                    updateSkinPreview();
                    return;
                }
            }

            // Mũi tên phải > (chuyển sang skin kế tiếp)
            if (m_viewingSkinIndex < TOTAL_SKINS - 1)
            {
                sf::FloatRect rightArrowBounds(520.f, 190.f, 60.f, 60.f);
                if (rightArrowBounds.contains(mousePos))
                {
                    m_viewingSkinIndex++;
                    Game::instance().playSound("assets/audio/sfx_click.wav");
                    updateSkinPreview();
                    return;
                }
            }

            // Nút SELECT
            bool isCurrent = (m_viewingSkinIndex == static_cast<int>(Game::instance().getPlayerSkin()));
            if (!isCurrent)
            {
                if (m_selectBtnSprite.getGlobalBounds().contains(mousePos))
                {
                    Game::instance().setPlayerSkin(static_cast<PlayerSkin>(m_viewingSkinIndex));
                    Game::instance().playSound("assets/audio/sfx_click.wav");
                    updateSkinPreview();
                    return;
                }
            }
        }
        return; // Chặn các nút menu phía sau
    }

    // ========================================================
    // MOUSE PRESSED
    // ========================================================
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

        // Nút Skins
        if (m_skinsButtonLoaded && m_skinsButton.hitbox.getGlobalBounds().contains(mousePos))
        {
            m_skinsButton.pressed = true;
            setButtonFrame(m_skinsButton, 2);
            m_skinsButton.sprite.setPosition(m_skinsButton.basePosition.x, m_skinsButton.basePosition.y - PRESSED_OFFSET);
        }

        // 7 nút menu khác
        for (int i = 0; i < BUTTON_COUNT; ++i)
        {
            if (!m_buttonLoaded[i]) continue;
            if (m_buttons[i].hitbox.getGlobalBounds().contains(mousePos))
            {
                m_buttons[i].pressed = true;
                setButtonFrame(m_buttons[i], 2);
                m_buttons[i].sprite.setPosition(m_buttons[i].basePosition.x, m_buttons[i].basePosition.y - PRESSED_OFFSET);
                break;
            }
        }
    }

    // ========================================================
    // MOUSE RELEASED
    // ========================================================
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

        // Nút Skins
        if (m_skinsButtonLoaded)
        {
            bool inside = m_skinsButton.hitbox.getGlobalBounds().contains(mousePos);
            if (m_skinsButton.pressed)
            {
                m_skinsButton.pressed = false;
                if (inside)
                {
                    Game::instance().playSound("assets/audio/sfx_click.wav");
                    m_viewingSkinIndex = static_cast<int>(Game::instance().getPlayerSkin());
                    updateSkinPreview();
                    m_showSkinsUI = true;
                    return;
                }
            }
        }

        // 7 nút menu khác
        for (int i = 0; i < BUTTON_COUNT; ++i)
        {
            if (!m_buttonLoaded[i]) continue;
            bool inside = m_buttons[i].hitbox.getGlobalBounds().contains(mousePos);
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

    if (m_showSkinsUI)
    {
        // 1. Hover Close Button X
        sf::FloatRect closeBounds(680.f, 70.f, 45.f, 45.f);
        bool closeOver = closeBounds.contains(mousePos);
        if (closeOver && !m_skinCloseHovered) Game::instance().playSound("assets/audio/sfx_hovering.wav");
        m_skinCloseHovered = closeOver;
        m_skinCloseBtn.setFillColor(m_skinCloseHovered ? sf::Color::Red : sf::Color(200, 200, 200));

        // 2. Hover Arrow Left <
        if (m_viewingSkinIndex > 0)
        {
            sf::FloatRect leftArrowBounds(220.f, 190.f, 60.f, 60.f);
            bool leftOver = leftArrowBounds.contains(mousePos);
            if (leftOver && !m_arrowLeftHovered) Game::instance().playSound("assets/audio/sfx_hovering.wav");
            m_arrowLeftHovered = leftOver;
            m_arrowLeftText.setFillColor(m_arrowLeftHovered ? sf::Color::White : sf::Color(255, 215, 0));
        }

        // 3. Hover Arrow Right >
        if (m_viewingSkinIndex < TOTAL_SKINS - 1)
        {
            sf::FloatRect rightArrowBounds(520.f, 190.f, 60.f, 60.f);
            bool rightOver = rightArrowBounds.contains(mousePos);
            if (rightOver && !m_arrowRightHovered) Game::instance().playSound("assets/audio/sfx_hovering.wav");
            m_arrowRightHovered = rightOver;
            m_arrowRightText.setFillColor(m_arrowRightHovered ? sf::Color::White : sf::Color(255, 215, 0));
        }

        // 4. Hover Select Button
        bool isCurrent = (m_viewingSkinIndex == static_cast<int>(Game::instance().getPlayerSkin()));
        if (!isCurrent)
        {
            bool selectOver = m_selectBtnSprite.getGlobalBounds().contains(mousePos);
            if (selectOver && !m_selectBtnHovered) Game::instance().playSound("assets/audio/sfx_hovering.wav");
            m_selectBtnHovered = selectOver;
            m_selectBtnSprite.setColor(m_selectBtnHovered ? sf::Color(230, 230, 255) : sf::Color::White);
            m_selectBtnSprite.setPosition(400.f, m_selectBtnHovered ? 402.f : 405.f);
        }
        return;
    }

    // ========================================================
    // Cập nhật Hover cho Skins Button (Góc dưới trái)
    // ========================================================
    if (m_skinsButtonLoaded)
    {
        bool mouseOver = m_skinsButton.hitbox.getGlobalBounds().contains(mousePos);
        bool wasHovered = m_skinsButton.hovered;
        m_skinsButton.hovered = mouseOver;

        if (m_skinsButton.hovered && !wasHovered)
        {
            Game::instance().playSound("assets/audio/sfx_hovering.wav");
        }

        if (m_skinsButton.pressed)
        {
            setButtonFrame(m_skinsButton, 2);
            m_skinsButton.sprite.setPosition(m_skinsButton.basePosition.x, m_skinsButton.basePosition.y - PRESSED_OFFSET);
        }
        else if (m_skinsButton.hovered)
        {
            setButtonFrame(m_skinsButton, 1);
            m_skinsButton.sprite.setPosition(m_skinsButton.basePosition.x, m_skinsButton.basePosition.y - HOVER_OFFSET);
        }
        else
        {
            setButtonFrame(m_skinsButton, 0);
            m_skinsButton.sprite.setPosition(m_skinsButton.basePosition);
        }
    }

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
    // 7 BUTTONS (Góc dưới bên phải)
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

    // ========================================================
    // SKINS BUTTON (Góc dưới bên trái)
    // ========================================================
    if (m_skinsButtonLoaded)
    {
        window.draw(m_skinsButton.sprite);
        if (m_skinsButton.hovered && m_skinsTextLoaded)
        {
            window.draw(m_skinsTextSprite);
        }
    }

    // ========================================================
    // POPUP CHỌN SKIN (Nếu đang bật)
    // ========================================================
    if (m_showSkinsUI)
    {
        window.draw(m_skinOverlayDim);
        window.draw(m_skinOuterFrame);
        window.draw(m_skinInnerFrame);

        if (m_fontLoaded)
        {
            window.draw(m_skinTitleText);
            window.draw(m_skinCloseBtn);
            window.draw(m_skinNameText);

            if (m_viewingSkinIndex > 0)
            {
                window.draw(m_arrowLeftText);
            }
            if (m_viewingSkinIndex < TOTAL_SKINS - 1)
            {
                window.draw(m_arrowRightText);
            }
        }

        if (m_skinPreviewLoaded[m_viewingSkinIndex])
        {
            window.draw(m_skinPreviewSprite);
        }

        if (m_selectTextLoaded || m_selectedTextLoaded)
        {
            window.draw(m_selectBtnSprite);
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