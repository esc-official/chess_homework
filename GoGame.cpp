#include "GoGame.h"

// 构造时注入围棋策略
GoGame::GoGame(int s) : AbstractGame(s, std::make_shared<GoMoveStrategy>(), std::make_shared<GoWinStrategy>()) {}

// 检查一块棋子的气
int GoGame::countLiberties(int x, int y, int color, std::set<Point>& visited) {
    if (x < 0 || x >= size || y < 0 || y >= size) return 0;
    if (board[x][y] == 0) return 1; // 遇到空位，有气
    if (board[x][y] != color) return 0; // 遇到对方，无气
    if (visited.count({x, y})) return 0; // 已访问

    visited.insert({x, y});
    int liberties = 0;
    int dirs[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
    for (auto& d : dirs) {
        liberties += countLiberties(x + d[0], y + d[1], color, visited);
    }
    return liberties;
}

// 移除无气的棋子块
void GoGame::removeDeadGroup(int x, int y, int color) {
    std::set<Point> group;
    if (countLiberties(x, y, color, group) == 0) {
        for (auto& p : group) {
            board[p.x][p.y] = 0; // 提子
        }
        if (!group.empty()) notifyMessage("提吃 " + std::to_string(group.size()) + " 子");
    }
}

// 落子后处理（提子逻辑）
void GoGame::postMoveProcess(int x, int y) {
    int myColor = board[x][y];
    int opColor = (myColor == 1) ? 2 : 1;
    int dirs[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};

    // 检查四周对手的棋子是否气尽，提子
    for (auto& d : dirs) {
        int nx = x + d[0];
        int ny = y + d[1];
        if (nx >= 0 && nx < size && ny >= 0 && ny < size && board[nx][ny] == opColor) {
            removeDeadGroup(nx, ny, opColor);
        }
    }
}
