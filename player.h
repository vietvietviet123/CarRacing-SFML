#pragma once
#include "Car.h"

class Player : public Car
{
public:
    Player(float speed);

    // Ghi đè (override) hàm ảo của lớp cha - CLO2
    virtual void update(sf::Time dt) override;
    
    // Tải texture riêng cho Player, gọi hàm của lớp cha
    void initTexture(const std::string& textureFile);
};