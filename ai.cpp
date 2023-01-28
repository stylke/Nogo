#include "ai.h"

#include <ctime>

extern int LINE_NUM;

void Node::deleteMalloc(Node *node)
{
    for(int i = 0;i < node->children.size();i++){
        deleteMalloc(node->children[i]);
    }
    delete node;
}

double Node::quickEvaluate(Node *node,int AIPLay)
{
    int blackAvaliable = 0;
    int whiteAvalialbe = 0;
    int player = node->player;
    for(int i = 1;i <= LINE_NUM;i++){
        for(int j = 1;j <= LINE_NUM;j++){
            node->player = 1;
            if(isAvailable(node,Point{i,j})) blackAvaliable++;
            node->player = 2;
            if(isAvailable(node,Point{i,j})) whiteAvalialbe++;
        }
    }
    node->player = player;
    if(AIPLay == 1) return (blackAvaliable - whiteAvalialbe) * 1.0 / ((blackAvaliable + whiteAvalialbe)/5+1);
    else return (whiteAvalialbe - blackAvaliable) * 1.0 / ((blackAvaliable + whiteAvalialbe)/5+1);
}

//double Node::quickEvaluate(Node *node,int AIPLay)
//{
//    int blackForbidden = 0;
//    int whiteForbidden = 0;
//    int player = node->player;
//    for(int i = 1;i <= LINE_NUM;i++){
//        for(int j = 1;j <= LINE_NUM;j++){
//            node->player = 1;
//            if(!isAvailable(node,Point{i,j})) blackForbidden++;
//            node->player = 2;
//            if(!isAvailable(node,Point{i,j})) whiteForbidden++;
//        }
//    }
//    node->player = player;
//    if(AIPLay == 1) return whiteForbidden - blackForbidden;
//    else return blackForbidden - whiteForbidden;
//}

bool Node::isAllExpanded(Node *node)
{
    return node->available_points.empty();
}

double Node::defaultPolicy(Node *node,int AIPlay)
{
    /*
    Node * temp = new Node;
    //temp = node;  //不能这样写！！！！！这样temp和node指向的是同一块地址，改变temp也就改变了node
    temp->player = node->player;
    for(int i = 0;i <= LINE_NUM+1;i++){
        for(int j = 0;j <= LINE_NUM;j++){
            temp->board[i][j] = node->board[i][j];
        }
    }
    //用随机下法在当前node棋局状态下模拟一盘游戏
    while(true){
        generateAvailablePoints(temp);
        if(temp->available_points.empty()) break; //没有可下的点了，说明已到了终局，跳出游戏循环
        srand((unsigned)time(NULL));
        int random = rand() % temp->available_points.size();
        Point point = temp->available_points[random];
        temp->board[point.x][point.y] = temp->player;
        temp->player = 3 - temp->player; //交换棋子颜色
    }
    int reward = -1;
    if(temp->player == AIPlay){ //AI输
        reward = 0;
    }else{ //AI赢
        reward = 1;
    }
    delete temp;
    return reward;
    */
    return quickEvaluate(node,AIPlay);
}

void Node::backUp(Node *node, double reward)
{
    if(node == nullptr) return;
    node->N ++;
    node->Q += reward;
    backUp(node->parent,reward);
}

bool Node::isAvailable(Node *node, Point point)
{
    //判断在当前node棋局下，能不能再在point处下一步棋
    if(node->board[point.x][point.y] != 0) return false; //若在当前点有子，肯定不能下
    //重置dfs_visited数组
    for(int i = 1;i <= LINE_NUM;i++){
        for(int j = 1;j <= LINE_NUM;j++){
            node->dfs_visited[i][j] = 0;
        }
    }
    int x = point.x;
    int y = point.y;
    int sides[4][2] = {{-1,0},{1,0},{0,1},{0,-1}}; //表示上下左右四个方向
    //模拟该步落子
    node->board[point.x][point.y] = node->player;
    //判断是否自杀
    if(!DFS(node,point)){
        node->board[point.x][point.y] = 0;
        return false;
    }
    //判断是否将别人气变为0
    for(int s = 0;s < 4;s++){
        Point p{x+sides[s][0],y+sides[s][1]};
        if(node->board[p.x][p.y] != 3-node->player) continue;
        //重置dfs_visited数组
        for(int i = 1;i <= LINE_NUM;i++){
            for(int j = 1;j <= LINE_NUM;j++){
                node->dfs_visited[i][j] = 0;
            }
        }
        if(!DFS(node,p)){
            node->board[point.x][point.y] = 0;
            return false;
        }
    }
    node->board[point.x][point.y] = 0; //在返回前，不要忘了取消模拟的落子
    return true;
}

bool Node::DFS(Node *node, Point point)
{
    int sides[4][2] = {{-1,0},{1,0},{0,1},{0,-1}}; //表示上下左右四个方向
    int x = point.x;
    int y = point.y;
    for(int i = 0;i < 4;i++){
        Point p{x+sides[i][0],y+sides[i][1]};
        //如果周围有在棋盘范围内的空位，则肯定是有气的
        if(node->board[p.x][p.y] == 0 && inBoard(p)){
            return true;
        }
    }
    node->dfs_visited[x][y] = 1;
    for(int i = 0;i < 4;i++){
        Point p{x+sides[i][0],y+sides[i][1]};
        if(node->board[p.x][p.y] == node->board[x][y] && !node->dfs_visited[p.x][p.y]){ //同色且没有被访问过
            if(DFS(node,p)){
                return true;
            }
        }
    }
    return false;
}

bool Node::inBoard(Point point)
{
    if(point.x > 0 && point.x <= LINE_NUM && point.y > 0 && point.y <= LINE_NUM){
        return true;
    }
    return false;
}

bool Node::isGameOver(Node *node)
{
    for(int i = 1;i <= LINE_NUM;i++){
        for(int j = 1;j <= LINE_NUM;j++){
            if(isAvailable(node,Point{i,j})) return true;
        }
    }
    return false;
}

void Node::generateAvailablePoints(Node *node)
{
    node->available_points.clear();
    for(int i = 1;i <= LINE_NUM;i++){
        for(int j = 1;j <= LINE_NUM;j++){
            if(node->board[i][j] != 0) continue;
            Point point{i,j};
            if(isAvailable(node,point)){
                node->available_points.push_back(point);
            }
        }
    }
}


Node *Node::treePolicy(Node *node)
{
    if(node == nullptr) return nullptr;

    Node * best_node = nullptr;
    if(isAllExpanded(node)){ //如果该结点所有可能下法已被遍历完
        best_node = treePolicy(bestChild(node,1));
    }else{
        Point p = node->available_points.front();
        node->available_points.pop_front();
        best_node = expand(node,p);
    }
    return best_node;
}

Node *Node::expand(Node *node,Point point)
{
    Node * new_node = new Node;
    new_node->N = 0;
    new_node->Q = 0;
    new_node->parent = node;
    new_node->children.clear();
    node->children.push_back(new_node);
    new_node->player = 3 - node->player;
    for(int i = 0;i <= LINE_NUM+1;i++){
        for(int j = 0;j <= LINE_NUM+1;j++){
            new_node->board[i][j] = node->board[i][j];
        }
    }
    new_node->board[point.x][point.y] = node->player;
    new_node->generateAvailablePoints(new_node);
    return new_node;
}

Node *Node::bestChild(Node *node,double c)
{
    Node * best_child = nullptr;
    double best_score = -2e20;
    for(int i = 0;i < node->children.size();i++){
        Node * child = node->children[i];
        double score = child->Q*1.0/child->N + c * sqrt(2*log(node->N)/child->N);
        if(score > best_score){
            best_child = child;
            best_score = score;
        }
    }
    return best_child;
}
