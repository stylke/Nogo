# 基于Qt5.9.9开发的不围棋(Nogo)小游戏

v1.0版，具备基本PVP,PVE功能，ai算法采用minimax配合alpha-beta剪枝

---
v2.0版，ai部分新增了最基本的MCTS算法。其基本实现函数放在ai.h/ai.cpp的node类中，总逻辑(MCTS函数)放在gamemodel类中

v2.1版，修改了MCTS的defaultPolicy部分，由2.0版的随机落子至终局改为直接返回当前棋局的quickEvaluate值。quickEvaluate采取的算法是计算黑棋不能下的点数和白棋不能下的点数，返回两者之差（根据AI执子颜色确定是谁减去谁，保证返回值越大，对AI越有利）。
实践表明，在与玩家对战时，2.1版表现明显优于2.0版。