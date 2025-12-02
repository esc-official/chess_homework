/**
 * 面向对象程序设计课程作业 - 棋类对战平台
 * 包含：五子棋 & 围棋
 * 设计模式：工厂、单例、模板方法、观察者、备忘录、享元、建造者
 */

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <stack>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <set>

// ==========================================
// 0. 基础定义与工具
// ==========================================

enum class PieceColor { NONE, BLACK, WHITE };
enum class GameType { GOMOKU, GO };

// 简单的自定义异常
class GameException : public std::exception {
    std::string msg;
public:
    GameException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// 辅助工具：将颜色转为字符串 ID
std::string colorToString(PieceColor c) {
    return (c == PieceColor::BLACK) ? "BLACK" : (c == PieceColor::WHITE ? "WHITE" : "NONE");
}

PieceColor stringToColor(const std::string& s) {
    if (s == "BLACK") return PieceColor::BLACK;
    if (s == "WHITE") return PieceColor::WHITE;
    return PieceColor::NONE;
}

std::string getGameName(GameType t) {
    return (t == GameType::GOMOKU) ? "五子棋" : "围棋";
}

// ==========================================
// 1. 享元模式 (Flyweight Pattern)
// ==========================================

// 抽象棋子
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

// ==========================================
// 2. 备忘录模式 (Memento Pattern)
// ==========================================

class AbstractGame; // 前置声明

// 备忘录：存储游戏快照
class GameMemento {
    friend class AbstractGame; // 允许 Game 访问内部数据
private:
    std::vector<std::vector<int>> boardData; // 0:None, 1:Black, 2:White
    PieceColor currentPlayer;
    int boardSize;
    GameType type;
	int passCount; // <--- 新增
public:
    GameMemento(const std::vector<std::vector<int>>& data, PieceColor p, int size, GameType t, int pass)
        : boardData(data), currentPlayer(p), boardSize(size), type(t), passCount(pass) {} // <--- 修改构造
        
	GameType getGameType() const { return type; }
    int getBoardSize() const { return boardSize; }
    PieceColor getCurrentPlayer() const { return currentPlayer; }
    
    // 序列化为字符串（用于存档）
    std::string serialize() const {
        std::stringstream ss;
        ss << (type == GameType::GOMOKU ? "GOMOKU" : "GO") << "\n";
        ss << boardSize << "\n";
        ss << passCount << "\n"; // <--- 新增序列化
        ss << colorToString(currentPlayer) << "\n";
        for (const auto& row : boardData) {
            for (int val : row) ss << val << " ";
            ss << "\n";
        }
        return ss.str();
    }

    // 从字符串反序列化（用于读档）
    static std::shared_ptr<GameMemento> deserialize(std::istream& is) {
        std::string typeStr, playerStr;
        int size, pass; // <--- 新增变量
        is >> typeStr >> size >> pass >> playerStr; // <--- 新增读取
        
        GameType t = (typeStr == "GOMOKU") ? GameType::GOMOKU : GameType::GO;
        PieceColor p = stringToColor(playerStr);

        std::vector<std::vector<int>> data(size, std::vector<int>(size));
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                is >> data[i][j];
            }
        }return std::make_shared<GameMemento>(data, p, size, t, pass);
    }
};

// ==========================================
// 3. 观察者模式 (Observer Pattern)
// ==========================================

class IGameObserver {
public:
    virtual void onBoardUpdate(const std::vector<std::vector<int>>& board, int size) = 0;
    virtual void onMessage(const std::string& msg) = 0;
    virtual void onGameOver(PieceColor winner) = 0;
    virtual ~IGameObserver() = default;
};

// ==========================================
// 4. 模板方法模式 (Template Method Pattern)
// ==========================================


// ==========================================
// 策略接口定义
// ==========================================

// 策略接口：判断落子合法性
class IMoveStrategy {
public:
    virtual bool isValid(int x, int y, const std::vector<std::vector<int>>& board, int size) = 0;
    virtual ~IMoveStrategy() = default;
};

// 策略接口：判断胜负
class IWinStrategy {
public:
    virtual PieceColor checkWin(const std::vector<std::vector<int>>& board, int size, bool forceEnd = false) = 0;
    // 新增：获取胜负详情描述（默认为空，五子棋可以不实现）
    virtual std::string getResultDescription() { return ""; }
    virtual ~IWinStrategy() = default;
};

// 游戏逻辑基类
class AbstractGame {
protected:
    int size;
    std::vector<std::vector<int>> board; // 存储棋盘状态：0空, 1黑, 2白
    PieceColor currentPlayer;
    std::vector<std::shared_ptr<IGameObserver>> observers;
    std::stack<std::shared_ptr<GameMemento>> history; // 历史记录用于悔棋
	
	std::shared_ptr<IMoveStrategy> moveStrategy;
    std::shared_ptr<IWinStrategy> winStrategy;
    int passCount = 0; // <--- 新增
    
    void notifyBoardUpdate() {
        for (auto& obs : observers) obs->onBoardUpdate(board, size);
    }
    void notifyMessage(const std::string& msg) {
        for (auto& obs : observers) obs->onMessage(msg);
    }
    void notifyGameOver(PieceColor winner) {
        for (auto& obs : observers) obs->onGameOver(winner);
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == PieceColor::BLACK) ? PieceColor::WHITE : PieceColor::BLACK;
    }

public:
    AbstractGame(int s, std::shared_ptr<IMoveStrategy> moveStrat, std::shared_ptr<IWinStrategy> winStrat) 
        : size(s), currentPlayer(PieceColor::BLACK), moveStrategy(moveStrat), winStrategy(winStrat) {
        board.resize(size, std::vector<int>(size, 0));
    }
    virtual ~AbstractGame() = default;
    virtual GameType getType() const = 0;

    void addObserver(std::shared_ptr<IGameObserver> obs) {
        observers.push_back(obs);
    }
    
	void refresh() {
        notifyBoardUpdate();
        notifyMessage("当前轮到: " + colorToString(currentPlayer));
    }
    
    // --- 模板方法：落子流程 ---
    void makeMove(int x, int y) {
        if (x < 0 || x >= size || y < 0 || y >= size) throw GameException("坐标超出范围");
        if (!moveStrategy->isValid(x, y, board, size)) throw GameException("此处不可落子");

        passCount = 0; 
        saveStateToHistory();
        board[x][y] = (currentPlayer == PieceColor::BLACK) ? 1 : 2;
        postMoveProcess(x, y);

        // 【修改点 1】常规落子，forceEnd = false
        // 对于围棋，这里 GoWinStrategy 会返回 NONE。
        // 对于五子棋，GomokuWinStrategy 会忽略参数，检查是否连五。
        PieceColor winner = winStrategy->checkWin(board, size, false); 
        
        notifyBoardUpdate();

        // [修改] AbstractGame 类中 makeMove 方法底部
		if (winner != PieceColor::NONE) {
		    // 因为 UI 不再自动弹窗提示胜者，这里需要手动发消息
		    std::string w = colorToString(winner);
		    notifyMessage(">>> 决出胜负！获胜者: " + w + " <<<");
		    
		    notifyGameOver(winner);
		} else {
		    switchPlayer();
		    notifyMessage("轮到 " + colorToString(currentPlayer) + " 落子");
		}
    }
// --- 模板方法：停一手 (围棋) ---
    void passTurn() {
        if (getType() == GameType::GOMOKU) throw GameException("五子棋不能停一手");
        
        saveStateToHistory(); 
        passCount++; 
        
        if (passCount >= 2) {
			// 双方停手，forceEnd = true
	        PieceColor winner = winStrategy->checkWin(board, size, true);
	        
	        std::string details = winStrategy->getResultDescription();
	        std::string winnerStr = colorToString(winner);
	
	        // 组装完整信息：包含 1.结算提示 2.分数详情 3.最终胜者
	        std::stringstream ss;
	        ss << ">>> 双方停手，开始结算 <<<\n";
	        if (!details.empty()) {
	            ss << details << "\n";
	        }
	        ss << ">>> 最终结果: " + winnerStr + " 胜 <<<";
	
	        notifyMessage(ss.str()); // 发送合并后的长消息
	        notifyGameOver(winner);  // 此时 UI 只会更新状态栏，不会覆盖这条消息
	
	        passCount = 0; 
	        return;
        }

        switchPlayer();
        notifyMessage(colorToString(currentPlayer == PieceColor::BLACK ? PieceColor::WHITE : PieceColor::BLACK) + " 停一手");
        notifyMessage("轮到 " + colorToString(currentPlayer) + " 落子");
    }

    // --- 通用功能：悔棋 ---
    void undo() {
        if (history.empty()) throw GameException("没有可以悔棋的记录");
        auto mem = history.top();
        history.pop();
        restoreMemento(mem);
        notifyMessage("已悔棋，轮到 " + colorToString(currentPlayer));
        notifyBoardUpdate();
    }

    // --- 通用功能：认输 ---
    void resign() {
	    PieceColor winner = (currentPlayer == PieceColor::BLACK) ? PieceColor::WHITE : PieceColor::BLACK;
	    std::string w = colorToString(winner);
	    
	    // 手动发送胜负消息
	    notifyMessage(">>> 对方认输，获胜者: " + w + " <<<");
	    
	    notifyGameOver(winner);
	}

    // --- 备忘录管理 ---
    void saveStateToHistory() {
		history.push(std::make_shared<GameMemento>(board, currentPlayer, size, getType(), passCount));
    }

    std::shared_ptr<GameMemento> createMemento() {
        return std::make_shared<GameMemento>(board, currentPlayer, size, getType(), passCount);
    }

    void restoreMemento(std::shared_ptr<GameMemento> mem) {
        this->board = mem->boardData;
        this->currentPlayer = mem->currentPlayer;
        this->size = mem->boardSize;
        this->passCount = mem->passCount; // <--- 恢复计数
    }

    // --- 抽象钩子方法 ---
    virtual void postMoveProcess(int x, int y) = 0;
};

// ==========================================
// 5. 具体游戏实现 (Strategy)
// ==========================================


// ==========================================
// 具体策略实现
// ==========================================

// --- 五子棋策略 ---

class GomokuMoveStrategy : public IMoveStrategy {
public:
    bool isValid(int x, int y, const std::vector<std::vector<int>>& board, int size) override {
        // 五子棋规则：只要不越界（Game类已判）且该位置为空即可
        // 这里假设Game类已经判断了坐标范围，这里只判业务规则
        return board[x][y] == 0;
    }
};

class GomokuWinStrategy : public IWinStrategy {
public:
    // 五子棋忽略 forceEnd，因为每一步都需要检查是否连五
    PieceColor checkWin(const std::vector<std::vector<int>>& board, int size, bool forceEnd = false) override {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (board[i][j] == 0) continue;
                int currentVal = board[i][j];
                for (auto& d : dirs) {
                    int count = 1;
                    for (int k = 1; k < 5; ++k) {
                        int ni = i + d[0] * k;
                        int nj = j + d[1] * k;
                        if (ni < 0 || ni >= size || nj < 0 || nj >= size || board[ni][nj] != currentVal) break;
                        count++;
                    }
                    if (count == 5) return (currentVal == 1) ? PieceColor::BLACK : PieceColor::WHITE;
                }
            }
        }
        return PieceColor::NONE;
    }
};


// --- 围棋策略 ---

class GoMoveStrategy : public IMoveStrategy {
public:
    bool isValid(int x, int y, const std::vector<std::vector<int>>& board, int size) override {
        // 简化的围棋规则：只判空
        // 可以在这里扩展复杂的打劫判断，或者自杀禁手判断
        return board[x][y] == 0;
    }
};
class GoWinStrategy : public IWinStrategy {
private:
    std::string resultDesc;
    struct Point { int x, y; };

public:
    // 围棋策略修复：
    // 只有在 forceEnd 为 true (双方停手) 时才进行计算并返回胜负。
    // 否则在正常落子阶段返回 NONE，让游戏继续。
    PieceColor checkWin(const std::vector<std::vector<int>>& board, int size, bool forceEnd = false) override {
        
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

    std::string getResultDescription() override {
        return resultDesc;
    }
};

// 五子棋
class GomokuGame : public AbstractGame {
public:
    // 构造时注入五子棋策略
    GomokuGame(int s) : AbstractGame(s, std::make_shared<GomokuMoveStrategy>(), std::make_shared<GomokuWinStrategy>()) {}
    
    GameType getType() const override { return GameType::GOMOKU; }

    void postMoveProcess(int x, int y) override {
        // 五子棋无副作用
    }
};

// 围棋 (简化版：含提子逻辑)
class GoGame : public AbstractGame {
private:
    struct Point { int x, y; bool operator<(const Point& o) const { return x < o.x || (x == o.x && y < o.y); } };

    // 检查一块棋子的气
    int countLiberties(int x, int y, int color, std::set<Point>& visited) {
        if (x < 0 || x >= size || y < 0 || y >= size) return 0;
        if (board[x][y] == 0) return 1; // 遇到空位，有气
        if (board[x][y] != color) return 0; // 遇到对方，无气
        if (visited.count({x, y})) return 0; // 已访问

        visited.insert({x, y});
        int liberties = 0;
        int dirs[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
        for (auto& d : dirs) {
            liberties += countLiberties(x + d[0], y + d[1], color, visited);
        }
        return liberties;
    }

    // 移除无气的棋子块
    void removeDeadGroup(int x, int y, int color) {
        std::set<Point> group;
        if (countLiberties(x, y, color, group) == 0) {
            for (auto& p : group) {
                board[p.x][p.y] = 0; // 提子
            }
            if (!group.empty()) notifyMessage("提吃 " + std::to_string(group.size()) + " 子");
        }
    }

public:
    // 构造时注入围棋策略
    GoGame(int s) : AbstractGame(s, std::make_shared<GoMoveStrategy>(), std::make_shared<GoWinStrategy>()) {}
    
    GameType getType() const override { return GameType::GO; }

    void postMoveProcess(int x, int y) override {
        int myColor = board[x][y];
        int opColor = (myColor == 1) ? 2 : 1;
        int dirs[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};

        // 1. 检查四周对手的棋子是否气尽，提子
        for (auto& d : dirs) {
            int nx = x + d[0];
            int ny = y + d[1];
            if (nx >= 0 && nx < size && ny >= 0 && ny < size && board[nx][ny] == opColor) {
                removeDeadGroup(nx, ny, opColor);
            }
        }
        
    }
};

// ==========================================
// 6. 抽象工厂模式 (Abstract Factory)
// ==========================================

// 抽象工厂接口
class IGameFactory {
public:
    // 负责创建游戏实例（在更复杂的场景下，这里还可以创建配套的 Rule, Board 等产品族）
    virtual std::shared_ptr<AbstractGame> createGame(int size) = 0;
    virtual ~IGameFactory() = default;
};

// 具体工厂：五子棋工厂
class GomokuFactory : public IGameFactory {
public:
    std::shared_ptr<AbstractGame> createGame(int size) override {
        // 创建五子棋实例
        return std::make_shared<GomokuGame>(size);
    }
};

// 具体工厂：围棋工厂
class GoFactory : public IGameFactory {
public:
    std::shared_ptr<AbstractGame> createGame(int size) override {
        // 创建围棋实例
        return std::make_shared<GoGame>(size);
    }
};

// ==========================================
// 7. 界面组件与组合模式 (View Layer) - 重构版
// ==========================================

// 1. 抽象组件 (Component)
class UIComponent {
public:
    virtual void draw() = 0;
    virtual void add(std::shared_ptr<UIComponent> c) {
        // 默认实现：叶子节点不支持添加，或者抛出异常
        throw GameException("叶子节点不支持添加子组件");
    }
    virtual ~UIComponent() = default;
};

// 2. 组合节点 (Composite) - 容器
// 它可以包含其他组件（叶子或容器），形成树形结构
class PanelComponent : public UIComponent {
private:
    std::vector<std::shared_ptr<UIComponent>> children;
    std::string name; // 容器名称，方便调试

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

// 3. 叶子节点 - 文本组件 (Leaf)
// 通用的文本显示，可用于标题、提示、状态栏
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

// 4. 叶子节点 - 棋盘组件 (Leaf)
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
                // 这里为了演示，依旧使用了享元工厂，实际项目中建议解耦
                else if (val == 1) std::cout << PieceFactory::getPiece(PieceColor::BLACK)->getSymbol() << " ";
                else if (val == 2) std::cout << PieceFactory::getPiece(PieceColor::WHITE)->getSymbol() << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n"; // 下留白
    }
};

// ==========================================
// 控制台 UI 类 (Observer)
// ==========================================

class ConsoleUI : public IGameObserver {
private:
    // 组合模式的核心：只持有一个根节点即可完成渲染
    std::shared_ptr<UIComponent> rootComponent;

    // 为了能够更新数据，我们仍然需要持有特定组件的引用
    // (注意：这里只是引用，实际对象的所有权在 rootComponent 树中)
    std::shared_ptr<BoardComponent> boardRef;
    std::shared_ptr<TextComponent> hintRef;
    std::shared_ptr<TextComponent> statusRef; // 新增：状态栏引用

public:
    ConsoleUI(std::shared_ptr<UIComponent> root,
              std::shared_ptr<BoardComponent> board,
              std::shared_ptr<TextComponent> hint,
              std::shared_ptr<TextComponent> status)
        : rootComponent(root), boardRef(board), hintRef(hint), statusRef(status) {}

    // --- IGameObserver 实现 ---
    void onBoardUpdate(const std::vector<std::vector<int>>& board, int size) override {
        boardRef->update(board, size);
        //render();
    }

    void onMessage(const std::string& msg) override {
        hintRef->setText("[系统消息] " + msg);
        //render();
    }

    void onGameOver(PieceColor winner) override {
        std::string w = colorToString(winner);
        // 同时更新两个组件
		statusRef->setText("游戏结束 (胜者: " + w + ")");

        //render();
    }
    
    // -------------------------
	void updateGameStatus(const std::string& gameName) {
        std::string display = "当前游戏: <" + gameName + "> ";
        statusRef->setText(display);
    }
    
    void toggleHints() {
        static bool v = true;
        v = !v;
        hintRef->setVisible(v);
        render();
    }

    void render() {
        // 清屏
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        // 组合模式的威力：一键调用，递归绘制整个 UI 树
        if (rootComponent) {
            rootComponent->draw();
        }
        
        std::cout << "请输入指令 (help 查看帮助): ";
    }
};

// ==========================================
// 建造者模式 (Builder) - 组装组合模式的树
// ==========================================

class UIBuilder {
public:
    virtual std::shared_ptr<ConsoleUI> build() = 0;
};

class StandardUIBuilder : public UIBuilder {
public:
    std::shared_ptr<ConsoleUI> build() override {
        // 1. 创建各个叶子组件
        auto title = std::make_shared<TextComponent>("=== 面向对象对战平台 (五子棋/围棋) ===");
        auto separator = std::make_shared<TextComponent>("------------------------------------");
        auto status = std::make_shared<TextComponent>("状态: 准备就绪");
        auto board = std::make_shared<BoardComponent>();
        auto hint = std::make_shared<TextComponent>("欢迎！请输入 start [type] [size] 开始。");
        auto footer = std::make_shared<TextComponent>("------------------------------------");

        // 2. 创建组合容器 (Root Panel)
        auto mainPanel = std::make_shared<PanelComponent>("MainPanel");

        // 3. 组装树形结构 (组合模式的应用)
        // 结构：
        // MainPanel
        //   ├── Title
        //   ├── Separator
        //   ├── Status
        //   ├── Board
        //   ├── Separator
        //   ├── Hint
        //   └── Footer
        
        mainPanel->add(title);
        mainPanel->add(separator);
        mainPanel->add(status); // 状态栏在棋盘上方
        mainPanel->add(board);
        mainPanel->add(separator);
        mainPanel->add(hint);   // 提示在棋盘下方
        mainPanel->add(footer);

        // 4. 返回 UI 控制器，注入根节点和关键节点的引用
        return std::make_shared<ConsoleUI>(mainPanel, board, hint, status);
    }
};

// ==========================================
// 8. 系统控制器 (Facade + Singleton)
// ==========================================

class GameSystem {
private:
    static GameSystem* instance;
    std::shared_ptr<AbstractGame> game;
    std::shared_ptr<ConsoleUI> ui;
    bool running;

    GameSystem() : running(true) {
        StandardUIBuilder builder;
        ui = builder.build();
    }

public:
    static GameSystem* getInstance() {
        if (instance == nullptr) instance = new GameSystem();
        return instance;
    }

    void run() {
        ui->render();
        std::string line;
        while (running && std::getline(std::cin, line)) {
            if (line.empty()) continue;
            processCommand(line);
        }
    }

    void processCommand(const std::string& line) {
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
        
        // 确保命令执行后刷新（如果不是exit）
        if (running && needRender) {
             ui->render();
        }
    }
};

// ==========================================
// 9. 主程序入口
// ==========================================

GameSystem* GameSystem::instance = nullptr;

int main() {
    // 设置本地化以支持中文显示（视控制台环境而定，Windows可能需要chcp 65001）
    std::ios::sync_with_stdio(false);
    
    GameSystem::getInstance()->run();
    return 0;
}
