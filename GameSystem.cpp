#include "GameSystem.h"
#include "UIBuilder.h"
#include "GameFactory.h"
#include <sstream>
#include <fstream>
#include <iostream>

// 单例实例
GameSystem* GameSystem::instance = nullptr;

// 私有构造函数
GameSystem::GameSystem() : running(true) {
    StandardUIBuilder builder;
    ui = builder.build();
}

// 单例获取方法
GameSystem* GameSystem::getInstance() {
    if (instance == nullptr) instance = new GameSystem();
    return instance;
}

// 主运行循环
void GameSystem::run() {
    ui->render();
    std::string line;
    while (running && std::getline(std::cin, line)) {
        if (line.empty()) continue;
        processCommand(line);
    }
}

// 命令处理
void GameSystem::processCommand(const std::string& line) {
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;
    bool needRender = true;
    
    try {
        if (cmd == "exit") {
            running = false;
            needRender = false;
        } else if (cmd == "help") {
            std::string help = "指令列表:\n"
                               "  start gomoku|go [8-19] : 开始新游戏\n"
                               "  move x y : 落子 (行 列，从1开始)\n"
                               "  pass : 停一手 (仅围棋)\n"
                               "  undo : 悔棋\n"
                               "  resign : 认输\n"
                               "  save filename : 保存\n"
                               "  load filename : 读取\n"
                               "  hint : 开关提示\n"
                               "  exit : 退出";
            ui->onMessage(help);
        } else if (cmd == "start") {
            std::string typeStr;
            int size;
            ss >> typeStr >> size;
            if (size < 8 || size > 19) throw GameException("尺寸必须在 8 到 19 之间");
            
            // 根据输入选择对应的工厂
            std::shared_ptr<IGameFactory> factory;
            if (typeStr == "go") {
                factory = std::make_shared<GoFactory>();
            } else if (typeStr == "gomoku") {
                factory = std::make_shared<GomokuFactory>();
            } else {
                throw GameException("未知的游戏类型，请输入 go 或 gomoku");
            }

            // 使用工厂创建产品
            game = factory->createGame(size);
            ui->updateGameStatus(getGameName(game->getType()));
            
            game->addObserver(ui);
            game->refresh();
        } else if (cmd == "move") {
            if (!game) throw GameException("游戏未开始");
            int r, c;
            ss >> r >> c;
            game->makeMove(r - 1, c - 1); // 用户输入1-based，内部0-based
        } else if (cmd == "pass") {
            if (!game) throw GameException("游戏未开始");
            game->passTurn();
        } else if (cmd == "undo") {
            if (!game) throw GameException("游戏未开始");
            game->undo();
        } else if (cmd == "resign") {
            if (!game) throw GameException("游戏未开始");
            game->resign();
            game = nullptr; // 结束引用
        } else if (cmd == "save") {
            if (!game) throw GameException("游戏未开始");
            std::string file;
            ss >> file;
            std::ofstream ofs(file);
            if (!ofs) throw GameException("文件创建失败");
            ofs << game->createMemento()->serialize();
            ui->onMessage("游戏已保存至 " + file);
        } else if (cmd == "load") {
            std::string file;
            ss >> file;
            std::ifstream ifs(file);
            if (!ifs) throw GameException("文件读取失败");
            
            // 反序列化备忘录
            auto mem = GameMemento::deserialize(ifs);
            
            // 根据存档记录的游戏类型选择工厂
            std::shared_ptr<IGameFactory> factory;
            if (mem->getGameType() == GameType::GO) {
                factory = std::make_shared<GoFactory>();
            } else {
                factory = std::make_shared<GomokuFactory>();
            }

            // 重建游戏并恢复状态
            game = factory->createGame(mem->getBoardSize());
            game->restoreMemento(mem);
            
            ui->updateGameStatus(getGameName(game->getType()));
            game->addObserver(ui);
            game->refresh();
            ui->onMessage("游戏已读取: " + file);
        } else if (cmd == "hint") {
            ui->toggleHints();
        } else {
            throw GameException("未知指令");
        }
    } catch (const std::exception& e) {
        // 异常处理：在UI层显示错误
        ui->onMessage(std::string("错误: ") + e.what());
    }
    
    // 确保命令执行后刷新
    if (running && needRender) {
         ui->render();
    }
}
