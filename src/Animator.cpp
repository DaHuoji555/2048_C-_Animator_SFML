// src/Animator.cpp
#include "Animator.h"
#include <algorithm>
#include <cmath>

Animator::Animator()
    : isAnimating_(false),
      isSpawning_(false),
      animationTime_(0.0f),
      moveDuration_(0.2f),
      mergeDuration_(0.1f),
      spawnDuration_(0.1f),
      cellSize_(100.0f),
      padding_(10.0f),
      gridStartX_(0.0f),
      gridStartY_(0.0f) {
}

void Animator::setGridParams(float cellSize, float padding, float gridStartX, float gridStartY) {
    cellSize_ = cellSize;
    padding_ = padding;
    gridStartX_ = gridStartX;
    gridStartY_ = gridStartY;
}

void Animator::startMoveAnimation(const std::vector<MoveEvent>& events,
                                  const int currentGrid[4][4]) {
    (void)currentGrid; // 当前暂未使用，可用于校验

    visualTiles_.clear();
    if (events.empty()) {
        isAnimating_ = false;
        animationTime_ = 0.0f;
        return;
    }

    // 为每个事件创建 VisualTile（关键：固定 startX/startY，避免 lerp 起点漂移导致鬼畜）
    for (const auto& event : events) {
        if (event.value <= 0) continue; // 安全：忽略空 tile

        VisualTile tile{};
        tile.value = event.value;

        // 固定起点
        gridToPixel(event.fromRow, event.fromCol, tile.startX, tile.startY);
        tile.currentX = tile.startX;
        tile.currentY = tile.startY;

        // 固定终点
        gridToPixel(event.toRow, event.toCol, tile.targetX, tile.targetY);

        tile.scale = 1.0f;

        if (event.type == EventType::MOVE) {
            tile.isMoving = true;
            tile.isMerging = false;
        } else if (event.type == EventType::MERGE) {
            tile.isMoving = true;
            tile.isMerging = true;
        } else {
            // 其他类型先按移动处理（可扩展）
            tile.isMoving = true;
            tile.isMerging = false;
        }

        tile.isSpawning = false;
        visualTiles_.push_back(tile);
    }

    isAnimating_ = !visualTiles_.empty();
    isSpawning_ = false;  // 移动/合并动画
    animationTime_ = 0.0f;
}

void Animator::startSpawnAnimation(int row, int col, int value) {

    visualTiles_.clear();

    if (value <= 0) {
        isAnimating_ = false;
        animationTime_ = 0.0f;
        return;
    }

    VisualTile tile{};
    tile.value = value;

    gridToPixel(row, col, tile.startX, tile.startY);
    tile.currentX = tile.startX;
    tile.currentY = tile.startY;

    tile.targetX = tile.startX;
    tile.targetY = tile.startY;

    tile.scale = 0.0f; // 从0开始弹出
    tile.isMoving = false;
    tile.isMerging = false;
    tile.isSpawning = true;

    visualTiles_.push_back(tile);

    isAnimating_ = true;
    isSpawning_ = true;  // 生成动画
    animationTime_ = 0.0f;
}

void Animator::update(float deltaTime) {
    if (!isAnimating_) return;

    // 防御：避免传入异常 deltaTime（比如暂停/窗口拖动导致超大dt）
    if (deltaTime < 0.0f) deltaTime = 0.0f;
    if (deltaTime > 0.1f)  deltaTime = 0.1f; // 限制最大dt，避免跳帧鬼畜

    animationTime_ += deltaTime;

    bool allComplete = true;

    for (auto& tile : visualTiles_) {
        if (tile.isMoving) {
            // 1) 移动阶段
            float moveT = std::min(1.0f, animationTime_ / moveDuration_);

            // 关键修复：用固定 startX/startY 做 lerp，不要用 currentX/currentY
            tile.currentX = lerp(tile.startX, tile.targetX, moveT);
            tile.currentY = lerp(tile.startY, tile.targetY, moveT);

            // 移动阶段所有方块保持 scale=1（不管是否要合并）
            tile.scale = 1.0f;

            if (moveT < 1.0f) {
                allComplete = false;
                continue;
            }

            // 2) 合并弹出阶段（只对 isMerging 的 tile）
            if (tile.isMerging) {
                if (animationTime_ < moveDuration_) {
                    // 还没到合并阶段（理论上不会发生，但防御一下）
                    tile.scale = 1.0f;
                    allComplete = false;
                    continue;
                }

                // 关键修复：进入合并阶段时，显示合并后的值（原值*2）
                // 使用tile自己的标志，避免多个tile冲突
                if (!tile.mergeValueUpdated) {
                    tile.value = tile.value * 2; // 8 -> 16
                    tile.mergeValueUpdated = true;
                }

                float mergeT = std::min(1.0f, (animationTime_ - moveDuration_) / mergeDuration_);

                // pop：1.0 -> 1.2 -> 1.0
                if (mergeT < 0.5f) {
                    tile.scale = 1.0f + (mergeT / 0.5f) * 0.2f; // 1.0 -> 1.2
                } else {
                    tile.scale = 1.2f - ((mergeT - 0.5f) / 0.5f) * 0.2f; // 1.2 -> 1.0
                }

                if (mergeT < 1.0f) {
                    allComplete = false;
                }
            }
        } else if (tile.isSpawning) {
            // 生成弹出
            float t = std::min(1.0f, animationTime_ / spawnDuration_);

            // pop：0 -> 1.2 -> 1.0
            if (t < 0.5f) {
                tile.scale = (t / 0.5f) * 1.2f; // 0 -> 1.2
            } else {
                tile.scale = 1.2f - ((t - 0.5f) / 0.5f) * 0.2f; // 1.2 -> 1.0
            }

            if (t < 1.0f) {
                allComplete = false;
            }
        }
    }

    if (allComplete) {
        isAnimating_ = false;
        isSpawning_ = false;  // 重置标志
        visualTiles_.clear();

        if (onComplete_) {
            auto cb = onComplete_; // 防止回调里又 setOnComplete 修改
            cb();
        }
    }
}

const std::vector<VisualTile>& Animator::getVisualTiles() const {
    return visualTiles_;
}

bool Animator::isAnimating() const {
    return isAnimating_;
}

bool Animator::isSpawningAnimation() const {
    return isSpawning_;
}

void Animator::setOnComplete(std::function<void()> callback) {
    onComplete_ = std::move(callback);
}

void Animator::stop() {
    isAnimating_ = false;
    isSpawning_ = false;
    animationTime_ = 0.0f;
    visualTiles_.clear();
}

void Animator::gridToPixel(int row, int col, float& x, float& y) const {
    // 关键修复：每个单元格占用 (cellSize_ + padding_) 的空间
    x = gridStartX_ + static_cast<float>(col) * (cellSize_ + padding_);
    y = gridStartY_ + static_cast<float>(row) * (cellSize_ + padding_);
}

float Animator::lerp(float a, float b, float t) const {
    return a + (b - a) * t;
}
