#pragma once
#include "CVEHICLE.h"

// Xe tải - lớn và chậm
// Kích thước: 120x44 pixel, tốc độ: 60-120 px/s
// Màu nâu
class CTRUCK : public CVEHICLE {
public:
    CTRUCK(float x, float y, float speed, int direction);
    ~CTRUCK() override = default;

private:
    // Thiết lập kích thước và màu
    void setupFallback();
};
