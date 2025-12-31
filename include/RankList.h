#ifndef RANKLIST_H
#define RANKLIST_H

#include <string>
#include <vector>

// 链表节点
struct RankNode {
    std::string username;
    int score;
    RankNode* next;
    
    RankNode(const std::string& name, int s) 
        : username(name), score(s), next(nullptr) {}
};

class RankList {
public:
    RankList(const std::string& rankFilePath = "ranks.txt");
    ~RankList();
    
    // 从文件加载排行榜
    void load();
    
    // 保存排行榜到文件
    void save();
    
    // 插入或更新（如果用户名存在且新分数更高则更新，否则插入）
    void insertOrUpdate(const std::string& username, int score);
    
    // 删除指定用户名的记录
    bool remove(const std::string& username);
    
    // 获取最高分（用于显示Best Score）
    int getBestScore() const;
    
    // 获取指定用户的最高分
    int getUserScore(const std::string& username) const;
    
    // 获取所有排名（用于显示排行榜）
    std::vector<std::pair<std::string, int>> getAll() const;
    
    // 清空链表
    void clear();
    
private:
    RankNode* head_;
    std::string rankFilePath_;
    
    // 插入节点到链表的正确位置（按分数降序）
    void insertNode(RankNode* node);
    
    // 查找节点
    RankNode* findNode(const std::string& username);
};

#endif // RANKLIST_H

