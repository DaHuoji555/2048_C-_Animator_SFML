#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "MoveEvent.h"

class Board;
class Menu;
class RankList;
struct Button;  // 前置声明

class Renderer {
public:
    Renderer(sf::RenderWindow& window);
    
    // 初始化（加载字体等）
    bool init();
    
    // 绘制整个游戏界面
    void render(const Board& board, 
                const std::vector<VisualTile>& visualTiles,
                const std::string& username,
                int bestScore,
                const std::string& statusText);
    
    // 绘制增强版菜单界面（支持文本输入和鼠标交互）
    void renderMenu(const Menu& menu);
    
    // 绘制排行榜界面
    void renderRankList(const RankList& rankList, const Menu& menu);
    
    // 获取网格参数（供Animator使用）
    float getCellSize() const { return cellSize_; }//获取单元格尺寸
    float getPadding() const { return padding_; }//获取网格间距
    float getGridStartX() const { return gridStartX_; }//获取网格起始X坐标
    float getGridStartY() const { return gridStartY_; }//获取网格起始Y坐标
    
private:
    sf::RenderWindow& window_;//窗口
    sf::Font font_;//字体
    
    float cellSize_;//单元格尺寸
    float gridStartX_;//网格起始X坐标
    float gridStartY_;//网格起始Y坐标
    float padding_;//网格间距
    
    // 绘制辅助函数
    void drawBackground();//绘制背景
    void drawGrid();//绘制网格
    void drawTile(int row, int col, int value, float scale = 1.0f);//绘制方块
    void drawTileAt(float x, float y, int value, float scale = 1.0f);//绘制方块
    void drawUI(const std::string& username, int score, int bestScore, 
                const std::string& statusText);//绘制UI
    
    // 获取方块颜色
    sf::Color getTileColor(int value) const;
    sf::Color getTextColor(int value) const;
    
    // 新增的菜单绘制辅助函数
    void drawRoundedRect(float x, float y, float width, float height, 
                        float radius, const sf::Color& color);//绘制圆角矩形
    void drawButton(const Button& button, bool hovered);//绘制按钮
    void drawText(const std::string& text, float x, float y, 
                 unsigned int size, const sf::Color& color, bool centered = true);//绘制文本
    sf::Color darkenColor(const sf::Color& color, float factor) const;//变暗颜色
};

#endif // RENDERER_H

