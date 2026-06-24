#pragma once
#include "Terrain.h"
#include "Item.h"
#include <vector>
#include <memory>

// Hàng cỏ - khu vực an toàn cho người chơi
// Có thể chứa vật phẩm để nhặt
class Grass : public Terrain {
public:
    explicit Grass(float posY);
    ~Grass() override = default;

    void update(float dt) override;

    // Sinh vật phẩm ngẫu nhiên trên hàng này
    void spawnItem();

    // Truy cập danh sách vật phẩm
    const std::vector<std::unique_ptr<Item>>& getItems() const { return m_items; }
    std::vector<std::unique_ptr<Item>>& getItems() { return m_items; }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<std::unique_ptr<Item>> m_items;
};
