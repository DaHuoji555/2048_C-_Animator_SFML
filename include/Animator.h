// include/Animator.h
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "MoveEvent.h"
#include <vector>
#include <functional>

class Animator {
public:
    Animator();

    // 设置单元格尺寸、网格起始位置和间距（用于像素坐标转换）
    void setGridParams(float cellSize, float padding, float gridStartX, float gridStartY);

    // 开始移动/合并动画（由 MoveEvent 驱动）
    void startMoveAnimation(const std::vector<MoveEvent>& events,
                            const int currentGrid[4][4]);

    // 开始生成动画（新方块弹出）
    void startSpawnAnimation(int row, int col, int value);

    // 每帧更新动画（deltaTime: 秒）
    void update(float deltaTime);

    // 获取当前视觉方块列表（用于渲染）
    const std::vector<VisualTile>& getVisualTiles() const;

    // 是否正在动画中
    bool isAnimating() const;
    
    // 是否是生成动画（用于渲染判断）
    bool isSpawningAnimation() const;

    // 设置动画完成回调
    void setOnComplete(std::function<void()> callback);

    // 可选：手动停止动画（调试/切场景用）
    void stop();

private:
    std::vector<VisualTile> visualTiles_;
    bool isAnimating_;
    bool isSpawning_;  // 标记当前是否是生成动画
    float animationTime_;

    float moveDuration_;   // 移动动画时长（秒）
    float mergeDuration_;  // 合并弹出动画时长（秒）
    float spawnDuration_;  // 生成弹出动画时长（秒）

    float cellSize_;
    float padding_;
    float gridStartX_;
    float gridStartY_;

    std::function<void()> onComplete_;

    // 辅助函数
    void gridToPixel(int row, int col, float& x, float& y) const;
    float lerp(float a, float b, float t) const;
};

#endif // ANIMATOR_H
