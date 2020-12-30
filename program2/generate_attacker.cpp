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

//深さ優先探索
vector<bool> seen;
void dfs(Graph &gr, int ver)
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

//深さ優先探索その2
/*bool dfs2(int state, int goal, Graph &gr)
{
    if (state == goal)
    {
        return true;
    }
    for (auto next : gr[state])
    {
        if (dfs2(next.to, goal, gr) == true)
        {
            return true;
        };
    }
    return false;
}*/

//幅優先探索
//Hop数をトポロジから事前計算

queue<int> bf_que;
vector<int> bfs(const Graph &gr, int n)
{
    vector<int> bf_dist(n, -1); // 全頂点を「未訪問」に初期化
    // 初期条件 (頂点 0 を初期ノードとする)
    bf_dist[0] = 0;
    bf_que.push(0); // 0 を橙色頂点にする

    // BFS 開始 (キューが空になるまで探索を行う)
    while (!bf_que.empty())
    {
        int v = bf_que.front(); // キューから先頭頂点を取り出す
        bf_que.pop();

        // v から辿れる頂点をすべて調べる
        for (auto nv : gr[v])
        {
            if (bf_dist[nv.to] != -1)
                continue; // すでに発見済みの頂点は探索しない

            // 新たな白色頂点 nv について距離情報を更新してキューに追加する
            bf_dist[nv.to] = bf_dist[v] + 1;
            bf_que.push(nv.to);
        }
    }
    return bf_dist;
}

int main(int argc, char *argv[])
{
    int N = atoi(argv[1]);
    int d = N - 1;
    int number_of_malnodes = atoi(argv[2]);
    map<int, int> nd;
    string fname = "attacker-";
    fname += to_string(number_of_malnodes);
    fname += ".txt";
    //配列:1~48
    vector<int> arr(d - 1);
    for (int i = 0; i < d - 1; i++)
    {
        arr[i] = i + 1;
    }
    mt19937_64 mt_r(rnd());
    shuffle(arr.begin(), arr.end(), mt_r);
    ofstream ofs(fname);
    for (int i = 0; i < number_of_malnodes; i++)
    {
        ofs << arr[i] << endl;
    }
    ofs.close();
    return 0;
}