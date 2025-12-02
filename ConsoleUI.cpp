#include "ConsoleUI.h"
#include <iostream>
#include <cstdlib>

// 构造函数
ConsoleUI::ConsoleUI(std::shared_ptr<UIComponent> root,
          std::shared_ptr<BoardComponent> board,
          std::shared_ptr<TextComponent> hint,
          std::shared_ptr<TextComponent> status)
    : rootComponent(root), boardRef(board), hintRef(hint), statusRef(status) {}

// IGameObserver 实现
void ConsoleUI::onBoardUpdate(const std::vector<std::vector<int>>& board, int size) {
    boardRef->update(board, size);
}

void ConsoleUI::onMessage(const std::string& msg) {
    hintRef->setText("[系统消息] " + msg);
}

void ConsoleUI::onGameOver(PieceColor winner) {
    std::string w = colorToString(winner);
    statusRef->setText("游戏结束 (胜者: " + w + ")");
}

// UI 控制方法
void ConsoleUI::updateGameStatus(const std::string& gameName) {
    std::string display = "当前游戏: <" + gameName + "> ";
    statusRef->setText(display);
}

void ConsoleUI::toggleHints() {
    static bool v = true;
    v = !v;
    hintRef->setVisible(v);
    render();
}

void ConsoleUI::render() {
    // 清屏
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    // 组合模式：一键调用，递归绘制整个 UI 树
    if (rootComponent) {
        rootComponent->draw();
    }
    
    std::cout << "请输入指令 (help 查看帮助): ";
}
