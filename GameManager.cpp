#include "GameManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iostream> 

// CLO1: Hàm khởi tạo
GameManager::GameManager()
    : mWindow()
    , mPlayer(400.f) // Tốc độ player
    , mIsPlaying(false)
    , mScore(0)
    , mTimeSinceLastSpawn(sf::Time::Zero)
    , mRoadSpeed(300.f) 
    , mRoadPadding(124.f) // Giá trị mặc định
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

// CLO4: Đọc file cấu hình (config.ini)
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
        std::string s_value; // Dùng để đọc string (file name)

        if (std::getline(ss, key, '='))
        {
            // Thử đọc string trước
            if (key == "road_texture_file")
            {
                std::getline(ss, mRoadTextureFile);
            }
            // Nếu không phải, thử đọc số
            else if (ss >> value) 
            {
                if (key == "width") mWindowWidth = static_cast<int>(value);
                else if (key == "height") mWindowHeight = static_cast<int>(value);
                else if (key == "enemy_speed_min") mEnemySpeedMin = value;
                else if (key == "enemy_speed_max") mEnemySpeedMax = value;
                else if (key == "spawn_interval") mSpawnInterval = value;
                else if (key == "road_speed") mRoadSpeed = value; 
                else if (key == "road_padding") mRoadPadding = value;
            }
        }
    }
    file.close();
}

// Tải tài nguyên
void GameManager::loadResources()
{
    // ... (Code load "arial.ttf" giữ nguyên) ...
    if (!mFont.loadFromFile("arial.ttf"))
    {
        throw std::runtime_error("CLO4 Exception: Font file not found (arial.ttf)");
    }
    
    // Tải texture player
    mPlayer.initTexture("player_car.png"); 

    // Load nhiều ảnh enemy vào vector
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
    
    // Tải ảnh đường cuộn
    if (mRoadTextureFile.empty())
    {
        throw std::runtime_error("CLO4 Exception: road_texture_file not specified in config.ini");
    }
    if (!mRoadTexture.loadFromFile(mRoadTextureFile))
    {
        throw std::runtime_error("CLO4 Exception: File Not Found: " + mRoadTextureFile);
    }
    
    // ... (Code load "mBackgroundMusic") ...
    if (!mBackgroundMusic.openFromFile("background_music.ogg"))
    {
        throw std::runtime_error("CLO4 Exception: File Not Found: background_music.ogg");
    }
    mBackgroundMusic.setLoop(true); 
    mBackgroundMusic.setVolume(50); 
    mBackgroundMusic.play(); // Chạy nhạc lần đầu
}

// *** HÀM ĐÃ SỬA HOÀN TOÀN ***
void GameManager::setupRoad()
{
    // Tính toán tỷ lệ scale để ảnh VỪA KHÍT chiều rộng cửa sổ
    float scaleX = (float)mWindowWidth / mRoadTexture.getSize().x; // (800 / 840)

    // Lấy chiều cao của texture (sau khi đã scale)
    float roadTextureHeight = mRoadTexture.getSize().y * scaleX; // (650 * (800/840)) = 619px

    mRoadSprite1.setTexture(mRoadTexture);
    mRoadSprite1.setPosition(0.f, 0.f); // Bắt đầu từ (0,0)
    mRoadSprite1.setScale(scaleX, scaleX); 

    mRoadSprite2.setTexture(mRoadTexture);
    // Đặt sprite 2 ngay trên sprite 1
    mRoadSprite2.setPosition(0.f, -roadTextureHeight); 
    mRoadSprite2.setScale(scaleX, scaleX); 
}

// HÀM NÀY PHẢI TỒN TẠI
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

// *** HÀM ĐÃ SỬA ***
void GameManager::resetGame()
{
    mIsPlaying = true;
    mScore = 0;
    mScoreText.setString("Score: 0");
    mTimeSinceLastSpawn = sf::Time::Zero;
    mEnemies.clear(); // CLO3: Xóa vector

    // Đặt player ở giữa đường đua (dựa trên padding)
    float playerStartX = mRoadPadding + ((mWindowWidth - mRoadPadding * 2) / 2.f);
    mPlayer.setPosition(playerStartX, mWindowHeight - mPlayer.getGlobalBounds().height);

    // *** THÊM DÒNG NÀY: Chơi lại nhạc ***
    mBackgroundMusic.play(); 
}

// HÀM NÀY PHẢI TỒN TẠI
void GameManager::run()
{
    sf::Clock clock;
    // Đây là Game Loop chính
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

// CLO1: Xử lý sự kiện
void GameManager::processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        // Xử lý restart game
        if (!mIsPlaying && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
        {
            resetGame();
        }
    }
}

// *** HÀM ĐÃ SỬA (FIX LỖI KHOẢNG HỞ) ***
void GameManager::update(sf::Time dt)
{
    // Cập nhật đường cuộn (máy chạy bộ)
    float roadTextureHeight = mRoadTexture.getSize().y * mRoadSprite1.getScale().y;
    
    mRoadSprite1.move(0, mRoadSpeed * dt.asSeconds());
    mRoadSprite2.move(0, mRoadSpeed * dt.asSeconds());

    // Nếu sprite 1 ra khỏi màn hình, đưa nó lên trên sprite 2
    if (mRoadSprite1.getPosition().y > mWindowHeight)
    {
        mRoadSprite1.setPosition(0.f, mRoadSprite2.getPosition().y - roadTextureHeight);
    }
    // Nếu sprite 2 ra khỏi màn hình, đưa nó lên trên sprite 1
    if (mRoadSprite2.getPosition().y > mWindowHeight)
    {
        mRoadSprite2.setPosition(0.f, mRoadSprite1.getPosition().y - roadTextureHeight);
    }
    // --------------------------------------------------

    // Cập nhật Player và giữ player trong lề đường
    mPlayer.update(dt);
    sf::Vector2f playerPos = mPlayer.getPosition();
    float playerHalfWidth = mPlayer.getGlobalBounds().width / 2.f;
    
    // Giới hạn dựa trên padding (lề đường trong ảnh)
    float leftLimit = mRoadPadding + playerHalfWidth; 
    float rightLimit = mWindowWidth - mRoadPadding - playerHalfWidth;
    if (playerPos.x < leftLimit) mPlayer.setPosition(leftLimit, playerPos.y);
    if (playerPos.x > rightLimit) mPlayer.setPosition(rightLimit, playerPos.y);


    // ... (Code spawn enemy và cập nhật enemy giữ nguyên) ...
    mTimeSinceLastSpawn += dt;
    if (mTimeSinceLastSpawn.asSeconds() > mSpawnInterval)
    {
        spawnEnemy();
        mTimeSinceLastSpawn = sf::Time::Zero;
    }
    for (auto it = mEnemies.begin(); it != mEnemies.end(); )
    {
        it->update(dt);
        if (it->getPosition().y > mWindowHeight + it->getGlobalBounds().height)
        {
            it = mEnemies.erase(it); 
            mScore++; 
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

// *** HÀM ĐÃ SỬA (VẼ ĐƯỜNG CUỘN) ***
void GameManager::render()
{
    // Clear màn hình
    mWindow.clear(sf::Color::Black); 
    
    // Vẽ 2 sprite đường cuộn
    mWindow.draw(mRoadSprite1);
    mWindow.draw(mRoadSprite2);
    // ------------------------------------
    
    // Vẽ xe và text
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

// *** HÀM ĐÃ SỬA (NGĂN ĐÈ NHAU) ***
void GameManager::spawnEnemy()
{
    // Fix: Kiểm tra xem có xe nào còn ở quá gần đỉnh không
    for (const auto& enemy : mEnemies)
    {
        if (enemy.getPosition().y < 150.f)
        {
            return; // BỎ QUA, không spawn
        }
    }
    
    float speed = mEnemySpeedMin + (rand() % static_cast<int>(mEnemySpeedMax - mEnemySpeedMin + 1));
    
    // Chọn ngẫu nhiên 1 texture từ vector
    if (mEnemyTextures.empty()) return; 
    int textureIndex = rand() % mEnemyTextures.size(); 
    mEnemies.emplace_back(speed, mEnemyTextures[textureIndex]); 
    
    Enemy& newEnemy = mEnemies.back();
    float enemyHalfWidth = newEnemy.getGlobalBounds().width / 2.f;

    // Tính toán spawn (dựa trên padding)
    int spawnableWidth = (mWindowWidth - mRoadPadding * 2) - static_cast<int>(enemyHalfWidth * 2);

    if (spawnableWidth <= 0) 
    {
        float xPos = mRoadPadding + ((mWindowWidth - mRoadPadding * 2) / 2.f);
        newEnemy.setPosition(xPos, -100.f);
    }
    else
    {
        float xPos = static_cast<float>(rand() % spawnableWidth) + mRoadPadding + enemyHalfWidth;
        newEnemy.setPosition(xPos, -100.f); 
    }
}

// *** HÀM ĐÃ SỬA ***
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

        // Kiểm tra va chạm bằng hitbox
        if (playerHitbox.intersects(enemyHitbox))
        {
            mIsPlaying = false; // Game over
            
            // *** THÊM DÒNG NÀY: Dừng nhạc ***
            mBackgroundMusic.stop(); 
            
            mScoreboard.insert({mScore, "Player"}); 
            saveScoreboard(); 
            break;
        }
    }
}


// CLO4: Đọc/Lưu file scoreboard
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