#ifndef GOGAME_H
#define GOGAME_H

#include <set>
#include "AbstractGame.h"
#include "GoStrategy.h"

// 围棋游戏 (简化版：含提子逻辑)
class GoGame : public AbstractGame {
private:
    struct Point { 
        int x, y; 
        bool operator<(const Point& o) const { return x < o.x || (x == o.x && y < o.y); } 
    };

    // 检查一块棋子的气
    int countLiberties(int x, int y, int color, std::set<Point>& visited);
    
    // 移除无气的棋子块
    void removeDeadGroup(int x, int y, int color);

public:
    // 构造时注入围棋策略
    GoGame(int s);
    
    GameType getType() const override { return GameType::GO; }
    void postMoveProcess(int x, int y) override;
};

#endif // GOGAME_H
