#pragma once
#include "Terrain.h"
#include "Vehicle.h"
#include "TrafficLight.h"
#include <vector>
#include <memory>

// Hàng đường - có xe cộ qua lại, có đèn giao thông
class Road : public Terrain {
public:
    // vehicleSpeed: tốc độ cơ bản, direction: +1 phải, -1 trái
    Road(float posY, float vehicleSpeed, int direction);
    ~Road() override = default;

    void update(float dt) override;

    // Sinh xe mới (Car hoặc Truck ngẫu nhiên)
    void spawnVehicle();

    // Truy cập danh sách xe
    const std::vector<std::unique_ptr<Vehicle>>& getVehicles() const { return m_vehicles; }

    // Truy cập đèn giao thông
    TrafficLight& getTrafficLight() { return m_trafficLight; }
    const TrafficLight& getTrafficLight() const { return m_trafficLight; }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<std::unique_ptr<Vehicle>> m_vehicles;
    float m_vehicleSpeed;    // Tốc độ cơ bản (pixel/giây)
    int m_direction;         // Hướng di chuyển xe
    float m_spawnTimer;      // Bộ đếm thời gian spawn
    float m_spawnInterval;   // Khoảng cách giữa các lần spawn (1.5-3.5s)

    TrafficLight m_trafficLight;

    // Vạch kẻ đường đứt nét (dashed lane markings)
    std::vector<sf::RectangleShape> m_laneMarkings;
    void setupLaneMarkings();
};
