#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "../Core/State.h"
#include "../UI/TextBox.h"
#include "../UI/Button.h"
#include "../Managers/SaveManager.h"

// ============================================================
// Terrain - Lớp cơ sở cho các hàng địa hình (Cỏ, Đường, Sông)
// ============================================================
enum class TerrainType { Grass, Road, River, Railway };

enum class LightState { Green, Blinking, Red };

struct TrafficLightData {
    LightState state = LightState::Green;
    float timer = 0.f;
    float animTimer = 0.f;
    int frameIndex = 0;
};

struct TrainData {
    sf::RectangleShape shape;
    float speed = 0.f;
    bool movingRight = true;
    bool isActive = false;
};

struct Obstacle {
    sf::RectangleShape shape;
    float speed;
    bool movingRight;
};

struct LilyPad {
    sf::RectangleShape shape;
    float speed;
    bool movingRight;
};

struct Item {
    sf::CircleShape shape;
    bool collected = false;
    int points = 10;
};

struct TerrainRow {
    TerrainType type;
    float yPosition;
    sf::RectangleShape background;
    std::vector<Obstacle> obstacles;   // Xe cộ trên đường
    std::vector<LilyPad> lilyPads;     // Lá sen trên sông
    std::vector<Item> items;           // Vật phẩm thu thập
    TrafficLightData trafficLight;     // Đèn giao thông (cho đường ray)
    TrainData train;                   // Tàu hoả (cho đường ray)
};

struct SaveData;

class GameState : public State {
public:
    GameState();
    GameState(int level, int score); // Constructor cho load game

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    
    void loadGame(const std::string& sessionName, const SaveData& data);

    sf::Font m_font;
    bool m_fontLoaded = false;
    
    // Đồ hoạ thật
    bool m_texturesLoaded = false;
    
    // Flag for drowning
    bool m_playerDrowned = false;

    sf::Sprite m_playerSprite;
    sf::Sprite m_grassSprite;
    sf::Sprite m_roadSprite;
    sf::Sprite m_riverSprite;
    sf::Sprite m_logSprite;
    sf::Sprite m_carBlueSprite;
    sf::Sprite m_carRedSprite;
    sf::Sprite m_carYellowSprite;
    sf::Sprite m_trackSprite;
    sf::Sprite m_trainSprite;
    sf::Sprite m_lightGreenSprite;
    sf::Sprite m_lightRedSprite;
    sf::Sprite m_lightBlinkSprite;
    sf::Texture m_itemTexture;
    sf::Sprite m_itemSprite;
    
    sf::Texture m_hitByCarTexture;
    bool m_hitByCarLoaded = false;
    
    // UI Game Over Animation
    int m_playerAnimRow = 0;   // 0: Lên, 1: Xuống, 2: Phải, 3: Trái
    int m_playerAnimFrame = 0; // 0, 1, 2, 3
    bool m_isPlayerAnimating = false;
    float m_playerAnimTimer = 0.f;

    // Player
    sf::RectangleShape m_playerShape;
    sf::Vector2f m_playerPos;
    float m_playerSize = 40.f;
    bool m_playerDead = false;
    float m_deathTimer = 0.f;
    float m_moveCooldownTimer = 0.f; // Thời gian chờ giữa 2 lần di chuyển
    float m_maxPlayerY = 600.f; // theo dõi độ cao cao nhất đạt được để tính điểm

    // Map terrain
    std::vector<TerrainRow> m_terrains;
    int m_totalRows = 13;
    float m_cellSize = 48.f;

    // Game stats
    int m_level = 1;
    int m_score = 0;

    // HUD text
    sf::Text m_levelText;
    sf::Text m_scoreText;

    // Game over overlay
    sf::RectangleShape m_gameOverOverlay;
    sf::Text m_gameOverText;
    
    // UI Game Over
    enum class GameOverUIState { None, Delay, EnterName, ConfirmIdentity, ShowScore };
    GameOverUIState m_goState = GameOverUIState::None;

    enum class DeferredAction { None, Restart, Quit };
    DeferredAction m_deferredAction = DeferredAction::None;

    std::unique_ptr<TextBox> m_nameInput;
    std::unique_ptr<Button> m_btnOk;
    std::unique_ptr<Button> m_playAgainBtn;
    std::unique_ptr<Button> m_menuBtn;
    
    sf::Text m_confirmText;
    std::unique_ptr<Button> m_btnYes;
    std::unique_ptr<Button> m_btnNo;

    sf::RectangleShape m_scoreBoard;
    sf::RectangleShape m_enterNameBoard;
    sf::Text m_rankText;
    sf::Text m_enterNameText;
    int m_finalRank = 0;
    bool m_scoreSaved = false;
    
    std::function<void()> m_submitNameFunc;

    // Pause overlay
    bool m_paused = false;
    sf::RectangleShape m_pauseOverlay;
    sf::Text m_pauseText;
    sf::Text m_pauseInstruction;
    
    // Save Game UI
    enum class PauseUIState { Main, EnterSaveName };
    PauseUIState m_pauseUIState = PauseUIState::Main;
    std::unique_ptr<TextBox> m_saveNameInput;
    bool m_ignoreNextText = false;
    std::unique_ptr<Button> m_btnSaveOk;
    sf::RectangleShape m_saveNameBoard;
    sf::Text m_saveNamePrompt;
    
    float m_totalPlaytime = 0.f;
    std::string m_currentSaveSession = "";
    bool m_isLoadedGame = false;
    void saveCurrentGameState(const std::string& sessionName);
    
    // Nút tạm
    std::unique_ptr<Button> m_testBtn;

    // Phương thức nội bộ
    void initPlayer();
    void generateMap();
    void createGrassRow(float y, bool safeZone = false);
    void createRoadRow(float y);
    void createRiverRow(float y);
    void createRailwayRow(float y);
    void createExactRow(const SavedTerrainRow& savedRow);
    void updateObstacles(float dt);
    void updateLilyPads(float dt);
    void updateRailway(float dt);
    void checkCollisions(float dt);
    void checkWinCondition();
    void movePlayer(float dx, float dy);
    void resetForNextLevel();
    void initHUD();
    void initOverlays();
    void setupGameOverUI();
};
