#include "Renderer.h"
#include "Board.h"
#include "Menu.h"
#include "RankList.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>

Renderer::Renderer(sf::RenderWindow& window) 
    : window_(window), cellSize_(100.0f), padding_(10.0f) {
    
    // 计算网格起始位置（居中）
    float gridSize = cellSize_ * 4 + padding_ * 5;
    gridStartX_ = (window_.getSize().x - gridSize) / 2.0f + padding_;
    gridStartY_ = 200.0f;  // 为顶部UI留出空间
}

bool Renderer::init() {
    // 优先加载支持中文的字体
    // 1. Noto Sans CJK（最佳中文支持）
    if (font_.loadFromFile("/usr/share/fonts/opentype/noto/NotoSansCJK-Bold.ttc")) {
        return true;
    }
    if (font_.loadFromFile("/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc")) {
        return true;
    }
    
    // 2. WenQuanYi Micro Hei（文泉驿微米黑）
    if (font_.loadFromFile("/usr/share/fonts/truetype/wqy/wqy-microhei.ttc")) {
        return true;
    }
    
    // 3. Droid Sans Fallback（Android字体）
    if (font_.loadFromFile("/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf")) {
        return true;
    }
    
    // 4. DejaVu（英文fallback）
    if (font_.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        return true;
    }
    if (font_.loadFromFile("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf")) {
        return true;
    }
    
    // 5. macOS中文字体
    if (font_.loadFromFile("/System/Library/Fonts/PingFang.ttc")) {
        return true;
    }
    if (font_.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
        return true;
    }
    
    // 6. Windows中文字体
    if (font_.loadFromFile("C:\\Windows\\Fonts\\msyh.ttc")) {  // 微软雅黑
        return true;
    }
    if (font_.loadFromFile("C:\\Windows\\Fonts\\simhei.ttf")) {  // 黑体
        return true;
    }
    if (font_.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        return true;
    }
    
    return false;
}


void Renderer::render(const Board& board, 
                      const std::vector<VisualTile>& visualTiles,
                      const std::string& username,
                      int bestScore,
                      const std::string& statusText) {
window_.clear(sf::Color(250, 248, 239));  // 背景色

drawBackground();
drawGrid();

// 绘制方块
// 策略：
// - 移动/合并动画期间：只画visualTiles（包含所有方块）
// - 生成动画期间：先画Board（新状态），再画visualTiles（新方块弹出）
// - 静态期间：只画Board

if (visualTiles.empty()) {
    // 静态态：绘制Board
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int value = board.getValue(row, col);
            if (value > 0) {
                drawTile(row, col, value);
            }
        }
    }
} else {
    // 动画态：需要判断动画类型
    // 检查是否是生成动画（visualTiles中只有生成方块）
    bool isSpawningAnim = false;
    for (const auto& tile : visualTiles) {
        if (tile.isSpawning) {
            isSpawningAnim = true;
            break;
        }
    }
    
    if (isSpawningAnim) {
        // 生成动画：先画Board（所有方块），再画新方块覆盖
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                int value = board.getValue(row, col);
                if (value > 0) {
                    drawTile(row, col, value);
                }
            }
        }
        // 再画生成动画（覆盖在Board上，显示缩放效果）
        for (const auto& tile : visualTiles) {
            if (tile.value > 0) {
                drawTileAt(tile.currentX, tile.currentY, tile.value, tile.scale);
            }
        }
    } else {
        // 移动/合并动画：只画visualTiles（包含所有方块）
        for (const auto& tile : visualTiles) {
            if (tile.value > 0) {
                drawTileAt(tile.currentX, tile.currentY, tile.value, tile.scale);
            }
        }
    }
}

drawUI(username, board.getScore(), bestScore, statusText);
window_.display();
}

void Renderer::drawBackground() {
    // 绘制标题
    sf::Text titleText;
    titleText.setFont(font_);
    titleText.setString("2048");
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color(119, 110, 101));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(50.0f, 50.0f);
    window_.draw(titleText);
}

void Renderer::drawGrid() {
    // 绘制网格背景
    sf::RectangleShape gridBg(sf::Vector2f(
        cellSize_ * 4 + padding_ * 5,
        cellSize_ * 4 + padding_ * 5
    ));
    gridBg.setPosition(gridStartX_ - padding_, gridStartY_ - padding_);
    gridBg.setFillColor(sf::Color(187, 173, 160));
    window_.draw(gridBg);
    
    // 绘制空单元格
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            sf::RectangleShape cell(sf::Vector2f(cellSize_, cellSize_));
            cell.setPosition(
                gridStartX_ + col * (cellSize_ + padding_),
                gridStartY_ + row * (cellSize_ + padding_)
            );
            cell.setFillColor(sf::Color(205, 193, 180, 150));
            window_.draw(cell);
        }
    }
}

void Renderer::drawTile(int row, int col, int value, float scale) {
    float x = gridStartX_ + col * (cellSize_ + padding_);
    float y = gridStartY_ + row * (cellSize_ + padding_);
    drawTileAt(x, y, value, scale);
}

void Renderer::drawTileAt(float x, float y, int value, float scale) {
    float scaledSize = cellSize_ * scale;
    float offset = (cellSize_ - scaledSize) / 2.0f;
    
    // 绘制方块背景
    sf::RectangleShape tile(sf::Vector2f(scaledSize, scaledSize));
    tile.setPosition(x + offset, y + offset);
    tile.setFillColor(getTileColor(value));
    window_.draw(tile);
    
    // 绘制数字
    sf::Text text;
    text.setFont(font_);
    text.setString(std::to_string(value));
    
    // 根据数字位数调整字体大小
    int digitCount = std::to_string(value).length();
    int fontSize = 40;
    if (digitCount > 2) fontSize = 35;
    if (digitCount > 3) fontSize = 30;
    
    text.setCharacterSize(static_cast<unsigned>(fontSize * scale));
    text.setFillColor(getTextColor(value));
    text.setStyle(sf::Text::Bold);
    
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        x + (cellSize_ - textBounds.width) / 2.0f,
        y + (cellSize_ - textBounds.height) / 2.0f - 5.0f
    );
    window_.draw(text);
}

void Renderer::drawUI(const std::string& username, int score, int bestScore, 
                     const std::string& statusText) {
    // 绘制用户名
    sf::Text userText;
    userText.setFont(font_);
    userText.setString("Player: " + username);
    userText.setCharacterSize(24);
    userText.setFillColor(sf::Color(119, 110, 101));
    userText.setPosition(50.0f, 150.0f);
    window_.draw(userText);
    
    // 绘制分数
    sf::RectangleShape scoreBg(sf::Vector2f(150.0f, 60.0f));
    scoreBg.setPosition(window_.getSize().x - 200.0f, 50.0f);
    scoreBg.setFillColor(sf::Color(187, 173, 160));
    window_.draw(scoreBg);
    
    sf::Text scoreLabel;
    scoreLabel.setFont(font_);
    scoreLabel.setString("SCORE");
    scoreLabel.setCharacterSize(16);
    scoreLabel.setFillColor(sf::Color(238, 228, 218));
    scoreLabel.setPosition(window_.getSize().x - 180.0f, 55.0f);
    window_.draw(scoreLabel);
    
    sf::Text scoreValue;
    scoreValue.setFont(font_);
    scoreValue.setString(std::to_string(score));
    scoreValue.setCharacterSize(24);
    scoreValue.setFillColor(sf::Color::White);
    scoreValue.setStyle(sf::Text::Bold);
    scoreValue.setPosition(window_.getSize().x - 180.0f, 78.0f);
    window_.draw(scoreValue);
    
    // 绘制最高分
    sf::RectangleShape bestBg(sf::Vector2f(150.0f, 60.0f));
    bestBg.setPosition(window_.getSize().x - 200.0f, 120.0f);
    bestBg.setFillColor(sf::Color(187, 173, 160));
    window_.draw(bestBg);
    
    sf::Text bestLabel;
    bestLabel.setFont(font_);
    bestLabel.setString("BEST");
    bestLabel.setCharacterSize(16);
    bestLabel.setFillColor(sf::Color(238, 228, 218));
    bestLabel.setPosition(window_.getSize().x - 180.0f, 125.0f);
    window_.draw(bestLabel);
    
    sf::Text bestValue;
    bestValue.setFont(font_);
    bestValue.setString(std::to_string(bestScore));
    bestValue.setCharacterSize(24);
    bestValue.setFillColor(sf::Color::White);
    bestValue.setStyle(sf::Text::Bold);
    bestValue.setPosition(window_.getSize().x - 180.0f, 148.0f);
    window_.draw(bestValue);
    
    // 绘制状态文本
    if (!statusText.empty()) {
        sf::Text status;
        status.setFont(font_);
        status.setString(statusText);
        status.setCharacterSize(36);
        status.setFillColor(sf::Color(237, 194, 46));
        status.setStyle(sf::Text::Bold);
        
        sf::FloatRect statusBounds = status.getLocalBounds();
        status.setPosition(
            (window_.getSize().x - statusBounds.width) / 2.0f,
            gridStartY_ + cellSize_ * 4 + padding_ * 5 + 30.0f
        );
        window_.draw(status);
    }
}

sf::Color Renderer::getTileColor(int value) const {
    switch (value) {
        case 2:    return sf::Color(238, 228, 218);
        case 4:    return sf::Color(237, 224, 200);
        case 8:    return sf::Color(242, 177, 121);
        case 16:   return sf::Color(245, 149, 99);
        case 32:   return sf::Color(246, 124, 95);
        case 64:   return sf::Color(246, 94, 59);
        case 128:  return sf::Color(237, 207, 114);
        case 256:  return sf::Color(237, 204, 97);
        case 512:  return sf::Color(237, 200, 80);
        case 1024: return sf::Color(237, 197, 63);
        case 2048: return sf::Color(237, 194, 46);
        default:   return sf::Color(60, 58, 50);
    }
}

sf::Color Renderer::getTextColor(int value) const {
    if (value <= 4) {
        return sf::Color(119, 110, 101);  // 深色文字
    }
    return sf::Color(249, 246, 242);  // 浅色文字
}

// ===== 新增的菜单绘制功能 =====

void Renderer::drawRoundedRect(float x, float y, float width, float height, 
                               float radius, const sf::Color& color) {
    (void)radius;  // 未来可用于真正的圆角实现
    // 简化版：绘制普通矩形
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(x, y);
    rect.setFillColor(color);
    window_.draw(rect);
}

void Renderer::drawText(const std::string& text, float x, float y, 
                       unsigned int size, const sf::Color& color, bool centered) {
    sf::Text textObj;
    textObj.setFont(font_);
    
    // 关键修复：使用sf::String::fromUtf8支持中文显示
    sf::String unicodeString = sf::String::fromUtf8(text.begin(), text.end());
    textObj.setString(unicodeString);
    
    textObj.setCharacterSize(size);
    textObj.setFillColor(color);
    textObj.setStyle(sf::Text::Bold);
    
    if (centered) {
        sf::FloatRect bounds = textObj.getLocalBounds();
        textObj.setOrigin(bounds.left + bounds.width / 2.0f, 
                         bounds.top + bounds.height / 2.0f);
        textObj.setPosition(x, y);
    } else {
        textObj.setPosition(x, y);
    }
    
    window_.draw(textObj);
}

void Renderer::drawButton(const Button& button, bool hovered) {
    sf::Color finalColor = hovered ? darkenColor(button.color, 0.85f) : button.color;
    drawRoundedRect(button.x, button.y, button.width, button.height, 5, finalColor);
    drawText(button.text, button.x + button.width / 2, button.y + button.height / 2, 
            22, button.textColor, true);
}

sf::Color Renderer::darkenColor(const sf::Color& color, float factor) const {
    return sf::Color(
        static_cast<sf::Uint8>(color.r * factor),
        static_cast<sf::Uint8>(color.g * factor),
        static_cast<sf::Uint8>(color.b * factor),
        color.a
    );
}

void Renderer::renderMenu(const Menu& menu) {
    window_.clear(sf::Color(250, 248, 239));
    
    // 绘制标题
    drawText("2048", window_.getSize().x / 2.0f, 100, 80, sf::Color(119, 110, 101));
    
    // 获取输入框位置
    float inputX, inputY, inputWidth, inputHeight;
    menu.getInputBox(inputX, inputY, inputWidth, inputHeight);
    
    // 如果输入框激活，绘制高亮边框
    if (menu.isInputActive()) {
        drawRoundedRect(inputX - 3, inputY - 3, inputWidth + 6, inputHeight + 6, 
                       5, sf::Color(237, 194, 46));
    }
    
    // 绘制输入框背景（悬停时变暗）
    sf::Color inputBgColor = sf::Color(187, 173, 160);
    if (menu.isInputBoxHovered() && !menu.isInputActive()) {
        inputBgColor = darkenColor(inputBgColor, 0.85f);
    }
    drawRoundedRect(inputX, inputY, inputWidth, inputHeight, 5, inputBgColor);
    
    // 绘制提示文字
    std::string prompt = menu.isInputActive() ? "请输入您的名字:" : "点击输入框以输入名字";
    drawText(prompt, window_.getSize().x / 2.0f, 220, 24, sf::Color(119, 110, 101));
    
    // 绘制玩家名称
    std::string displayName = menu.getPlayerName();
    if (menu.isInputActive() && displayName == "玩家") {
        displayName = "";
    }
    
    // 添加光标（如果激活）
    if (menu.isInputActive()) {
        static sf::Clock cursorClock;
        if (cursorClock.getElapsedTime().asMilliseconds() / 500 % 2 == 0) {
            displayName += "|";
        }
    }
    
    drawText(displayName, window_.getSize().x / 2.0f, inputY + inputHeight / 2, 
            28, sf::Color(255, 255, 255));
    
    // 绘制按钮
    if (menu.hasLoadGame()) {
        drawButton(menu.getContinueButton(), menu.isButtonHovered(menu.getContinueButton()));
        drawButton(menu.getNewGameButton(), menu.isButtonHovered(menu.getNewGameButton()));
    } else {
        drawButton(menu.getStartButton(), menu.isButtonHovered(menu.getStartButton()));
    }
    
    drawButton(menu.getRankButton(), menu.isButtonHovered(menu.getRankButton()));
    drawButton(menu.getQuitButton(), menu.isButtonHovered(menu.getQuitButton()));
    
    // 绘制操作提示
    drawText("提示: 鼠标点击按钮进行操作", window_.getSize().x / 2.0f, 
            window_.getSize().y - 30, 16, sf::Color(119, 110, 101));
    
    window_.display();
}

void Renderer::renderRankList(const RankList& rankList, const Menu& menu) {
    window_.clear(sf::Color(250, 248, 239));
    
    // 绘制标题
    drawText("排行榜", window_.getSize().x / 2.0f, 80, 50, sf::Color(119, 110, 101));
    
    // 绘制排行榜背景（居中：(600-500)/2 = 50）
    drawRoundedRect(50, 150, 500, 480, 10, sf::Color(187, 173, 160));
    
    // 绘制排行榜内容
    auto ranks = rankList.getAll();
    
    if (ranks.empty()) {
        drawText("暂无记录", window_.getSize().x / 2.0f, 400, 24, sf::Color(255, 255, 255));
    } else {
        int y = 200;
        int rank = 1;
        
        for (const auto& entry : ranks) {
            if (rank > 10) break;  // 最多显示10名
            
            std::stringstream ss;
            ss << rank << ". " << entry.first << " - " << entry.second;
            
            drawText(ss.str(), 80, y, 20, sf::Color(255, 255, 255), false);
            
            y += 40;
            rank++;
        }
    }
    
    // 绘制返回按钮
    drawButton(menu.getBackButton(), menu.isButtonHovered(menu.getBackButton()));
    
    window_.display();
}

