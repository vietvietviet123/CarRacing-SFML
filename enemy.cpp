#include "Enemy.h"

// *** THAY ĐỔI: Hàm khởi tạo ***
Enemy::Enemy(float speed, const sf::Texture& texture)
{
    mSpeed = speed;
    
    // *** Gán texture trực tiếp ***
    mSprite.setTexture(texture); 
    
    // Đặt origin (gốc xoay/vị trí) ở trên cùng, chính giữa xe
    mSprite.setOrigin(getGlobalBounds().width / 2.f, 0); 
}

// Cập nhật trạng thái (CLO1) cho Enemy
void Enemy::update(sf::Time dt)
{
    // Enemy chỉ di chuyển thẳng xuống dưới
    mSprite.move(0.f, mSpeed * dt.asSeconds());
}