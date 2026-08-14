#pragma once

// LightStateEnum.h - Định nghĩa duy nhất cho enum trạng thái đèn giao thông
// Được dùng bởi cả TrafficLight.h (đường) và CTRAFFICLIGHT.h (đường ray)

enum class LightState {
    Green,    // Đèn xanh - xe/tàu được phép đi
    Blinking, // Đèn nhấp nháy vàng - cảnh báo (dùng cho đường ray)
    Red       // Đèn đỏ - xe/tàu phải dừng
};
