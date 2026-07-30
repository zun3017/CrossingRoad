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
            

            if (tItem.getSize().x > 0) m_itemSprite.setTexture(tItem);
            
            if (tTrack.getSize().x > 0) m_trackSprite.setTexture(tTrack);
            if (tTrain.getSize().x > 0) m_trainSprite.setTexture(tTrain);
            if (tLightGreen.getSize().x > 0) m_lightGreenSprite.setTexture(tLightGreen);
            if (tLightRed.getSize().x > 0) m_lightRedSprite.setTexture(tLightRed);
            if (tLightBlink.getSize().x > 0) m_lightBlinkSprite.setTexture(tLightBlink);
            
            m_hitByCarLoaded = m_hitByCarTexture.loadFromFile("assets/textures/hitbycar.png");
            
            m_texturesLoaded = true;
        }
    } catch (...) {}

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

    // Thêm vật phẩm (ItemData) ngẫu nhiên trên cỏ (không phải vùng an toàn)
    if (!safeZone && std::rand() % 100 < 30) {
        ItemData item;
        float itemX = static_cast<float>(50 + std::rand() % 700);
        item.shape.setRadius(14.f); // To hơn để dễ nhìn (bán kính 14 -> đường kính 28)
        item.shape.setFillColor(sf::Color(255, 215, 0)); // Vàng gold
        item.shape.setPosition(itemX, y + m_cellSize / 2.f - 14.f);
        item.collected = false;
        item.points = 10; // Vật phẩm luôn luôn được 10 điểm
        row.items.push_back(item);
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

    // Vẽ vạch kẻ đường (trang trí, lưu vào obstacles sẽ phức tạp quá)
    // Tạo 2-4 xe ngẫu nhiên trên mỗi hàng đường
    int numVehicles = 2 + std::rand() % 3;
    bool movingRight = (std::rand() % 2 == 0); // Hướng chạy ngẫu nhiên
    float baseSpeed = 80.f + static_cast<float>(m_level * 15 + std::rand() % 40);

    for (int v = 0; v < numVehicles; v++) {
        // Spawn vehicles spread across the full route so they enter naturally:
        // - Right-moving: spread from -200 to 600 (some off-screen left, some on-screen)
        // - Left-moving:  spread from 200 to 1000 (some on-screen, some off-screen right)
        float startX;
        if (movingRight) {
            // First vehicle guaranteed off-screen, rest spread across route
            float spacing = 900.f / numVehicles;
            startX = -150.f + static_cast<float>(v) * spacing + static_cast<float>(std::rand() % 60);
        } else {
            // Vehicles enter from the right side naturally
            float spacing = 900.f / numVehicles;
            startX = 950.f - static_cast<float>(v) * spacing - static_cast<float>(std::rand() % 60);
        }

        float speed = baseSpeed;
        int direction = movingRight ? 1 : -1;
        
        if (std::rand() % 2 == 0) {
            row.vehicles.push_back(std::make_unique<CCAR>(startX, y + 2.f, speed, direction));
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
    bool movingRight = (std::rand() % 2 == 0);
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
    // Panel nền mờ cho HUD (che sprite nhân vật phía sau)
    m_hudBg.setSize(sf::Vector2f(130.f, 55.f));
    m_hudBg.setPosition(0.f, 0.f);
    m_hudBg.setFillColor(sf::Color(0, 0, 0, 160)); // Đen bán trong suốt

    m_levelText.setFont(m_font);
    m_levelText.setCharacterSize(18);
    m_levelText.setFillColor(sf::Color::White);
    m_levelText.setOutlineColor(sf::Color::Black);
    m_levelText.setOutlineThickness(1.f);
    m_levelText.setPosition(10.f, 5.f);
    m_levelText.setString("Level: " + std::to_string(m_level));

    m_scoreText.setFont(m_font);
    m_scoreText.setCharacterSize(18);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setOutlineColor(sf::Color::Black);
    m_scoreText.setOutlineThickness(1.f);
    m_scoreText.setPosition(10.f, 28.f);
    m_scoreText.setString("Score: " + std::to_string(m_score));
}

// ============================================================
// Khởi tạo overlay Game Over và Pause
// ============================================================
void GameState::initOverlays() {
    // Game Over overlay
    m_gameOverOverlay.setSize(sf::Vector2f(800.f, 600.f));
    m_gameOverOverlay.setFillColor(sf::Color(0, 0, 0, 160));

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
    m_saveNamePrompt.setPosition(400.f, 250.f);

    m_saveNameInput = std::make_unique<TextBox>(300.f, 280.f, 200.f, 30.f, m_font);

    m_btnSaveOk = std::make_unique<Button>(350.f, 320.f, 100.f, 30.f, "OK", m_font, [this]() {
        if (!m_saveNameInput->getString().empty()) {
            m_currentSaveSession = m_saveNameInput->getString();
            saveCurrentGameState(m_currentSaveSession);
            m_pauseUIState = PauseUIState::Main;
            m_pauseInstruction.setString("Game Saved to " + m_currentSaveSession + "!\nPress P or Escape to Resume\nPress Q to Quit to Menu");
        }
    });
}

void GameState::saveCurrentGameState(const std::string& sessionName) {
    SaveData data;
    data.playerName = "Player";
    data.score = m_score;
    data.level = m_level;
    data.playerX = m_player->getPosition().x;
    data.playerY = m_player->getPosition().y;
    data.maxPlayerY = m_maxPlayerY;
    
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
                    m_currentSaveSession = m_saveNameInput->getString();
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

    // Nếu di chuyển lên/xuống sang vùng không phải sông, căn x về cột lưới gần nhất
    if (dy != 0.f) {
        float targetY = newPos.y + m_playerSize / 2.f;
        
        bool isTargetRiver = false;
        for (auto& row : m_terrains) {
            if (row.type == TerrainType::River) {
                sf::FloatRect rowBounds = row.background.getGlobalBounds();
                if (targetY > rowBounds.top && targetY < rowBounds.top + rowBounds.height) {
                    isTargetRiver = true;
                    break;
                }
            }
        }
        if (!isTargetRiver) {
            float offsetX = (m_cellSize - m_playerSize) / 2.f;
            int col = static_cast<int>(std::round((newPos.x - offsetX) / m_cellSize));
            newPos.x = col * m_cellSize + offsetX;
            
        }
    }

    // Gọi startMove để kích hoạt animation
    m_player->startMove(newPos.x - m_player->getPosition().x, newPos.y - m_player->getPosition().y);
    
    // Tăng điểm khi tiến lên (chỉ thưởng nếu đi lên cao hơn mức cao nhất từng đạt được)
    if (newPos.y < m_maxPlayerY) {
        m_score += 5;
        m_maxPlayerY = newPos.y;
    }
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
        return;
    } else if (m_deferredAction == DeferredAction::Quit) {
        m_deferredAction = DeferredAction::None;
        Game::instance().getStateMachine().popState();
        return;
    }

    if (m_playerDead) {
        if (m_goState == GameOverUIState::Delay) {
            m_deathTimer += dt;
            if (m_deathTimer >= 1.0f) {
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

    updateObstacles(dt);
    updateLilyPads(dt);
    updateRailway(dt);
    checkCollisions(dt);
    checkWinCondition();
    
    // m_player has its own update and animation logic if we use it, but for now we just call update.
    m_player->update(dt);

    // Cập nhật HUD
    m_levelText.setString("Level: " + std::to_string(m_level));
    m_scoreText.setString("Score: " + std::to_string(m_score));
}

// ============================================================
// Cập nhật vị trí xe cộ trên đường
// ============================================================
void GameState::updateObstacles(float dt) {
    for (auto& row : m_terrains) {
        if (row.type == TerrainType::Road) {
            for (auto& v : row.vehicles) {
                v->update(dt);
                
                // Wrap around logic cho xe cộ (Logic Game)
                // Wrap khi xe vừa thoát hết khỏi màn hình
                if (v->getDirection() > 0 && v->getPosition().x > 860.f) {
                    v->setPosition(-180.f, v->getPosition().y);
                } else if (v->getDirection() < 0 && v->getPosition().x < -90.f) {
                    v->setPosition(860.f, v->getPosition().y);
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
    // Thu nhỏ hitbox để gameplay fair hơn
    sf::FloatRect playerHitbox(
        playerBounds.left + 4.f, playerBounds.top + 10.f, // Cắt viền trên nhiều hơn để tránh chạm nhầm hàng địa hình phía trên
        playerBounds.width - 8.f, playerBounds.height - 16.f // Cắt viền dưới để hitbox nằm lọt thỏm trong 1 ô
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
                // Nếu đang animation LERP nhảy LÊN khỏi hàng sông (về phía bờ),
                // bỏ qua kiểm tra chết đuối để tránh chết nhầm khi nhảy lên bờ
                if (m_player->m_isAnimating && m_player->m_targetPos.y < rowBounds.top) {
                    continue; // Đang nhảy lên bờ -> bỏ qua
                }

                bool onLog = false;
                for (auto& log : row.logs) {
                    if (playerHitbox.intersects(log.shape.getGlobalBounds())) {
                        onLog = true;
                        // Di chuyển theo khúc gỗ đồng bộ với dt
                        float logMove = log.speed * dt * (log.movingRight ? 1.f : -1.f);
                        m_player->setPosition(m_player->getPosition().x + logMove, m_player->getPosition().y);

                        // Nếu bị đẩy ra ngoài màn hình thì chết
                        if (m_player->getPosition().x < -m_playerSize || m_player->getPosition().x > 800.f) {
                            if (!m_playerDead) {
                                m_playerDead = true;
                                m_player->die(DeathType::Drowned);
                                m_deathTimer = 0.f;
                                m_goState = GameOverUIState::Delay;
                                if (!m_currentSaveSession.empty()) {
                                    SaveManager::deleteGame(m_currentSaveSession);
                                    m_currentSaveSession = "";
                                }
                            }
                            return;
                        }
                        break;
                    }
                }
                if (!onLog) {
                    // Rơi xuống sông -> chết đuối
                    if (!m_playerDead) {
                        m_playerDead = true;
                        m_player->die(DeathType::Drowned);
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


        // Kiểm tra thu thập item
        for (auto& item : row.items) {
            if (!item.collected && playerHitbox.intersects(item.shape.getGlobalBounds())) {
                item.collected = true;
                m_score += item.points;
            }
        }
    }
}

// ============================================================
// Kiểm tra điều kiện thắng (qua được hết màn hình)
// ============================================================
void GameState::checkWinCondition() {
    if (m_player->getPosition().y <= 0.f) {
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

        // Vẽ xe cộ
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

        // Vẽ items chưa thu thập
        for (auto& item : row.items) {
            if (!item.collected) {
                if (m_texturesLoaded && m_itemSprite.getTexture()) {
                    auto texSize = m_itemSprite.getTexture()->getSize();
                    float diam = item.shape.getRadius() * 2.f;
                    m_itemSprite.setScale(diam / texSize.x, diam / texSize.y);
                    m_itemSprite.setPosition(item.shape.getPosition());
                    window.draw(m_itemSprite);
                } else {
                    window.draw(item.shape);
                }
            }
        }
    }

    // Vẽ người chơi
    if (!m_playerDrowned) {
        window.draw(*m_player);
    }

    // Vẽ HUD (panel nền + text)
    if (m_fontLoaded) {
        window.draw(m_hudBg);   // Vẽ nền trước để che mọi thứ phía sau
        window.draw(m_levelText);
        window.draw(m_scoreText);
    }

    // Vẽ overlay Game Over
    if (m_playerDead) {
        window.draw(m_gameOverOverlay);
        if (m_fontLoaded) {
            if (m_goState == GameOverUIState::Delay) {
                window.draw(m_gameOverText);
            } else if (m_goState == GameOverUIState::EnterName) {
                window.draw(m_enterNameBoard);
                window.draw(m_enterNameText);
                if (m_nameInput) window.draw(*m_nameInput);
                if (m_btnOk) window.draw(*m_btnOk);
            } else if (m_goState == GameOverUIState::ConfirmIdentity) {
                window.draw(m_confirmText);
                if (m_btnYes) window.draw(*m_btnYes);
                if (m_btnNo) window.draw(*m_btnNo);
            } else if (m_goState == GameOverUIState::ShowScore) {
                window.draw(m_scoreBoard);
                window.draw(m_rankText);
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
                window.draw(m_saveNameBoard);
                window.draw(m_saveNamePrompt);
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
    m_goState = GameOverUIState::EnterName;

    m_enterNameBoard.setSize(sf::Vector2f(350.f, 200.f));
    m_enterNameBoard.setFillColor(sf::Color(135, 206, 250, 220)); // Xanh nước biển nhạt
    m_enterNameBoard.setOutlineColor(sf::Color::White);
    m_enterNameBoard.setOutlineThickness(3.f);
    m_enterNameBoard.setOrigin(175.f, 100.f);
    m_enterNameBoard.setPosition(400.f, 290.f);

    m_enterNameText.setFont(m_font);
    m_enterNameText.setString("Enter Your Name:");
    m_enterNameText.setCharacterSize(20);
    m_enterNameText.setFillColor(sf::Color::Black);
    sf::FloatRect eBounds = m_enterNameText.getLocalBounds();
    m_enterNameText.setOrigin(eBounds.left + eBounds.width / 2.f, eBounds.top + eBounds.height / 2.f);
    m_enterNameText.setPosition(400.f, 230.f);

    m_nameInput = std::make_unique<TextBox>(300.f, 260.f, 200.f, 40.f, m_font);
    m_nameInput->setActive(true);

    auto showScoreBoard = [this](const std::string& name) {
        m_goState = GameOverUIState::ShowScore;
        
        m_scoreBoard.setSize(sf::Vector2f(400.f, 250.f));
        m_scoreBoard.setFillColor(sf::Color(0, 80, 180, 220)); // Xanh dương trong suốt
        m_scoreBoard.setOutlineColor(sf::Color::White);
        m_scoreBoard.setOutlineThickness(3.f);
        m_scoreBoard.setOrigin(200.f, 125.f);
        m_scoreBoard.setPosition(400.f, 300.f);

        auto scores = SaveManager::loadHighscores();
        m_finalRank = 1;
        for (const auto& s : scores) {
            if (m_score >= s.score) break;
            m_finalRank++;
        }

        m_rankText.setFont(m_font);
        m_rankText.setString("Name: " + name + "\nScore: " + std::to_string(m_score) + "\nRank: #" + std::to_string(m_finalRank));
        m_rankText.setCharacterSize(24);
        m_rankText.setFillColor(sf::Color(255, 215, 0));
        sf::FloatRect rBounds = m_rankText.getLocalBounds();
        m_rankText.setOrigin(rBounds.left + rBounds.width / 2.f, rBounds.top + rBounds.height / 2.f);
        m_rankText.setPosition(400.f, 260.f);

        m_playAgainBtn = std::make_unique<Button>(230.f, 340.f, 140.f, 45.f, "Play Again", m_font, [this]() {
            m_deferredAction = DeferredAction::Restart;
        });

        m_menuBtn = std::make_unique<Button>(430.f, 340.f, 140.f, 45.f, "Main Menu", m_font, [this]() {
            m_deferredAction = DeferredAction::Quit;
        });
    };

    m_submitNameFunc = [this, showScoreBoard]() {
        if (m_nameInput->getString().empty()) return;
        std::string name = m_nameInput->getString();
        int existingScore = 0;
        
        if (SaveManager::hasHighscore(name, existingScore)) {
            m_goState = GameOverUIState::ConfirmIdentity;
            
            m_confirmText.setFont(m_font);
            m_confirmText.setString("Name exists! Is this you?\n(Current best: " + std::to_string(existingScore) + ")");
            m_confirmText.setCharacterSize(20);
            m_confirmText.setFillColor(sf::Color::Yellow);
            sf::FloatRect cBounds = m_confirmText.getLocalBounds();
            m_confirmText.setOrigin(cBounds.left + cBounds.width / 2.f, cBounds.top + cBounds.height / 2.f);
            m_confirmText.setPosition(400.f, 260.f);
            
            m_btnYes = std::make_unique<Button>(280.f, 320.f, 100.f, 45.f, "Yes", m_font, [this, name, showScoreBoard]() {
                SaveManager::updateHighscore(name, m_score);
                m_scoreSaved = true;
                showScoreBoard(name);
            });
            m_btnNo = std::make_unique<Button>(420.f, 320.f, 100.f, 45.f, "No", m_font, [this]() {
                m_goState = GameOverUIState::EnterName;
            });
        } else {
            SaveManager::addHighscore(name, m_score);
            m_scoreSaved = true;
            showScoreBoard(name);
        }
    };

    m_btnOk = std::make_unique<Button>(350.f, 320.f, 100.f, 40.f, "OK", m_font, [this]() {
        if (m_submitNameFunc) m_submitNameFunc();
    });
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
        item.shape.setRadius(8.f);
        item.shape.setFillColor(sf::Color::Yellow);
        item.shape.setPosition(sItem.x, sItem.y);
        item.collected = sItem.collected;
        item.points = sItem.points;
        row.items.push_back(item);
    }

    // Khôi phục vehicles
    for (const auto& sVeh : savedRow.vehicles) {
        if (sVeh.type == 0) {
            row.vehicles.push_back(std::make_unique<CCAR>(sVeh.x, sVeh.y, sVeh.speed, sVeh.direction));
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

    m_terrains.push_back(std::move(row));
}
