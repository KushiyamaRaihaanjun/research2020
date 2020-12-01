#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <string>
#include <map>
#include <bitset>
#include <stack>
#include <queue>
#include "Rand.h"
using namespace std;

//制約条件
//dfsで0とN-1が接続になっているか調べデータセットを生成する
struct Edge
{
    int to;
    double tsuccess_rate;
    Edge(int t, double rate) : to(t), tsuccess_rate(rate){};
};
using Graph = vector<vector<Edge>>;
vector<bool> seen;
void dfs(const Graph &gr, int ver)
{
    seen[ver] = true;
    for (auto next_ver : gr[ver])
    {
        if (seen[next_ver.to])
        {
            continue;
        }
        dfs(gr, next_ver.to);
    }
}

int main(int argc, char *argv[])
{
    //ofstream out;
    int N = atoi(argv[1]); //ノード数
    int M = atoi(argv[2]); //エッジ数
    //ノード番号,宛先ノード番号，確率の順に格納
    int cnt = 0;
    seen.assign(N, false);
    Graph g(N);
    int dst = N - 1;
    while (seen[dst] != true)
    {
        ofstream out("topology.txt");
        while (cnt < M)
        {
            double cost = rnd.randDoubleRange(0.5, 0.8); //通信成功確率
            int from = rnd(N);                           //0〜N-1の乱数(一様分布)
            int to = rnd(N);                             //0〜N-1の乱数(一様分布)
            if (from < to)
            {
                g[from].push_back(Edge(to, cost));
            }
            if (g.size() > 0)
            {
                dfs(g, 0);
            }
            if (from < to)
            {
                out << from << " " << to << " " << cost << endl;
                cnt++;
            }
            seen.assign(N, false);
        }
        cnt = 0;
        if (g.size() > 0)
        {
            g.clear();
        }
    }
    return 0;
}