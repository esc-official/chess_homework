#ifndef ABSTRACTGAME_H
#define ABSTRACTGAME_H

#include <vector>
#include <memory>
#include <stack>
#include <sstream>
#include "GameTypes.h"
#include "Observer.h"
#include "Strategy.h"
#include "GameMemento.h"

// 游戏逻辑基类（Template Method Pattern）
class AbstractGame {
protected:
    int size;
    std::vector<std::vector<int>> board; // 存储棋盘状态：0空, 1黑, 2白
    PieceColor currentPlayer;
    std::vector<std::shared_ptr<IGameObserver>> observers;
    std::stack<std::shared_ptr<GameMemento>> history; // 历史记录用于悔棋
	
	std::shared_ptr<IMoveStrategy> moveStrategy;
    std::shared_ptr<IWinStrategy> winStrategy;
    int passCount = 0;
    
    void notifyBoardUpdate();
    void notifyMessage(const std::string& msg);
    void notifyGameOver(PieceColor winner);
    void switchPlayer();

public:
    AbstractGame(int s, std::shared_ptr<IMoveStrategy> moveStrat, std::shared_ptr<IWinStrategy> winStrat);
    virtual ~AbstractGame() = default;
    virtual GameType getType() const = 0;
    virtual void postMoveProcess(int x, int y) = 0; // 钩子方法：具体游戏的额外处理

    void addObserver(std::shared_ptr<IGameObserver> obs);
	void refresh();
    
    // 模板方法
    void makeMove(int x, int y);
    void passTurn();
    void undo();
    void resign();
    
    // 备忘录管理
    void saveStateToHistory();
    std::shared_ptr<GameMemento> createMemento();
    void restoreMemento(std::shared_ptr<GameMemento> mem);
};

#endif // ABSTRACTGAME_H
