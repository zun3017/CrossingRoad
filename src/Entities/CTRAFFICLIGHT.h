#pragma once
#include <SFML/Graphics.hpp>
#include "LightStateEnum.h"

// CTRAFFICLIGHT.h - Struct dữ liệu đèn tín hiệu cho hàng đường ray (Railway)
// Có 3 trạng thái: Green -> Blinking -> Red (khi tàu đến)
// Dùng trong struct TerrainRow của GameState
//
// LƯU Ý: Toàn bộ logic update (chuyển trạng thái, đếm giờ) được xử lý
// trực tiếp trong GameState::updateRailway() — struct này chỉ là data container.

struct CTRAFFICLIGHT {
    LightState state    = LightState::Green;
    float timer         = 0.f;
    float greenDuration = 3.f;
    float redDuration   = 2.f;
    int   frameIndex    = 0;
    float animTimer     = 0.f;
    // shape chỉ dùng cho fallback (không có texture)
    sf::RectangleShape shape;

    CTRAFFICLIGHT() {
        shape.setSize(sf::Vector2f(12.f, 30.f));
        shape.setFillColor(sf::Color::Green);
    }
    // Không có hàm update() — GameState::updateRailway() xử lý toàn bộ logic
};
