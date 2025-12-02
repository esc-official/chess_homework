#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <memory>
#include <string>
#include "AbstractGame.h"
#include "ConsoleUI.h"

// ÏµÍ³¿ØÖÆÆ÷£¨Singleton + Facade pattern£©
class GameSystem {
private:
    static GameSystem* instance;
    std::shared_ptr<AbstractGame> game;
    std::shared_ptr<ConsoleUI> ui;
    bool running;

    GameSystem();

public:
    static GameSystem* getInstance();
    void run();
    void processCommand(const std::string& line);
};

#endif // GAMESYSTEM_H
