#include "GameManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iostream> 
#include <cmath> 

// CLO1: Hàm khởi tạo
GameManager::GameManager()
    : mWindow()
    , mPlayer(400.f) // Tốc độ player
    , mScore(0)
    , mTimeSinceLastSpawn(sf::Time::Zero)
    , mRoadSpeed(300.f) 
    , mRoadPadding(124.f) // Giá trị mặc định
    , mNextLevelScore(10) // Mốc điểm đầu tiên là 10
    , mCurrentState(GameState::MainMenu) // *** SỬA: Bắt đầu ở MainMenu ***
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

    // Cài đặt text
    setupTexts();
    
    // CLO4: Đọc file điểm
    loadScoreboard(); 

    // *** BỎ: resetGame(); (Không bắt đầu game ngay) ***
}

// ... (Hàm loadConfig giữ nguyên) ...
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

// ... (Hàm loadResources giữ nguyên, nhạc vẫn chạy ở menu) ...
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
    mBackgroundMusic.play(); // Nhạc chạy từ menu      
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

// *** HÀM ĐÃ SỬA: Thêm Text cho Menu ***
void GameManager::setupTexts()
{
    // Text Điểm
    mScoreText.setFont(mFont);
    mScoreText.setCharacterSize(24);
    mScoreText.setFillColor(sf::Color::White);
    mScoreText.setPosition(10.f, 10.f);

    // Text Game Over
    mGameOverText.setFont(mFont);
    mGameOverText.setCharacterSize(40);
    mGameOverText.setFillColor(sf::Color::Red);
    mGameOverText.setString("GAME OVER!\nPress Enter to Restart");
    sf::FloatRect goRect = mGameOverText.getLocalBounds();
    mGameOverText.setOrigin(goRect.left + goRect.width / 2.0f, goRect.top + goRect.height / 2.0f);
    mGameOverText.setPosition(mWindowWidth / 2.0f, mWindowHeight / 2.0f);

    // *** THÊM TEXT MENU ***
    // Text Tiêu đề
    mTitleText.setFont(mFont);
    mTitleText.setCharacterSize(60);
    mTitleText.setFillColor(sf::Color::White);
    mTitleText.setString("CAR RACING");
    sf::FloatRect titleRect = mTitleText.getLocalBounds();
    mTitleText.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
    mTitleText.setPosition(mWindowWidth / 2.0f, mWindowHeight / 2.0f - 100.f); // Đặt ở trên

    // Text Hướng dẫn
    mMenuText.setFont(mFont);
    mMenuText.setCharacterSize(30);
    mMenuText.setFillColor(sf::Color::White);
    mMenuText.setString("Press Enter to Start");
    sf::FloatRect menuRect = mMenuText.getLocalBounds();
    mMenuText.setOrigin(menuRect.left + menuRect.width / 2.0f, menuRect.top + menuRect.height / 2.0f);
    mMenuText.setPosition(mWindowWidth / 2.0f, mWindowHeight / 2.0f + 50.f); // Đặt ở dưới
}

// *** HÀM ĐÃ SỬA: Chuyển trạng thái sang Playing ***
void GameManager::resetGame()
{
    // *** THAY ĐỔI: mIsPlaying = true -> mCurrentState = GameState::Playing ***
    mCurrentState = GameState::Playing; 

    mScore = 0;
    mScoreText.setString("Score: 0");
    mTimeSinceLastSpawn = sf::Time::Zero;
    mEnemies.clear(); 

    float playerStartX = mRoadPadding + ((mWindowWidth - mRoadPadding * 2) / 2.f);
    mPlayer.setPosition(playerStartX, mWindowHeight - mPlayer.getGlobalBounds().height);

    // Reset tốc độ và độ khó
    mRoadSpeed = mBaseRoadSpeed;
    mEnemySpeedMin = mBaseEnemySpeedMin;
    mEnemySpeedMax = mBaseEnemySpeedMax;
    mSpawnInterval = mBaseSpawnInterval;
    mNextLevelScore = 10; 

    // Nhạc đã chạy từ menu, không cần play() lại
    // mBackgroundMusic.play(); 
}

// *** HÀM ĐÃ SỬA: Chỉ update khi đang Playing ***
void GameManager::run()
{
    sf::Clock clock;
    // Đây là Game Loop chính
    while (mWindow.isOpen())
    {
        sf::Time dt = clock.restart();
        processEvents(); // Luôn xử lý sự kiện

        // *** THAY ĐỔI: Chỉ update khi ĐANG CHƠI ***
        if (mCurrentState == GameState::Playing)
        {
            update(dt);
        }
        
        render(); // Luôn vẽ
    }
}

// *** HÀM ĐÃ SỬA: Xử lý phím Enter cho từng trạng thái ***
void GameManager::processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        // Xử lý restart game
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
        {
            // *** THAY ĐỔI: Bắt đầu game hoặc Restart game ***
            if (mCurrentState == GameState::MainMenu || mCurrentState == GameState::GameOver)
            {
                resetGame();
            }
        }
    }
}

// ... (Hàm update giữ nguyên, vì nó chỉ được gọi khi đang Playing) ...
void GameManager::update(sf::Time dt)
{
    // Cập nhật đường cuộn (máy chạy bộ)
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
    
    // Cập nhật Player và giữ player trong lề đường
    mPlayer.update(dt);
    sf::Vector2f playerPos = mPlayer.getPosition();
    float playerHalfWidth = mPlayer.getGlobalBounds().width / 2.f;
    
    float leftLimit = mRoadPadding + playerHalfWidth; 
    float rightLimit = mWindowWidth - mRoadPadding - playerHalfWidth;
    if (playerPos.x < leftLimit) mPlayer.setPosition(leftLimit, playerPos.y);
    if (playerPos.x > rightLimit) mPlayer.setPosition(rightLimit, playerPos.y);

    // Cập nhật thời gian spawn
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
            
            // Kiểm tra tăng độ khó
            if (mScore >= mNextLevelScore)
            {
                mRoadSpeed *= 1.1f;
                mEnemySpeedMin *= 1.1f;
                mEnemySpeedMax *= 1.1f;
                if (mSpawnInterval > 0.4f)
                {
                    mSpawnInterval *= 0.95f; 
                }
                mNextLevelScore += 10; 
            }
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

// *** HÀM ĐÃ SỬA: Vẽ dựa trên trạng thái (State) ***
void GameManager::render()
{
    mWindow.clear(sf::Color::Black); 
    
    // 1. Luôn vẽ đường cuộn (làm nền cho mọi trạng thái)
    mWindow.draw(mRoadSprite1);
    mWindow.draw(mRoadSprite2);
    
    // 2. Vẽ các đối tượng dựa trên trạng thái
    if (mCurrentState == GameState::Playing)
    {
        mPlayer.draw(mWindow);
        for (auto& enemy : mEnemies)
        {
            enemy.draw(mWindow);
        }
        mWindow.draw(mScoreText);
    }
    else if (mCurrentState == GameState::MainMenu)
    {
        // Vẽ Menu
        mWindow.draw(mTitleText);
        mWindow.draw(mMenuText);
    }
    else if (mCurrentState == GameState::GameOver)
    {
        // Vẽ xe (vị trí lúc thua)
        mPlayer.draw(mWindow);
        for (auto& enemy : mEnemies)
        {
            enemy.draw(mWindow);
        }
        // Vẽ điểm và chữ Game Over
        mWindow.draw(mScoreText);
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
        float safeDistanceY = refEnemyWidth * 2.0f; 
        float safeDistanceX = refEnemyWidth * 1.5f; 

        if (enemy.getPosition().y < safeDistanceY)
        {
            if (std::abs(enemy.getPosition().x - xPos) < safeDistanceX)
            {
                return; 
            }
        }
    }

    mEnemies.emplace_back(speed, mEnemyTextures[textureIndex]); 
    Enemy& newEnemy = mEnemies.back();
    newEnemy.setPosition(xPos, -100.f); 
}

// *** HÀM ĐÃ SỬA: Chuyển sang trạng thái GameOver ***
void GameManager::checkCollisions()
{
    sf::FloatRect playerBounds = mPlayer.getGlobalBounds();
    float paddingX = 10.f; 
    float paddingY = 10.f;
    
    sf::FloatRect playerHitbox(
        playerBounds.left + paddingX,
        playerBounds.top + paddingY,
        playerBounds.width - (paddingX * 2),
        playerBounds.height - (paddingX * 2)
    );
    
    for (const auto& enemy : mEnemies)
    {
        sf::FloatRect enemyBounds = enemy.getGlobalBounds();
        sf::FloatRect enemyHitbox(
            enemyBounds.left + paddingX,
            enemyBounds.top + paddingY,
            enemyBounds.width - (paddingX * 2),
            enemyBounds.height - (paddingX * 2)
        );

        if (playerHitbox.intersects(enemyHitbox))
        {
            // *** THAY ĐỔI: mIsPlaying = false -> mCurrentState = GameState::GameOver ***
            mCurrentState = GameState::GameOver;
            
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
