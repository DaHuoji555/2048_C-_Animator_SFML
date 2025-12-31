#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Board.h"
#include "Animator.h"
#include "Renderer.h"
#include "SaveManager.h"
#include "RankList.h"
#include "Menu.h"

enum class GameState {
    MENU,//菜单状态
    PLAYING,
    WON,//胜利状态
    GAME_OVER,//游戏结束状态
    RANK_LIST  // 排行榜显示状态
};

class Game {
public:
    Game();
    ~Game();
    
    // 主运行循环
    void run();
    
private:
    sf::RenderWindow window_;//窗口
    sf::Clock clock_;//时钟
    
    Board board_;//游戏逻辑计算
    Animator animator_;//动画
    Renderer renderer_;//渲染
    SaveManager saveManager_;//存档管理
    RankList rankList_;//排行榜
    Menu menu_;  // 新增：菜单管理器
    
    GameState state_;//游戏状态
    std::string username_;//用户名
    bool wonDisplayed_;//是否显示胜利
    
    int menuSelection_;  // 菜单选项索引（保留兼容性）
    
    // 状态机处理
    void handleMenuState();
    void handlePlayingState();
    void handleWonState();
    void handleGameOverState();
    void handleRankListState();  // 排行榜状态
    
    // 输入处理
    void processInput();
    
    // 移动处理
    void handleMove(Direction dir);
    
    // 生成MoveEvent列表（需要知道方向以避免斜向移动）
    std::vector<MoveEvent> computeMoveEvents(const int gridBefore[4][4], 
                                            const int gridAfter[4][4],
                                            Direction dir);
    
    // 动画完成回调
    void onMoveAnimationComplete(const MoveResult& result);
    void onSpawnAnimationComplete();
    
    // 游戏状态检查
    void checkGameState();
    
    // 初始化新游戏
    void startNewGame();
    
    // 继续存档游戏
    void continueGame();
    
    // 获取用户名输入（简化版，使用预设名称）
    void getUsernameInput();
};

#endif // GAME_H

