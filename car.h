#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <stdexcept>

class Car
{
public:
    Car();
    virtual ~Car(); // Hàm hủy ảo

    // Hàm ảo thuần túy (pure virtual function) - CLO2
    virtual void update(sf::Time dt) = 0; 
    
    void draw(sf::RenderWindow& window);

    // FIX: Thêm 'const'
    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getPosition() const;

    void setPosition(float x, float y);

protected:
    void loadTexture(const std::string& textureFile);

    sf::Sprite mSprite;
    sf::Texture mTexture;
    float mSpeed;
};