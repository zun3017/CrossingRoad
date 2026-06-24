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
        auto& tPlayer = ResourceManager<sf::Texture>::getInstance().get("assets/textures/player.png");
        auto& tGrass = ResourceManager<sf::Texture>::getInstance().get("assets/textures/grass.png");
        auto& tRoad = ResourceManager<sf::Texture>::getInstance().get("assets/textures/road.png");
        auto& tRiver = ResourceManager<sf::Texture>::getInstance().get("assets/textures/river.png");
        auto& tLog = ResourceManager<sf::Texture>::getInstance().get("assets/textures/log.png");
        auto& tCarBlue = ResourceManager<sf::Texture>::getInstance().get("assets/textures/car_blue.png");
        auto& tCarRed = ResourceManager<sf::Texture>::getInstance().get("assets/textures/car_red.png");
        auto& tCarYellow = ResourceManager<sf::Texture>::getInstance().get("assets/textures/car_yellow.png");
        auto& tItem = ResourceManager<sf::Texture>::getInstance().get("assets/textures/item.png");
        
        if (tPlayer.getSize().x > 0) {
            m_playerSprite.setTexture(tPlayer);
            int frameW = tPlayer.getSize().x / 4;
            int frameH = tPlayer.getSize().y / 4;
            m_playerSprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
            m_playerSprite.setScale(m_playerSize / frameW, m_playerSize / frameH);
            
            m_grassSprite.setTexture(tGrass);
            m_grassSprite.setScale(800.f / tGrass.getSize().x, m_cellSize / tGrass.getSize().y);
            
            m_roadSprite.setTexture(tRoad);
            m_roadSprite.setScale(800.f / tRoad.getSize().x, m_cellSize / tRoad.getSize().y);
            
            m_riverSprite.setTexture(tRiver);
            m_riverSprite.setScale(800.f / tRiver.getSize().x, m_cellSize / tRiver.getSize().y);
            
            m_logSprite.setTexture(tLog);
            m_logSprite.setScale(56.f / tLog.getSize().x, 40.f / tLog.getSize().y);
            
            if (tCarBlue.getSize().x > 0) m_carBlueSprite.setTexture(tCarBlue);
            if (tCarRed.getSize().x > 0) m_carRedSprite.setTexture(tCarRed);
            if (tCarYellow.getSize().x > 0) m_carYellowSprite.setTexture(tCarYellow);
            if (tItem.getSize().x > 0) m_itemSprite.setTexture(tItem);
            
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
    m_playerShape.setSize(sf::Vector2f(m_playerSize, m_playerSize));
    m_playerShape.setFillColor(sf::Color(0, 200, 0)); // Màu xanh lá
    m_playerShape.setOutlineColor(sf::Color::White);
    m_playerShape.setOutlineThickness(2.f);

    // Vị trí khởi đầu: giữa dưới cùng
    m_playerPos = sf::Vector2f(400.f - m_playerSize / 2.f,
                                600.f - m_cellSize - 4.f);
    m_playerShape.setPosition(m_playerPos);
    m_playerDead = false;
    m_maxPlayerY = m_playerPos.y;
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
        try {
            auto& tPlayer = ResourceManager<sf::Texture>::getInstance().get("assets/textures/player.png");
            m_playerSprite.setTexture(tPlayer, true);
            int frameW = tPlayer.getSize().x / 4;
            int frameH = tPlayer.getSize().y / 4;
            m_playerSprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
            m_playerSprite.setScale(m_playerSize / frameW, m_playerSize / frameH);
            m_playerSprite.setColor(sf::Color::White);
            
            // Khôi phục animation
            m_playerAnimRow = 0;
            m_playerAnimFrame = 0;
            m_isPlayerAnimating = false;
        } catch(...) {}
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

        if (i == m_totalRows - 1) {
            // Hàng trên cùng luôn là cỏ (vùng đích)
            createGrassRow(y, true);
        } else if (terrainRoll < 40) {
            createRoadRow(y);
        } else if (terrainRoll < 65) {
            createRiverRow(y);
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

    // Thêm item ngẫu nhiên trên cỏ (không phải vùng an toàn)
    if (!safeZone && std::rand() % 100 < 30) {
        Item item;
        float itemX = static_cast<float>(50 + std::rand() % 700);
        item.shape.setRadius(14.f); // To hơn để dễ nhìn (bán kính 14 -> đường kính 28)
        item.shape.setFillColor(sf::Color(255, 215, 0)); // Vàng gold
        item.shape.setPosition(itemX, y + m_cellSize / 2.f - 14.f);
        item.collected = false;
        item.points = 10 + (m_level - 1) * 5;
        row.items.push_back(item);
    }

    m_terrains.push_back(row);
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
    bool movingRight = false; // Luôn chạy từ phải sang trái
    float baseSpeed = 80.f + static_cast<float>(m_level * 15 + std::rand() % 40);

    for (int v = 0; v < numVehicles; v++) {
        Obstacle obs;
        float vehicleWidth = 80.f + static_cast<float>(std::rand() % 40); // To hơn
        float vehicleHeight = m_cellSize - 4.f; // Gần bằng chiều cao làn đường (44px)

        obs.shape.setSize(sf::Vector2f(vehicleWidth, vehicleHeight));
        float startX = static_cast<float>(v * 200 + std::rand() % 100);
        obs.shape.setPosition(startX, y + 2.f); // Căn giữa làn đường

        // Màu ngẫu nhiên cho xe
        sf::Color vehicleColors[] = {
            sf::Color(200, 0, 0),    // Đỏ
            sf::Color(0, 0, 200),    // Xanh dương
            sf::Color(200, 200, 0),  // Vàng
            sf::Color(200, 100, 0),  // Cam
            sf::Color(150, 0, 150)   // Tím
        };
        obs.shape.setFillColor(vehicleColors[std::rand() % 5]);
        obs.shape.setOutlineColor(sf::Color(30, 30, 30));
        obs.shape.setOutlineThickness(1.f);

        obs.speed = baseSpeed + static_cast<float>(std::rand() % 30);
        obs.movingRight = movingRight;

        row.obstacles.push_back(obs);
    }

    m_terrains.push_back(row);
}

// ============================================================
// Tạo hàng sông với lá sen
// ============================================================
void GameState::createRiverRow(float y) {
    TerrainRow row;
    row.type = TerrainType::River;
    row.yPosition = y;

    row.background.setSize(sf::Vector2f(800.f, m_cellSize));
    row.background.setPosition(0.f, y);
    row.background.setFillColor(sf::Color(30, 100, 200)); // Xanh nước

    // Tạo 3-5 lá sen trên sông
    int numPads = 3 + std::rand() % 3;
    bool movingRight = (std::rand() % 2 == 0);
    float padSpeed = 50.f + static_cast<float>(m_level * 10 + std::rand() % 30);

    for (int p = 0; p < numPads; p++) {
        LilyPad pad;
        float padWidth = 55.f + static_cast<float>(std::rand() % 25);
        float padHeight = m_cellSize - 8.f;

        pad.shape.setSize(sf::Vector2f(padWidth, padHeight));
        float startX = static_cast<float>(p * 170 + std::rand() % 60);
        pad.shape.setPosition(startX, y + 4.f);
        pad.shape.setFillColor(sf::Color(0, 160, 0)); // Xanh lá sen
        pad.shape.setOutlineColor(sf::Color(0, 100, 0));
        pad.shape.setOutlineThickness(1.f);

        pad.speed = padSpeed;
        pad.movingRight = movingRight;

        row.lilyPads.push_back(pad);
    }

    m_terrains.push_back(row);
}

// ============================================================
// Khởi tạo HUD hiển thị level và điểm
// ============================================================
void GameState::initHUD() {
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
    data.playerX = m_playerPos.x;
    data.playerY = m_playerPos.y;
    data.maxPlayerY = m_maxPlayerY;
    
    data.numTerrains = m_terrains.size();
    for (auto& row : m_terrains) {
        SavedTerrainRow sRow;
        sRow.type = static_cast<int>(row.type);
        sRow.yPosition = row.yPosition;
        
        for (auto& obs : row.obstacles) {
            SavedObstacle sObs;
            sObs.x = obs.shape.getPosition().x;
            sObs.y = obs.shape.getPosition().y;
            sObs.width = obs.shape.getSize().x;
            sObs.height = obs.shape.getSize().y;
            sObs.speed = obs.speed;
            sObs.movingRight = obs.movingRight;
            sf::Color c = obs.shape.getFillColor();
            sObs.r = c.r; sObs.g = c.g; sObs.b = c.b;
            sRow.obstacles.push_back(sObs);
        }
        
        for (auto& pad : row.lilyPads) {
            SavedLilyPad sPad;
            sPad.x = pad.shape.getPosition().x;
            sPad.y = pad.shape.getPosition().y;
            sPad.width = pad.shape.getSize().x;
            sPad.height = pad.shape.getSize().y;
            sPad.speed = pad.speed;
            sPad.movingRight = pad.movingRight;
            sf::Color c = pad.shape.getFillColor();
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
        
        data.terrains.push_back(sRow);
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
    // Không cho phép đi ngang khi đang trên khúc gỗ (sông)
    if (dx != 0.f) {
        for (auto& row : m_terrains) {
            if (row.type == TerrainType::River) {
                sf::FloatRect rowBounds = row.background.getGlobalBounds();
                float playerCenterY = m_playerPos.y + m_playerSize / 2.f;
                if (playerCenterY > rowBounds.top && playerCenterY < rowBounds.top + rowBounds.height) {
                    return; // Chặn di chuyển trái phải
                }
            }
        }
    }

    sf::Vector2f newPos = m_playerPos + sf::Vector2f(dx, dy);

    // Giới hạn trong màn hình
    if (newPos.x < 0.f) newPos.x = 0.f;
    if (newPos.x > 800.f - m_playerSize) newPos.x = 800.f - m_playerSize;
    if (newPos.y > 600.f - m_cellSize) newPos.y = 600.f - m_cellSize;
    // Cho phép y đi lên trên cùng để kiểm tra win

    m_playerPos = newPos;
    m_playerShape.setPosition(m_playerPos);
    
    // Cập nhật hoạt ảnh (Animation)
    if (m_texturesLoaded) {
        if (dy < 0.f) m_playerAnimRow = 0;      // Lên
        else if (dy > 0.f) m_playerAnimRow = 1; // Xuống
        else if (dx > 0.f) m_playerAnimRow = 2; // Phải
        else if (dx < 0.f) m_playerAnimRow = 3; // Trái

        m_isPlayerAnimating = true;
        m_playerAnimTimer = 0.f;
        m_playerAnimFrame = 1; // Bắt đầu ở khung thứ 1 (khung 0 là đứng yên)
        
        auto tex = m_playerSprite.getTexture();
        if (tex) {
            int frameW = tex->getSize().x / 4;
            int frameH = tex->getSize().y / 4;
            m_playerSprite.setTextureRect(sf::IntRect(m_playerAnimFrame * frameW, m_playerAnimRow * frameH, frameW, frameH));
        }
    }

    // Tăng điểm khi tiến lên (chỉ thưởng nếu đi lên cao hơn mức cao nhất từng đạt được)
    if (m_playerPos.y < m_maxPlayerY) {
        m_score += 5;
        m_maxPlayerY = m_playerPos.y;
    }
}

// ============================================================
// Cập nhật trạng thái trò chơi (chạy mỗi frame)
// ============================================================
void GameState::update(float dt) {
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
    checkCollisions(dt);
    checkWinCondition();
    
    // Cập nhật hoạt ảnh người chơi
    if (m_isPlayerAnimating && !m_playerDead) {
        m_playerAnimTimer += dt;
        if (m_playerAnimTimer >= 0.05f) { // Chuyển frame mỗi 0.05 giây
            m_playerAnimTimer = 0.f;
            m_playerAnimFrame++;
            
            if (m_playerAnimFrame >= 4) {
                m_playerAnimFrame = 0; // Quay về đứng yên
                m_isPlayerAnimating = false; // Kết thúc hoạt ảnh
            }
            
            auto tex = m_playerSprite.getTexture();
            if (tex) {
                int frameW = tex->getSize().x / 4;
                int frameH = tex->getSize().y / 4;
                m_playerSprite.setTextureRect(sf::IntRect(m_playerAnimFrame * frameW, m_playerAnimRow * frameH, frameW, frameH));
            }
        }
    }

    // Cập nhật HUD
    m_levelText.setString("Level: " + std::to_string(m_level));
    m_scoreText.setString("Score: " + std::to_string(m_score));
}

// ============================================================
// Cập nhật vị trí xe cộ trên đường
// ============================================================
void GameState::updateObstacles(float dt) {
    for (auto& row : m_terrains) {
        if (row.type != TerrainType::Road) continue;

        for (auto& obs : row.obstacles) {
            float moveX = obs.speed * dt * (obs.movingRight ? 1.f : -1.f);
            obs.shape.move(moveX, 0.f);

            // Quay vòng khi ra khỏi màn hình
            sf::Vector2f pos = obs.shape.getPosition();
            float w = obs.shape.getSize().x;
            if (obs.movingRight && pos.x > 820.f) {
                obs.shape.setPosition(-w, pos.y);
            } else if (!obs.movingRight && pos.x + w < -20.f) {
                obs.shape.setPosition(820.f, pos.y);
            }
        }
    }
}

// ============================================================
// Cập nhật vị trí lá sen trên sông
// ============================================================
void GameState::updateLilyPads(float dt) {
    for (auto& row : m_terrains) {
        if (row.type != TerrainType::River) continue;

        for (auto& pad : row.lilyPads) {
            float moveX = pad.speed * dt * (pad.movingRight ? 1.f : -1.f);
            pad.shape.move(moveX, 0.f);

            // Quay vòng khi ra khỏi màn hình
            sf::Vector2f pos = pad.shape.getPosition();
            float w = pad.shape.getSize().x;
            if (pad.movingRight && pos.x > 820.f) {
                pad.shape.setPosition(-w, pos.y);
            } else if (!pad.movingRight && pos.x + w < -20.f) {
                pad.shape.setPosition(820.f, pos.y);
            }
        }
    }
}

// ============================================================
// Kiểm tra va chạm giữa người chơi và các vật thể
// ============================================================
void GameState::checkCollisions(float dt) {
    sf::FloatRect playerBounds = m_playerShape.getGlobalBounds();
    // Thu nhỏ hitbox để gameplay fair hơn
    sf::FloatRect playerHitbox(
        playerBounds.left + 4.f, playerBounds.top + 4.f,
        playerBounds.width - 8.f, playerBounds.height - 8.f
    );

    for (auto& row : m_terrains) {
        // Kiểm tra va chạm xe cộ
        if (row.type == TerrainType::Road) {
            for (auto& obs : row.obstacles) {
                // Thu nhỏ hitbox của xe cộ vì hình ảnh png có thể có viền trong suốt
                sf::FloatRect carHitbox = obs.shape.getGlobalBounds();
                carHitbox.left += 15.f; // Cắt viền trái
                carHitbox.width -= 30.f; // Cắt viền phải
                carHitbox.top += 8.f; // Cắt viền trên
                carHitbox.height -= 16.f; // Cắt viền dưới

                if (playerHitbox.intersects(carHitbox)) {
                    if (!m_playerDead) {
                        m_playerDead = true;
                        m_deathTimer = 0.f;
                        m_goState = GameOverUIState::Delay;
                        if (!m_currentSaveSession.empty()) {
                            SaveManager::deleteGame(m_currentSaveSession);
                            m_currentSaveSession = "";
                        }
                        
                        if (m_hitByCarLoaded) {
                            m_playerSprite.setTexture(m_hitByCarTexture, true);
                            auto texSize = m_hitByCarTexture.getSize();
                            // Không thiết lập TextureRect nếu sprite là nguyên bản
                            m_playerSprite.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
                            // Thiết lập tỉ lệ sao cho to hơn 1 chút
                            m_playerSprite.setScale(m_playerSize * 1.5f / texSize.x, m_playerSize * 1.5f / texSize.y);
                            // Điều chỉnh lại vị trí cho khớp tâm
                            m_playerSprite.move(-m_playerSize * 0.25f, -m_playerSize * 0.25f);
                        }
                    }
                    m_playerShape.setFillColor(sf::Color::Red);
                    return;
                }
            }
        }

        // Kiểm tra sông: phải đứng trên lá sen
        if (row.type == TerrainType::River) {
            sf::FloatRect rowBounds = row.background.getGlobalBounds();
            // Kiểm tra nếu người chơi đang ở trên hàng sông này
            float playerCenterY = m_playerPos.y + m_playerSize / 2.f;
            if (playerCenterY > rowBounds.top && playerCenterY < rowBounds.top + rowBounds.height) {
                bool onPad = false;
                for (auto& pad : row.lilyPads) {
                    if (playerHitbox.intersects(pad.shape.getGlobalBounds())) {
                        onPad = true;
                        // Di chuyển theo lá sen đồng bộ với dt
                        float padMove = pad.speed * dt * (pad.movingRight ? 1.f : -1.f);
                        m_playerPos.x += padMove;
                        m_playerShape.setPosition(m_playerPos);

                        // Nếu bị đẩy ra ngoài màn hình thì chết
                        if (m_playerPos.x < -m_playerSize || m_playerPos.x > 800.f) {
                            if (!m_playerDead) {
                                m_playerDead = true;
                                m_deathTimer = 0.f;
                                m_goState = GameOverUIState::Delay;
                                if (!m_currentSaveSession.empty()) {
                                    SaveManager::deleteGame(m_currentSaveSession);
                                    m_currentSaveSession = "";
                                }
                            }
                            m_playerShape.setFillColor(sf::Color::Red);
                            return;
                        }
                        break;
                    }
                }
                if (!onPad) {
                    // Rơi xuống sông -> chết
                    if (!m_playerDead) {
                        m_playerDead = true;
                        m_deathTimer = 0.f;
                        m_goState = GameOverUIState::Delay;
                        if (!m_currentSaveSession.empty()) {
                            SaveManager::deleteGame(m_currentSaveSession);
                            m_currentSaveSession = "";
                        }
                        
                        // Làm nhân vật tàng hình để giả hiệu ứng chìm xuống nước
                        m_playerSprite.setColor(sf::Color::Transparent);
                    }
                    m_playerShape.setFillColor(sf::Color(0, 50, 150)); // Đổi màu xanh (chìm)
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
    if (m_playerPos.y <= 0.f) {
        m_level++;
        m_score += 50 * m_level; // Thưởng qua level
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

    // Vẽ các hàng địa hình
    for (auto& row : m_terrains) {
        if (m_texturesLoaded) {
            if (row.type == TerrainType::Grass) { m_grassSprite.setPosition(0, row.yPosition); window.draw(m_grassSprite); }
            else if (row.type == TerrainType::Road) { m_roadSprite.setPosition(0, row.yPosition); window.draw(m_roadSprite); }
            else if (row.type == TerrainType::River) { m_riverSprite.setPosition(0, row.yPosition); window.draw(m_riverSprite); }
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
        for (auto& obs : row.obstacles) {
            if (m_texturesLoaded) {
                // Chọn ngẫu nhiên xe dựa trên tốc độ để ổn định
                int carType = static_cast<int>(obs.speed) % 3;
                sf::Sprite* carSprite = nullptr;
                if (carType == 0 && m_carBlueSprite.getTexture()) carSprite = &m_carBlueSprite;
                else if (carType == 1 && m_carRedSprite.getTexture()) carSprite = &m_carRedSprite;
                else if (carType == 2 && m_carYellowSprite.getTexture()) carSprite = &m_carYellowSprite;
                
                if (!carSprite) {
                    if (m_carBlueSprite.getTexture()) carSprite = &m_carBlueSprite;
                    else if (m_carRedSprite.getTexture()) carSprite = &m_carRedSprite;
                    else if (m_carYellowSprite.getTexture()) carSprite = &m_carYellowSprite;
                }

                if (carSprite) {
                    auto texSize = carSprite->getTexture()->getSize();
                    carSprite->setScale(obs.shape.getSize().x / texSize.x, obs.shape.getSize().y / texSize.y);
                    carSprite->setPosition(obs.shape.getPosition());
                    window.draw(*carSprite);
                } else {
                    window.draw(obs.shape);
                }
            } else {
                window.draw(obs.shape);
                // Vẽ cửa sổ xe (chi tiết nhỏ)
                sf::RectangleShape carWindow(sf::Vector2f(10.f, 8.f));
                carWindow.setFillColor(sf::Color(150, 200, 255, 180));
                carWindow.setPosition(
                    obs.shape.getPosition().x + obs.shape.getSize().x / 2.f - 5.f,
                    obs.shape.getPosition().y + 5.f
                );
                window.draw(carWindow);
            }
        }

        // Vẽ lá sen
        for (auto& pad : row.lilyPads) {
            if (m_texturesLoaded) {
                m_logSprite.setPosition(pad.shape.getPosition());
                window.draw(m_logSprite);
            } else {
                window.draw(pad.shape);
                // Vẽ chi tiết trên lá sen
                sf::CircleShape padDetail(5.f);
                padDetail.setFillColor(sf::Color(0, 200, 0, 120));
                padDetail.setPosition(
                    pad.shape.getPosition().x + pad.shape.getSize().x / 2.f - 5.f,
                    pad.shape.getPosition().y + pad.shape.getSize().y / 2.f - 5.f
                );
                window.draw(padDetail);
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
    if (true) {
        if (m_texturesLoaded) {
            m_playerSprite.setPosition(m_playerPos);
            window.draw(m_playerSprite);
        } else {
            window.draw(m_playerShape);
            // Vẽ mắt cho nhân vật
            sf::CircleShape eye(3.f);
            eye.setFillColor(sf::Color::White);
            eye.setPosition(m_playerPos.x + 8.f, m_playerPos.y + 8.f);
            window.draw(eye);
            eye.setPosition(m_playerPos.x + m_playerSize - 14.f, m_playerPos.y + 8.f);
            window.draw(eye);
            
            sf::CircleShape pupil(1.5f);
            pupil.setFillColor(sf::Color::Black);
            pupil.setPosition(m_playerPos.x + 9.5f, m_playerPos.y + 9.5f);
            window.draw(pupil);
            pupil.setPosition(m_playerPos.x + m_playerSize - 12.5f, m_playerPos.y + 9.5f);
            window.draw(pupil);
        }
    }

    // Vẽ HUD
    if (m_fontLoaded) {
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
    m_playerPos.x = data.playerX;
    m_playerPos.y = data.playerY;
    m_playerShape.setPosition(m_playerPos);
    m_maxPlayerY = data.maxPlayerY;
    
    m_isLoadedGame = true;
    m_currentSaveSession = sessionName;
    
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
        Item item;
        item.shape.setRadius(8.f);
        item.shape.setFillColor(sf::Color::Yellow);
        item.shape.setPosition(sItem.x, sItem.y);
        item.collected = sItem.collected;
        item.points = sItem.points;
        row.items.push_back(item);
    }

    // Khôi phục obstacles
    for (const auto& sObs : savedRow.obstacles) {
        Obstacle obs;
        obs.shape.setSize(sf::Vector2f(sObs.width, sObs.height));
        obs.shape.setPosition(sObs.x, sObs.y);
        obs.shape.setFillColor(sf::Color(sObs.r, sObs.g, sObs.b));
        obs.shape.setOutlineColor(sf::Color(30, 30, 30));
        obs.shape.setOutlineThickness(1.f);
        obs.speed = sObs.speed;
        obs.movingRight = sObs.movingRight;
        row.obstacles.push_back(obs);
    }

    // Khôi phục lilypads
    for (const auto& sPad : savedRow.lilyPads) {
        LilyPad pad;
        pad.shape.setSize(sf::Vector2f(sPad.width, sPad.height));
        pad.shape.setPosition(sPad.x, sPad.y);
        pad.shape.setFillColor(sf::Color(sPad.r, sPad.g, sPad.b));
        pad.shape.setOutlineColor(sf::Color(0, 100, 0));
        pad.shape.setOutlineThickness(1.f);
        pad.speed = sPad.speed;
        pad.movingRight = sPad.movingRight;
        row.lilyPads.push_back(pad);
    }

    m_terrains.push_back(row);
}
