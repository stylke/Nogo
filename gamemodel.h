#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QVector>
#include <QQueue>

//游戏类型
enum GameType{
    PVP = 1,
    PVE,
};

//游戏状态
enum GameState{
    Playing = 1,
    GameOver,
    Suspension,
};

enum Player{
    Black = 1,
    White,
};  //表示黑子和白子


//教训：常量可用const int在头文件里定义，如果是在多个文件里要用的变量，不要在头文件里定义！应在一个cpp文件用
//     "int p = v;"定义，在其他要用这个变量的cpp文件中用"extern int p;"声明
const int MARGIN = 50; //棋盘距边界的距离
const int LINE_DIST = 50; //棋盘格线之间的距离
const int CHESS_DIAM = 30; //棋子的直径
const int DOT_DIAM = 10; //提示点的直径
const int SELECTED_DIAM = 30; //触发提示点的范围直径
const double MCTS_TIME_LIMIT = 1.95; //MCTS搜索限制时间

struct EvaluateValue{
    int lx; //落子点横坐标
    int ly; //落子点纵坐标
    double value; //棋形评估分
};


class GameModel
{
private:
    int board[20][20]; //存储棋盘状态，1-LINE_NUM行和列为棋盘区域，0表示无子，1表示黑子，2表示白子,3表示边界外
    int qi[20][20]; //存储棋子的“气”;
    GameType type;
    GameState state;
    Player player;
public:
    GameModel();
    ~GameModel();
    void updateQi(int lx,int ly,int player);

    void checkWin(int player);
    void onWin(int player);

    //提供访问与修改私有成员的接口
    GameType getType(){return type;};
    GameState getState(){return state;};
    Player getPlayer(){return player;};
    int getBoard(int lx,int ly){return board[lx][ly];};
    int getQi(int lx,int ly){return qi[lx][ly];};
    void changeBoard(int lx,int ly,int value){board[lx][ly] = value;};
    void changePlayer(Player p){player = p;};
    void changeGameType(GameType gt){type = gt;};
    void changeGameState(GameState gs){state = gs;};

    //AI评估
    int evaluateBoard(Player AIPlay);
    bool isAbleToPlaceChess(int lx,int ly,int player);
    EvaluateValue miniMax(int alpha,int beta,int depth,int maxDepth,Player AIPlay);

    EvaluateValue MCTS(Player AIPlay);

};


#endif // GAMEMODEL_H
