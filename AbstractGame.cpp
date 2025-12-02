#include "AbstractGame.h"

// 构造函数
AbstractGame::AbstractGame(int s, std::shared_ptr<IMoveStrategy> moveStrat, std::shared_ptr<IWinStrategy> winStrat) 
    : size(s), currentPlayer(PieceColor::BLACK), moveStrategy(moveStrat), winStrategy(winStrat) {
    board.resize(size, std::vector<int>(size, 0));
}

// 通知方法
void AbstractGame::notifyBoardUpdate() {
    for (auto& obs : observers) obs->onBoardUpdate(board, size);
}

void AbstractGame::notifyMessage(const std::string& msg) {
    for (auto& obs : observers) obs->onMessage(msg);
}

void AbstractGame::notifyGameOver(PieceColor winner) {
    for (auto& obs : observers) obs->onGameOver(winner);
}

void AbstractGame::switchPlayer() {
    currentPlayer = (currentPlayer == PieceColor::BLACK) ? PieceColor::WHITE : PieceColor::BLACK;
}

// 观察者管理
void AbstractGame::addObserver(std::shared_ptr<IGameObserver> obs) {
    observers.push_back(obs);
}

void AbstractGame::refresh() {
    notifyBoardUpdate();
    notifyMessage("当前轮到: " + colorToString(currentPlayer));
}

// 模板方法：落子流程
void AbstractGame::makeMove(int x, int y) {
    if (x < 0 || x >= size || y < 0 || y >= size) throw GameException("坐标超出范围");
    if (!moveStrategy->isValid(x, y, board, size)) throw GameException("此处不可落子");

    passCount = 0; 
    saveStateToHistory();
    board[x][y] = (currentPlayer == PieceColor::BLACK) ? 1 : 2;
    postMoveProcess(x, y);

    // 常规落子，forceEnd = false
    // 对于围棋，这里 GoWinStrategy 会返回 NONE
    // 对于五子棋，GomokuWinStrategy 会忽略参数，检查是否连五
    PieceColor winner = winStrategy->checkWin(board, size, false); 
    
    notifyBoardUpdate();

    if (winner != PieceColor::NONE) {
        std::string w = colorToString(winner);
        notifyMessage(">>> 决出胜负！获胜者: " + w + " <<<");
        notifyGameOver(winner);
    } else {
        switchPlayer();
        notifyMessage("轮到 " + colorToString(currentPlayer) + " 落子");
    }
}

// 模板方法：停一手 (围棋)
void AbstractGame::passTurn() {
    if (getType() == GameType::GOMOKU) throw GameException("五子棋不能停一手");
    
    saveStateToHistory(); 
    passCount++; 
    
    if (passCount >= 2) {
        // 双方停手，forceEnd = true
        PieceColor winner = winStrategy->checkWin(board, size, true);
        
        std::string details = winStrategy->getResultDescription();
        std::string winnerStr = colorToString(winner);

        // 组装完整信息：包含 1.结算提示 2.分数详情 3.最终胜者
        std::stringstream ss;
        ss << ">>> 双方停手，开始结算 <<<\n";
        if (!details.empty()) {
            ss << details << "\n";
        }
        ss << ">>> 最终结果: " + winnerStr + " 胜 <<<";

        notifyMessage(ss.str());
        notifyGameOver(winner);

        passCount = 0; 
        return;
    }

    switchPlayer();
    notifyMessage(colorToString(currentPlayer == PieceColor::BLACK ? PieceColor::WHITE : PieceColor::BLACK) + " 停一手");
    notifyMessage("轮到 " + colorToString(currentPlayer) + " 落子");
}

// 通用功能：悔棋
void AbstractGame::undo() {
    if (history.empty()) throw GameException("没有可以悔棋的记录");
    auto mem = history.top();
    history.pop();
    restoreMemento(mem);
    notifyMessage("已悔棋，轮到 " + colorToString(currentPlayer));
    notifyBoardUpdate();
}

// 通用功能：认输
void AbstractGame::resign() {
    PieceColor winner = (currentPlayer == PieceColor::BLACK) ? PieceColor::WHITE : PieceColor::BLACK;
    std::string w = colorToString(winner);
    
    notifyMessage(">>> 对方认输，获胜者: " + w + " <<<");
    notifyGameOver(winner);
}

// 备忘录管理
void AbstractGame::saveStateToHistory() {
    history.push(std::make_shared<GameMemento>(board, currentPlayer, size, getType(), passCount));
}

std::shared_ptr<GameMemento> AbstractGame::createMemento() {
    return std::make_shared<GameMemento>(board, currentPlayer, size, getType(), passCount);
}

void AbstractGame::restoreMemento(std::shared_ptr<GameMemento> mem) {
    this->board = mem->boardData;
    this->currentPlayer = mem->currentPlayer;
    this->size = mem->boardSize;
    this->passCount = mem->passCount;
}
