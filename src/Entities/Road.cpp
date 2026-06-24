#include "Road.h"
#include "Car.h"
#include "Truck.h"
#include <random>
#include <algorithm>

// Bộ sinh số ngẫu nhiên cho đường
static std::mt19937& getRoadRNG() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

Road::Road(float posY, float vehicleSpeed, int direction)
    : Terrain(posY)
    , m_vehicleSpeed(vehicleSpeed)
    , m_direction(direction)
    , m_spawnTimer(0.f)
    , m_trafficLight(10.f, posY + 9.f) // Đèn ở bên trái, căn giữa dọc
{
    // Nền xám đậm cho đường
    m_background.setFillColor(sf::Color(60, 60, 60));

    // Khoảng cách spawn ngẫu nhiên ban đầu
    std::uniform_real_distribution<float> intervalDist(1.5f, 3.5f);
    m_spawnInterval = intervalDist(getRoadRNG());

    // Tạo vạch kẻ đường
    setupLaneMarkings();

    // Spawn xe đầu tiên ngay lập tức
    spawnVehicle();
}

void Road::setupLaneMarkings() {
    // Vạch kẻ đường đứt nét màu trắng ở giữa hàng
    m_laneMarkings.clear();

    float dashWidth = 30.f;
    float dashHeight = 3.f;
    float gap = 20.f;
    float y = m_posY + m_height / 2.f - dashHeight / 2.f;

    for (float x = 0.f; x < m_width; x += dashWidth + gap) {
        sf::RectangleShape dash;
        dash.setSize({ dashWidth, dashHeight });
        dash.setPosition(x, y);
        dash.setFillColor(sf::Color(200, 200, 200, 180)); // Trắng mờ
        m_laneMarkings.push_back(dash);
    }
}

void Road::update(float dt) {
    // Cập nhật đèn giao thông
    m_trafficLight.update(dt);

    // Cập nhật xe: dừng/chạy theo đèn, rồi di chuyển
    bool redLight = m_trafficLight.isRed();
    for (auto& vehicle : m_vehicles) {
        if (redLight) {
            vehicle->stop();
        }
        else {
            vehicle->resume();
        }
        vehicle->update(dt);
    }

    // Xóa xe đã đi ra ngoài màn hình
    m_vehicles.erase(
        std::remove_if(m_vehicles.begin(), m_vehicles.end(),
            [](const std::unique_ptr<Vehicle>& v) {
                return v->isOffScreen();
            }),
        m_vehicles.end()
    );

    // Spawn xe mới theo bộ đếm thời gian
    m_spawnTimer += dt;
    if (m_spawnTimer >= m_spawnInterval) {
        m_spawnTimer = 0.f;
        spawnVehicle();

        // Ngẫu nhiên lại khoảng cách spawn
        std::uniform_real_distribution<float> intervalDist(1.5f, 3.5f);
        m_spawnInterval = intervalDist(getRoadRNG());
    }
}

void Road::spawnVehicle() {
    // Chọn ngẫu nhiên: xe ô tô (0) hoặc xe tải (1)
    std::uniform_int_distribution<int> typeDist(0, 1);

    // Vị trí spawn: ngoài màn hình phía xe đến
    float startX;
    if (m_direction > 0) {
        startX = -130.f; // Từ bên trái
    }
    else {
        startX = 810.f;  // Từ bên phải
    }

    if (typeDist(getRoadRNG()) == 0) {
        // Xe ô tô: nhỏ và nhanh (100-200 px/s)
        std::uniform_real_distribution<float> speedDist(100.f, 200.f);
        float speed = speedDist(getRoadRNG());
        float y = m_posY + 4.f; // Căn giữa xe 40px trong hàng 48px
        m_vehicles.push_back(std::make_unique<Car>(startX, y, speed, m_direction));
    }
    else {
        // Xe tải: lớn và chậm (60-120 px/s)
        std::uniform_real_distribution<float> speedDist(60.f, 120.f);
        float speed = speedDist(getRoadRNG());
        float y = m_posY + 2.f; // Căn giữa xe 44px trong hàng 48px
        m_vehicles.push_back(std::make_unique<Truck>(startX, y, speed, m_direction));
    }
}

void Road::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // 1. Vẽ nền đường xám
    Terrain::draw(target, states);

    // 2. Vẽ vạch kẻ đường
    for (const auto& dash : m_laneMarkings) {
        target.draw(dash, states);
    }

    // 3. Vẽ xe
    for (const auto& vehicle : m_vehicles) {
        target.draw(*vehicle, states);
    }

    // 4. Vẽ đèn giao thông
    m_trafficLight.draw(target);
}
