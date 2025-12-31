#include "Board.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Board::Board() : score_(0) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::memset(grid_, 0, sizeof(grid_));
}

void Board::init() {
    std::memset(grid_, 0, sizeof(grid_));
    score_ = 0;
    
    // 正常模式：生成两个随机初始方块
    spawnNewTile();
    spawnNewTile();
}

void Board::getGrid(int outGrid[4][4]) const {
    copyGrid(grid_, outGrid);
}

void Board::setGrid(const int inGrid[4][4]) {
    copyGrid(inGrid, grid_);
}

int Board::getValue(int row, int col) const {
    return grid_[row][col];
}

MoveResult Board::simulateMove(Direction dir) const {
    MoveResult result;
    copyGrid(grid_, result.grid);
    result.scoreGain = 0;
    result.changed = false;
    
    switch (dir) {
        case Direction::LEFT:
            moveLeft(result.grid, result.scoreGain);
            break;
        case Direction::RIGHT:
            moveRight(result.grid, result.scoreGain);
            break;
        case Direction::UP:
            moveUp(result.grid, result.scoreGain);
            break;
        case Direction::DOWN:
            moveDown(result.grid, result.scoreGain);
            break;
    }
    
    result.changed = !gridEquals(grid_, result.grid);
    return result;
}

void Board::commitGrid(const int newGrid[4][4]) {
    copyGrid(newGrid, grid_);
}

std::pair<std::pair<int, int>, int> Board::spawnNewTile() {
    auto emptyCells = getEmptyCells();
    
    if (emptyCells.empty()) {
        return {{-1, -1}, 0};
    }
    
    // 随机选择空位置
    int index = std::rand() % emptyCells.size();
    auto pos = emptyCells[index];
    
    // 90% 概率生成2，10% 概率生成4
    int value = (std::rand() % 10 == 0) ? 4 : 2;
    
    grid_[pos.first][pos.second] = value;
    
    return {pos, value};
}

bool Board::hasWon() const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (grid_[i][j] >= 2048) {
                return true;
            }
        }
    }
    return false;
}

bool Board::isGameOver() const {
    // 如果有空格，游戏未结束
    if (!getEmptyCells().empty()) {
        return false;
    }
    
    // 检查是否还能移动
    return !canMove();
}

int Board::getScore() const {
    return score_;
}

void Board::setScore(int score) {
    score_ = score;
}

void Board::addScore(int delta) {
    score_ += delta;
}

// ===== 私有辅助函数 =====

void Board::moveLeft(int tempGrid[4][4], int& scoreGain) const {
    scoreGain = 0;
    
    for (int row = 0; row < 4; ++row) {
        int temp[4] = {0};
        int pos = 0;
        
        // 收集非零值
        for (int col = 0; col < 4; ++col) {
            if (tempGrid[row][col] != 0) {
                temp[pos++] = tempGrid[row][col];
            }
        }
        
        // 合并相同值（从左到右，每个方块只合并一次）
        int merged[4] = {0};
        int mergePos = 0;
        
        for (int i = 0; i < pos; ++i) {
            if (i + 1 < pos && temp[i] == temp[i + 1]) {
                // 合并
                merged[mergePos++] = temp[i] * 2;
                scoreGain += temp[i] * 2;
                ++i; // 跳过下一个已合并的
            } else {
                merged[mergePos++] = temp[i];
            }
        }
        
        // 写回行
        for (int col = 0; col < 4; ++col) {
            tempGrid[row][col] = merged[col];
        }
    }
}

void Board::moveRight(int tempGrid[4][4], int& scoreGain) const {
    scoreGain = 0;
    
    for (int row = 0; row < 4; ++row) {
        int temp[4] = {0};
        int pos = 0;
        
        // 从右到左收集非零值
        for (int col = 3; col >= 0; --col) {
            if (tempGrid[row][col] != 0) {
                temp[pos++] = tempGrid[row][col];
            }
        }
        
        // 合并相同值（从右到左）
        int merged[4] = {0};
        int mergePos = 0;
        
        for (int i = 0; i < pos; ++i) {
            if (i + 1 < pos && temp[i] == temp[i + 1]) {
                merged[mergePos++] = temp[i] * 2;
                scoreGain += temp[i] * 2;
                ++i;
            } else {
                merged[mergePos++] = temp[i];
            }
        }
        
        // 从右到左写回
        for (int col = 0; col < 4; ++col) {
            tempGrid[row][3 - col] = merged[col];
        }
    }
}

void Board::moveUp(int tempGrid[4][4], int& scoreGain) const {
    scoreGain = 0;
    
    for (int col = 0; col < 4; ++col) {
        int temp[4] = {0};
        int pos = 0;
        
        // 从上到下收集非零值
        for (int row = 0; row < 4; ++row) {
            if (tempGrid[row][col] != 0) {
                temp[pos++] = tempGrid[row][col];
            }
        }
        
        // 合并
        int merged[4] = {0};
        int mergePos = 0;
        
        for (int i = 0; i < pos; ++i) {
            if (i + 1 < pos && temp[i] == temp[i + 1]) {
                merged[mergePos++] = temp[i] * 2;
                scoreGain += temp[i] * 2;
                ++i;
            } else {
                merged[mergePos++] = temp[i];
            }
        }
        
        // 写回列
        for (int row = 0; row < 4; ++row) {
            tempGrid[row][col] = merged[row];
        }
    }
}

void Board::moveDown(int tempGrid[4][4], int& scoreGain) const {
    scoreGain = 0;
    
    for (int col = 0; col < 4; ++col) {
        int temp[4] = {0};
        int pos = 0;
        
        // 从下到上收集非零值
        for (int row = 3; row >= 0; --row) {
            if (tempGrid[row][col] != 0) {
                temp[pos++] = tempGrid[row][col];
            }
        }
        
        // 合并
        int merged[4] = {0};
        int mergePos = 0;
        
        for (int i = 0; i < pos; ++i) {
            if (i + 1 < pos && temp[i] == temp[i + 1]) {
                merged[mergePos++] = temp[i] * 2;
                scoreGain += temp[i] * 2;
                ++i;
            } else {
                merged[mergePos++] = temp[i];
            }
        }
        
        // 从下到上写回
        for (int row = 0; row < 4; ++row) {
            tempGrid[3 - row][col] = merged[row];
        }
    }
}

bool Board::canMove() const {
    // 检查是否有相邻的相同值
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int val = grid_[row][col];
            
            // 检查右边
            if (col < 3 && grid_[row][col + 1] == val) {
                return true;
            }
            
            // 检查下边
            if (row < 3 && grid_[row + 1][col] == val) {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<std::pair<int, int>> Board::getEmptyCells() const {
    std::vector<std::pair<int, int>> cells;
    
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (grid_[row][col] == 0) {
                cells.push_back({row, col});
            }
        }
    }
    
    return cells;
}

void Board::copyGrid(const int src[4][4], int dst[4][4]) const {
    std::memcpy(dst, src, sizeof(int) * 16);
}

bool Board::gridEquals(const int grid1[4][4], const int grid2[4][4]) const {
    return std::memcmp(grid1, grid2, sizeof(int) * 16) == 0;
}

