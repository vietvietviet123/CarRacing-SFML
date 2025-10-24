#include "GameManager.h"
#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        GameManager game;
        game.run();
    }
    // CLO4: Bắt tất cả các ngoại lệ (file không tìm thấy, sai định dạng,...)
    catch (const std::exception& e)
    {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        // Có thể hiển thị lỗi này bằng 1 cửa sổ thông báo
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}