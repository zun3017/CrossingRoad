#include "Grass.h"
#include <random>
#include <algorithm>

// Bộ sinh số ngẫu nhiên cho hàng cỏ
static std::mt19937& getGrassRNG() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

Grass::Grass(float posY)
    : Terrain(posY)
{
    // Nền xanh lá cây
    m_background.setFillColor(sf::Color(34, 139, 34));
    loadTexture("assets/textures/grass.png");
}

void Grass::update(float dt) {
    // Cập nhật các vật phẩm
    for (auto& item : m_items) {
        item->update(dt);
    }

    // Xóa vật phẩm đã được thu thập
    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
            [](const std::unique_ptr<Item>& item) {
                return item->isCollected();
            }),
        m_items.end()
    );
}

void Grass::spawnItem() {
    // Chọn vị trí ngẫu nhiên trên hàng, căn vào ô lưới
    int numCells = static_cast<int>(m_width) / 48;
    std::uniform_int_distribution<int> cellDist(0, numCells - 1);
    std::uniform_int_distribution<int> pointsDist(1, 5);

    int cellX = cellDist(getGrassRNG());
    // Căn giữa vật phẩm 24x24 trong ô 48x48
    float x = static_cast<float>(cellX * 48 + 12);
    float y = m_posY + 12.f;

    // Điểm là bội của 5 (5, 10, 15, 20, 25)
    int points = pointsDist(getGrassRNG()) * 5;

    m_items.push_back(std::make_unique<Item>(x, y, points));
}

void Grass::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Vẽ nền cỏ xanh
    Terrain::draw(target, states);

    // Vẽ các vật phẩm lên trên nền
    for (const auto& item : m_items) {
        if (!item->isCollected()) {
            target.draw(*item, states);
        }
    }
}
