#pragma once
#include "CVEHICLE.h"

// Xe ô tô - nhỏ và nhanh
// Kích thước: 60x40 pixel, tốc độ: 100-200 px/s
// Màu ngẫu nhiên: Đỏ, Xanh, Cam, Trắng
class CCAR : public CVEHICLE {
public:
    CCAR(float x, float y, float speed, int direction, bool isCrazy = false);
    ~CCAR() override = default;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    // Thiết lập kích thước và màu ngẫu nhiên
    void setupFallback();

    sf::Sprite m_smokeSprite;
    bool m_hasSmokeTexture = false;
};
