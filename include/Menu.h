#ifndef MENU_H
#define MENU_H

#include <string>
#include <SFML/Graphics.hpp>


struct Button {//按钮结构
    float x;//按钮x坐标
    float y;
    float width;
    float height;//按钮高度
    std::string text;//按钮文本
    sf::Color color;//按钮颜色
    sf::Color textColor;
    

    bool contains(int mouseX, int mouseY) const;
};


enum class MenuAction {//菜单操作枚举
    NONE,//无操作
    START_GAME,//开始游戏
    CONTINUE_GAME,//继续游戏
    NEW_GAME,//新游戏
    VIEW_RANK,//查看排行榜
    BACK_TO_MENU,//返回菜单
    QUIT//退出游戏
};


class Menu {
private:
    std::string playerName;     // 玩家名称
    bool hasSaveFile;           // 是否有存档
    bool inputActive;           // 输入框是否激活
    int hoveredButton;          // 当前悬停的按钮索引
    
    // 按钮定义
    Button continueButton;//继续游戏按钮
    Button newGameButton;//新游戏按钮
    Button startButton;//开始游戏按钮
    Button rankButton;//排行榜按钮
    Button quitButton;//退出按钮
    Button backButton;//返回按钮
    
    // 输入框定义
    float inputBoxX;//输入框x坐标
    float inputBoxY;//输入框y坐标
    float inputBoxWidth;//输入框宽度
    float inputBoxHeight;//输入框高度
    
    void initButtons();//初始化按钮

public:
    Menu();
    
    void setHasSaveFile(bool hasSave);//设置是否有存档
    std::string getPlayerName() const;//获取玩家名称
    void setPlayerName(const std::string& name);
    void clearPlayerName();//清除玩家名称
    
    void handleTextInput(sf::Uint32 unicode);
    bool checkInputBoxClick(int mouseX, int mouseY);//检查输入框点击
    bool isInputActive() const;//检查输入框是否激活 
    void deactivateInput();//禁用输入框
    
    void updateHover(int mouseX, int mouseY);
    bool isInputBoxHovered() const;//检查输入框是否悬停
    bool isButtonHovered(const Button& button) const;//检查按钮是否悬停
    
    MenuAction handleClick(int mouseX, int mouseY);//处理鼠标点击
    
    const Button& getContinueButton() const;//获取继续游戏按钮
    const Button& getNewGameButton() const;//获取新游戏按钮
    const Button& getStartButton() const;//获取开始游戏按钮
    const Button& getRankButton() const;//获取排行榜按钮
    const Button& getQuitButton() const;//获取退出按钮
    const Button& getBackButton() const;//获取返回按钮
    
    void getInputBox(float& x, float& y, float& width, float& height) const;//获取输入框
    bool hasLoadGame() const;//检查是否有存档
};

#endif // MENU_H
