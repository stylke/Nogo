# 基于Qt5.9.9开发的不围棋(Nogo)小游戏

v1.0版，具备基本PVP,PVE功能，ai算法采用minimax配合alpha-beta剪枝

---
v2.0版，ai部分新增了最基本的MCTS算法。其基本实现函数放在ai.h/ai.cpp的node类中，总逻辑(MCTS函数)放在gamemodel类中