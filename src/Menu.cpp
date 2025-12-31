#include "Menu.h"

// Button类方法实现
bool Button::contains(int mouseX, int mouseY) const {
    return mouseX >= x && mouseX <= x + width && 
           mouseY >= y && mouseY <= y + height;
}

// Menu构造函数
Menu::Menu() : playerName(""), hasSaveFile(false), inputActive(false), hoveredButton(-1),
               inputBoxX(100), inputBoxY(230),  // 居中：(600-400)/2 = 100
               inputBoxWidth(400), inputBoxHeight(60) {
    initButtons();
}

// 初始化按钮位置（适配600宽度窗口）
void Menu::initButtons() {
    // 窗口宽度600，按钮宽度400，居中 = (600-400)/2 = 100
    float centerX = 100.0f;
    float buttonWidth = 400.0f;
    
    // 继续游戏按钮
    continueButton.x = centerX;
    continueButton.y = 350;
    continueButton.width = buttonWidth;
    continueButton.height = 70;
    continueButton.text = "继续上次游戏";
    continueButton.color = sf::Color(237, 194, 46);
    continueButton.textColor = sf::Color(255, 255, 255);
    
    // 新游戏按钮（有存档时）
    newGameButton.x = centerX;
    newGameButton.y = 440;
    newGameButton.width = buttonWidth;
    newGameButton.height = 70;
    newGameButton.text = "开始新游戏";
    newGameButton.color = sf::Color(143, 122, 102);
    newGameButton.textColor = sf::Color(255, 255, 255);
    
    // 开始按钮（无存档时）
    startButton.x = centerX;
    startButton.y = 380;
    startButton.width = buttonWidth;
    startButton.height = 70;
    startButton.text = "开始游戏";
    startButton.color = sf::Color(237, 194, 46);
    startButton.textColor = sf::Color(255, 255, 255);
    
    // 排行榜按钮
    rankButton.x = centerX;
    rankButton.y = 540;
    rankButton.width = buttonWidth;
    rankButton.height = 60;
    rankButton.text = "查看排行榜";
    rankButton.color = sf::Color(143, 122, 102);
    rankButton.textColor = sf::Color(255, 255, 255);
    
    // 退出按钮
    quitButton.x = centerX;
    quitButton.y = 620;
    quitButton.width = buttonWidth;
    quitButton.height = 60;
    quitButton.text = "退出游戏";
    quitButton.color = sf::Color(187, 173, 160);
    quitButton.textColor = sf::Color(119, 110, 101);
    
    // 返回按钮（排行榜用）
    backButton.x = 150;  // (600-300)/2 = 150
    backButton.y = 660;
    backButton.width = 300;
    backButton.height = 60;
    backButton.text = "返回菜单";
    backButton.color = sf::Color(143, 122, 102);
    backButton.textColor = sf::Color(255, 255, 255);
}

void Menu::setHasSaveFile(bool hasSave) {
    hasSaveFile = hasSave;
}

std::string Menu::getPlayerName() const {
    return playerName.empty() ? "玩家" : playerName;
}

void Menu::setPlayerName(const std::string& name) {
    playerName = name;
}

void Menu::clearPlayerName() {
    playerName.clear();
    inputActive = false;
}

bool Menu::checkInputBoxClick(int mouseX, int mouseY) {
    if (mouseX >= inputBoxX && mouseX <= inputBoxX + inputBoxWidth &&
        mouseY >= inputBoxY && mouseY <= inputBoxY + inputBoxHeight) {
        inputActive = true;
        return true;
    }
    return false;
}

bool Menu::isInputActive() const {
    return inputActive;
}

void Menu::deactivateInput() {
    inputActive = false;
}

void Menu::handleTextInput(sf::Uint32 unicode) {
    if (!inputActive) {
        return;
    }
    
    if (unicode < 128) {
        char inputChar = static_cast<char>(unicode);
        
        // 退格键
        if (inputChar == 8) {
            if (!playerName.empty()) {
                playerName.pop_back();
            }
        }
        // 可打印字符
        else if (inputChar >= 32 && inputChar < 127 && playerName.length() < 20) {
            playerName += inputChar;
        }
    } else {
        // 支持中文输入（UTF-8）
        if (playerName.length() < 60) {
            if (unicode < 0x80) {
                playerName += static_cast<char>(unicode);
            } else if (unicode < 0x800) {
                playerName += static_cast<char>(0xC0 | (unicode >> 6));
                playerName += static_cast<char>(0x80 | (unicode & 0x3F));
            } else if (unicode < 0x10000) {
                playerName += static_cast<char>(0xE0 | (unicode >> 12));
                playerName += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
                playerName += static_cast<char>(0x80 | (unicode & 0x3F));
            }
        }
    }
}

MenuAction Menu::handleClick(int mouseX, int mouseY) {
    if (hasSaveFile) {
        if (continueButton.contains(mouseX, mouseY)) {
            return MenuAction::CONTINUE_GAME;
        }
        
        if (newGameButton.contains(mouseX, mouseY)) {
            return MenuAction::NEW_GAME;
        }
    } else {
        if (startButton.contains(mouseX, mouseY)) {
            return MenuAction::START_GAME;
        }
    }
    
    if (rankButton.contains(mouseX, mouseY)) {
        return MenuAction::VIEW_RANK;
    }
    
    if (quitButton.contains(mouseX, mouseY)) {
        return MenuAction::QUIT;
    }
    
    if (backButton.contains(mouseX, mouseY)) {
        return MenuAction::BACK_TO_MENU;
    }
    
    return MenuAction::NONE;
}

const Button& Menu::getContinueButton() const {
    return continueButton;
}

const Button& Menu::getNewGameButton() const {
    return newGameButton;
}

const Button& Menu::getStartButton() const {
    return startButton;
}

const Button& Menu::getRankButton() const {
    return rankButton;
}

const Button& Menu::getQuitButton() const {
    return quitButton;
}

const Button& Menu::getBackButton() const {
    return backButton;
}

void Menu::getInputBox(float& x, float& y, float& width, float& height) const {
    x = inputBoxX;
    y = inputBoxY;
    width = inputBoxWidth;
    height = inputBoxHeight;
}

bool Menu::hasLoadGame() const {
    return hasSaveFile;
}

void Menu::updateHover(int mouseX, int mouseY) {
    hoveredButton = -1;
    
    // 检查输入框
    if (mouseX >= inputBoxX && mouseX <= inputBoxX + inputBoxWidth &&
        mouseY >= inputBoxY && mouseY <= inputBoxY + inputBoxHeight) {
        hoveredButton = 0;
        return;
    }
    
    // 检查各个按钮
    if (hasSaveFile) {
        if (continueButton.contains(mouseX, mouseY)) {
            hoveredButton = 1;
            return;
        }
        if (newGameButton.contains(mouseX, mouseY)) {
            hoveredButton = 2;
            return;
        }
    } else {
        if (startButton.contains(mouseX, mouseY)) {
            hoveredButton = 1;
            return;
        }
    }
    
    if (rankButton.contains(mouseX, mouseY)) {
        hoveredButton = 3;
        return;
    }
    
    if (quitButton.contains(mouseX, mouseY)) {
        hoveredButton = 4;
        return;
    }
    
    if (backButton.contains(mouseX, mouseY)) {
        hoveredButton = 5;
        return;
    }
}

bool Menu::isInputBoxHovered() const {
    return hoveredButton == 0;
}

bool Menu::isButtonHovered(const Button& button) const {
    if (hasSaveFile) {
        if (&button == &continueButton) return hoveredButton == 1;
        if (&button == &newGameButton) return hoveredButton == 2;
    } else {
        if (&button == &startButton) return hoveredButton == 1;
    }
    if (&button == &rankButton) return hoveredButton == 3;
    if (&button == &quitButton) return hoveredButton == 4;
    if (&button == &backButton) return hoveredButton == 5;
    return false;
}

