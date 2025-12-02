#ifndef GAMETYPES_H
#define GAMETYPES_H

#include <string>
#include <exception>

// 枚举定义
enum class PieceColor { NONE, BLACK, WHITE };
enum class GameType { GOMOKU, GO };

// 自定义异常类
class GameException : public std::exception {
private:
    std::string msg;
public:
    GameException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// 工具函数：将颜色转为字符串
inline std::string colorToString(PieceColor c) {
    if (c == PieceColor::BLACK) return "BLACK";
    if (c == PieceColor::WHITE) return "WHITE";
    return "NONE";
}

// 工具函数：将字符串转为颜色
inline PieceColor stringToColor(const std::string& s) {
    if (s == "BLACK") return PieceColor::BLACK;
    if (s == "WHITE") return PieceColor::WHITE;
    return PieceColor::NONE;
}

// 工具函数：获取游戏名称
inline std::string getGameName(GameType t) {
    return (t == GameType::GOMOKU) ? "五子棋" : "围棋";
}

#endif // GAMETYPES_H
