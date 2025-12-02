#ifndef GOSTRATEGY_H
#define GOSTRATEGY_H

#include <sstream>
#include <set>
#include <queue>
#include "Strategy.h"

// 围棋移动策略
class GoMoveStrategy : public IMoveStrategy {
public:
    bool isValid(int x, int y, const std::vector<std::vector<int>>& board, int size) override {
        // 简化的围棋规则：只判空
        // 可以在这里扩展复杂的打劫判断，或者自杀禁手判断
        return board[x][y] == 0;
    }
};

// 围棋胜负判定策略
class GoWinStrategy : public IWinStrategy {
private:
    std::string resultDesc;
    struct Point { int x, y; };

public:
    // 围棋策略修复：
    // 只有在 forceEnd 为 true (双方停手) 时才进行计算并返回胜负。
    // 否则在正常落子阶段返回 NONE，让游戏继续。
    PieceColor checkWin(const std::vector<std::vector<int>>& board, int size, bool forceEnd = false) override;
    
    std::string getResultDescription() override {
        return resultDesc;
    }
};

#endif // GOSTRATEGY_H
