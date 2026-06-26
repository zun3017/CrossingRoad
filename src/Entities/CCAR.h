#pragma once
#include "CVEHICLE.h"

// Xe ô tô - nhỏ và nhanh
// Kích thước: 60x40 pixel, tốc độ: 100-200 px/s
// Màu ngẫu nhiên: Đỏ, Xanh, Cam, Trắng
class CCAR : public CVEHICLE {
public:
    CCAR(float x, float y, float speed, int direction);
    ~CCAR() override = default;

private:
    // Thiết lập kích thước và màu ngẫu nhiên
    void setupFallback();
};
