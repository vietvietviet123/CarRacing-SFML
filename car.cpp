#include "Car.h"

Car::Car() : mSpeed(0.f)
{
}

Car::~Car()
{
}

// CLO4: Xử lý ngoại lệ khi không tải được file
void Car::loadTexture(const std::string& textureFile)
{
    if (!mTexture.loadFromFile(textureFile))
    {
        // Ném ra một ngoại lệ nếu không tìm thấy file
        throw std::runtime_error("CLO4 Exception: File Not Found: " + textureFile);
    }
    mSprite.setTexture(mTexture);
}

void Car::draw(sf::RenderWindow& window)
{
    window.draw(mSprite);
}

// FIX: Thêm 'const'
sf::FloatRect Car::getGlobalBounds() const
{
    return mSprite.getGlobalBounds();
}

// FIX: Thêm 'const'
sf::Vector2f Car::getPosition() const
{
    return mSprite.getPosition();
}

void Car::setPosition(float x, float y)
{
    mSprite.setPosition(x, y);
}