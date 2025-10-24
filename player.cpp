#include "Player.h"

Player::Player(float speed)
{
    mSpeed = speed;
}

void Player::initTexture(const std::string& textureFile)
{
    // Gọi hàm loadTexture của lớp cha (Car)
    // Hàm này đã bao gồm xử lý ngoại lệ (CLO4)
    loadTexture(textureFile); 
    // Chỉnh lại origin để di chuyển mượt hơn
    mSprite.setOrigin(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f);
}

// Xử lý sự kiện (CLO1) và cập nhật trạng thái (CLO1) cho Player
void Player::update(sf::Time dt)
{
    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        movement.x -= mSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        movement.x += mSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        movement.y -= mSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        movement.y += mSpeed;
    }

    mSprite.move(movement * dt.asSeconds());
}