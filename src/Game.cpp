#include "Game.h"
#include <iostream>
#include <algorithm>

Game::Game() 
    : window_(sf::VideoMode(600, 800), "2048 Game"),
      renderer_(window_),
      state_(GameState::MENU),
      wonDisplayed_(false),
      menuSelection_(0) {
    
    window_.setFramerateLimit(60);
    
    // 初始化渲染器
    if (!renderer_.init()) {
        std::cerr << "警告: 无法加载字体，文字可能无法显示" << std::endl;
    }
    
    // 设置动画器参数
    animator_.setGridParams(
        renderer_.getCellSize(),
        renderer_.getPadding(),
        renderer_.getGridStartX(),
        renderer_.getGridStartY()
    );
    
    // 加载排行榜
    rankList_.load();
    
    // 初始化菜单
    menu_.setHasSaveFile(saveManager_.hasSave());
}

Game::~Game() {
}

void Game::run() {
    while (window_.isOpen()) {
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            }
            
            // 处理文本输入（菜单状态）
            if (event.type == sf::Event::TextEntered && state_ == GameState::MENU) {
                menu_.handleTextInput(event.text.unicode);
            }
            
            // 处理鼠标移动（菜单和排行榜状态）
            if (event.type == sf::Event::MouseMoved) {
                if (state_ == GameState::MENU || state_ == GameState::RANK_LIST) {
                    menu_.updateHover(event.mouseMove.x, event.mouseMove.y);
                }
            }
            
            // 处理鼠标点击
            if (event.type == sf::Event::MouseButtonPressed && 
                event.mouseButton.button == sf::Mouse::Left) {
                if (state_ == GameState::MENU) {
                    // 检查输入框点击
                    menu_.checkInputBoxClick(event.mouseButton.x, event.mouseButton.y);
                    
                    // 检查按钮点击
                    MenuAction action = menu_.handleClick(event.mouseButton.x, event.mouseButton.y);
                    
                    if (action == MenuAction::CONTINUE_GAME) {
                        continueGame();
                    } else if (action == MenuAction::START_GAME || action == MenuAction::NEW_GAME) {
                        username_ = menu_.getPlayerName();
                        startNewGame();
                    } else if (action == MenuAction::VIEW_RANK) {
                        state_ = GameState::RANK_LIST;
                    } else if (action == MenuAction::QUIT) {
                        window_.close();
                    }
                } else if (state_ == GameState::RANK_LIST) {
                    MenuAction action = menu_.handleClick(event.mouseButton.x, event.mouseButton.y);
                    if (action == MenuAction::BACK_TO_MENU) {
                        state_ = GameState::MENU;
                        menu_.setHasSaveFile(saveManager_.hasSave());
                    }
                }
            }
            
            if (event.type == sf::Event::KeyPressed) {
                processInput();
            }
        }
        
        // 更新动画
        float deltaTime = clock_.restart().asSeconds();
        animator_.update(deltaTime);
        
        // 状态机处理
        switch (state_) {
            case GameState::MENU:
                handleMenuState();
                break;
            case GameState::PLAYING:
                handlePlayingState();
                break;
            case GameState::WON:
                handleWonState();
                break;
            case GameState::GAME_OVER:
                handleGameOverState();
                break;
            case GameState::RANK_LIST:
                handleRankListState();
                break;
        }
    }
}

void Game::handleMenuState() {
    renderer_.renderMenu(menu_);
}

void Game::handleRankListState() {
    renderer_.renderRankList(rankList_, menu_);
}

void Game::handlePlayingState() {
    renderer_.render(
        board_,
        animator_.getVisualTiles(),
        username_,
        rankList_.getBestScore(),
        ""
    );
}

void Game::handleWonState() {
    renderer_.render(
        board_,
        animator_.getVisualTiles(),
        username_,
        rankList_.getBestScore(),
        "You Win! Press any key to continue..."
    );
}

void Game::handleGameOverState() {
    renderer_.render(
        board_,
        animator_.getVisualTiles(),
        username_,
        rankList_.getBestScore(),
        "Game Over! Press R to restart..."
    );
}

void Game::processInput() {
    if (state_ == GameState::MENU) {
        // 菜单状态下支持快捷键
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
            if (saveManager_.hasSave()) {
                continueGame();
            } else {
                username_ = menu_.getPlayerName();
                startNewGame();
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
            if (saveManager_.hasSave()) {
                username_ = menu_.getPlayerName();
                startNewGame();
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            state_ = GameState::RANK_LIST;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window_.close();
        }
    } else if (state_ == GameState::RANK_LIST) {
        // 排行榜状态下按ESC返回
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            state_ = GameState::MENU;
            menu_.setHasSaveFile(saveManager_.hasSave());
        }
    } else if (state_ == GameState::PLAYING) {
        // 游戏中的方向键输入
        if (animator_.isAnimating()) {
            return;  // 动画进行中，锁定输入
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            handleMove(Direction::UP);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            handleMove(Direction::DOWN);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            handleMove(Direction::LEFT);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            handleMove(Direction::RIGHT);
        }
    } else if (state_ == GameState::WON) {
        // 胜利后可以继续玩
        state_ = GameState::PLAYING;
    } else if (state_ == GameState::GAME_OVER) {
        // 游戏结束，按任意键返回菜单
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            state_ = GameState::MENU;
            menu_.setHasSaveFile(saveManager_.hasSave());
            menu_.clearPlayerName();
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            state_ = GameState::MENU;
            menu_.setHasSaveFile(saveManager_.hasSave());
            menu_.clearPlayerName();
        }
    }
}

void Game::handleMove(Direction dir) {
    // 获取移动前的网格
    int gridBefore[4][4];
    board_.getGrid(gridBefore);
    
    // 模拟移动
    MoveResult result = board_.simulateMove(dir);
    
    if (!result.changed) {
        return;  // 无效移动
    }
    
    // 生成移动事件（传入方向以避免斜向移动）数组
    auto events = computeMoveEvents(gridBefore, result.grid, dir);
    
    // 启动移动动画
    animator_.startMoveAnimation(events, gridBefore);
    
    // 设置动画完成回调
    animator_.setOnComplete([this, result]() {
        onMoveAnimationComplete(result);
    });
}

std::vector<MoveEvent> Game::computeMoveEvents(const int gridBefore[4][4], 
                                              const int gridAfter[4][4],
                                              Direction dir) {
    std::vector<MoveEvent> events;
    bool usedBefore[4][4] = {false};
    bool processedAfter[4][4] = {false};  // 标记已处理的after位置
    
    bool isHorizontal = (dir == Direction::LEFT || dir == Direction::RIGHT);
    
    // 按移动方向的扫描顺序遍历after
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            // 根据方向确定实际遍历的坐标
            int afterRow, afterCol;
            
            if (dir == Direction::LEFT) {
                afterRow = row;
                afterCol = col;  // 从左到右
            } else if (dir == Direction::RIGHT) {
                afterRow = row;
                afterCol = 3 - col;  // 从右到左
            } else if (dir == Direction::UP) {
                afterRow = row;  // 从上到下
                afterCol = col;
            } else { // DOWN
                afterRow = 3 - row;  // 从下到上
                afterCol = col;
            }
            
            int afterValue = gridAfter[afterRow][afterCol];
            
            if (afterValue == 0) {
                continue; // 空位置，跳过
            }
            
            // 只检查已处理的位置中是否有相同值
            // 这样可以正确区分合并和移动
            bool hasOtherSameValueInProcessed = false;
            for (int r = 0; r < 4; ++r) {
                for (int c = 0; c < 4; ++c) {
                    if (r == afterRow && c == afterCol) continue;
                    if (isHorizontal && r != afterRow) continue;
                    if (!isHorizontal && c != afterCol) continue;
                    
                    // 只检查已处理的位置
                    if (processedAfter[r][c] && gridAfter[r][c] == afterValue) {
                        hasOtherSameValueInProcessed = true;
                        break;
                    }
                }
                if (hasOtherSameValueInProcessed) break;
            }
            
            // 判断是合并还是移动
            // 关键：先检查目标位置在Before中是否已有相同值（方块不动）
            int beforeValue = gridBefore[afterRow][afterCol];
            
            bool isMerge = false;
            
            if (beforeValue == afterValue && !usedBefore[afterRow][afterCol]) {
                // 目标位置本身就有相同值，且未使用 → 方块不动，普通移动
                isMerge = false;
            } else {
                // 目标位置不同或已使用，检查是否有2个halfValue
                int halfValue = afterValue / 2;
                int halfValueCount = 0;
                
                for (int r = 0; r < 4; ++r) {
                    for (int c = 0; c < 4; ++c) {
                        if (isHorizontal && r != afterRow) continue;
                        if (!isHorizontal && c != afterCol) continue;
                        
                        if (!usedBefore[r][c] && gridBefore[r][c] == halfValue) {
                            halfValueCount++;
                        }
                    }
                }
                
                isMerge = (halfValueCount >= 2);
            }
            
            if (!isMerge) {
                // After中已处理位置没有相同值
                // 先尝试精确匹配（必须限制方向，避免反向移动误判）
                bool hasExactMatch = false;
                for (int r = 0; r < 4; ++r) {
                    for (int c = 0; c < 4; ++c) {
                        if (isHorizontal && r != afterRow) continue;
                        if (!isHorizontal && c != afterCol) continue;
                        
                        // 关键：精确匹配也要验证方向
                        bool validDirection = false;
                        if (dir == Direction::RIGHT) validDirection = (c <= afterCol);
                        else if (dir == Direction::LEFT) validDirection = (c >= afterCol);
                        else if (dir == Direction::DOWN) validDirection = (r <= afterRow);
                        else if (dir == Direction::UP) validDirection = (r >= afterRow);
                        
                        if (!validDirection) continue;
                        
                        if (!usedBefore[r][c] && gridBefore[r][c] == afterValue) {
                            hasExactMatch = true;
                            break;
                        }
                    }
                    if (hasExactMatch) break;
                }
                
                // 如果有精确匹配，是移动；否则检查半值
                if (!hasExactMatch) {
                    int halfValue = afterValue / 2;
                    int halfValueCount = 0;
                    
                    for (int r = 0; r < 4; ++r) {
                        for (int c = 0; c < 4; ++c) {
                            if (isHorizontal && r != afterRow) continue;
                            if (!isHorizontal && c != afterCol) continue;
                            
                            // 合并查找源时不限制方向
                            
                            if (!usedBefore[r][c] && gridBefore[r][c] == halfValue) {
                                halfValueCount++;
                            }
                        }
                    }
                    
                    isMerge = (halfValueCount >= 2);
                }
            }
            
            if (isMerge) {
                // 合并：需要查找**两个**半值方块（合并需要两个源）
                int halfValue = afterValue / 2;
                std::vector<std::pair<int, int>> sources; // 存储找到的源方块位置
                
                for (int r = 0; r < 4; ++r) {
                    for (int c = 0; c < 4; ++c) {
                        // 只在同一行/列查找
                        if (isHorizontal && r != afterRow) continue;
                        if (!isHorizontal && c != afterCol) continue;
                        
                        // 不限制方向，查找所有未使用的halfValue
                        
                        if (!usedBefore[r][c] && gridBefore[r][c] == halfValue) {
                            sources.push_back({r, c});
                        }
                    }
                }
                
                // 按移动方向排序（优先选择在移动方向上靠前的方块）
                // 向右/下：选择位置最大的（靠右/下的）
                // 向左/上：选择位置最小的（靠左/上的）
                std::sort(sources.begin(), sources.end(), 
                    [dir](const auto& a, const auto& b) {
                        if (dir == Direction::RIGHT || dir == Direction::LEFT) {
                            // 水平移动：比较列
                            if (dir == Direction::RIGHT) {
                                return a.second > b.second;  // 降序：右边的优先
                            } else {
                                return a.second < b.second;  // 升序：左边的优先
                            }
                        } else {
                            // 垂直移动：比较行
                            if (dir == Direction::DOWN) {
                                return a.first > b.first;    // 降序：下边的优先
                            } else {
                                return a.first < b.first;    // 升序：上边的优先
                            }
                        }
                    });
                
                // 查找第一对位置连续的方块
                // 2048规则：只有相邻方块才能合并
                bool foundPair = false;
                for (size_t i = 0; i + 1 < sources.size(); ++i) {
                    int pos1 = isHorizontal ? sources[i].second : sources[i].first;
                    int pos2 = isHorizontal ? sources[i+1].second : sources[i+1].first;
                    
                    if (abs(pos1 - pos2) == 1) {
                        // 找到连续对，生成两个事件
                        events.push_back(MoveEvent(EventType::MOVE, 
                            sources[i].first, sources[i].second, afterRow, afterCol, halfValue));
                        events.push_back(MoveEvent(EventType::MERGE, 
                            sources[i+1].first, sources[i+1].second, afterRow, afterCol, halfValue));
                        
                        usedBefore[sources[i].first][sources[i].second] = true;
                        usedBefore[sources[i+1].first][sources[i+1].second] = true;
                        foundPair = true;
                        break;
                    }
                }
                
                // 如果没找到连续对，fallback：取前2个（避免方块消失）
                if (!foundPair && sources.size() >= 2) {
                    for (int i = 0; i < 2; ++i) {
                        EventType type = (i == 1) ? EventType::MERGE : EventType::MOVE;
                        events.push_back(MoveEvent(type, 
                            sources[i].first, sources[i].second, afterRow, afterCol, halfValue));
                        usedBefore[sources[i].first][sources[i].second] = true;
                    }
                }
            } else {
                // 普通移动：查找相同值的方块
                int bestFromRow = -1, bestFromCol = -1;
                int minDist = 999;
                
                for (int r = 0; r < 4; ++r) {
                    for (int c = 0; c < 4; ++c) {
                        if (isHorizontal && r != afterRow) continue;
                        if (!isHorizontal && c != afterCol) continue;
                        
                        // 普通移动只能从移动反方向来（避免反向移动）
                        // 向右：源必须 <= 目标（从左边或原地）
                        // 向左：源必须 >= 目标（从右边或原地）
                        // 向下：源必须 <= 目标（从上边或原地）
                        // 向上：源必须 >= 目标（从下边或原地）
                        bool validDirection = false;
                        if (dir == Direction::RIGHT) validDirection = (c <= afterCol);
                        else if (dir == Direction::LEFT) validDirection = (c >= afterCol);
                        else if (dir == Direction::DOWN) validDirection = (r <= afterRow);
                        else if (dir == Direction::UP) validDirection = (r >= afterRow);
                        
                        if (!validDirection) continue;
                        
                        if (!usedBefore[r][c] && gridBefore[r][c] == afterValue) {
                            int dist = abs(r - afterRow) + abs(c - afterCol);
                            if (dist < minDist) {
                                minDist = dist;
                                bestFromRow = r;
                                bestFromCol = c;
                            }
                        }
                    }
                }
                
                if (bestFromRow != -1) {
                    events.push_back(MoveEvent(EventType::MOVE, 
                        bestFromRow, bestFromCol, afterRow, afterCol, afterValue));
                    usedBefore[bestFromRow][bestFromCol] = true;
                }
            }
            
            // 标记当前位置已处理
            processedAfter[afterRow][afterCol] = true;
        }
    }
    
    return events;
}

void Game::onMoveAnimationComplete(const MoveResult& result) {
    // 提交移动结果
    board_.commitGrid(result.grid);
    board_.addScore(result.scoreGain);
    
    // 生成新方块
    auto spawnInfo = board_.spawnNewTile();
    
    if (spawnInfo.first.first != -1) {
        // 启动生成动画
        animator_.startSpawnAnimation(
            spawnInfo.first.first,
            spawnInfo.first.second,
            spawnInfo.second
        );
        
        animator_.setOnComplete([this]() {
            onSpawnAnimationComplete();
        });
    } else {
        onSpawnAnimationComplete();
    }
}

void Game::onSpawnAnimationComplete() {
    // 保存游戏
    saveManager_.save(username_, board_);
    
    // 检查游戏状态
    checkGameState();
}

void Game::checkGameState() {
    if (board_.hasWon() && !wonDisplayed_) {
        state_ = GameState::WON;
        wonDisplayed_ = true;
        return;
    }
    
    if (board_.isGameOver()) {
        state_ = GameState::GAME_OVER;
        
        // 更新排行榜
        rankList_.insertOrUpdate(username_, board_.getScore());
        rankList_.save();
        
        // 删除存档
        saveManager_.deleteSave();
    }
}

void Game::startNewGame() {
    board_.init();
    state_ = GameState::PLAYING;
    wonDisplayed_ = false;
    
    // 删除旧存档
    saveManager_.deleteSave();
    
    // 保存新游戏
    saveManager_.save(username_, board_);
}

void Game::continueGame() {
    if (saveManager_.load(username_, board_)) {
        state_ = GameState::PLAYING;
        wonDisplayed_ = board_.hasWon();  // 如果已经胜利过，不再显示胜利消息
    } else {
        // 加载失败，开始新游戏
        getUsernameInput();
        startNewGame();
    }
}

void Game::getUsernameInput() {
    // 从Menu获取用户输入的名称
    username_ = menu_.getPlayerName();
}

