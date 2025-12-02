#ifndef GOMOKUSTRATEGY_H
#define GOMOKUSTRATEGY_H

#include "Strategy.h"

// 五子棋移动策略
class GomokuMoveStrategy : public IMoveStrategy {
public:
    bool isValid(int x, int y, const std::vector<std::vector<int>>& board, int size) override {
        // 五子棋规则：只要不越界（Game类已判）且该位置为空即可
        return board[x][y] == 0;
    }
};

// 五子棋胜负判定策略
class GomokuWinStrategy : public IWinStrategy {
public:
    // 五子棋忽略 forceEnd，因为每一步都需要检查是否连五
    PieceColor checkWin(const std::vector<std::vector<int>>& board, int size, bool forceEnd = false) override {
        // 检查四个方向：横、纵、主对角、副对角
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                int c = board[i][j];
                if (c == 0) continue;
                // 横向
                if (j + 4 < size && board[i][j+1]==c && board[i][j+2]==c && board[i][j+3]==c && board[i][j+4]==c)
                    return (c==1 ? PieceColor::BLACK : PieceColor::WHITE);
                // 纵向
                if (i + 4 < size && board[i+1][j]==c && board[i+2][j]==c && board[i+3][j]==c && board[i+4][j]==c)
                    return (c==1 ? PieceColor::BLACK : PieceColor::WHITE);
                // 主对角
                if (i + 4 < size && j + 4 < size && board[i+1][j+1]==c && board[i+2][j+2]==c && board[i+3][j+3]==c && board[i+4][j+4]==c)
                    return (c==1 ? PieceColor::BLACK : PieceColor::WHITE);
                // 副对角
                if (i + 4 < size && j - 4 >= 0 && board[i+1][j-1]==c && board[i+2][j-2]==c && board[i+3][j-3]==c && board[i+4][j-4]==c)
                    return (c==1 ? PieceColor::BLACK : PieceColor::WHITE);
            }
        }
        return PieceColor::NONE;
    }
};

#endif // GOMOKUSTRATEGY_H
