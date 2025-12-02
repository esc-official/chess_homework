#ifndef STRATEGY_H
#define STRATEGY_H

#include <vector>
#include <string>
#include "GameTypes.h"

// 策略接口：判断落子合法性
class IMoveStrategy {
public:
    virtual bool isValid(int x, int y, const std::vector<std::vector<int>>& board, int size) = 0;
    virtual ~IMoveStrategy() = default;
};

// 策略接口：判断胜负
class IWinStrategy {
public:
    virtual PieceColor checkWin(const std::vector<std::vector<int>>& board, int size, bool forceEnd = false) = 0;
    // 获取胜负详情描述（默认为空，五子棋可以不实现）
    virtual std::string getResultDescription() { return ""; }
    virtual ~IWinStrategy() = default;
};

#endif // STRATEGY_H
