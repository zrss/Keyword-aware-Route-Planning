# A straight Forward Solution

使用邻接矩阵找出符合最远距离限制的候选关键词表

k1 -> n1, n2, ..., 

k2 -> nX, nX + 1, ...,

... -> ...

将对应关键词候选节点按评分降序排列

k1 -> nA(8), nB(6), ...

k2 -> nG(3), nH(2), ...

计算剩余得分和最大值

SMAX[i] = SMAX[i + 1] + ... + SMAX[N]，SMAX[N] = kN[0]

递归搜索结果，使用当前最优结果，及剩余最大得分和剪枝

CoSKQ.cpp