#ifndef PIECE_H
#define PIECE_H

#include <memory>
#include "GameTypes.h"

// 抽象棋子接口
class Piece {
public:
    virtual PieceColor getColor() const = 0;
    virtual std::string getSymbol() const = 0;
    virtual ~Piece() = default;
};

// 具体棋子：黑棋
class BlackPiece : public Piece {
public:
    PieceColor getColor() const override { return PieceColor::BLACK; }
    std::string getSymbol() const override { return "回"; } 
};

// 具体棋子：白棋
class WhitePiece : public Piece {
public:
    PieceColor getColor() const override { return PieceColor::WHITE; }
    std::string getSymbol() const override { return "口"; } 
};

// 享元工厂：管理棋子实例，确保全局只有两个棋子对象
class PieceFactory {
public:
    static std::shared_ptr<Piece> getPiece(PieceColor color) {
        static std::shared_ptr<Piece> black = std::make_shared<BlackPiece>();
        static std::shared_ptr<Piece> white = std::make_shared<WhitePiece>();
        if (color == PieceColor::BLACK) return black;
        if (color == PieceColor::WHITE) return white;
        return nullptr;
    }
};

#endif // PIECE_H
