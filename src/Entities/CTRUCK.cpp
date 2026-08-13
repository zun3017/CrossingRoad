#include "CTRUCK.h"
#include "../Core/ResourceManager.h"

CTRUCK::CTRUCK(float x, float y, float speed, int direction)
    : CVEHICLE(x, y, speed, direction)
{
    // Ưu tiên load truck.png, nếu không có thì dùng car_red.png tạm
    const std::string texFiles[] = {
        "assets/textures/truck.png",
        "assets/textures/car_red.png"
    };

    for (const auto& path : texFiles) {
        try {
            auto& tex = ResourceManager<sf::Texture>::getInstance().get(path);
            if (tex.getSize().x > 0) {
                m_sprite.setTexture(tex);
                m_usesFallback = false;
                m_sprite.setTextureRect(sf::IntRect(0, 0,
                    static_cast<int>(tex.getSize().x),
                    static_cast<int>(tex.getSize().y)));
                // Xe tải: giữ nguyên tỷ lệ, cao 44px
                float scaleY = 44.f / static_cast<float>(tex.getSize().y);
                float scaleX = scaleY;
                if (m_direction > 0) {
                    m_sprite.setScale(-scaleX, scaleY);
                    m_sprite.setOrigin(static_cast<float>(tex.getSize().x), 0.f);
                } else {
                    m_sprite.setScale(scaleX, scaleY);
                    m_sprite.setOrigin(0.f, 0.f);
                }
                break; // Load thành công → thoát vòng lặp
            }
        } catch (...) {}
    }

    setupFallback();
}

void CTRUCK::setupFallback() {
    // Xe tải: lớn hơn và màu nâu đất để dễ phân biệt với xe hơi
    setFallbackSize({ 90.f, 44.f });
    setFallbackColor(sf::Color(139, 90, 43)); // Nâu đất
    m_fallbackShape.setOutlineThickness(2.f);
    m_fallbackShape.setOutlineColor(sf::Color(80, 50, 20));
}



