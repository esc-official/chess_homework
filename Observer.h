#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
#include <string>
#include "GameTypes.h"

// 观察者接口
class IGameObserver {
public:
    virtual void onBoardUpdate(const std::vector<std::vector<int>>& board, int size) = 0;
    virtual void onMessage(const std::string& msg) = 0;
    virtual void onGameOver(PieceColor winner) = 0;
    virtual ~IGameObserver() = default;
};

#endif // OBSERVER_H
