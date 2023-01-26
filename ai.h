#ifndef NODE_H
#define NODE_H

#include <QVector>

struct Point{
    int x;
    int y;
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

#endif // NODE_H
