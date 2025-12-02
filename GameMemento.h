#ifndef GAMEMEMENTO_H
#define GAMEMEMENTO_H

#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <iostream>
#include "GameTypes.h"

class AbstractGame; // 前置声明

// 备忘录：存储游戏快照
class GameMemento {
    friend class AbstractGame; // 允许 Game 访问内部数据
private:
    std::vector<std::vector<int>> boardData; // 0:None, 1:Black, 2:White
    PieceColor currentPlayer;
    int boardSize;
    GameType type;
    int passCount; // 围棋停手计数

public:
    GameMemento(const std::vector<std::vector<int>>& data, PieceColor p, int size, GameType t, int pass)
        : boardData(data), currentPlayer(p), boardSize(size), type(t), passCount(pass) {}

    GameType getGameType() const { return type; }
    int getBoardSize() const { return boardSize; }
    PieceColor getCurrentPlayer() const { return currentPlayer; }

    // 序列化为字符串（用于存档）
    std::string serialize() const {
        std::stringstream ss;
        ss << (type == GameType::GOMOKU ? "GOMOKU" : "GO") << " "
           << boardSize << " " << passCount << " "
           << colorToString(currentPlayer) << "\n";
        for (int i = 0; i < boardSize; ++i) {
            for (int j = 0; j < boardSize; ++j) {
                ss << boardData[i][j] << " ";
            }
            ss << "\n";
        }
        return ss.str();
    }

    // 从字符串反序列化（用于读档）
    static std::shared_ptr<GameMemento> deserialize(std::istream& is) {
        std::string typeStr, playerStr;
        int size, pass;
        is >> typeStr >> size >> pass >> playerStr;
        
        GameType t = (typeStr == "GOMOKU") ? GameType::GOMOKU : GameType::GO;
        PieceColor p = stringToColor(playerStr);

        std::vector<std::vector<int>> data(size, std::vector<int>(size));
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                is >> data[i][j];
            }
        }
        return std::make_shared<GameMemento>(data, p, size, t, pass);
    }
};

#endif // GAMEMEMENTO_H
