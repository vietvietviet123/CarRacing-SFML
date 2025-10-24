#include "GameManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iostream> 
#include <cmath> // Đã có

// CLO1: Hàm khởi tạo
GameManager::GameManager()
    : mWindow()
    , mPlayer(400.f) // Tốc độ player
    , mIsPlaying(false)
    , mScore(0)
    , mTimeSinceLastSpawn(sf::Time::Zero)
    , mRoadSpeed(300.f) 
    , mRoadPadding(124.f) // Giá trị mặc định
    , mNextLevelScore(10) // Mốc điểm đầu tiên là 10
{
    // CLO4: Đọc file config
    loadConfig("config.ini"); 

    // CLO1: Tạo màn hình
    mWindow.create(sf::VideoMode(mWindowWidth, mWindowHeight), "Car Racing");
    mWindow.setFramerateLimit(60);

    srand(static_cast<unsigned int>(time(0)));

    // Tải tài nguyên
    loadResources(); 
    
    // Tạo đường đua
    setupRoad(); 

    setupTexts();
    
    // CLO4: Đọc file điểm
    loadScoreboard(); 

    resetGame();
}

// *** HÀM ĐÃ SỬA: Lưu tốc độ GỐC ***
void GameManager::loadConfig(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("CLO4 Exception: Config file not found! (" + filename + ")");
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::stringstream ss(line);
        std::string key;
        float value;
        std::string s_value; 

        if (std::getline(ss, key, '='))
        {
            if (key == "road_texture_file")
            {
                std::getline(ss, mRoadTextureFile);
            }
            else if (ss >> value) 
            {
                if (key == "width") mWindowWidth = static_cast<int>(value);
                else if (key == "height") mWindowHeight = static_cast<int>(value);
                else if (key == "road_padding") mRoadPadding = value;

                // *** THAY ĐỔI: Lưu tốc độ vào cả biến GỐC và biến HIỆN TẠI ***
                else if (key == "road_speed") 
                { 
                    mBaseRoadSpeed = value; 
                    mRoadSpeed = value; 
                }
                else if (key == "enemy_speed_min") 
                { 
                    mBaseEnemySpeedMin = value; 
                    mEnemySpeedMin = value; 
                }
                else if (key == "enemy_speed_max") 
                { 
                    mBaseEnemySpeedMax = value; 
                    mEnemySpeedMax = value; 
                }
                else if (key == "spawn_interval") 
                { 
                    mBaseSpawnInterval = value; 
                    mSpawnInterval = value; 
                }
            }
        }
    }
    file.close();
}

// ... (Hàm loadResources giữ nguyên) ...
void GameManager::loadResources()
{
    if (!mFont.loadFromFile("arial.ttf"))
    {
        throw std::runtime_error("CLO4 Exception: Font file not found (arial.ttf)");
    }
    
    mPlayer.initTexture("player_car.png"); 

    sf::Texture tex1;
    if (!tex1.loadFromFile("enemy_car_1.png"))
    {
        throw std::runtime_error("CLO4 Exception: File Not Found: enemy_car_1.png");
    }
    mEnemyTextures.push_back(tex1); 
    sf::Texture tex2;
    if (!tex2.loadFromFile("enemy_car_2.png"))
    {
        throw std::runtime_error("CLO4 Exception: File Not Found: enemy_car_2.png");
    }
    mEnemyTextures.push_back(tex2); 
    
    if (mRoadTextureFile.empty())
    {
        throw std::runtime_error("CLO4 Exception: road_texture_file not specified in config.ini");
    }
    if (!mRoadTexture.loadFromFile(mRoadTextureFile))
    {
        throw std::runtime_error("CLO4 Exception: File Not Found: " + mRoadTextureFile);
    }
    
    if (!mBackgroundMusic.openFromFile("background_music.ogg"))
    {
        throw std::runtime_error("CLO4 Exception: File Not Found: background_music.ogg");
    }
    mBackgroundMusic.setLoop(true); 
    mBackgroundMusic.setVolume(50); 
    mBackgroundMusic.play();        
}

// ... (Hàm setupRoad giữ nguyên) ...
void GameManager::setupRoad()
{
    float scaleX = (float)mWindowWidth / mRoadTexture.getSize().x; 
    float roadTextureHeight = mRoadTexture.getSize().y * scaleX; 

    mRoadSprite1.setTexture(mRoadTexture);
    mRoadSprite1.setPosition(0.f, 0.f); 
    mRoadSprite1.setScale(scaleX, scaleX); 

    mRoadSprite2.setTexture(mRoadTexture);
    mRoadSprite2.setPosition(0.f, -roadTextureHeight); 
    mRoadSprite2.setScale(scaleX, scaleX); 
}

// ... (Hàm setupTexts giữ nguyên) ...
void GameManager::setupTexts()
{
    mScoreText.setFont(mFont);
    mScoreText.setCharacterSize(24);
    mScoreText.setFillColor(sf::Color::White);
    mScoreText.setPosition(10.f, 10.f);

    mGameOverText.setFont(mFont);
    mGameOverText.setCharacterSize(40);
    mGameOverText.setFillColor(sf::Color::Red);
    mGameOverText.setString("GAME OVER!\nPress Enter to Restart");
    
    sf::FloatRect textRect = mGameOverText.getLocalBounds();
    mGameOverText.setOrigin(textRect.left + textRect.width / 2.0f,
                            textRect.top + textRect.height / 2.0f);
    mGameOverText.setPosition(mWindowWidth / 2.0f, mWindowHeight / 2.0f);
}

// *** HÀM ĐÃ SỬA: Reset tốc độ về GỐC ***
void GameManager::resetGame()
{
    mIsPlaying = true;
    mScore = 0;
    mScoreText.setString("Score: 0");
    mTimeSinceLastSpawn = sf::Time::Zero;
    mEnemies.clear(); 

    float playerStartX = mRoadPadding + ((mWindowWidth - mRoadPadding * 2) / 2.f);
    mPlayer.setPosition(playerStartX, mWindowHeight - mPlayer.getGlobalBounds().height);

    // *** THÊM ĐOẠN NÀY: Reset tốc độ và độ khó ***
    mRoadSpeed = mBaseRoadSpeed;
    mEnemySpeedMin = mBaseEnemySpeedMin;
    mEnemySpeedMax = mBaseEnemySpeedMax;
    mSpawnInterval = mBaseSpawnInterval;
    mNextLevelScore = 10; // Đặt lại mốc điểm
    // ------------------------------------

    mBackgroundMusic.play(); 
}

// ... (Hàm run giữ nguyên) ...
void GameManager::run()
{
    sf::Clock clock;
    while (mWindow.isOpen())
    {
        sf::Time dt = clock.restart();
        processEvents();
        if (mIsPlaying)
        {
            update(dt);
        }
        render();
    }
}

// ... (Hàm processEvents giữ nguyên) ...
void GameManager::processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        if (!mIsPlaying && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
        {
            resetGame();
        }
    }
}


// *** HÀM ĐÃ SỬA: Thêm logic tăng độ khó ***
void GameManager::update(sf::Time dt)
{
    // ... (Code cuộn đường (mRoadSprite1, mRoadSprite2) giữ nguyên) ...
    float roadTextureHeight = mRoadTexture.getSize().y * mRoadSprite1.getScale().y;
    mRoadSprite1.move(0, mRoadSpeed * dt.asSeconds());
    mRoadSprite2.move(0, mRoadSpeed * dt.asSeconds());
    if (mRoadSprite1.getPosition().y > mWindowHeight)
    {
        mRoadSprite1.setPosition(0.f, mRoadSprite2.getPosition().y - roadTextureHeight);
    }
    if (mRoadSprite2.getPosition().y > mWindowHeight)
    {
        mRoadSprite2.setPosition(0.f, mRoadSprite1.getPosition().y - roadTextureHeight);
    }
    
    // ... (Code cập nhật Player và giữ player trong lề giữ nguyên) ...
    mPlayer.update(dt);
    sf::Vector2f playerPos = mPlayer.getPosition();
    float playerHalfWidth = mPlayer.getGlobalBounds().width / 2.f;
    float leftLimit = mRoadPadding + playerHalfWidth; 
    float rightLimit = mWindowWidth - mRoadPadding - playerHalfWidth;
    if (playerPos.x < leftLimit) mPlayer.setPosition(leftLimit, playerPos.y);
    if (playerPos.x > rightLimit) mPlayer.setPosition(rightLimit, playerPos.y);

    // ... (Code cập nhật mTimeSinceLastSpawn và gọi spawnEnemy() giữ nguyên) ...
    mTimeSinceLastSpawn += dt;
    if (mTimeSinceLastSpawn.asSeconds() > mSpawnInterval)
    {
        spawnEnemy();
        mTimeSinceLastSpawn = sf::Time::Zero;
    }

    // Cập nhật enemies
    for (auto it = mEnemies.begin(); it != mEnemies.end(); )
    {
        it->update(dt);
        if (it->getPosition().y > mWindowHeight + it->getGlobalBounds().height)
        {
            it = mEnemies.erase(it); 
            mScore++; // Tăng điểm
            
            // *** THÊM ĐOẠN NÀY: Kiểm tra tăng độ khó ***
            if (mScore >= mNextLevelScore)
            {
                // Tăng tốc độ (ví dụ: tăng 10%)
                mRoadSpeed *= 1.1f;
                mEnemySpeedMin *= 1.1f;
                mEnemySpeedMax *= 1.1f;

                // Giảm thời gian spawn (nhanh hơn 5%, đặt giới hạn 0.4s)
                if (mSpawnInterval > 0.4f)
                {
                    mSpawnInterval *= 0.95f; 
                }

                // Đặt mốc điểm tiếp theo
                mNextLevelScore += 10; // Cứ mỗi 10 điểm
            }
            // -----------------------------------------
        }
        else
        {
            ++it;
        }
    }
    
    mScoreText.setString("Score: " + std::to_string(mScore));

    // Kiểm tra va chạm
    checkCollisions();
}

// ... (Hàm render giữ nguyên) ...
void GameManager::render()
{
    mWindow.clear(sf::Color::Black); 
    
    mWindow.draw(mRoadSprite1);
    mWindow.draw(mRoadSprite2);
    
    mPlayer.draw(mWindow);
    for (auto& enemy : mEnemies)
    {
        enemy.draw(mWindow);
    }
    mWindow.draw(mScoreText);
    if (!mIsPlaying)
    {
        mWindow.draw(mGameOverText);
    }

    mWindow.display();
}

// ... (Hàm spawnEnemy giữ nguyên) ...
void GameManager::spawnEnemy()
{
    float speed = mEnemySpeedMin + (rand() % static_cast<int>(mEnemySpeedMax - mEnemySpeedMin + 1));
    
    if (mEnemyTextures.empty()) return; 
    int textureIndex = rand() % mEnemyTextures.size(); 
    
    float refEnemyWidth = mEnemyTextures[textureIndex].getSize().x;
    float refEnemyHalfWidth = refEnemyWidth / 2.f;

    int spawnableWidth = (mWindowWidth - mRoadPadding * 2) - static_cast<int>(refEnemyWidth);
    float xPos;
    if (spawnableWidth <= 0) 
    {
        xPos = mRoadPadding + ((mWindowWidth - mRoadPadding * 2) / 2.f);
    }
    else
    {
        xPos = static_cast<float>(rand() % spawnableWidth) + mRoadPadding + refEnemyHalfWidth;
    }

    for (const auto& enemy : mEnemies)
    {
        if (enemy.getPosition().y < (refEnemyWidth * 3.f))
        {
            if (std::abs(enemy.getPosition().x - xPos) < (refEnemyWidth * 1.5f))
            {
                return; 
            }
        }
    }

    mEnemies.emplace_back(speed, mEnemyTextures[textureIndex]); 
    Enemy& newEnemy = mEnemies.back();
    newEnemy.setPosition(xPos, -100.f); 
}

// ... (Hàm checkCollisions giữ nguyên) ...
void GameManager::checkCollisions()
{
    sf::FloatRect playerBounds = mPlayer.getGlobalBounds();
    float paddingX = 10.f; 
    float paddingY = 10.f;
    
    sf::FloatRect playerHitbox(
        playerBounds.left + paddingX,
        playerBounds.top + paddingY,
        playerBounds.width - (paddingX * 2),
        playerBounds.height - (paddingY * 2)
    );
    
    for (const auto& enemy : mEnemies)
    {
        sf::FloatRect enemyBounds = enemy.getGlobalBounds();
        sf::FloatRect enemyHitbox(
            enemyBounds.left + paddingX,
            enemyBounds.top + paddingY,
            enemyBounds.width - (paddingX * 2),
            enemyBounds.height - (paddingY * 2)
        );

        if (playerHitbox.intersects(enemyHitbox))
        {
            mIsPlaying = false; // Game over
            mBackgroundMusic.stop(); 
            mScoreboard.insert({mScore, "Player"}); 
            saveScoreboard(); 
            break;
        }
    }
}

// ... (Hàm loadScoreboard, saveScoreboard giữ nguyên) ...
void GameManager::loadScoreboard()
{
    std::ifstream file("scoreboard.txt");
    if (!file.is_open())
    {
        return; 
    }
    
    int score;
    std::string name;
    while (file >> score >> name)
    {
        mScoreboard.insert({score, name}); 
    }
    file.close();
}

void GameManager::saveScoreboard()
{
    std::ofstream file("scoreboard.txt");
    if (!file.is_open())
    {
        std::cerr << "Warning: Could not save scoreboard." << std::endl;
        return;
    }
    
    int count = 0;
    for (const auto& pair : mScoreboard)
    {
        file << pair.first << " " << pair.second << "\n";
        if (++count >= 10) break;
    }
    file.close();
}
