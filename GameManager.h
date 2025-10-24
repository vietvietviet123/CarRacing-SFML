#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>   // Đã có từ trước
#include "Player.h"
#include "Enemy.h"
#include <vector>       
#include <map>          
#include <string>
#include <memory>

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
    
    // Hàm cài đặt (setupRoad sẽ bị thay đổi nhiều)
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

    // *** BIẾN MỚI CHO ĐƯỜNG CUỘN ***
    sf::Texture mRoadTexture;
    sf::Sprite mRoadSprite1;
    sf::Sprite mRoadSprite2;
    std::string mRoadTextureFile;
    // ------------------------------------

    float mRoadPadding; // Vẫn giữ lại để tính toán

    // Vector chứa nhiều texture enemy
    std::vector<sf::Texture> mEnemyTextures; 

    // CLO3: Sử dụng std::vector để quản lý danh sách đối thủ
    std::vector<Enemy> mEnemies; 

    // CLO3: Sử dụng std::map để quản lý điểm số
    std::multimap<int, std::string, std::greater<int>> mScoreboard; 

    float mRoadSpeed;                           

    // Biến quản lý game
    sf::Font mFont;
    sf::Text mScoreText;
    sf::Text mGameOverText;

    bool mIsPlaying;
    int mScore;

    // Các biến đọc từ file config (CLO4)
    int mWindowWidth;
    int mWindowHeight;
    float mEnemySpeedMin;
    float mEnemySpeedMax;
    float mSpawnInterval;
    sf::Time mTimeSinceLastSpawn;
};