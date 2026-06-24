#pragma once

// =============================================================================
// ResourceManager.h - Quản lý tài nguyên (Texture, Font, SoundBuffer)
// Singleton template - mỗi loại tài nguyên có 1 instance duy nhất
// Tự động load file lần đầu, trả về cache cho các lần sau
// =============================================================================

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>

template <typename Resource>
class ResourceManager {
public:
    // Lấy instance duy nhất (Singleton pattern)
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Lấy tài nguyên theo tên file, tự động load nếu chưa có trong cache
    Resource& get(const std::string& filename) {
        // Tìm trong cache trước
        auto it = m_resources.find(filename);
        if (it != m_resources.end()) {
            return *(it->second);
        }

        // Chưa có -> load từ file
        auto resource = std::make_unique<Resource>();
        if (!resource->loadFromFile(filename)) {
            std::cerr << "[ResourceManager] Khong the load: " << filename << std::endl;
            // Vẫn lưu resource rỗng vào cache để tránh load lại liên tục
            // Resource mặc định (texture trắng, font rỗng...) sẽ được dùng
            m_resources[filename] = std::move(resource);
            return *(m_resources[filename]);
        }

        std::cout << "[ResourceManager] Da load: " << filename << std::endl;
        m_resources[filename] = std::move(resource);
        return *(m_resources[filename]);
    }

    // Load trước tài nguyên (dùng khi muốn preload lúc loading screen)
    void preload(const std::string& filename) {
        get(filename); // Gọi get() sẽ tự load nếu chưa có
    }

    // Kiểm tra tài nguyên đã được load chưa
    bool isLoaded(const std::string& filename) const {
        return m_resources.find(filename) != m_resources.end();
    }

    // Xóa tài nguyên khỏi cache (giải phóng bộ nhớ)
    void unload(const std::string& filename) {
        m_resources.erase(filename);
    }

    // Xóa toàn bộ cache
    void clear() {
        m_resources.clear();
    }

private:
    // Constructor/Destructor private (Singleton)
    ResourceManager() = default;
    ~ResourceManager() = default;

    // Không cho copy hoặc move
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // Cache tài nguyên: key = đường dẫn file, value = con trỏ tài nguyên
    std::map<std::string, std::unique_ptr<Resource>> m_resources;
};

// =============================================================================
// Alias tiện dụng cho các loại tài nguyên thường dùng
// Sử dụng: TextureManager::getInstance().get("assets/player.png");
// =============================================================================
using TextureManager  = ResourceManager<sf::Texture>;
using FontManager     = ResourceManager<sf::Font>;
using SoundManager    = ResourceManager<sf::SoundBuffer>;

// =============================================================================
// GHI CHÚ: Template class được định nghĩa hoàn toàn trong header file.
// Các instantiation tường minh (explicit instantiation) không cần thiết
// vì toàn bộ implementation nằm trong header.
//
// Các kiểu được hỗ trợ:
//   - ResourceManager<sf::Texture>    (alias: TextureManager)
//   - ResourceManager<sf::Font>       (alias: FontManager)
//   - ResourceManager<sf::SoundBuffer>(alias: SoundManager)
// =============================================================================
