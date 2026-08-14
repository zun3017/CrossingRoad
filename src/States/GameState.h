#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "../Core/State.h"
#include "../UI/TextBox.h"
#include "../UI/Button.h"
#include "../Managers/SaveManager.h"
#include "../Entities/CVEHICLE.h"
#include "../Entities/CPEOPLE.h"
#include "../Entities/CTRAFFICLIGHT.h"
#include "../Entities/CCAR.h"
#include "../Entities/CTRUCK.h"
// Terrain - Lớp cơ sở cho các hàng địa hình (Cỏ, Đường, Sông)
// ============================================================
enum class TerrainType { Grass, Road, River, Railway };


// Removed TrafficLightData (using CTRAFFICLIGHT instead)

struct TrainData {
    sf::RectangleShape shape;
    float speed = 0.f;
    bool movingRight = true;
    bool isActive = false;
};

// Removed Obstacle

struct Log {
    sf::RectangleShape shape;
    float speed;
    bool movingRight;
};

struct ItemData {
    sf::CircleShape shape;
    bool collected = false;
    int points = 10;
};

struct TerrainRow {
    TerrainType type;
    float yPosition;
    sf::RectangleShape background;
    std::vector<std::unique_ptr<CVEHICLE>> vehicles;
    std::vector<Log> logs;              // Khúc gỗ trôi trên sông
    std::vector<ItemData> items;           // Vật phẩm thu thập
    CTRAFFICLIGHT trafficLight;     // Đèn giao thông (cho đường ray)
    TrainData train;                   // Tàu hoả (cho đường ray)

    TerrainRow() = default;
    TerrainRow(TerrainRow&&) noexcept = default;
    TerrainRow& operator=(TerrainRow&&) noexcept = default;
};

// (SaveData được định nghĩa đầy đủ trong SaveManager.h đã include ở trên)

class GameState : public State {
public:
    GameState();
    GameState(int level, int score); // Constructor cho load game

    void init() override;
    void handleInput(sf::RenderWindow& window, sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    
    void loadGame(const std::string& sessionName, const SaveData& data);
private:
    sf::Font m_font;
    bool m_fontLoaded = false;
    
    // Đồ hoạ thật
    bool m_texturesLoaded = false;
    
    // Flag for drowning
    bool m_playerDrowned = false;

    // (m_playerSprite đã xóa - player được vẽ qua window.draw(*m_player) trong CPEOPLE::draw())
    sf::Sprite m_grassSprite;
    sf::Sprite m_roadSprite;
    sf::Sprite m_riverSprite;
    sf::Sprite m_logSprite;
    sf::Sprite m_trackSprite;
    sf::Sprite m_trainSprite;
    sf::Sprite m_lightGreenSprite;
    sf::Sprite m_lightRedSprite;
    sf::Sprite m_lightBlinkSprite;
    sf::Texture m_itemTexture;
    sf::Sprite m_itemSprite;
    
    sf::Texture m_hitByCarTexture;
    bool m_hitByCarLoaded = false;
    
    sf::Texture m_playerDrownTexture;
    bool m_playerDrownLoaded = false;
    

    // Player
    std::unique_ptr<CPEOPLE> m_player;
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

    // HUD text & board
    sf::Text m_levelText;
    sf::Text m_scoreText;
    sf::RectangleShape m_hudBg; // Fallback panel nền cho HUD
    sf::Sprite m_hudBoardSprite;
    bool m_hudBoardLoaded = false;

    // Game over overlay
    sf::RectangleShape m_gameOverOverlay;
    sf::Text m_gameOverText;
    sf::Sprite m_gameOverSprite;
    sf::Texture m_texGameOver;
    sf::Texture m_texConfirm;
    sf::Texture m_texYes;
    sf::Texture m_texNo;
    sf::Texture m_texPlayAgain;
    sf::Texture m_texHome;
    sf::Texture m_texLabel;
    sf::Sprite m_labelSprite;
    sf::Texture m_texEnterNameText;
    sf::Sprite m_enterNameTextSprite;
    sf::Texture m_texNameExistsText;
    sf::Sprite m_nameExistsTextSprite;
    sf::Texture m_texNameScoreRankText;
    sf::Sprite m_nameScoreRankTextSprite;
    sf::Texture m_goldMedalTex;
    sf::Texture m_silverMedalTex;
    sf::Texture m_bronzeMedalTex;
    sf::Sprite m_medalSprite;
    bool m_texGameOverLoaded = false;
    bool m_texConfirmLoaded = false;
    bool m_texYesLoaded = false;
    bool m_texNoLoaded = false;
    bool m_texPlayAgainLoaded = false;
    bool m_texHomeLoaded = false;
    bool m_texLabelLoaded = false;
    bool m_texEnterNameTextLoaded = false;
    bool m_texNameExistsTextLoaded = false;
    bool m_texNameScoreRankTextLoaded = false;
    bool m_medalsLoaded = false;
    
    sf::Text m_confirmBestScoreText;
    sf::Text m_showNameText;
    sf::Text m_showScoreText;
    sf::Text m_showRankText;
    
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
    
    // HUD Buttons (Back, Pause/Continue, Save)
    sf::Texture m_texHudBack;
    sf::Texture m_texHudPause;
    sf::Texture m_texHudContinue;
    sf::Texture m_texHudSave;
    bool m_texHudButtonsLoaded = false;

    std::unique_ptr<Button> m_hudBtnBack;
    std::unique_ptr<Button> m_hudBtnPause;
    std::unique_ptr<Button> m_hudBtnSave;
    void updatePauseButtonTexture();

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
