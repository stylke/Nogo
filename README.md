# 基于Qt5.9.9开发的不围棋(Nogo)小游戏

v1.0版，具备基本PVP,PVE功能，ai算法采用minimax配合alpha-beta剪枝

---
v2.0版，ai部分新增了最基本的MCTS算法。其基本实现函数放在ai.h/ai.cpp的node类中，总逻辑(MCTS函数)放在gamemodel类中

v2.1版，修改了MCTS的defaultPolicy部分，由2.0版的随机落子至终局改为直接返回当前棋局的quickEvaluate值。quickEvaluate采取的算法是计算黑棋不能下的点数和白棋不能下的点数，返回两者之差（根据AI执子颜色确定是谁减去谁，保证返回值越大，对AI越有利）。
实践表明，在与玩家对战时，2.1版表现明显优于2.0版。

v2.2版，
 - 修改了quickEvaluate算法：记x为ai能下的点数，y为玩家能下的点数，则返回值为 $(x-y)/[(x+y)/5+1]$。 可减少前期贪心所占比重。
 - 新增局势评估功能，以折线图形式呈现。横轴代表步数，纵轴代表两种棋子不能下的点数之差。

---
###nogo_for_botzone
在[botzone](https://www.botzone.org.cn/)平台上创建bot时用的代码，可供测试本项目算法在botzone平台的表现，如果是想参考算法代码的，不要参考该文件里的（这个文件纯粹是为了让它在botzone上能用就行）。 在botzone上创建bot时，直接复制文件夹中的main.cpp即可，编译器选G++ 7.2.0，一定要勾选使用简单交互。
对于想修改main.cpp的,建议：
 - 要修改MCTS限制时间，修改第8行的值即可
 - 要从MCTS算法切换到minimax算法，注释掉73、77行，去掉72、76行的注释即可
 - 要修改MCTS算法中结点的Q值计算的，关注defaultPolicy函数和quickEvaluate函数即可
 - 要改变UCB公式(在bestChild函数中)中常数c的，修改treePolicy函数中调用bestChild时的第二个参数即可
 - 要改变minimax搜索深度的，修改72、76行调用miniMax的第4个参数即可(建议搜索深度为2或3)