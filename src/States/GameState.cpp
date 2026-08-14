#define _CRT_SECURE_NO_WARNINGS
#include "GameState.h"
#include "MainMenuState.h"
#include "../Core/ResourceManager.h"
#include "../Core/Game.h"
#include "../Managers/SaveManager.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>

// ============================================================
// GameState - Trạng thái gameplay chính của game
// ============================================================

GameState::GameState()
    : m_level(1), m_score(0)
{
}

GameState::GameState(int level, int score)
    : m_level(level), m_score(score)
{
}

void GameState::init() {
    Game::instance().playBackgroundMusic("assets/audio/bgm_gameplay.ogg");

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    m_fontLoaded = m_font.loadFromFile("assets/fonts/arial.ttf");

    try {
        // Preload player texture vào ResourceManager cache,
        // để CPEOPLE constructor dùng lại mà không cần load từ file lần nữa.
        auto& tPlayer = ResourceManager<sf::Texture>::getInstance().get("assets/textures/player.png");
        auto& tGrass = ResourceManager<sf::Texture>::getInstance().get("assets/textures/grass.png");
        auto& tRoad = ResourceManager<sf::Texture>::getInstance().get("assets/textures/road.png");
        auto& tRiver = ResourceManager<sf::Texture>::getInstance().get("assets/textures/river.png");
        auto& tLog = ResourceManager<sf::Texture>::getInstance().get("assets/textures/log.png");
        auto& tCarBlue = ResourceManager<sf::Texture>::getInstance().get("assets/textures/car_blue.png");
        auto& tCarRed = ResourceManager<sf::Texture>::getInstance().get("assets/textures/car_red.png");
        auto& tCarYellow = ResourceManager<sf::Texture>::getInstance().get("assets/textures/car_yellow.png");
        auto& tItem = ResourceManager<sf::Texture>::getInstance().get("assets/textures/item.png");
        auto& tTrack = ResourceManager<sf::Texture>::getInstance().get("assets/textures/track.png");
        auto& tTrain = ResourceManager<sf::Texture>::getInstance().get("assets/textures/train.png");
        auto& tLightGreen = ResourceManager<sf::Texture>::getInstance().get("assets/textures/light_green.png");
        auto& tLightRed = ResourceManager<sf::Texture>::getInstance().get("assets/textures/light_red.png");
        auto& tLightBlink = ResourceManager<sf::Texture>::getInstance().get("assets/textures/light_blink.png");
        
        if (tPlayer.getSize().x > 0) {
            // (m_playerSprite đã xóa - player load texture trực tiếp trong CPEOPLE constructor)

            m_grassSprite.setTexture(tGrass);
            m_grassSprite.setScale(800.f / tGrass.getSize().x, m_cellSize / tGrass.getSize().y);
            
            m_roadSprite.setTexture(tRoad);
            m_roadSprite.setScale(800.f / tRoad.getSize().x, m_cellSize / tRoad.getSize().y);
            
            m_riverSprite.setTexture(tRiver);
            m_riverSprite.setScale(800.f / tRiver.getSize().x, m_cellSize / tRiver.getSize().y);
            
            m_logSprite.setTexture(tLog);
            m_logSprite.setScale(56.f / tLog.getSize().x, 40.f / tLog.getSize().y);
            

        auto& tWatch = ResourceManager<sf::Texture>::getInstance().get("assets/textures/watch.png");
        if (tWatch.getSize().x > 0) {
            m_watchSprite.setTexture(tWatch);
            m_watchLoaded = true;
        }

        if (tItem.getSize().x > 0) m_itemSprite.setTexture(tItem);
        
        if (tTrack.getSize().x > 0) m_trackSprite.setTexture(tTrack);
        if (tTrain.getSize().x > 0) m_trainSprite.setTexture(tTrain);
        if (tLightGreen.getSize().x > 0) m_lightGreenSprite.setTexture(tLightGreen);
        if (tLightRed.getSize().x > 0) m_lightRedSprite.setTexture(tLightRed);
        if (tLightBlink.getSize().x > 0) m_lightBlinkSprite.setTexture(tLightBlink);
        
        m_hitByCarLoaded = m_hitByCarTexture.loadFromFile("assets/textures/hitbycar.png");
        if (!m_hitByCarLoaded) m_hitByCarLoaded = m_hitByCarTexture.loadFromFile("CrossingRoad/assets/textures/hitbycar.png");
        
        m_playerDrownLoaded = m_playerDrownTexture.loadFromFile("assets/textures/player_drown.png");
        if (!m_playerDrownLoaded) m_playerDrownLoaded = m_playerDrownTexture.loadFromFile("CrossingRoad/assets/textures/player_drown.png");
        
        m_texturesLoaded = true;
    }
} catch (...) {}

m_freezeOverlay.setSize(sf::Vector2f(800.f, 600.f));
m_freezeOverlay.setFillColor(sf::Color(100, 210, 255, 45)); // Ánh xanh băng tuyết

m_freezeBadgeText.setFont(m_font);
m_freezeBadgeText.setCharacterSize(13);
m_freezeBadgeText.setFillColor(sf::Color(0, 229, 255)); // Cyan #00e5ff
m_freezeBadgeText.setOutlineColor(sf::Color::Black);
m_freezeBadgeText.setOutlineThickness(1.5f);
m_freezeBadgeText.setStyle(sf::Text::Bold);

    if (!m_isLoadedGame) {
        initPlayer();
        generateMap();
        initHUD();
    }
    initOverlays();
}

// ============================================================
// Khởi tạo nhân vật người chơi
// ============================================================
void GameState::initPlayer() {
    m_player = std::make_unique<CPEOPLE>();
    float offsetX = (m_cellSize - m_playerSize) / 2.f;
    int col = static_cast<int>(800.f / m_cellSize) / 2; // Cột ở giữa (cột 8)
    m_player->forcePosition(col * m_cellSize + offsetX, 600.f - m_cellSize - offsetX);
    m_playerDead = false;
    m_maxPlayerY = m_player->getPosition().y;
    m_scoreSaved = false;
    m_goState = GameOverUIState::None;
    m_deferredAction = DeferredAction::None;
    m_deathTimer = 0.f;
    if (m_nameInput) m_nameInput.reset();
    if (m_btnOk) m_btnOk.reset();
    if (m_btnYes) m_btnYes.reset();
    if (m_btnNo) m_btnNo.reset();
    if (m_playAgainBtn) m_playAgainBtn.reset();
    if (m_menuBtn) m_menuBtn.reset();

    // Khôi phục lại hình ảnh ban đầu (nếu trước đó chết đổi thành hitbycar hoặc tàng hình)
    if (m_texturesLoaded) {
        m_player->setTextureLoaded(true);
        m_playerDrowned = false; // Reset drowned flag
    }
}

// ============================================================
// Tạo bản đồ ngẫu nhiên với các hàng địa hình
// ============================================================
void GameState::generateMap() {
    m_terrains.clear();

    // 2 hàng cỏ an toàn ở dưới cùng
    createGrassRow(600.f - m_cellSize, true);       // Hàng 12 (dưới cùng)
    createGrassRow(600.f - 2 * m_cellSize, true);   // Hàng 11

    // Tạo ngẫu nhiên các hàng còn lại (hàng 10 -> hàng 0)
    for (int i = 2; i < m_totalRows; i++) {
        float y = 600.f - (i + 1) * m_cellSize;
        int terrainRoll = std::rand() % 100;

        if (i >= m_totalRows - 2) {
            // Hàng trên cùng luôn là cỏ (vùng đích và để UI không bị xe đè)
            createGrassRow(y, true);
        } else if (terrainRoll < 30) {
            createRoadRow(y);
        } else if (terrainRoll < 60) {
            createRiverRow(y);
        } else if (terrainRoll < 80) {
            createRailwayRow(y);
        } else {
            createGrassRow(y, false);
        }
    }

    // Đảm bảo mỗi màn chơi luôn có ít nhất 1 đồng hồ (watch.png)
    bool hasClock = false;
    for (const auto& row : m_terrains) {
        for (const auto& item : row.items) {
            if (item.type == ItemType::Clock) {
                hasClock = true;
                break;
            }
        }
        if (hasClock) break;
    }

    if (!hasClock) {
        // Tìm các hàng đường hoặc hàng cỏ (bỏ 2 hàng xuất phát dưới cùng và hàng đích trên cùng)
        std::vector<int> candidateIndices;
        for (int idx = 2; idx < static_cast<int>(m_terrains.size()) - 2; ++idx) {
            if (m_terrains[idx].type == TerrainType::Road || m_terrains[idx].type == TerrainType::Grass) {
                candidateIndices.push_back(idx);
            }
        }
        if (!candidateIndices.empty()) {
            int chosenIdx = candidateIndices[std::rand() % candidateIndices.size()];
            auto& targetRow = m_terrains[chosenIdx];
            ItemData clockItem;
            float itemX = static_cast<float>(80 + std::rand() % 640);
            clockItem.shape.setRadius(23.f); // Tăng kích thước đồng hồ to rõ, nổi bật (đường kính 46px)
            clockItem.shape.setFillColor(sf::Color(0, 229, 255));
            clockItem.shape.setPosition(itemX, targetRow.yPosition + m_cellSize / 2.f - 23.f);
            clockItem.collected = false;
            clockItem.points = 0;
            clockItem.type = ItemType::Clock;
            targetRow.items.push_back(clockItem);
        }
    }
}

// ============================================================
// Tạo hàng cỏ
// ============================================================
void GameState::createGrassRow(float y, bool safeZone) {
    TerrainRow row;
    row.type = TerrainType::Grass;
    row.yPosition = y;

    row.background.setSize(sf::Vector2f(800.f, m_cellSize));
    row.background.setPosition(0.f, y);
    row.background.setFillColor(sf::Color(34, 139, 34)); // Xanh lá đậm

    // Thêm vật phẩm ngẫu nhiên trên cỏ (không phải vùng an toàn)
    if (!safeZone) {
        int r = std::rand() % 100;
        if (r < 25) { // 25% cơ hội ra mô hình siêu nhân (10 điểm)
            ItemData item;
            float itemX = static_cast<float>(50 + std::rand() % 700);
            item.shape.setRadius(14.f);
            item.shape.setFillColor(sf::Color(255, 215, 0));
            item.shape.setPosition(itemX, y + m_cellSize / 2.f - 14.f);
            item.collected = false;
            item.points = 10;
            item.type = ItemType::Superhero;
            row.items.push_back(item);
        } else if (r < 40) { // 15% cơ hội ra đồng hồ ngưng đọng thời gian
            ItemData item;
            float itemX = static_cast<float>(50 + std::rand() % 700);
            item.shape.setRadius(23.f); // Đồng hồ to nổi bật
            item.shape.setFillColor(sf::Color(0, 229, 255));
            item.shape.setPosition(itemX, y + m_cellSize / 2.f - 23.f);
            item.collected = false;
            item.points = 0;
            item.type = ItemType::Clock;
            row.items.push_back(item);
        }
    }

    m_terrains.push_back(std::move(row));
}

// ============================================================
// Tạo hàng đường với xe cộ
// ============================================================
void GameState::createRoadRow(float y) {
    TerrainRow row;
    row.type = TerrainType::Road;
    row.yPosition = y;

    row.background.setSize(sf::Vector2f(800.f, m_cellSize));
    row.background.setPosition(0.f, y);
    row.background.setFillColor(sf::Color(80, 80, 80)); // Xám đường

    // 18% cơ hội xuất hiện đồng hồ ngưng đọng thời gian trên làn đường xe chạy
    if (std::rand() % 100 < 18) {
        ItemData item;
        float itemX = static_cast<float>(80 + std::rand() % 640);
        item.shape.setRadius(23.f); // Đồng hồ to nổi bật
        item.shape.setFillColor(sf::Color(0, 229, 255));
        item.shape.setPosition(itemX, y + m_cellSize / 2.f - 23.f);
        item.collected = false;
        item.points = 0;
        item.type = ItemType::Clock;
        row.items.push_back(item);
    }

    // Vẽ vạch kẻ đường (trang trí, lưu vào obstacles sẽ phức tạp quá)
    // Tạo 2-4 xe ngẫu nhiên trên mỗi hàng đường
    int numVehicles = 2 + std::rand() % 3;
    // Dùng toạ độ y để luân phiên hướng chạy, đảm bảo các hàng liền kề luôn đi ngược chiều nhau
    bool movingRight = (static_cast<int>(std::round(std::abs(y) / m_cellSize)) % 2 == 0);
    float baseSpeed = 80.f + static_cast<float>(m_level * 15 + std::rand() % 40);

    for (int v = 0; v < numVehicles; v++) {
        float startX;
        if (movingRight) {
            float spacing = 900.f / numVehicles;
            startX = -150.f + static_cast<float>(v) * spacing + static_cast<float>(std::rand() % 150 - 50);
        } else {
            float spacing = 900.f / numVehicles;
            startX = 950.f - static_cast<float>(v) * spacing - static_cast<float>(std::rand() % 150 - 50);
        }

        // Tốc độ thay đổi ngẫu nhiên từng xe (thêm bớt 30) để có hiện tượng vượt nhau.
        float speed = baseSpeed + static_cast<float>(std::rand() % 60 - 30);
        if (speed < 40.f) speed = 40.f;

        int direction = movingRight ? 1 : -1;
        
        // 35% cơ hội chiếc xe đầu tiên trong làn là "Xe Điên" (chạy siêu tốc x2.3 và xả khói)
        bool isCrazy = (v == 0 && std::rand() % 100 < 35);
        if (isCrazy) {
            float crazySpeed = (baseSpeed + 60.f) * 2.3f;
            row.vehicles.push_back(std::make_unique<CCAR>(startX, y + 2.f, crazySpeed, direction, true));
        } else if (std::rand() % 100 < 70) {
            row.vehicles.push_back(std::make_unique<CCAR>(startX, y + 2.f, speed, direction, false));
        } else {
            row.vehicles.push_back(std::make_unique<CTRUCK>(startX, y + 2.f, speed, direction));
        }
    }

    m_terrains.push_back(std::move(row));
}

// ============================================================
// Tạo hàng sông với khúc gỗ
// ============================================================
void GameState::createRiverRow(float y) {
    TerrainRow row;
    row.type = TerrainType::River;
    row.yPosition = y;

    row.background.setSize(sf::Vector2f(800.f, m_cellSize));
    row.background.setPosition(0.f, y);
    row.background.setFillColor(sf::Color(30, 100, 200)); // Xanh nước

    // Tạo 3-5 khúc gỗ trên sông
    int numLogs = 3 + std::rand() % 3;
    // Dùng toạ độ y để luân phiên hướng chạy, tránh 2 hàng gỗ đi cùng chiều bị so le làm kẹt người chơi
    bool movingRight = (static_cast<int>(std::round(std::abs(y) / m_cellSize)) % 2 == 0);
    float logSpeed = 50.f + static_cast<float>(m_level * 10 + std::rand() % 30);

    for (int p = 0; p < numLogs; p++) {
        Log log;
        float logWidth = 55.f + static_cast<float>(std::rand() % 25);
        float logHeight = m_cellSize - 8.f;

        log.shape.setSize(sf::Vector2f(logWidth, logHeight));
        float startX = static_cast<float>(p * 170 + std::rand() % 60);
        log.shape.setPosition(startX, y + 4.f);
        log.shape.setFillColor(sf::Color(139, 90, 43)); // Nâu gỗ (fallback khi không có texture)
        log.shape.setOutlineColor(sf::Color(80, 50, 20));
        log.shape.setOutlineThickness(1.f);

        log.speed = logSpeed;
        log.movingRight = movingRight;

        row.logs.push_back(log);
    }

    m_terrains.push_back(std::move(row));
}

// ============================================================
// Tạo hàng đường ray và tàu hoả
// ============================================================
void GameState::createRailwayRow(float y) {
    TerrainRow row;
    row.type = TerrainType::Railway;
    row.yPosition = y;

    row.background.setSize(sf::Vector2f(800.f, m_cellSize));
    row.background.setPosition(0.f, y);
    row.background.setFillColor(sf::Color(70, 50, 30)); // Nền sỏi đường ray

    // Khởi tạo đèn giao thông
    row.trafficLight.state = LightState::Green;
    row.trafficLight.timer = 3.f + static_cast<float>(std::rand() % 3); // 3-5s đèn xanh

    // Khởi tạo tàu hoả
    row.train.movingRight = false; // Luôn chạy từ phải sang trái vì đầu tàu hướng sang trái
    row.train.speed = 1200.f; // Tàu chạy cực nhanh
    row.train.isActive = false;

    float trainWidth = 800.f; // Tàu dài bằng màn hình
    float trainHeight = m_cellSize;
    row.train.shape.setSize(sf::Vector2f(trainWidth, trainHeight));
    row.train.shape.setPosition(800.f + 100.f, y);

    m_terrains.push_back(std::move(row));
}

// ============================================================
// Khởi tạo HUD hiển thị level và điểm
// ============================================================
void GameState::initHUD() {
    auto& labelTex = TextureManager::getInstance().get("assets/textures/label.png");
    if (labelTex.getSize().x > 0) {
        m_hudBoardLoaded = true;
        m_hudBoardSprite.setTexture(labelTex);
        // Tâm hiển thị thực tế của bảng gỗ trong ảnh label.png (loại bỏ viền trong suốt)
        m_hudBoardSprite.setOrigin(184.f, 113.f);
        m_hudBoardSprite.setScale(145.f / 324.f, 62.f / 184.f);
        m_hudBoardSprite.setPosition(80.f, 36.f);
    } else {
        // Fallback panel nền mờ cho HUD nếu không tải được ảnh
        m_hudBg.setSize(sf::Vector2f(140.f, 60.f));
        m_hudBg.setOrigin(70.f, 30.f);
        m_hudBg.setPosition(80.f, 36.f);
        m_hudBg.setFillColor(sf::Color(0, 0, 0, 160));
    }

    m_levelText.setFont(m_font);
    m_levelText.setCharacterSize(15);
    m_levelText.setFillColor(sf::Color(253, 216, 53)); // Vàng nổi bật #fdd835
    m_levelText.setOutlineColor(sf::Color::Black);
    m_levelText.setOutlineThickness(1.5f);
    m_levelText.setStyle(sf::Text::Bold);

    m_scoreText.setFont(m_font);
    m_scoreText.setCharacterSize(15);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setOutlineColor(sf::Color::Black);
    m_scoreText.setOutlineThickness(1.5f);
    m_scoreText.setStyle(sf::Text::Bold);

    updateHUDText();
}

void GameState::updateHUDText() {
    float boardCenterX = 80.f;

    m_levelText.setString("Level: " + std::to_string(m_level));
    sf::FloatRect lb = m_levelText.getLocalBounds();
    m_levelText.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
    m_levelText.setPosition(boardCenterX, 25.f);

    m_scoreText.setString("Score: " + std::to_string(m_score));
    sf::FloatRect sb = m_scoreText.getLocalBounds();
    m_scoreText.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
    m_scoreText.setPosition(boardCenterX, 47.f);
}

// ============================================================
// Khởi tạo overlay Game Over và Pause
// ============================================================
void GameState::initOverlays() {
    m_texGameOverLoaded = m_texGameOver.loadFromFile("assets/textures/game_over.png");
    m_texConfirmLoaded = m_texConfirm.loadFromFile("assets/textures/confirm_button.png");
    m_texYesLoaded = m_texYes.loadFromFile("assets/textures/yes_text.png");
    m_texNoLoaded = m_texNo.loadFromFile("assets/textures/no_text.png");
    m_texPlayAgainLoaded = m_texPlayAgain.loadFromFile("assets/textures/play_again_text.png");
    m_texHomeLoaded = m_texHome.loadFromFile("assets/textures/main_menu_text.png");
    m_texLabelLoaded = m_texLabel.loadFromFile("assets/textures/label.png");
    m_texEnterNameTextLoaded = m_texEnterNameText.loadFromFile("assets/textures/enter_name_text.png");
    m_texNameExistsTextLoaded = m_texNameExistsText.loadFromFile("assets/textures/name_exists_text.png");
    m_texNameScoreRankTextLoaded = m_texNameScoreRankText.loadFromFile("assets/textures/name_score_rank_text.png");

    bool gL = m_goldMedalTex.loadFromFile("assets/textures/gold_medal.png");
    bool sL = m_silverMedalTex.loadFromFile("assets/textures/silver_medal.png");
    bool bL = m_bronzeMedalTex.loadFromFile("assets/textures/bronze_medal.png");
    m_medalsLoaded = gL && sL && bL;

    if (m_texLabelLoaded) {
        m_labelSprite.setTexture(m_texLabel);
        sf::Vector2u texSize = m_texLabel.getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            m_labelSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
        }
    }

    if (m_texEnterNameTextLoaded) {
        m_enterNameTextSprite.setTexture(m_texEnterNameText);
        sf::Vector2u size = m_texEnterNameText.getSize();
        if (size.x > 0 && size.y > 0) {
            m_enterNameTextSprite.setOrigin(size.x / 2.f, size.y / 2.f);
            m_enterNameTextSprite.setScale(220.f / static_cast<float>(size.x), 32.f / static_cast<float>(size.y));
        }
        m_enterNameTextSprite.setPosition(400.f, 230.f);
    }

    if (m_texNameExistsTextLoaded) {
        m_nameExistsTextSprite.setTexture(m_texNameExistsText);
        sf::Vector2u size = m_texNameExistsText.getSize();
        if (size.x > 0 && size.y > 0) {
            m_nameExistsTextSprite.setOrigin(size.x / 2.f, size.y / 2.f);
        }
    }

    if (m_texNameScoreRankTextLoaded) {
        m_nameScoreRankTextSprite.setTexture(m_texNameScoreRankText);
        sf::Vector2u size = m_texNameScoreRankText.getSize();
        if (size.x > 0 && size.y > 0) {
            m_nameScoreRankTextSprite.setOrigin(0.f, 0.f);
        }
    }

    // Game Over overlay
    m_gameOverOverlay.setSize(sf::Vector2f(800.f, 600.f));
    m_gameOverOverlay.setFillColor(sf::Color(0, 0, 0, 160));

    if (m_texGameOverLoaded) {
        m_gameOverSprite.setTexture(m_texGameOver);
        sf::Vector2u texSize = m_texGameOver.getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            m_gameOverSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
            m_gameOverSprite.setScale(300.f / static_cast<float>(texSize.x), 80.f / static_cast<float>(texSize.y));
        }
        m_gameOverSprite.setPosition(400.f, 150.f);
    }

    m_gameOverText.setFont(m_font);
    m_gameOverText.setString("GAME OVER");
    m_gameOverText.setCharacterSize(56);
    m_gameOverText.setFillColor(sf::Color::Red);
    m_gameOverText.setStyle(sf::Text::Bold);
    sf::FloatRect goBounds = m_gameOverText.getLocalBounds();
    m_gameOverText.setOrigin(goBounds.left + goBounds.width / 2.f,
                              goBounds.top + goBounds.height / 2.f);
    m_gameOverText.setPosition(400.f, 250.f);

    // Pause overlay
    m_pauseOverlay.setSize(sf::Vector2f(800.f, 600.f));
    m_pauseOverlay.setFillColor(sf::Color(0, 0, 0, 140));

    m_pauseText.setFont(m_font);
    m_pauseText.setString("PAUSED");
    m_pauseText.setCharacterSize(56);
    m_pauseText.setFillColor(sf::Color(255, 215, 0));
    m_pauseText.setStyle(sf::Text::Bold);
    sf::FloatRect pBounds = m_pauseText.getLocalBounds();
    m_pauseText.setOrigin(pBounds.left + pBounds.width / 2.f,
                           pBounds.top + pBounds.height / 2.f);
    m_pauseText.setPosition(400.f, 260.f);

    m_pauseInstruction.setFont(m_font);
    m_pauseInstruction.setString("Press P or Escape to Resume\nPress S to Save Game\nPress Q to Quit to Menu");
    m_pauseInstruction.setCharacterSize(18);
    m_pauseInstruction.setFillColor(sf::Color::White);
    sf::FloatRect piBounds = m_pauseInstruction.getLocalBounds();
    m_pauseInstruction.setOrigin(piBounds.left + piBounds.width / 2.f,
                                  piBounds.top + piBounds.height / 2.f);
    m_pauseInstruction.setPosition(400.f, 330.f);

    // HUD Button textures
    bool b1 = m_texHudBack.loadFromFile("assets/textures/back_button.png");
    bool b2 = m_texHudPause.loadFromFile("assets/textures/pause_button.png");
    bool b3 = m_texHudContinue.loadFromFile("assets/textures/continue_button.png");
    bool b4 = m_texHudSave.loadFromFile("assets/textures/save_button.png");
    m_texHudButtonsLoaded = b1 && b2 && b3 && b4;

    if (m_texHudButtonsLoaded) {
        auto onBackClick = [this]() {
            m_deferredAction = DeferredAction::Quit;
        };
        m_hudBtnBack = std::make_unique<Button>(20.f, 530.f, 50.f, 50.f, m_texHudBack, onBackClick);

        auto onSaveClick = [this]() {
            m_paused = true;
            m_pauseUIState = PauseUIState::EnterSaveName;
            if (m_saveNameInput) {
                m_saveNameInput->clear();
                m_saveNameInput->setActive(true);
            }
            updatePauseButtonTexture();
        };
        m_hudBtnSave = std::make_unique<Button>(140.f, 530.f, 50.f, 50.f, m_texHudSave, onSaveClick);

        updatePauseButtonTexture();
    }

    // Save Game UI in Pause
    m_saveNameBoard.setSize(sf::Vector2f(350.f, 150.f));
    m_saveNameBoard.setFillColor(sf::Color(50, 50, 50, 240));
    m_saveNameBoard.setOutlineColor(sf::Color::White);
    m_saveNameBoard.setOutlineThickness(2.f);
    m_saveNameBoard.setOrigin(175.f, 75.f);
    m_saveNameBoard.setPosition(400.f, 300.f);

    m_saveNamePrompt.setFont(m_font);
    m_saveNamePrompt.setString("Enter Save Name:");
    m_saveNamePrompt.setCharacterSize(18);
    m_saveNamePrompt.setFillColor(sf::Color::White);
    sf::FloatRect prBounds = m_saveNamePrompt.getLocalBounds();
    m_saveNamePrompt.setOrigin(prBounds.left + prBounds.width / 2.f, prBounds.top + prBounds.height / 2.f);
    m_saveNamePrompt.setPosition(400.f, 230.f);

    m_saveNameInput = std::make_unique<TextBox>(290.f, 270.f, 220.f, 40.f, m_font);

    auto onSaveOk = [this]() {
        if (!m_saveNameInput->getString().empty()) {
            std::string inputName = m_saveNameInput->getString();
            m_currentSaveSession = SaveManager::getUniqueSaveFileName(inputName);
            saveCurrentGameState(m_currentSaveSession);
            m_pauseUIState = PauseUIState::Main;
            m_pauseInstruction.setString("Game Saved to " + m_currentSaveSession + "!\nPress P or Escape to Resume\nPress Q to Quit to Menu");
        }
    };

    if (m_texConfirmLoaded) {
        m_btnSaveOk = std::make_unique<Button>(340.f, 320.f, 120.f, 36.f, m_texConfirm, onSaveOk);
    } else {
        m_btnSaveOk = std::make_unique<Button>(350.f, 320.f, 100.f, 35.f, "OK", m_font, onSaveOk);
    }
}

void GameState::updatePauseButtonTexture() {
    if (!m_texHudButtonsLoaded) return;
    auto onPauseClick = [this]() {
        m_paused = !m_paused;
        if (m_paused) m_pauseUIState = PauseUIState::Main;
        updatePauseButtonTexture();
    };
    // Tỷ lệ ảnh pause/continue_button.png có viền lớn hơn back/save_button.png 14%, 
    // điều chỉnh kích thước 43x43 tại (83.5, 533.5) để 3 nút đồng kích thước khung viền 100%
    if (m_paused) {
        m_hudBtnPause = std::make_unique<Button>(83.5f, 533.5f, 43.f, 43.f, m_texHudContinue, onPauseClick);
    } else {
        m_hudBtnPause = std::make_unique<Button>(83.5f, 533.5f, 43.f, 43.f, m_texHudPause, onPauseClick);
    }
}

void GameState::saveCurrentGameState(const std::string& sessionName) {
    SaveData data;
    data.playerName = "Player";
    data.score = m_score;
    data.level = m_level;
    data.playerX = m_player->getPosition().x;
    data.playerY = m_player->getPosition().y;
    data.maxPlayerY = m_maxPlayerY;
    data.timeFreezeTimer = m_timeFreezeTimer;
    
    data.numTerrains = static_cast<int>(m_terrains.size());
    for (auto& row : m_terrains) {
        SavedTerrainRow sRow;
        sRow.type = static_cast<int>(row.type);
        sRow.yPosition = row.yPosition;
        
        for (auto& v : row.vehicles) {
            SavedVehicle sVeh;
            sVeh.type = (dynamic_cast<CCAR*>(v.get()) != nullptr) ? 0 : 1;
            sVeh.x = v->getPosition().x;
            sVeh.y = v->getPosition().y;
            sVeh.speed = v->getSpeed();
            sVeh.direction = v->getDirection();
            sVeh.isCrazy = v->isCrazy();
            sRow.vehicles.push_back(sVeh);
        }
        

        for (auto& log : row.logs) {
            SavedLilyPad sPad;
            sPad.x = log.shape.getPosition().x;
            sPad.y = log.shape.getPosition().y;
            sPad.width = log.shape.getSize().x;
            sPad.height = log.shape.getSize().y;
            sPad.speed = log.speed;
            sPad.movingRight = log.movingRight;
            sf::Color c = log.shape.getFillColor();
            sPad.r = c.r; sPad.g = c.g; sPad.b = c.b;
            sRow.lilyPads.push_back(sPad);
        }
        
        for (auto& item : row.items) {
            SavedItem sItem;
            sItem.x = item.shape.getPosition().x;
            sItem.y = item.shape.getPosition().y;
            sItem.collected = item.collected;
            sItem.points = item.points;
            sItem.type = static_cast<int>(item.type);
            sRow.items.push_back(sItem);
        }
        
        data.terrains.push_back(std::move(sRow));
    }
    data.playTimeSeconds = static_cast<int>(m_totalPlaytime);
    
    std::time_t t = std::time(nullptr);
    char buf[100];
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t))) {
        data.timestamp = buf;
    } else {
        data.timestamp = "Unknown Time";
    }
    
    SaveManager::saveGame(sessionName, data);
}

// ============================================================
// Xử lý input từ người chơi
// ============================================================
void GameState::handleInput(sf::RenderWindow& window, sf::Event& event) {
    // Xử lý sự kiện cho 3 nút HUD (Back, Pause/Continue, Save)
    if (!m_playerDead && m_texHudButtonsLoaded) {
        if (m_hudBtnBack) m_hudBtnBack->handleEvent(event, window);
        if (m_hudBtnPause) m_hudBtnPause->handleEvent(event, window);
        if (m_hudBtnSave) m_hudBtnSave->handleEvent(event, window);
    }

    // Nếu đã chết
    if (m_playerDead) {
        if (m_goState == GameOverUIState::EnterName) {
            if (m_nameInput) m_nameInput->handleEvent(event, window);
            if (m_btnOk) m_btnOk->handleEvent(event, window);
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (m_submitNameFunc) m_submitNameFunc();
            }
        } else if (m_goState == GameOverUIState::ConfirmIdentity) {
            if (m_btnYes) m_btnYes->handleEvent(event, window);
            if (m_btnNo) m_btnNo->handleEvent(event, window);
        } else if (m_goState == GameOverUIState::ShowScore) {
            if (m_playAgainBtn) m_playAgainBtn->handleEvent(event, window);
            if (m_menuBtn) m_menuBtn->handleEvent(event, window);
        }
        return;
    }
    
    // Nếu đang pause
    if (m_paused) {
        if (m_pauseUIState == PauseUIState::EnterSaveName) {
            if (m_ignoreNextText && event.type == sf::Event::TextEntered) {
                m_ignoreNextText = false;
                return;
            }
            if (m_saveNameInput) m_saveNameInput->handleEvent(event, window);
            if (m_btnSaveOk) m_btnSaveOk->handleEvent(event, window);
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (!m_saveNameInput->getString().empty()) {
                    std::string inputName = m_saveNameInput->getString();
                    m_currentSaveSession = SaveManager::getUniqueSaveFileName(inputName);
                    saveCurrentGameState(m_currentSaveSession);
                    m_pauseUIState = PauseUIState::Main;
                    m_pauseInstruction.setString("Game Saved to " + m_currentSaveSession + "!\nPress P or Escape to Resume\nPress Q to Quit to Menu");
                }
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                m_pauseUIState = PauseUIState::Main;
            }
        } else {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P ||
                    event.key.code == sf::Keyboard::Escape) {
                    m_paused = false;
                    updatePauseButtonTexture();
                } else if (event.key.code == sf::Keyboard::S) {
                    if (!m_currentSaveSession.empty()) {
                        saveCurrentGameState(m_currentSaveSession);
                        m_pauseInstruction.setString("Game Saved to " + m_currentSaveSession + "!\nPress P or Escape to Resume\nPress Q to Quit to Menu");
                    } else {
                        m_pauseUIState = PauseUIState::EnterSaveName;
                        if (m_saveNameInput) {
                            m_saveNameInput->clear();
                            m_saveNameInput->setActive(true);
                        }
                        m_ignoreNextText = true;
                    }
                } else if (event.key.code == sf::Keyboard::Q) {
                    if (!m_currentSaveSession.empty()) {
                        saveCurrentGameState(m_currentSaveSession);
                    }
                    m_deferredAction = DeferredAction::Quit;
                }
            }
        }
        return;
    }
    
    if (event.type == sf::Event::KeyPressed) {
        // Điều khiển nhân vật - Player 1: WASD, Player 2: Arrow keys
        switch (event.key.code) {
        case sf::Keyboard::W:
        case sf::Keyboard::Up:
            movePlayer(0.f, -m_cellSize);
            break;
        case sf::Keyboard::S:
        case sf::Keyboard::Down:
            movePlayer(0.f, m_cellSize);
            break;
        case sf::Keyboard::A:
        case sf::Keyboard::Left:
            movePlayer(-m_cellSize, 0.f);
            break;
        case sf::Keyboard::D:
        case sf::Keyboard::Right:
            movePlayer(m_cellSize, 0.f);
            break;
        case sf::Keyboard::P:
        case sf::Keyboard::Escape:
            m_paused = true;
            updatePauseButtonTexture();
            break;
        default:
            break;
        }
    }
}

// ============================================================
// Di chuyển người chơi với kiểm tra giới hạn màn hình
// ============================================================
void GameState::movePlayer(float dx, float dy) {
    if (m_moveCooldownTimer > 0.f) return;
    if (m_player->m_isAnimating) return; // Chặn di chuyển nếu đang LERP animation
    m_moveCooldownTimer = 0.15f; // Thời gian chờ giữa các lần di chuyển là 0.15s (ngăn spam)

    // Không cho phép đi ngang khi đang trên khúc gỗ (sông)
    if (dx != 0.f) {
        for (auto& row : m_terrains) {
            if (row.type == TerrainType::River) {
                sf::FloatRect rowBounds = row.background.getGlobalBounds();
                float playerCenterY = m_player->getPosition().y + m_playerSize / 2.f;
                if (playerCenterY > rowBounds.top && playerCenterY < rowBounds.top + rowBounds.height) {
                    return; // Chặn di chuyển trái phải
                }
            }
        }
    }

    sf::Vector2f newPos = m_player->getPosition() + sf::Vector2f(dx, dy);

    // Giới hạn trong màn hình
    if (newPos.x < 0.f) newPos.x = 0.f;
    if (newPos.x > 800.f - m_playerSize) newPos.x = 800.f - m_playerSize;
    if (newPos.y > 600.f - m_cellSize) newPos.y = 600.f - m_cellSize;

    // Không còn ép grid X nữa, người chơi nhảy thẳng tắp từ vị trí hiện tại

    // Gọi startMove để kích hoạt animation
    m_player->startMove(newPos.x - m_player->getPosition().x, newPos.y - m_player->getPosition().y);
}

// ============================================================
// Cập nhật trạng thái trò chơi (chạy mỗi frame)
// ============================================================
void GameState::update(float dt) {
    if (m_moveCooldownTimer > 0.f) m_moveCooldownTimer -= dt;

    if (!m_paused && !m_playerDead) {
        m_totalPlaytime += dt;
    }

    if (m_deferredAction == DeferredAction::Restart) {
        m_deferredAction = DeferredAction::None;
        m_level = 1; m_score = 0;
        initPlayer(); generateMap(); initHUD();
        m_playerDead = false;
        m_goState = GameOverUIState::None;
        Game::instance().playBackgroundMusic("assets/audio/bgm_gameplay.ogg");
        return;
    } else if (m_deferredAction == DeferredAction::Quit) {
        m_deferredAction = DeferredAction::None;
        Game::instance().getStateMachine().popState();
        return;
    }

    if (m_playerDead) {
        if (m_goState == GameOverUIState::Delay) {
            m_deathTimer += dt;
            if (m_player) m_player->update(dt);
            if (m_deathTimer >= 1.2f) {
                setupGameOverUI();
            }
        } else if (m_goState == GameOverUIState::EnterName) {
            if (m_nameInput) m_nameInput->update(dt);
            if (m_btnOk) m_btnOk->update(dt);
        } else if (m_goState == GameOverUIState::ConfirmIdentity) {
            if (m_btnYes) m_btnYes->update(dt);
            if (m_btnNo) m_btnNo->update(dt);
        } else if (m_goState == GameOverUIState::ShowScore) {
            if (m_playAgainBtn) m_playAgainBtn->update(dt);
            if (m_menuBtn) m_menuBtn->update(dt);
        }
        return;
    }
    
    if (m_paused) return;

    // Cập nhật ngưng đọng thời gian
    if (m_timeFreezeTimer > 0.f) {
        m_timeFreezeTimer -= dt;
        if (m_timeFreezeTimer < 0.f) m_timeFreezeTimer = 0.f;
    } else {
        updateObstacles(dt);
        updateLilyPads(dt);
        updateRailway(dt);
    }

    checkCollisions(dt);
    checkWinCondition();
    
    // m_player has its own update and animation logic if we use it, but for now we just call update.
    m_player->update(dt);

    // Cập nhật HUD
    updateHUDText();
}

// ============================================================
// Cập nhật vị trí xe cộ trên đường
// ============================================================
void GameState::updateObstacles(float dt) {
    for (auto& row : m_terrains) {
        if (row.type == TerrainType::Road) {
            for (auto& v : row.vehicles) {
                v->update(dt);
                
                // Xe điên bóp còi báo động to rõ khi vừa lao vào màn hình
                if (v->isCrazy()) {
                    float vx = v->getPosition().x;
                    if (vx >= -40.f && vx <= 840.f && !v->hasHonked()) {
                        Game::instance().playSound("assets/audio/sfx_car_horn.wav");
                        v->setHonked(true);
                    }
                }
                
                // Wrap quanh khi xe vừa thoát hết khỏi màn hình
                if (v->getDirection() > 0 && v->getPosition().x > 860.f) {
                    v->setPosition(-180.f, v->getPosition().y);
                    v->setHonked(false); // Reset còi cho vòng chạy tiếp theo
                    // Random lại tốc độ khi quay vòng
                    float baseSpeed = 80.f + static_cast<float>(m_level * 15);
                    float newSpeed = v->isCrazy() ? ((baseSpeed + 60.f) * 2.3f) : (baseSpeed + static_cast<float>(std::rand() % 100 - 30));
                    if (newSpeed < 40.f) newSpeed = 40.f;
                    v->setSpeed(newSpeed);
                } else if (v->getDirection() < 0 && v->getPosition().x < -90.f) {
                    v->setPosition(860.f, v->getPosition().y);
                    v->setHonked(false); // Reset còi cho vòng chạy tiếp theo
                    // Random lại tốc độ khi quay vòng
                    float baseSpeed = 80.f + static_cast<float>(m_level * 15);
                    float newSpeed = v->isCrazy() ? ((baseSpeed + 60.f) * 2.3f) : (baseSpeed + static_cast<float>(std::rand() % 100 - 30));
                    if (newSpeed < 40.f) newSpeed = 40.f;
                    v->setSpeed(newSpeed);
                }
            }
        }
    }
}

// ============================================================
// Cập nhật vị trí khúc gỗ trôi trên sông
// ============================================================
void GameState::updateLilyPads(float dt) {
    for (auto& row : m_terrains) {
        if (row.type != TerrainType::River) continue;

        for (auto& log : row.logs) {
            float moveX = log.speed * dt * (log.movingRight ? 1.f : -1.f);
            log.shape.move(moveX, 0.f);

            // Quay vòng khi ra khỏi màn hình
            sf::Vector2f pos = log.shape.getPosition();
            float w = log.shape.getSize().x;
            if (log.movingRight && pos.x > 820.f) {
                log.shape.setPosition(-w, pos.y);
            } else if (!log.movingRight && pos.x + w < -20.f) {
                log.shape.setPosition(820.f, pos.y);
            }
        }
    }
}

// ============================================================
// Cập nhật tín hiệu đèn giao thông và tàu hoả
// ============================================================
void GameState::updateRailway(float dt) {
    for (auto& row : m_terrains) {
        if (row.type != TerrainType::Railway) continue;

        // Cập nhật hoạt ảnh đèn giao thông
        row.trafficLight.animTimer += dt;
        if (row.trafficLight.animTimer >= 0.15f) { // Chuyển frame mỗi 0.15s
            row.trafficLight.animTimer = 0.f;
            row.trafficLight.frameIndex++;
        }

        row.trafficLight.timer -= dt;

        if (row.trafficLight.state == LightState::Green) {
            if (row.trafficLight.timer <= 0.f) {
                row.trafficLight.state = LightState::Blinking;
                row.trafficLight.timer = 2.f; // 2 giây nhấp nháy
                Game::instance().playSound("assets/audio/sfx_train_horn.wav");
            }
        } else if (row.trafficLight.state == LightState::Blinking) {
            if (row.trafficLight.timer <= 0.f) {
                row.trafficLight.state = LightState::Red;
                row.train.isActive = true;
                
                // Cập nhật lại chiều rộng hitbox tàu cho khớp với ảnh mới
                if (m_texturesLoaded && m_trainSprite.getTexture()) {
                    auto texSize = m_trainSprite.getTexture()->getSize();
                    float scale = 96.f / texSize.y; // Tàu cao 2 ô
                    row.train.shape.setSize(sf::Vector2f(texSize.x * scale, m_cellSize));
                }

                // Đặt lại vị trí tàu (chỉ đi từ phải qua trái)
                row.train.shape.setPosition(800.f + 100.f, row.yPosition);
            }
        } else if (row.trafficLight.state == LightState::Red) {
            if (row.train.isActive) {
                float moveX = row.train.speed * dt * (row.train.movingRight ? 1.f : -1.f);
                row.train.shape.move(moveX, 0.f);

                sf::Vector2f pos = row.train.shape.getPosition();
                float w = row.train.shape.getSize().x;
                
                // Kiểm tra tàu đi qua hết màn hình
                if (row.train.movingRight && pos.x > 800.f) {
                    row.train.isActive = false;
                } else if (!row.train.movingRight && pos.x + w < 0.f) {
                    row.train.isActive = false;
                }
            } else {
                // Tàu đã chạy xong, trở về đèn xanh
                row.trafficLight.state = LightState::Green;
                row.trafficLight.timer = 3.f + static_cast<float>(std::rand() % 4);
            }
        }
    }
}

// ============================================================
// Kiểm tra va chạm giữa người chơi và các vật thể
// ============================================================
void GameState::checkCollisions(float dt) {
    sf::FloatRect playerBounds = m_player->getBounds();
    // Thu nhỏ hitbox để gameplay fair hơn và khớp với hình ảnh nhân vật (bỏ viền trong suốt)
    sf::FloatRect playerHitbox(
        playerBounds.left + 12.f, playerBounds.top + 10.f, // Cắt viền trái 12px
        playerBounds.width - 24.f, playerBounds.height - 16.f // Cắt viền phải 12px (tổng 24)
    );

    for (auto& row : m_terrains) {
        // Kiểm tra va chạm xe cộ
        if (row.type == TerrainType::Road) {
            for (auto& v : row.vehicles) {
                sf::FloatRect carHitbox = v->getBounds();
                carHitbox.left += 15.f; // Cắt viền trái
                carHitbox.width -= 30.f; // Cắt viền phải
                carHitbox.top += 8.f; // Cắt viền trên
                carHitbox.height -= 16.f; // Cắt viền dưới

                if (playerHitbox.intersects(carHitbox)) {
                    if (!m_playerDead) {
                        m_playerDead = true;
                        Game::instance().playSound("assets/audio/sfx_car_hit.wav");
                        m_player->die(DeathType::HitByCar, m_hitByCarLoaded ? &m_hitByCarTexture : nullptr);
                        m_deathTimer = 0.f;
                        m_goState = GameOverUIState::Delay;
                        if (!m_currentSaveSession.empty()) {
                            SaveManager::deleteGame(m_currentSaveSession);
                            m_currentSaveSession = "";
                        }
                    }
                    return;
                }
            }
        }

        // [Animals removed from gameplay - no collision check needed]

        // Kiểm tra va chạm tàu hoả
        if (row.type == TerrainType::Railway) {
            if (row.train.isActive) {
                sf::FloatRect trainHitbox = row.train.shape.getGlobalBounds();
                trainHitbox.top += 8.f;
                trainHitbox.height -= 16.f;

                if (playerHitbox.intersects(trainHitbox)) {
                    if (!m_playerDead) {
                        m_playerDead = true;
                        Game::instance().playSound("assets/audio/sfx_train_hit.wav");
                        m_player->die(DeathType::HitByCar, m_hitByCarLoaded ? &m_hitByCarTexture : nullptr);
                        m_deathTimer = 0.f;
                        m_goState = GameOverUIState::Delay;
                        if (!m_currentSaveSession.empty()) {
                            SaveManager::deleteGame(m_currentSaveSession);
                            m_currentSaveSession = "";
                        }
                    }
                    return;
                }
            }
        }

        // Kiểm tra sông: phải đứng trên khúc gỗ
        if (row.type == TerrainType::River) {
            sf::FloatRect rowBounds = row.background.getGlobalBounds();
            // Mở rộng X của sông ra vô tận để bắt va chạm ngay cả khi player ra khỏi màn hình
            rowBounds.left = -10000.f;
            rowBounds.width = 20000.f;
            if (playerHitbox.intersects(rowBounds)) {
                // Nếu đang nhảy (animating), nhân vật lơ lửng trên không, 
                // bỏ qua kiểm tra chết đuối và không bị khúc gỗ đẩy đi để tránh chết oan khi đang nhảy vào khúc gỗ.
                if (m_player->m_isAnimating) {
                    continue; // Đang bay trên không -> bỏ qua
                }

                bool onLog = false;
                for (auto& log : row.logs) {
                    sf::FloatRect logHitbox = log.shape.getGlobalBounds();
                    // Thu nhỏ hitbox khúc gỗ để sát với hình ảnh gỗ thật (bỏ viền nước trong suốt)
                    logHitbox.left += 10.f;
                    logHitbox.width -= 20.f;

                    if (playerHitbox.intersects(logHitbox)) {
                        onLog = true;
                        // Chỉ di chuyển theo khúc gỗ nếu thời gian không bị ngưng đọng
                        if (m_timeFreezeTimer <= 0.f) {
                            float logMove = log.speed * dt * (log.movingRight ? 1.f : -1.f);
                            m_player->setPosition(m_player->getPosition().x + logMove, m_player->getPosition().y);

                            // Nếu bị đẩy ra ngoài màn hình thì chết
                            if (m_player->getPosition().x < -m_playerSize || m_player->getPosition().x > 800.f) {
                                if (!m_playerDead) {
                                    m_playerDead = true;
                                    Game::instance().playSound("assets/audio/sfx_water_splash.wav");
                                    m_player->die(DeathType::Drowned, m_playerDrownLoaded ? &m_playerDrownTexture : nullptr);
                                    m_deathTimer = 0.f;
                                    m_goState = GameOverUIState::Delay;
                                    if (!m_currentSaveSession.empty()) {
                                        SaveManager::deleteGame(m_currentSaveSession);
                                        m_currentSaveSession = "";
                                    }
                                }
                                return;
                            }
                        }
                        break;
                    }
                }
                if (!onLog) {
                    // Rơi xuống sông -> chết đuối
                    if (!m_playerDead) {
                        m_playerDead = true;
                        Game::instance().playSound("assets/audio/sfx_water_splash.wav");
                        m_player->die(DeathType::Drowned, m_playerDrownLoaded ? &m_playerDrownTexture : nullptr);
                        m_deathTimer = 0.f;
                        m_goState = GameOverUIState::Delay;
                        if (!m_currentSaveSession.empty()) {
                            SaveManager::deleteGame(m_currentSaveSession);
                            m_currentSaveSession = "";
                        }
                    }
                    return;
                }
            }
        }


        // Kiểm tra thu thập item (Mô hình siêu nhân & Đồng hồ ngưng đọng thời gian)
        for (auto& item : row.items) {
            if (!item.collected && playerHitbox.intersects(item.shape.getGlobalBounds())) {
                item.collected = true;
                if (item.type == ItemType::Clock) {
                    m_timeFreezeTimer = 1.0f; // 1.0 giây ngưng đọng thời gian
                    Game::instance().playSound("assets/audio/sfx_hovering.wav");
                } else {
                    m_score += item.points;
                    Game::instance().playSound("assets/audio/sfx_pick_up.wav");
                }
            }
        }
    }
}

// ============================================================
// Kiểm tra điều kiện thắng (qua được hết màn hình)
// ============================================================
void GameState::checkWinCondition() {
    if (m_player->getPosition().y <= 0.f) {
        Game::instance().playSound("assets/audio/sfx_next_level.wav");
        m_level++;
        m_score += 5; // Thưởng qua màn cố định là 5 điểm
        resetForNextLevel();
    }
}

// ============================================================
// Reset cho level tiếp theo
// ============================================================
void GameState::resetForNextLevel() {
    m_terrains.clear();
    generateMap();
    initPlayer();
}

// ============================================================
// Vẽ toàn bộ game lên màn hình
// ============================================================
void GameState::draw(sf::RenderWindow& window) {
    // Nền đen
    window.clear(sf::Color(20, 20, 20));

    // Vẽ các hàng địa hình từ trên (xa) xuống dưới (gần) để sửa lỗi đè hình (Z-index)
    for (auto it = m_terrains.rbegin(); it != m_terrains.rend(); ++it) {
        auto& row = *it;
        if (m_texturesLoaded) {
            if (row.type == TerrainType::Grass) { m_grassSprite.setPosition(0, row.yPosition); window.draw(m_grassSprite); }
            else if (row.type == TerrainType::Road) { m_roadSprite.setPosition(0, row.yPosition); window.draw(m_roadSprite); }
            else if (row.type == TerrainType::River) { m_riverSprite.setPosition(0, row.yPosition); window.draw(m_riverSprite); }
            else if (row.type == TerrainType::Railway) { m_trackSprite.setPosition(0, row.yPosition); window.draw(m_trackSprite); }
        } else {
            window.draw(row.background);
        }

        // Vẽ vạch kẻ đường cho đường
        if (row.type == TerrainType::Road && !m_texturesLoaded) {
            // Vạch kẻ giữa đường
            for (float x = 0.f; x < 800.f; x += 60.f) {
                sf::RectangleShape dashLine(sf::Vector2f(30.f, 3.f));
                dashLine.setPosition(x, row.yPosition + m_cellSize / 2.f - 1.5f);
                dashLine.setFillColor(sf::Color(200, 200, 200, 100));
                window.draw(dashLine);
            }
        }

        // Vẽ sóng nước cho sông
        if (row.type == TerrainType::River && !m_texturesLoaded) {
            for (float x = 10.f; x < 800.f; x += 40.f) {
                sf::RectangleShape wave(sf::Vector2f(20.f, 2.f));
                wave.setPosition(x, row.yPosition + m_cellSize / 2.f);
                wave.setFillColor(sf::Color(100, 180, 255, 60));
                window.draw(wave);
            }
        }

        // Vẽ items chưa thu thập trên mặt đất (Mô hình siêu nhân & Đồng hồ)
        // (Vẽ trước xe cộ và tàu hoả để khi xe chạy ngang qua sẽ che khuất vật phẩm)
        for (auto& item : row.items) {
            if (!item.collected) {
                float diam = item.shape.getRadius() * 2.f;
                if (item.type == ItemType::Clock && m_watchLoaded && m_watchSprite.getTexture()) {
                    auto texSize = m_watchSprite.getTexture()->getSize();
                    m_watchSprite.setScale(diam / texSize.x, diam / texSize.y);
                    m_watchSprite.setPosition(item.shape.getPosition());
                    window.draw(m_watchSprite);
                } else if (item.type == ItemType::Superhero && m_texturesLoaded && m_itemSprite.getTexture()) {
                    auto texSize = m_itemSprite.getTexture()->getSize();
                    m_itemSprite.setScale(diam / texSize.x, diam / texSize.y);
                    m_itemSprite.setPosition(item.shape.getPosition());
                    window.draw(m_itemSprite);
                } else {
                    window.draw(item.shape);
                }
            }
        }

        // Vẽ xe cộ (đè lên mặt đường và vật phẩm)
        if (row.type == TerrainType::Road) {
            for (auto& v : row.vehicles) {
                window.draw(*v);
            }
        }

        // Vẽ đèn giao thông và tàu hoả
        if (row.type == TerrainType::Railway) {
            // Vẽ đèn
            sf::Sprite* lightSprite = nullptr;
            int numFrames = 1;
            if (row.trafficLight.state == LightState::Green) {
                lightSprite = &m_lightGreenSprite;
                numFrames = 1;
            } else if (row.trafficLight.state == LightState::Blinking) {
                lightSprite = &m_lightBlinkSprite;
                numFrames = 2;
            } else if (row.trafficLight.state == LightState::Red) {
                lightSprite = &m_lightRedSprite;
                numFrames = 1;
            }

            if (m_texturesLoaded && lightSprite && lightSprite->getTexture()) {
                auto texSize = lightSprite->getTexture()->getSize();
                int frameW = texSize.x / numFrames;
                int currentFrame = row.trafficLight.frameIndex % numFrames;
                
                lightSprite->setTextureRect(sf::IntRect(currentFrame * frameW, 0, frameW, texSize.y));
                
                // Giữ nguyên aspect ratio, cho đèn cao bằng 2 ô (96px)
                float desiredHeight = 96.f; 
                float scale = desiredHeight / texSize.y;
                lightSprite->setScale(scale, scale);
                
                // Tính toán điểm neo (origin X) để bù trừ độ lệch cắt ảnh của các khung hình
                float originX = frameW / 2.f; 
                if (row.trafficLight.state == LightState::Green) {
                    originX = 85.f;
                } else if (row.trafficLight.state == LightState::Blinking) {
                    originX = (currentFrame == 0) ? 85.f : 116.f;
                } else if (row.trafficLight.state == LightState::Red) {
                    originX = 91.5f;
                }
                
                // Cố định điểm neo (origin) bằng toạ độ bù trừ chống giật
                lightSprite->setOrigin(originX, static_cast<float>(texSize.y));
                
                // Đặt vị trí cột đèn cắm xuống đáy của hàng hiện tại
                lightSprite->setPosition(750.f, row.yPosition + m_cellSize);
                
                window.draw(*lightSprite);
            } else {
                sf::CircleShape lightShape(10.f);
                lightShape.setPosition(750.f, row.yPosition + 14.f); // Chuyển sang bên phải
                if (row.trafficLight.state == LightState::Green) lightShape.setFillColor(sf::Color::Green);
                else if (row.trafficLight.state == LightState::Blinking) {
                    if (static_cast<int>(row.trafficLight.timer * 10) % 2 == 0) lightShape.setFillColor(sf::Color::Green);
                    else lightShape.setFillColor(sf::Color(100, 100, 100)); // Nhấp nháy tắt
                } else if (row.trafficLight.state == LightState::Red) lightShape.setFillColor(sf::Color::Red);
                window.draw(lightShape);
            }

            // Vẽ tàu
            if (row.train.isActive) {
                if (m_texturesLoaded && m_trainSprite.getTexture()) {
                    auto texSize = m_trainSprite.getTexture()->getSize();
                    // Giữ nguyên aspect ratio, cho tàu cao gấp đôi (96px)
                    float scale = 96.f / texSize.y;
                    m_trainSprite.setScale(scale, scale);
                    // Đặt origin ở góc dưới cùng bên trái
                    m_trainSprite.setOrigin(0.f, static_cast<float>(texSize.y));
                    // Vẽ ở vị trí chân tàu (đáy của ô)
                    m_trainSprite.setPosition(row.train.shape.getPosition().x, row.yPosition + m_cellSize);
                    window.draw(m_trainSprite);
                } else {
                    row.train.shape.setFillColor(sf::Color(50, 50, 50));
                    window.draw(row.train.shape);
                }
            }
        }

        // Vẽ khúc gỗ
        for (auto& log : row.logs) {
            if (m_texturesLoaded && m_logSprite.getTexture()) {
                auto texSize = m_logSprite.getTexture()->getSize();
                // Ép hình ảnh khúc gỗ co giãn đúng bằng kích thước hitbox vật lý
                m_logSprite.setScale(log.shape.getSize().x / texSize.x, log.shape.getSize().y / texSize.y);
                m_logSprite.setPosition(log.shape.getPosition());
                window.draw(m_logSprite);
            } else {
                window.draw(log.shape);
            }
        }
    }

    // Vẽ người chơi
    if (!m_playerDrowned) {
        window.draw(*m_player);
    }

    // Hiệu ứng ngưng đọng thời gian (Time Freeze Tint & Badge)
    if (m_timeFreezeTimer > 0.f) {
        window.draw(m_freezeOverlay);

        if (m_fontLoaded) {
            char timerStr[32];
            std::snprintf(timerStr, sizeof(timerStr), "[FREEZE: %.1fs]", m_timeFreezeTimer);
            m_freezeBadgeText.setString(timerStr);
            sf::FloatRect fb = m_freezeBadgeText.getLocalBounds();
            m_freezeBadgeText.setOrigin(fb.left + fb.width / 2.f, fb.top + fb.height / 2.f);
            m_freezeBadgeText.setPosition(80.f, 78.f);
            window.draw(m_freezeBadgeText);
        }
    }

    // Vẽ HUD (bảng gỗ nền + text)
    if (m_fontLoaded) {
        if (m_hudBoardLoaded) {
            window.draw(m_hudBoardSprite);
        } else {
            window.draw(m_hudBg);   // Vẽ nền fallback trước để che mọi thứ phía sau
        }
        window.draw(m_levelText);
        window.draw(m_scoreText);
    }

    // Vẽ HUD buttons (Back, Pause/Continue, Save) ở góc dưới bên trái
    if (m_texHudButtonsLoaded) {
        if (m_hudBtnBack) window.draw(*m_hudBtnBack);
        if (m_hudBtnPause) window.draw(*m_hudBtnPause);
        if (m_hudBtnSave) window.draw(*m_hudBtnSave);
    }

    // Vẽ overlay Game Over
    if (m_playerDead) {
        if (m_goState != GameOverUIState::Delay) {
            window.draw(m_gameOverOverlay);
        }
        if (m_goState == GameOverUIState::Delay) {
            // Delay 1.2s: Giữ nguyên màn hình chơi để nhìn rõ animation nhân vật chìm sông
        } else if (m_fontLoaded) {
            if (m_goState == GameOverUIState::EnterName) {
                if (m_texGameOverLoaded) window.draw(m_gameOverSprite);
                if (m_texLabelLoaded) {
                    sf::Vector2u texSize = m_texLabel.getSize();
                    if (texSize.x > 0 && texSize.y > 0) {
                        m_labelSprite.setScale(500.f / static_cast<float>(texSize.x), 290.f / static_cast<float>(texSize.y));
                    }
                    m_labelSprite.setPosition(400.f, 290.f);
                    window.draw(m_labelSprite);
                } else {
                    window.draw(m_enterNameBoard);
                }

                if (m_texEnterNameTextLoaded) {
                    window.draw(m_enterNameTextSprite);
                } else {
                    window.draw(m_enterNameText);
                }
                if (m_nameInput) window.draw(*m_nameInput);
                if (m_btnOk) window.draw(*m_btnOk);
            } else if (m_goState == GameOverUIState::ConfirmIdentity) {
                if (m_texGameOverLoaded) window.draw(m_gameOverSprite);
                if (m_texLabelLoaded) {
                    sf::Vector2u texSize = m_texLabel.getSize();
                    if (texSize.x > 0 && texSize.y > 0) {
                        m_labelSprite.setScale(500.f / static_cast<float>(texSize.x), 290.f / static_cast<float>(texSize.y));
                    }
                    m_labelSprite.setPosition(400.f, 290.f);
                    window.draw(m_labelSprite);
                } else {
                    window.draw(m_enterNameBoard);
                }

                if (m_texNameExistsTextLoaded) {
                    window.draw(m_nameExistsTextSprite);
                    window.draw(m_confirmBestScoreText);
                } else {
                    window.draw(m_confirmText);
                }
                if (m_btnYes) window.draw(*m_btnYes);
                if (m_btnNo) window.draw(*m_btnNo);
            } else if (m_goState == GameOverUIState::ShowScore) {
                if (m_texGameOverLoaded) window.draw(m_gameOverSprite);
                if (m_texLabelLoaded) {
                    sf::Vector2u texSize = m_texLabel.getSize();
                    if (texSize.x > 0 && texSize.y > 0) {
                        m_labelSprite.setScale(500.f / static_cast<float>(texSize.x), 360.f / static_cast<float>(texSize.y));
                    }
                    m_labelSprite.setPosition(400.f, 290.f);
                    window.draw(m_labelSprite);
                } else {
                    window.draw(m_scoreBoard);
                }

                if (m_texNameScoreRankTextLoaded) {
                    window.draw(m_nameScoreRankTextSprite);
                    window.draw(m_showNameText);
                    window.draw(m_showScoreText);
                    if (m_finalRank <= 3 && m_medalsLoaded) {
                        window.draw(m_medalSprite);
                    } else {
                        window.draw(m_showRankText);
                    }
                } else {
                    window.draw(m_rankText);
                }

                if (m_playAgainBtn) window.draw(*m_playAgainBtn);
                if (m_menuBtn) window.draw(*m_menuBtn);
            }
        }
    }

    // Vẽ overlay Pause
    if (m_paused) {
        window.draw(m_pauseOverlay);
        if (m_fontLoaded) {
            if (m_pauseUIState == PauseUIState::Main) {
                window.draw(m_pauseText);
                window.draw(m_pauseInstruction);
            } else if (m_pauseUIState == PauseUIState::EnterSaveName) {
                if (m_texLabelLoaded) {
                    sf::Vector2u texSize = m_texLabel.getSize();
                    if (texSize.x > 0 && texSize.y > 0) {
                        m_labelSprite.setScale(500.f / static_cast<float>(texSize.x), 290.f / static_cast<float>(texSize.y));
                    }
                    m_labelSprite.setPosition(400.f, 290.f);
                    window.draw(m_labelSprite);
                } else {
                    window.draw(m_saveNameBoard);
                }

                if (m_texEnterNameTextLoaded) {
                    window.draw(m_enterNameTextSprite);
                } else {
                    window.draw(m_saveNamePrompt);
                }
                if (m_saveNameInput) window.draw(*m_saveNameInput);
                if (m_btnSaveOk) window.draw(*m_btnSaveOk);
            }
        }
    }
}
// ============================================================
// Thiết lập giao diện Game Over
// ============================================================
void GameState::setupGameOverUI() {
    Game::instance().playSound("assets/audio/bgm_gameover.wav");
    m_goState = GameOverUIState::EnterName;

    if (m_texEnterNameTextLoaded) {
        sf::Vector2u size = m_texEnterNameText.getSize();
        if (size.x > 0 && size.y > 0) {
            m_enterNameTextSprite.setScale(220.f / static_cast<float>(size.x), 32.f / static_cast<float>(size.y));
        }
        m_enterNameTextSprite.setPosition(400.f, 230.f);
    }

    m_enterNameText.setFont(m_font);
    m_enterNameText.setString("Enter Your Name:");
    m_enterNameText.setCharacterSize(20);
    m_enterNameText.setFillColor(sf::Color::Black);
    sf::FloatRect eBounds = m_enterNameText.getLocalBounds();
    m_enterNameText.setOrigin(eBounds.left + eBounds.width / 2.f, eBounds.top + eBounds.height / 2.f);
    m_enterNameText.setPosition(400.f, 230.f);

    m_nameInput = std::make_unique<TextBox>(290.f, 270.f, 220.f, 40.f, m_font);
    m_nameInput->setActive(true);

    auto showScoreBoard = [this](const std::string& name) {
        m_goState = GameOverUIState::ShowScore;
        
        m_scoreBoard.setSize(sf::Vector2f(440.f, 340.f));
        m_scoreBoard.setFillColor(sf::Color(0, 80, 180, 220));
        m_scoreBoard.setOutlineColor(sf::Color::White);
        m_scoreBoard.setOutlineThickness(3.f);
        m_scoreBoard.setOrigin(220.f, 170.f);
        m_scoreBoard.setPosition(400.f, 290.f);

        auto scores = SaveManager::loadHighscores();
        m_finalRank = 1;
        for (const auto& s : scores) {
            if (m_score >= s.score) break;
            m_finalRank++;
        }

        if (m_texNameScoreRankTextLoaded) {
            sf::Vector2u size = m_texNameScoreRankText.getSize();
            if (size.x > 0 && size.y > 0) {
                m_nameScoreRankTextSprite.setScale(150.f / static_cast<float>(size.x), 95.f / static_cast<float>(size.y));
            }
            m_nameScoreRankTextSprite.setPosition(240.f, 198.f);
        }

        m_showNameText.setFont(m_font);
        m_showNameText.setString(name);
        m_showNameText.setCharacterSize(22);
        m_showNameText.setFillColor(sf::Color::White);
        m_showNameText.setPosition(405.f, 200.f);

        m_showScoreText.setFont(m_font);
        m_showScoreText.setString(std::to_string(m_score));
        m_showScoreText.setCharacterSize(22);
        m_showScoreText.setFillColor(sf::Color(100, 255, 100));
        m_showScoreText.setPosition(405.f, 232.f);

        if (m_finalRank <= 3 && m_medalsLoaded) {
            if (m_finalRank == 1) m_medalSprite.setTexture(m_goldMedalTex);
            else if (m_finalRank == 2) m_medalSprite.setTexture(m_silverMedalTex);
            else if (m_finalRank == 3) m_medalSprite.setTexture(m_bronzeMedalTex);

            sf::Vector2u mSize = m_medalSprite.getTexture()->getSize();
            if (mSize.x > 0 && mSize.y > 0) {
                m_medalSprite.setScale(28.f / static_cast<float>(mSize.x), 28.f / static_cast<float>(mSize.y));
            }
            m_medalSprite.setPosition(405.f, 264.f);
        } else {
            m_showRankText.setFont(m_font);
            m_showRankText.setString("#" + std::to_string(m_finalRank));
            m_showRankText.setCharacterSize(22);
            m_showRankText.setFillColor(sf::Color(255, 215, 0));
            m_showRankText.setPosition(405.f, 264.f);
        }

        m_rankText.setFont(m_font);
        m_rankText.setString("Name: " + name + "\nScore: " + std::to_string(m_score) + "\nRank: #" + std::to_string(m_finalRank));
        m_rankText.setCharacterSize(24);
        m_rankText.setFillColor(sf::Color(255, 215, 0));
        sf::FloatRect rBounds = m_rankText.getLocalBounds();
        m_rankText.setOrigin(rBounds.left + rBounds.width / 2.f, rBounds.top + rBounds.height / 2.f);
        m_rankText.setPosition(400.f, 230.f);

        // Nút PLAY AGAIN (180x44) & MAIN MENU (160x44) giúp tỷ lệ chữ đồng nhất
        if (m_texPlayAgainLoaded) {
            m_playAgainBtn = std::make_unique<Button>(310.f, 318.f, 180.f, 44.f, m_texPlayAgain, [this]() {
                m_deferredAction = DeferredAction::Restart;
            });
        } else {
            m_playAgainBtn = std::make_unique<Button>(310.f, 318.f, 180.f, 44.f, "Play Again", m_font, [this]() {
                m_deferredAction = DeferredAction::Restart;
            });
        }

        if (m_texHomeLoaded) {
            m_menuBtn = std::make_unique<Button>(310.f, 374.f, 180.f, 44.f, m_texHome, [this]() {
                m_deferredAction = DeferredAction::Quit;
            });
        } else {
            m_menuBtn = std::make_unique<Button>(310.f, 374.f, 180.f, 44.f, "Main Menu", m_font, [this]() {
                m_deferredAction = DeferredAction::Quit;
            });
        }
    };

    m_submitNameFunc = [this, showScoreBoard]() {
        if (m_nameInput->getString().empty()) return;
        std::string name = m_nameInput->getString();
        int existingScore = 0;
        
        if (SaveManager::hasHighscore(name, existingScore)) {
            m_goState = GameOverUIState::ConfirmIdentity;
            
            if (m_texNameExistsTextLoaded) {
                sf::Vector2u size = m_texNameExistsText.getSize();
                if (size.x > 0 && size.y > 0) {
                    m_nameExistsTextSprite.setScale(340.f / static_cast<float>(size.x), 55.f / static_cast<float>(size.y));
                }
                m_nameExistsTextSprite.setPosition(400.f, 245.f);
            }

            m_confirmBestScoreText.setFont(m_font);
            m_confirmBestScoreText.setString(std::to_string(existingScore));
            m_confirmBestScoreText.setCharacterSize(18);
            m_confirmBestScoreText.setFillColor(sf::Color(255, 215, 0));
            m_confirmBestScoreText.setStyle(sf::Text::Bold);
            sf::FloatRect bScoreBounds = m_confirmBestScoreText.getLocalBounds();
            m_confirmBestScoreText.setOrigin(bScoreBounds.width / 2.f, bScoreBounds.top + bScoreBounds.height / 2.f);
            m_confirmBestScoreText.setPosition(485.f, 259.f);

            m_confirmText.setFont(m_font);
            m_confirmText.setString("Name exists! Is this you?\n(Current best: " + std::to_string(existingScore) + ")");
            m_confirmText.setCharacterSize(20);
            m_confirmText.setFillColor(sf::Color::Yellow);
            sf::FloatRect cBounds = m_confirmText.getLocalBounds();
            m_confirmText.setOrigin(cBounds.left + cBounds.width / 2.f, cBounds.top + cBounds.height / 2.f);
            m_confirmText.setPosition(400.f, 245.f);
            
            auto onYes = [this, name, showScoreBoard]() {
                SaveManager::updateHighscore(name, m_score);
                m_scoreSaved = true;
                showScoreBoard(name);
            };
            auto onNo = [this]() {
                m_goState = GameOverUIState::EnterName;
            };

            // Nút YES và NO cùng kích thước và căn giữa
            if (m_texYesLoaded) {
                m_btnYes = std::make_unique<Button>(270.f, 330.f, 120.f, 45.f, m_texYes, onYes);
            } else {
                m_btnYes = std::make_unique<Button>(270.f, 330.f, 120.f, 45.f, "Yes", m_font, onYes);
            }

            if (m_texNoLoaded) {
                m_btnNo = std::make_unique<Button>(410.f, 330.f, 120.f, 45.f, m_texNo, onNo);
            } else {
                m_btnNo = std::make_unique<Button>(410.f, 330.f, 120.f, 45.f, "No", m_font, onNo);
            }
        } else {
            SaveManager::addHighscore(name, m_score);
            m_scoreSaved = true;
            showScoreBoard(name);
        }
    };

    auto onSubmit = [this]() {
        if (m_submitNameFunc) m_submitNameFunc();
    };

    if (m_texConfirmLoaded) {
        m_btnOk = std::make_unique<Button>(340.f, 340.f, 120.f, 40.f, m_texConfirm, onSubmit);
    } else {
        m_btnOk = std::make_unique<Button>(340.f, 340.f, 120.f, 40.f, "OK", m_font, onSubmit);
    }
}

// ============================================================
// Load game từ SaveData
// ============================================================
void GameState::loadGame(const std::string& sessionName, const SaveData& data) {
    m_currentSaveSession = sessionName;
    m_level = data.level;
    m_score = data.score;
    m_totalPlaytime = static_cast<float>(data.playTimeSeconds);
    
    // Khôi phục map chính xác
    m_terrains.clear();
    for (const auto& savedRow : data.terrains) {
        createExactRow(savedRow);
    }
    
    // Khôi phục nhân vật
    initPlayer(); // init default
    m_player->forcePosition(data.playerX, data.playerY);
    m_maxPlayerY = data.maxPlayerY;
    m_timeFreezeTimer = data.timeFreezeTimer;
    
    m_isLoadedGame = true;
    
    initHUD();
}

void GameState::createExactRow(const SavedTerrainRow& savedRow) {
    TerrainRow row;
    row.type = static_cast<TerrainType>(savedRow.type);
    row.yPosition = savedRow.yPosition;

    row.background.setSize(sf::Vector2f(800.f, m_cellSize));
    row.background.setPosition(0.f, row.yPosition);

    if (row.type == TerrainType::Grass) {
        row.background.setFillColor(sf::Color(34, 139, 34)); // Xanh lá
    } else if (row.type == TerrainType::Road) {
        row.background.setFillColor(sf::Color(50, 50, 50)); // Đen xám
    } else if (row.type == TerrainType::River) {
        row.background.setFillColor(sf::Color(30, 100, 200)); // Xanh nước
    }

    // Khôi phục items
    for (const auto& sItem : savedRow.items) {
        ItemData item;
        item.type = static_cast<ItemType>(sItem.type);
        float rad = (item.type == ItemType::Clock) ? 23.f : 14.f;
        item.shape.setRadius(rad);
        item.shape.setFillColor(item.type == ItemType::Clock ? sf::Color(0, 229, 255) : sf::Color(255, 215, 0));
        item.shape.setPosition(sItem.x, sItem.y);
        item.collected = sItem.collected;
        item.points = sItem.points;
        row.items.push_back(item);
    }

    // Khôi phục vehicles
    for (const auto& sVeh : savedRow.vehicles) {
        if (sVeh.type == 0) {
            row.vehicles.push_back(std::make_unique<CCAR>(sVeh.x, sVeh.y, sVeh.speed, sVeh.direction, sVeh.isCrazy));
        } else {
            row.vehicles.push_back(std::make_unique<CTRUCK>(sVeh.x, sVeh.y, sVeh.speed, sVeh.direction));
        }
    }
    

    // Khôi phục khúc gỗ
    for (const auto& sPad : savedRow.lilyPads) {
        Log log;
        log.shape.setSize(sf::Vector2f(sPad.width, sPad.height));
        log.shape.setPosition(sPad.x, sPad.y);
        log.shape.setFillColor(sf::Color(sPad.r, sPad.g, sPad.b));
        log.shape.setOutlineColor(sf::Color(80, 50, 20));
        log.shape.setOutlineThickness(1.f);
        log.speed = sPad.speed;
        log.movingRight = sPad.movingRight;
        row.logs.push_back(log);
    }

    // Khởi tạo thông số mặc định cho tàu hoả
    row.train.movingRight = false;
    row.train.speed = 1200.f;
    row.train.isActive = false;
    row.train.shape.setSize(sf::Vector2f(800.f, m_cellSize));
    row.train.shape.setPosition(800.f + 100.f, row.yPosition);

    m_terrains.push_back(std::move(row));
}
