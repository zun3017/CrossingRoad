#include "TrafficLight.h"
#include <random>

// Bộ sinh số ngẫu nhiên cho đèn giao thông
static std::mt19937& getTrafficRNG() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

TrafficLight::TrafficLight()
    : m_state(LightState::Green)
    , m_timer(0.f)
    , m_greenDuration(4.f)
    , m_redDuration(3.f)
{
    randomizeDuration();

    // Vỏ đèn (nền đen bao quanh bóng đèn)
    m_housing.setSize({ 16.f, 34.f });
    m_housing.setFillColor(sf::Color(30, 30, 30));
    m_housing.setOutlineThickness(1.f);
    m_housing.setOutlineColor(sf::Color(80, 80, 80));
    m_housing.setPosition(10.f, 0.f);

    // Bóng đèn (12x30px, đổi màu theo trạng thái)
    m_shape.setSize({ 12.f, 30.f });
    m_shape.setFillColor(sf::Color::Green);
    m_shape.setPosition(12.f, 2.f);
}

TrafficLight::TrafficLight(float posX, float posY)
    : TrafficLight()
{
    setPosition(posX, posY);
}

void TrafficLight::setPosition(float x, float y) {
    m_housing.setPosition(x, y);
    // Bóng đèn nằm trong vỏ, căn giữa
    m_shape.setPosition(x + 2.f, y + 2.f);
}

void TrafficLight::randomizeDuration() {
    std::uniform_real_distribution<float> greenDist(3.f, 6.f);
    std::uniform_real_distribution<float> redDist(2.f, 4.f);
    m_greenDuration = greenDist(getTrafficRNG());
    m_redDuration = redDist(getTrafficRNG());
}

void TrafficLight::update(float dt) {
    m_timer += dt;

    if (m_state == LightState::Green && m_timer >= m_greenDuration) {
        // Chuyển sang đỏ
        m_state = LightState::Red;
        m_timer = 0.f;
        m_shape.setFillColor(sf::Color::Red);
        randomizeDuration();
    }
    else if (m_state == LightState::Red && m_timer >= m_redDuration) {
        // Chuyển sang xanh
        m_state = LightState::Green;
        m_timer = 0.f;
        m_shape.setFillColor(sf::Color::Green);
        randomizeDuration();
    }
}

void TrafficLight::draw(sf::RenderTarget& target) const {
    target.draw(m_housing);
    target.draw(m_shape);
}
