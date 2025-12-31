#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <utility>

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// 移动结果结构
struct MoveResult {
    int grid[4][4];
    int scoreGain;
    bool changed;
};

class Board {
public:
    Board();
    
    // 初始化新游戏（生成两个初始方块）
    void init();
    
    // 获取当前网格
    void getGrid(int outGrid[4][4]) const;
    
    // 设置网格（用于加载存档）
    void setGrid(const int inGrid[4][4]);
    
    // 获取指定位置的值
    int getValue(int row, int col) const;
    
    // 模拟移动（不修改当前状态，返回结果）
    MoveResult simulateMove(Direction dir) const;
    
    // 提交移动结果（应用到当前状态）
    void commitGrid(const int newGrid[4][4]);
    
    // 生成新方块（在随机空位置，2占90%，4占10%）
    // 返回生成的位置和值
    std::pair<std::pair<int, int>, int> spawnNewTile();
    
    // 检查游戏状态
    bool hasWon() const;        // 是否出现2048
    bool isGameOver() const;    // 是否无法移动
    
    // 分数管理
    int getScore() const;
    void setScore(int score);
    void addScore(int delta);
    
private:
    int grid_[4][4];
    int score_;
    
    // 辅助函数
    void moveLeft(int tempGrid[4][4], int& scoreGain) const;
    void moveRight(int tempGrid[4][4], int& scoreGain) const;
    void moveUp(int tempGrid[4][4], int& scoreGain) const;
    void moveDown(int tempGrid[4][4], int& scoreGain) const;
    
    bool canMove() const;
    std::vector<std::pair<int, int>> getEmptyCells() const;
    void copyGrid(const int src[4][4], int dst[4][4]) const;
    bool gridEquals(const int grid1[4][4], const int grid2[4][4]) const;
};

#endif // BOARD_H

