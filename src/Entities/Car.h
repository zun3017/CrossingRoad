#pragma once
#include "Vehicle.h"

// Xe ô tô - nhỏ và nhanh
// Kích thước: 60x40 pixel, tốc độ: 100-200 px/s
// Màu ngẫu nhiên: Đỏ, Xanh, Cam, Trắng
class Car : public Vehicle {
public:
    Car(float x, float y, float speed, int direction);
    ~Car() override = default;

private:
    // Thiết lập kích thước và màu ngẫu nhiên
    void setupFallback();
};
