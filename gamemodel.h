#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <QVector>
#include <QQueue>
#include <vector>
using std::vector;

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


struct EvaluateValue{
    int lx; //落子点横坐标
    int ly; //落子点纵坐标
    double value; //棋形评估分
};

struct Point{
    int x;
    int y;
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

class Node
{
    //用于MCTS的类
public:
    int board[20][20]; //棋盘状态
    int N; //该结点被模拟总次数
    int Q; //该结点总收益值
    Node * parent; //父结点指针
    QVector<Node *> children; //子结点指针容器
    int player; //在该结点目前棋盘状态下，下一步要落子的棋子颜色
    QVector<Point> available_points; //在该结点目前棋盘状态下，下一步棋子可以落的点（不会让“气”变为0）的集合
    int dfs_visited[20][20]; //用于dfs搜索时，存储点是否被访问的数组

public:
    //MCTS骨干函数
    Node * treePolicy(Node * node); //查找并返回本次模拟的初始结点
        Node * expand(Node * node,Point point); //在当前结点的所有可能下法未遍历完(available_points不为空)时，选择一种下法，
                              //扩展出一个结点，并在本次以它为模拟初始结点。注意，选择完一种下法后，要将其从available_points中去除
        Node * bestChild(Node * node,double c);  //在当前结点的所有可能下法遍历完时，用UCB算出最优子结点，递归。c是公式中的权重
    int defaultPolicy(Node * node,int ALPlay); //以node为初始结点，开始随机模拟，并返回reward
    void backUp(Node * node,int reward); //从模拟初始结点开始一直回溯到根结点，更新N值和Q值
    //辅助函数
    bool isAllExpanded(Node * node); //判断一个结点是否已遍历完所有可能下法
    bool isAvailable(Node * node,Point point); //判断在当前棋盘状态下，能不能下point这个点
    bool DFS(Node * node,Point point); //用DFS算法判断在当前棋盘状态下，点point与其周边同色棋子形成的整体的气是否为0
    bool inBoard(Point point); //判断给定点是否在棋盘范围内
    bool isGameOver(Node * node); //判断游戏是否结束，即没有available_points，用于结束模拟
    void generateAvailablePoints(Node * node); //新创建一个结点时，生成该结点的available_points集合
    void deleteMalloc(Node * node); //完成一次MCTS并做出最后决策后，递归删除整个结点树，防止内存泄露
};

#endif // GAMEMODEL_H
