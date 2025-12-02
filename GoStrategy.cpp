#include "GoStrategy.h"

// 围棋胜负判定策略实现
PieceColor GoWinStrategy::checkWin(const std::vector<std::vector<int>>& board, int size, bool forceEnd) {
    
    // 【修复逻辑】如果在对局中（非终局结算），围棋不通过数子判断胜负
    if (!forceEnd) {
        return PieceColor::NONE; 
    }

    // --- 以下是原本的数子逻辑 (只有 forceEnd=true 时执行) ---
    
    int blackCount = 0; 
    int whiteCount = 0; 
    int blackTerritory = 0; 
    int whiteTerritory = 0; 
    
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));

    for(int i=0; i<size; ++i) {
        for(int j=0; j<size; ++j) {
            if (board[i][j] == 1) {
                blackCount++;
            } else if (board[i][j] == 2) {
                whiteCount++;
            } else if (!visited[i][j]) {
                int areaSize = 0;
                bool touchBlack = false;
                bool touchWhite = false;
                std::vector<Point> q;
                q.push_back({i, j});
                visited[i][j] = true;
                int head = 0;
                while(head < q.size()){
                    Point p = q[head++];
                    areaSize++;
                    int dirs[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
                    for(auto& d : dirs) {
                        int nx = p.x + d[0];
                        int ny = p.y + d[1];
                        if(nx >= 0 && nx < size && ny >= 0 && ny < size) {
                            int val = board[nx][ny];
                            if (val == 1) touchBlack = true;
                            else if (val == 2) touchWhite = true;
                            else if (!visited[nx][ny]) {
                                visited[nx][ny] = true;
                                q.push_back({nx, ny});
                            }
                        }
                    }
                }
                if (touchBlack && !touchWhite) blackTerritory += areaSize;
                else if (!touchBlack && touchWhite) whiteTerritory += areaSize;
            }
        }
    }

    double finalBlack = blackCount + blackTerritory;
    double finalWhite = whiteCount + whiteTerritory + 3.75; 

    std::stringstream ss;
    ss << "黑方: " << finalBlack << " (子" << blackCount << "+地" << blackTerritory << ")\n"
       << "白方: " << finalWhite << " (子" << whiteCount << "+地" << whiteTerritory << "+贴3.75)";
    resultDesc = ss.str();

    if (finalBlack > finalWhite) return PieceColor::BLACK;
    else return PieceColor::WHITE;
}
