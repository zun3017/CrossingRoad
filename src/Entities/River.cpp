#include "River.h"
#include <random>

// Bộ sinh số ngẫu nhiên cho sông
static std::mt19937& getRiverRNG() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

River::River(float posY, float lilypadSpeed, int direction)
    : Terrain(posY)
    , m_lilypadSpeed(lilypadSpeed)
    , m_direction(direction)
    , m_spawnTimer(0.f)
{
    // Nền xanh dương cho sông
    m_background.setFillColor(sf::Color(30, 100, 200));
    loadTexture("assets/textures/river.png");

    // Khoảng cách spawn ngẫu nhiên
    std::uniform_real_distribution<float> intervalDist(1.5f, 3.5f);
    m_spawnInterval = intervalDist(getRiverRNG());

    // Spawn vài lá sen ban đầu để sông không trống rỗng
    std::uniform_real_distribution<float> xDist(0.f, 700.f);
    float spacing = m_width / 4.f; // Chia đều 3 lá sen
    for (int i = 0; i < 3; ++i) {
        float x = spacing * static_cast<float>(i) + xDist(getRiverRNG()) * 0.15f;
        float y = m_posY + 4.f; // Căn giữa lá sen 40px trong hàng 48px
        m_lilypads.push_back(
            std::make_unique<Lilypad>(x, y, m_lilypadSpeed, m_direction)
        );
    }
}

void River::update(float dt) {
    // Cập nhật tất cả lá sen (di chuyển + quấn vòng)
    for (auto& lilypad : m_lilypads) {
        lilypad->update(dt);
    }

    // Spawn lá sen mới theo bộ đếm thời gian
    m_spawnTimer += dt;
    if (m_spawnTimer >= m_spawnInterval) {
        m_spawnTimer = 0.f;
        spawnLilypad();

        // Ngẫu nhiên lại khoảng cách spawn
        std::uniform_real_distribution<float> intervalDist(2.f, 4.f);
        m_spawnInterval = intervalDist(getRiverRNG());
    }
}

void River::spawnLilypad() {
    // Lá sen mới xuất hiện từ ngoài màn hình
    float startX;
    if (m_direction > 0) {
        startX = -60.f; // Từ bên trái
    }
    else {
        startX = 810.f; // Từ bên phải
    }

    float y = m_posY + 4.f;
    m_lilypads.push_back(
        std::make_unique<Lilypad>(startX, y, m_lilypadSpeed, m_direction)
    );
}

bool River::isOnWater(sf::FloatRect playerBounds) const {
    // Bước 1: Kiểm tra xem người chơi có nằm trong khu vực sông không
    sf::FloatRect riverBounds = getBounds();
    if (!riverBounds.intersects(playerBounds)) {
        return false; // Không ở trên sông -> an toàn
    }

    // Bước 2: Kiểm tra xem có đứng trên lá sen nào không
    for (const auto& lilypad : m_lilypads) {
        if (lilypad->getBounds().intersects(playerBounds)) {
            return false; // Đang trên lá sen -> an toàn
        }
    }

    // Trên sông mà không trên lá sen nào -> đang ở trên mặt nước -> NGUY HIỂM!
    return true;
}

void River::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // 1. Vẽ nền sông xanh dương
    Terrain::draw(target, states);

    // 2. Vẽ lá sen lên trên mặt nước
    for (const auto& lilypad : m_lilypads) {
        target.draw(*lilypad, states);
    }
}
