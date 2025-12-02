#ifndef UIBUILDER_H
#define UIBUILDER_H

#include <memory>
#include "ConsoleUI.h"
#include "UIComponent.h"

// UI建造者接口
class UIBuilder {
public:
    virtual std::shared_ptr<ConsoleUI> build() = 0;
    virtual ~UIBuilder() = default;
};

// 标准UI建造者
class StandardUIBuilder : public UIBuilder {
public:
    std::shared_ptr<ConsoleUI> build() override {
        // 1. 创建各个叶子组件
        auto title = std::make_shared<TextComponent>("=== 面向对象对战平台 (五子棋/围棋) ===");
        auto separator = std::make_shared<TextComponent>("------------------------------------");
        auto status = std::make_shared<TextComponent>("状态: 准备就绪");
        auto board = std::make_shared<BoardComponent>();
        auto hint = std::make_shared<TextComponent>("欢迎！请输入 start [type] [size] 开始。");
        auto footer = std::make_shared<TextComponent>("提示: 输入 'help' 查看帮助信息");

        // 2. 创建组合节点
        auto mainPanel = std::make_shared<PanelComponent>("MainPanel");

        // 3. 构建树形结构
        // 结构：MainPanel
        //   ├── Title
        //   ├── Separator
        //   ├── Status
        //   ├── Board
        //   ├── Separator
        //   ├── Hint
        //   └── Footer
        
        mainPanel->add(title);
        mainPanel->add(separator);
        mainPanel->add(status);
        mainPanel->add(board);
        mainPanel->add(separator);
        mainPanel->add(hint);
        mainPanel->add(footer);

        // 4. 返回 UI 控制器，注入根节点和关键节点的引用
        return std::make_shared<ConsoleUI>(mainPanel, board, hint, status);
    }
};

#endif // UIBUILDER_H
