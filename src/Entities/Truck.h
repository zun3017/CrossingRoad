#pragma once
#include "Vehicle.h"

// Xe tải - lớn và chậm
// Kích thước: 120x44 pixel, tốc độ: 60-120 px/s
// Màu nâu
class Truck : public Vehicle {
public:
    Truck(float x, float y, float speed, int direction);
    ~Truck() override = default;

private:
    // Thiết lập kích thước và màu
    void setupFallback();
};
