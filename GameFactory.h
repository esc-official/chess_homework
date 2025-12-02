#ifndef GAMEFACTORY_H
#define GAMEFACTORY_H

#include <memory>
#include "AbstractGame.h"
#include "GomokuGame.h"
#include "GoGame.h"

// 抽象工厂接口
class IGameFactory {
public:
    virtual std::shared_ptr<AbstractGame> createGame(int size) = 0;
    virtual ~IGameFactory() = default;
};

// 具体工厂：五子棋工厂
class GomokuFactory : public IGameFactory {
public:
    std::shared_ptr<AbstractGame> createGame(int size) override {
        return std::make_shared<GomokuGame>(size);
    }
};

// 具体工厂：围棋工厂
class GoFactory : public IGameFactory {
public:
    std::shared_ptr<AbstractGame> createGame(int size) override {
        return std::make_shared<GoGame>(size);
    }
};

#endif // GAMEFACTORY_H
