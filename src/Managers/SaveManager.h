#pragma once

// SaveManager.h - Quản lý lưu/đọc dữ liệu game
// Hỗ trợ save/load game state, bảng xếp hạng (highscores)

#include <string>
#include <vector>

struct SavedObstacle {
    float x, y, width, height, speed;
    bool movingRight;
    int r, g, b;
};

struct SavedLilyPad {
    float x, y, width, height, speed;
    bool movingRight;
    int r, g, b;
};

struct SavedItem {
    float x, y;
    bool collected;
    int points;
};

struct SavedTerrainRow {
    int type; // 0=Grass, 1=Road, 2=River
    float yPosition;
    std::vector<SavedObstacle> obstacles;
    std::vector<SavedLilyPad> lilyPads;
    std::vector<SavedItem> items;
};

// Dữ liệu lưu trữ game
struct SaveData {
    std::string playerName = "Player";   // Tên người chơi
    int characterType = 0;                // Loại nhân vật (0, 1, 2, ...)
    int score = 0;                        // Điểm số
    int level = 1;                        // Cấp độ hiện tại
    int gameMode = 0;                     // Chế độ chơi: 0=Classic, 1=Endless
    
    float playerX = 400.f;
    float playerY = 500.f;
    float maxPlayerY = 500.f;
    
    int numTerrains = 0;                  // Số lượng terrain đã tạo
    std::vector<SavedTerrainRow> terrains; // Chi tiết từng terrain
    
    int playTimeSeconds = 0;              // Thời gian chơi thực tế (giây)
    std::string timestamp = "";           // Thời gian hệ thống lúc lưu
};

// Một dòng trong bảng xếp hạng
struct HighscoreEntry {
    std::string name = "";  // Tên người chơi
    int score = 0;           // Điểm số

    // Để sắp xếp giảm dần theo điểm
    bool operator>(const HighscoreEntry& other) const {
        return score > other.score;
    }

    bool operator<(const HighscoreEntry& other) const {
        return score < other.score;
    }
};

struct SaveFileInfo {
    std::string filename;
    int playTimeSeconds;
    std::string timestamp;
};

class SaveManager {
public:
    // === QUẢN LÝ FILE SAVE GAME ===

    // Lưu game vào file assets/data/<filename>.sav
    static bool saveGame(const std::string& filename, const SaveData& data);

    // Đọc game từ file assets/data/<filename>.sav
    static bool loadGame(const std::string& filename, SaveData& data);

    // Xóa file save
    static bool deleteGame(const std::string& filename);

    // Đổi tên file save
    static bool renameGame(const std::string& oldName, const std::string& newName);

    // Lấy danh sách các file save hiện có
    static std::vector<SaveFileInfo> getSaveList();

    // === QUẢN LÝ BẢNG XẾP HẠNG ===

    // Lưu bảng xếp hạng vào file assets/data/highscores.dat
    static void saveHighscores(const std::vector<HighscoreEntry>& scores);

    // Đọc bảng xếp hạng (đã sắp xếp giảm dần, tối đa 10 dòng)
    static std::vector<HighscoreEntry> loadHighscores();

    // Thêm một điểm mới vào bảng xếp hạng (tự động sắp xếp và giới hạn 10)
    static void addHighscore(const std::string& name, int score);

    // Kiểm tra xem tên đã có trong bảng xếp hạng chưa
    static bool hasHighscore(const std::string& name, int& outScore);

    // Cập nhật điểm cho người chơi (chỉ ghi đè nếu điểm cao hơn)
    static void updateHighscore(const std::string& name, int newScore);

private:
    // Thư mục lưu trữ dữ liệu
    static const std::string DATA_DIR;

    // Đảm bảo thư mục tồn tại (tạo nếu chưa có)
    static void ensureDataDirectory();
};
