#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <iomanip>
#include "GameTypes.h"
#include "Piece.h"

// 抽象组件 (Component)
class UIComponent {
public:
    virtual void draw() = 0;
    virtual void add(std::shared_ptr<UIComponent> c) {
        // 默认实现：叶子节点不支持添加
        throw GameException("叶子节点不支持添加子组件");
    }
    virtual ~UIComponent() = default;
};

// 组合节点 (Composite) - 容器
class PanelComponent : public UIComponent {
private:
    std::vector<std::shared_ptr<UIComponent>> children;
    std::string name;

public:
    PanelComponent(const std::string& n = "Panel") : name(n) {}

    void add(std::shared_ptr<UIComponent> c) override {
        children.push_back(c);
    }

    void draw() override {
        // 依次绘制所有子组件
        for (const auto& child : children) {
            child->draw();
        }
    }
};

// 叶子节点 - 文本组件 (Leaf)
class TextComponent : public UIComponent {
private:
    std::string text;
    bool visible;
public:
    TextComponent(const std::string& t = "", bool v = true) : text(t), visible(v) {}

    void setText(const std::string& t) { text = t; }
    void setVisible(bool v) { visible = v; }

    void draw() override {
        if (visible && !text.empty()) {
            std::cout << text << "\n";
        }
    }
};

// 叶子节点 - 棋盘组件 (Leaf)
class BoardComponent : public UIComponent {
    const std::vector<std::vector<int>>* data = nullptr;
    int size = 0;
public:
    void update(const std::vector<std::vector<int>>& d, int s) { data = &d; size = s; }
    
    void draw() override {
        if (!data) return;
        std::cout << "\n"; // 上留白
        
        // 绘制列号
        std::cout << "   ";
        for (int i = 0; i < size; ++i) std::cout << std::setw(2) << i + 1 << " ";
        std::cout << "\n";

        // 绘制行号和棋盘
        for (int i = 0; i < size; ++i) {
            std::cout << std::setw(2) << i + 1 << " ";
            for (int j = 0; j < size; ++j) {
                int val = (*data)[i][j];
                if (val == 0) std::cout << "十 ";
                else if (val == 1) std::cout << PieceFactory::getPiece(PieceColor::BLACK)->getSymbol() << " ";
                else if (val == 2) std::cout << PieceFactory::getPiece(PieceColor::WHITE)->getSymbol() << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n"; // 下留白
    }
};

#endif // UICOMPONENT_H
