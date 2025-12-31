#include "RankList.h"
#include <fstream>
#include <algorithm>

RankList::RankList(const std::string& rankFilePath) 
    : head_(nullptr), rankFilePath_(rankFilePath) {
}

RankList::~RankList() {
    clear();
}

void RankList::load() {
    clear();
    
    std::ifstream file(rankFilePath_);
    if (!file.is_open()) {
        return;
    }
    
    std::string username;
    int score;
    
    while (file >> username >> score) {
        RankNode* node = new RankNode(username, score);
        insertNode(node);
    }
    
    file.close();
}

void RankList::save() {
    std::ofstream file(rankFilePath_);
    if (!file.is_open()) {
        return;
    }
    
    RankNode* current = head_;
    while (current != nullptr) {
        file << current->username << " " << current->score << "\n";
        current = current->next;
    }
    
    file.close();
}

void RankList::insertOrUpdate(const std::string& username, int score) {
    // 查找是否已存在该用户
    RankNode* existing = findNode(username);
    
    if (existing != nullptr) {
        // 用户已存在
        if (score > existing->score) {
            // 新分数更高，删除旧记录并插入新记录
            remove(username);
            RankNode* newNode = new RankNode(username, score);
            insertNode(newNode);
        }
        // 否则不更新（保留旧的更高分数）
    } else {
        // 新用户，直接插入
        RankNode* newNode = new RankNode(username, score);
        insertNode(newNode);
    }
}

bool RankList::remove(const std::string& username) {
    if (head_ == nullptr) {
        return false;
    }
    
    // 如果是头节点
    if (head_->username == username) {
        RankNode* temp = head_;
        head_ = head_->next;
        delete temp;
        return true;
    }
    
    // 查找要删除的节点
    RankNode* current = head_;
    while (current->next != nullptr) {
        if (current->next->username == username) {
            RankNode* temp = current->next;
            current->next = temp->next;
            delete temp;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

int RankList::getBestScore() const {
    if (head_ == nullptr) {
        return 0;
    }
    return head_->score; // 链表已按分数降序排列
}

int RankList::getUserScore(const std::string& username) const {
    RankNode* current = head_;
    while (current != nullptr) {
        if (current->username == username) {
            return current->score;
        }
        current = current->next;
    }
    return 0;
}

std::vector<std::pair<std::string, int>> RankList::getAll() const {
    std::vector<std::pair<std::string, int>> result;
    
    RankNode* current = head_;
    while (current != nullptr) {
        result.push_back({current->username, current->score});
        current = current->next;
    }
    
    return result;
}

void RankList::clear() {
    while (head_ != nullptr) {
        RankNode* temp = head_;
        head_ = head_->next;
        delete temp;
    }
}

void RankList::insertNode(RankNode* node) {
    // 按分数降序插入
    if (head_ == nullptr || node->score > head_->score) {
        // 插入到头部
        node->next = head_;
        head_ = node;
        return;
    }
    
    // 查找插入位置
    RankNode* current = head_;
    while (current->next != nullptr && current->next->score >= node->score) {
        current = current->next;
    }
    
    // 插入
    node->next = current->next;
    current->next = node;
}

RankNode* RankList::findNode(const std::string& username) {
    RankNode* current = head_;
    while (current != nullptr) {
        if (current->username == username) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

