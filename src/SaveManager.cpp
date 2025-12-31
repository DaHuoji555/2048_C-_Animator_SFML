#include "SaveManager.h"
#include "Board.h"
#include <fstream>
#include <sys/stat.h>

SaveManager::SaveManager(const std::string& saveFilePath) 
    : saveFilePath_(saveFilePath) {
}

bool SaveManager::save(const std::string& username, const Board& board) {
    std::ofstream file(saveFilePath_);
    if (!file.is_open()) {
        return false;
    }
    
    // 写入用户名
    file << username << "\n";
    
    // 写入分数
    file << board.getScore() << "\n";
    
    // 写入网格
    int grid[4][4];
    board.getGrid(grid);
    
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            file << grid[row][col];
            if (col < 3) {
                file << " ";
            }
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

bool SaveManager::load(std::string& username, Board& board) {
    std::ifstream file(saveFilePath_);
    if (!file.is_open()) {
        return false;
    }
    
    // 读取用户名
    if (!std::getline(file, username)) {
        file.close();
        return false;
    }
    
    // 读取分数
    int score;
    if (!(file >> score)) {
        file.close();
        return false;
    }
    
    // 读取网格
    int grid[4][4];
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (!(file >> grid[row][col])) {
                file.close();
                return false;
            }
        }
    }
    
    file.close();
    
    // 应用到Board
    board.setGrid(grid);
    board.setScore(score);
    
    return true;
}

bool SaveManager::hasSave() const {
    struct stat buffer;
    return (stat(saveFilePath_.c_str(), &buffer) == 0);
}

void SaveManager::deleteSave() {
    std::remove(saveFilePath_.c_str());
}

