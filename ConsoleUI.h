#ifndef CONSOLEUI_H
#define CONSOLEUI_H

#include <memory>
#include <string>
#include "Observer.h"
#include "UIComponent.h"

// 控制台UI类（实现观察者接口）
class ConsoleUI : public IGameObserver {
private:
    // 组合模式的核心：只持有一个根节点即可完成渲染
    std::shared_ptr<UIComponent> rootComponent;

    // 为了能够更新数据，持有特定组件的引用
    std::shared_ptr<BoardComponent> boardRef;
    std::shared_ptr<TextComponent> hintRef;
    std::shared_ptr<TextComponent> statusRef;

public:
    ConsoleUI(std::shared_ptr<UIComponent> root,
              std::shared_ptr<BoardComponent> board,
              std::shared_ptr<TextComponent> hint,
              std::shared_ptr<TextComponent> status);

    // IGameObserver 实现
    void onBoardUpdate(const std::vector<std::vector<int>>& board, int size) override;
    void onMessage(const std::string& msg) override;
    void onGameOver(PieceColor winner) override;
    
    // UI 控制方法
    void updateGameStatus(const std::string& gameName);
    void toggleHints();
    void render();
};

#endif // CONSOLEUI_H
