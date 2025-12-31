#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>

class Board;

class SaveManager {
public:
    SaveManager(const std::string& saveFilePath = "save.txt");
    
    // 保存游戏状态
    bool save(const std::string& username, const Board& board);
    
    // 加载游戏状态，返回是否成功
    bool load(std::string& username, Board& board);
    
    // 检查保存文件是否存在
    bool hasSave() const;
    
    // 删除保存文件
    void deleteSave();
    
private:
    std::string saveFilePath_;
};

#endif // SAVEMANAGER_H

