#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>   
#include "Player.h"
#include "Enemy.h"
#include <vector>       
#include <map>          
#include <string>
#include <memory>

// *** THÊM: Tạo các trạng thái cho game ***
enum class GameState
{
    MainMenu,
    Playing,
    GameOver
};


class GameManager
{
public:
    GameManager();
    void run(); // Hàm chạy game loop chính

private:
    // Các hàm xử lý rõ ràng (CLO1)
    void processEvents();   // Xử lý sự kiện
    void update(sf::Time dt); // Cập nhật trạng thái game
    void render();          // Vẽ màn hình

    // Các hàm khởi tạo và tài nguyên
    void loadConfig(const std::string& filename); // CLO4: Đọc file config
    void loadResources();
    void setupTexts();
    
    void setupRoad(); 

    // Các hàm xử lý logic game
    void spawnEnemy();
    void checkCollisions(); // Hàm kiểm tra va chạm (theo yêu cầu ảnh 2)
    void resetGame();
    
    // Hàm xử lý file điểm (CLO4)
    void loadScoreboard();
    void saveScoreboard();

private:
    // Các lớp đối tượng (CLO2)
    sf::RenderWindow mWindow;
    Player mPlayer;
    sf::Music mBackgroundMusic; // Nhạc nền

    // Biến cho đường cuộn
    sf::Texture mRoadTexture;
    sf::Sprite mRoadSprite1;
    sf::Sprite mRoadSprite2;
    std::string mRoadTextureFile;
    
    float mRoadPadding; 

    // Vector chứa nhiều texture enemy
    std::vector<sf::Texture> mEnemyTextures; 
    std::vector<Enemy> mEnemies; 
    std::multimap<int, std::string, std::greater<int>> mScoreboard; 

    float mRoadSpeed;                          

    // Biến quản lý game
    sf::Font mFont;
    sf::Text mScoreText;
    sf::Text mGameOverText;
    
    // *** THÊM CÁC BIẾN NÀY CHO MENU ***
    sf::Text mTitleText;
    sf::Text mMenuText;
    
    // *** BỎ: bool mIsPlaying; ***
    // *** THAY BẰNG: ***
    GameState mCurrentState; // Biến trạng thái mới

    int mScore;

    // ... (các biến config và tốc độ khác giữ nguyên) ...
    int mWindowWidth;
    int mWindowHeight;
    float mEnemySpeedMin; 
    float mEnemySpeedMax; 
    float mSpawnInterval; 
    sf::Time mTimeSinceLastSpawn;

    float mBaseRoadSpeed;
    float mBaseEnemySpeedMin;
    float mBaseEnemySpeedMax;
    float mBaseSpawnInterval;
    
    int mNextLevelScore; 
};
