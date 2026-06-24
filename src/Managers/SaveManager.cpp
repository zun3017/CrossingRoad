// SaveManager.cpp - Triển khai quản lý lưu/đọc dữ liệu game

#define _CRT_SECURE_NO_WARNINGS
#include "SaveManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <iostream>

// Đường dẫn thư mục lưu trữ
const std::string SaveManager::DATA_DIR = "assets/data/";

void SaveManager::ensureDataDirectory() {
    // Tạo thư mục nếu chưa tồn tại
    try {
        std::filesystem::create_directories(DATA_DIR);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[SaveManager] Loi tao thu muc: " << e.what() << std::endl;
    }
}

// ============================================================
// LƯU GAME
// ============================================================
bool SaveManager::saveGame(const std::string& filename, const SaveData& data) {
    ensureDataDirectory();

    std::string filepath = DATA_DIR + filename + ".sav";
    std::ofstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "[SaveManager] Khong the mo file de ghi: " << filepath << std::endl;
        return false;
    }

    // Ghi từng trường trên mỗi dòng
    // Dòng 1: Tên người chơi
    file << data.playerName << "\n";
    // Dòng 2: Loại nhân vật
    file << data.characterType << "\n";
    // Dòng 3: Điểm số
    file << data.score << "\n";
    // Dòng 4: Cấp độ
    file << data.level << "\n";
    // Dòng 5: Chế độ chơi
    file << data.gameMode << "\n";
    // Dòng 6: playerX playerY maxPlayerY
    file << data.playerX << " " << data.playerY << " " << data.maxPlayerY << "\n";
    // Dòng 7: Số lượng terrain
    file << data.numTerrains << "\n";
    // Dòng 8+: Chi tiết terrain
    for (int i = 0; i < data.numTerrains; ++i) {
        if (i < static_cast<int>(data.terrains.size())) {
            const auto& row = data.terrains[i];
            file << row.type << " " << row.yPosition << "\n";
            file << row.obstacles.size() << "\n";
            for (const auto& obs : row.obstacles) {
                file << obs.x << " " << obs.y << " " << obs.width << " " << obs.height << " " << obs.speed << " " << obs.movingRight << " " << obs.r << " " << obs.g << " " << obs.b << "\n";
            }
            file << row.lilyPads.size() << "\n";
            for (const auto& pad : row.lilyPads) {
                file << pad.x << " " << pad.y << " " << pad.width << " " << pad.height << " " << pad.speed << " " << pad.movingRight << " " << pad.r << " " << pad.g << " " << pad.b << "\n";
            }
            file << row.items.size() << "\n";
            for (const auto& item : row.items) {
                file << item.x << " " << item.y << " " << item.collected << " " << item.points << "\n";
            }
        } else {
            file << "0 0\n0\n0\n0\n";
        }
    }

    // Dòng cuối: playTimeSeconds
    file << data.playTimeSeconds << "\n";
    // Dòng 9: timestamp
    std::string finalTimestamp = data.timestamp;
    if (finalTimestamp.empty()) {
        std::time_t t = std::time(nullptr);
        char buf[100];
        if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t))) {
            finalTimestamp = buf;
        } else {
            finalTimestamp = "Unknown Time";
        }
    }
    file << finalTimestamp << "\n";

    file.close();
    return true;
}

// ============================================================
// ĐỌC GAME
// ============================================================
bool SaveManager::loadGame(const std::string& filename, SaveData& data) {
    std::string filepath = DATA_DIR + filename + ".sav";
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "[SaveManager] Khong the mo file de doc: " << filepath << std::endl;
        return false;
    }

    // Đọc từng trường
    // Dòng 1: Tên người chơi (có thể chứa khoảng trắng)
    if (!std::getline(file, data.playerName)) return false;

    // Các dòng số nguyên
    std::string line;

    // Dòng 2: characterType
    if (!std::getline(file, line)) return false;
    try { data.characterType = std::stoi(line); }
    catch (...) { data.characterType = 0; }

    // Dòng 3: score
    if (!std::getline(file, line)) return false;
    try { data.score = std::stoi(line); }
    catch (...) { data.score = 0; }

    // Dòng 4: level
    if (!std::getline(file, line)) return false;
    try { data.level = std::stoi(line); }
    catch (...) { data.level = 1; }

    // Dòng 5: gameMode
    if (!std::getline(file, line)) return false;
    try { data.gameMode = std::stoi(line); }
    catch (...) { data.gameMode = 0; }

    // Dòng 6: playerX playerY maxPlayerY
    if (!std::getline(file, line)) return false;
    {
        std::stringstream ss(line);
        ss >> data.playerX >> data.playerY >> data.maxPlayerY;
    }

    // Dòng 7: numTerrains
    if (!std::getline(file, line)) return false;
    try { data.numTerrains = std::stoi(line); }
    catch (...) { data.numTerrains = 0; }

    // Dòng 8+: terrains
    data.terrains.clear();
    for (int i = 0; i < data.numTerrains; ++i) {
        SavedTerrainRow row;
        if (!std::getline(file, line)) break;
        std::stringstream ss(line);
        ss >> row.type >> row.yPosition;

        // obstacles
        if (!std::getline(file, line)) break;
        int numObs = std::stoi(line);
        for (int j = 0; j < numObs; j++) {
            if (!std::getline(file, line)) break;
            std::stringstream ssObs(line);
            SavedObstacle obs;
            ssObs >> obs.x >> obs.y >> obs.width >> obs.height >> obs.speed >> obs.movingRight >> obs.r >> obs.g >> obs.b;
            row.obstacles.push_back(obs);
        }

        // lilyPads
        if (!std::getline(file, line)) break;
        int numPads = std::stoi(line);
        for (int j = 0; j < numPads; j++) {
            if (!std::getline(file, line)) break;
            std::stringstream ssPad(line);
            SavedLilyPad pad;
            ssPad >> pad.x >> pad.y >> pad.width >> pad.height >> pad.speed >> pad.movingRight >> pad.r >> pad.g >> pad.b;
            row.lilyPads.push_back(pad);
        }

        // items
        if (!std::getline(file, line)) break;
        int numItems = std::stoi(line);
        for (int j = 0; j < numItems; j++) {
            if (!std::getline(file, line)) break;
            std::stringstream ssItem(line);
            SavedItem item;
            ssItem >> item.x >> item.y >> item.collected >> item.points;
            row.items.push_back(item);
        }
        data.terrains.push_back(row);
    }

    // Đọc thêm các dòng playTimeSeconds và timestamp nếu có (tránh lỗi file cũ)
    if (std::getline(file, line)) {
        try { data.playTimeSeconds = std::stoi(line); }
        catch (...) { data.playTimeSeconds = 0; }
    } else {
        data.playTimeSeconds = 0;
    }
    
    if (std::getline(file, line)) {
        data.timestamp = line;
    } else {
        data.timestamp = "Unknown Time";
    }

    file.close();
    return true;
}

// ============================================================
// XÓA SAVE
// ============================================================
bool SaveManager::deleteGame(const std::string& filename) {
    std::string filepath = DATA_DIR + filename + ".sav";
    int result = std::remove(filepath.c_str());

    if (result != 0) {
        std::cerr << "[SaveManager] Khong the xoa file: " << filepath << std::endl;
        return false;
    }

    return true;
}

// ============================================================
// ĐỔI TÊN SAVE
// ============================================================
bool SaveManager::renameGame(const std::string& oldName, const std::string& newName) {
    std::string oldPath = DATA_DIR + oldName + ".sav";
    std::string newPath = DATA_DIR + newName + ".sav";

    int result = std::rename(oldPath.c_str(), newPath.c_str());

    if (result != 0) {
        std::cerr << "[SaveManager] Khong the doi ten file: "
                  << oldPath << " -> " << newPath << std::endl;
        return false;
    }

    return true;
}

// ============================================================
// LẤY DANH SÁCH SAVE
// ============================================================
std::vector<SaveFileInfo> SaveManager::getSaveList() {
    std::vector<SaveFileInfo> saveFiles;

    ensureDataDirectory();

    try {
        for (const auto& entry : std::filesystem::directory_iterator(DATA_DIR)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".sav") {
                    std::string stem = entry.path().stem().string();
                    SaveData temp;
                    if (loadGame(stem, temp)) {
                        // Bỏ qua file cũ bị lỗi hoặc không có địa hình (do phiên bản cũ)
                        if (temp.numTerrains > 0 && !temp.terrains.empty()) {
                            SaveFileInfo info;
                            info.filename = stem;
                            info.playTimeSeconds = temp.playTimeSeconds;
                            info.timestamp = temp.timestamp;
                            saveFiles.push_back(info);
                        }
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[SaveManager] Loi quet thu muc: " << e.what() << std::endl;
    }

    // Sắp xếp theo alphabet
    std::sort(saveFiles.begin(), saveFiles.end(), [](const SaveFileInfo& a, const SaveFileInfo& b) {
        return a.filename < b.filename;
    });

    return saveFiles;
}

// ============================================================
// LƯU BẢNG XẾP HẠNG
// ============================================================
void SaveManager::saveHighscores(const std::vector<HighscoreEntry>& scores) {
    ensureDataDirectory();

    std::string filepath = DATA_DIR + "highscores.dat";
    std::ofstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "[SaveManager] Khong the ghi highscores: " << filepath << std::endl;
        return;
    }

    // Giới hạn tối đa 10 entry
    int count = std::min(static_cast<int>(scores.size()), 10);
    file << count << "\n";

    for (int i = 0; i < count; ++i) {
        // Mỗi entry: tên trên 1 dòng, điểm trên dòng tiếp theo
        file << scores[static_cast<size_t>(i)].name << "\n";
        file << scores[static_cast<size_t>(i)].score << "\n";
    }

    file.close();
}

// ============================================================
// ĐỌC BẢNG XẾP HẠNG
// ============================================================
std::vector<HighscoreEntry> SaveManager::loadHighscores() {
    std::vector<HighscoreEntry> scores;

    std::string filepath = DATA_DIR + "highscores.dat";
    std::ifstream file(filepath);

    if (!file.is_open()) {
        // Không có file -> trả về danh sách rỗng
        return scores;
    }

    // Đọc số lượng entry
    std::string line;
    if (!std::getline(file, line)) return scores;

    int count = 0;
    try { count = std::stoi(line); }
    catch (...) { return scores; }

    // Giới hạn tối đa 10
    count = std::min(count, 10);

    for (int i = 0; i < count; ++i) {
        HighscoreEntry entry;

        // Đọc tên
        if (!std::getline(file, entry.name)) break;

        // Đọc điểm
        if (!std::getline(file, line)) break;
        try { entry.score = std::stoi(line); }
        catch (...) { entry.score = 0; }

        scores.push_back(entry);
    }

    file.close();

    // Sắp xếp giảm dần theo điểm
    std::sort(scores.begin(), scores.end(),
        [](const HighscoreEntry& a, const HighscoreEntry& b) {
            return a.score > b.score;
        });

    // Chỉ giữ tối đa 10
    if (scores.size() > 10) {
        scores.resize(10);
    }

    return scores;
}

// ============================================================
// THÊM ĐIỂM VÀO BẢNG XẾP HẠNG
// ============================================================
void SaveManager::addHighscore(const std::string& name, int score) {
    // Đọc bảng xếp hạng hiện tại
    std::vector<HighscoreEntry> scores = loadHighscores();

    // Thêm entry mới
    HighscoreEntry newEntry;
    newEntry.name = name;
    newEntry.score = score;
    scores.push_back(newEntry);

    // Sắp xếp giảm dần
    std::sort(scores.begin(), scores.end(),
        [](const HighscoreEntry& a, const HighscoreEntry& b) {
            return a.score > b.score;
        });

    // Giới hạn 10 entry
    if (scores.size() > 10) {
        scores.resize(10);
    }

    // Ghi lại
    saveHighscores(scores);
}

// ============================================================
// KIỂM TRA ĐIỂM
// ============================================================
bool SaveManager::hasHighscore(const std::string& name, int& outScore) {
    std::vector<HighscoreEntry> scores = loadHighscores();
    for (const auto& entry : scores) {
        if (entry.name == name) {
            outScore = entry.score;
            return true;
        }
    }
    return false;
}

// ============================================================
// CẬP NHẬT ĐIỂM
// ============================================================
void SaveManager::updateHighscore(const std::string& name, int newScore) {
    std::vector<HighscoreEntry> scores = loadHighscores();
    bool found = false;
    for (auto& entry : scores) {
        if (entry.name == name) {
            found = true;
            if (newScore > entry.score) {
                entry.score = newScore;
            }
            break;
        }
    }
    
    if (!found) {
        HighscoreEntry newEntry;
        newEntry.name = name;
        newEntry.score = newScore;
        scores.push_back(newEntry);
    }
    
    // Sắp xếp giảm dần
    std::sort(scores.begin(), scores.end(),
        [](const HighscoreEntry& a, const HighscoreEntry& b) {
            return a.score > b.score;
        });

    // Giới hạn 10 entry
    if (scores.size() > 10) {
        scores.resize(10);
    }

    // Ghi lại
    saveHighscores(scores);
}
