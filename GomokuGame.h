#ifndef GOMOKUGAME_H
#define GOMOKUGAME_H

#include "AbstractGame.h"
#include "GomokuStrategy.h"

// 五子棋游戏
class GomokuGame : public AbstractGame {
public:
    // 构造时注入五子棋策略
    GomokuGame(int s) : AbstractGame(s, std::make_shared<GomokuMoveStrategy>(), std::make_shared<GomokuWinStrategy>()) {}

    GameType getType() const override { return GameType::GOMOKU; }

    void postMoveProcess(int x, int y) override {
        // 五子棋无副作用
    }
};

#endif // GOMOKUGAME_H
