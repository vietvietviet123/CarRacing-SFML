#pragma once
#include "Car.h"

class Enemy : public Car
{
public:
    // *** THAY ĐỔI: Hàm khởi tạo nhận texture, không nhận tên file ***
    Enemy(float speed, const sf::Texture& texture);

    // Ghi đè (override) hàm ảo của lớp cha - CLO2
    virtual void update(sf::Time dt) override;
};