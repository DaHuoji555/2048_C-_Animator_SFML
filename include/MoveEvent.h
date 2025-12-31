#ifndef MOVEEVENT_H
#define MOVEEVENT_H

// 事件类型：用于驱动动画
enum class EventType {
    MOVE,    // 方块移动（不合并）
    MERGE,   // 方块移动并合并
    SPAWN    // 新方块生成
};

// 数值层 -> 动画层 的事件
struct MoveEvent {
    EventType type;
    int fromRow, fromCol;  // 起始网格位置
    int toRow, toCol;      // 目标网格位置
    int value;             // 方块数值

    MoveEvent(EventType t, int fr, int fc, int tr, int tc, int v)
        : type(t), fromRow(fr), fromCol(fc),
          toRow(tr), toCol(tc), value(v) {}
};

// 视觉方块（动画用，不参与逻辑计算）
struct VisualTile {
    int value;

    // 固定的动画起点（非常重要）
    float startX;
    float startY;

    // 当前像素位置（每帧更新）
    float currentX;
    float currentY;

    // 固定的动画终点
    float targetX;
    float targetY;

    // 缩放比例（合并 / 生成动画）
    float scale;

    // 动画状态标志
    bool isMoving;
    bool isMerging;
    bool isSpawning;
    
    // 合并专用：是否已更新为合并后的值
    bool mergeValueUpdated;

    VisualTile()
        : value(0),
          startX(0.0f), startY(0.0f),
          currentX(0.0f), currentY(0.0f),
          targetX(0.0f), targetY(0.0f),
          scale(1.0f),
          isMoving(false),
          isMerging(false),
          isSpawning(false),
          mergeValueUpdated(false) {}
};

#endif // MOVEEVENT_H
