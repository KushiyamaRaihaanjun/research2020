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
    //ofstream out;
    int N = atoi(argv[1]); //ノード数
    int M = atoi(argv[2]); //エッジ数
    //ノード番号,宛先ノード番号，確率の順に格納
    int cnt = 0;
    seen.assign(N, false);
    Graph g(N);
    Graph g2(N);
    int dst = N - 1;
    seen.assign(N, false);
    /*
    配列に N*(N-1)/2の数の組み合わせを代入し，シャッフルして
    最初のM個を取る
    dfsで接続性を計算して接続でなかったら最初に戻る
    */
    //mt19937 engine(0);
    //for (int i = 0; i < N; i++)
    //{
    //    for (int j = i + 1; j < N; j++)
    //    {
    //        double cost = rnd.randDoubleRange(0.5, 0.8);
    //        g[i].push_back(Edge(j, cost));
    //    }
    //}
    map<pair<int, int>, int> nodeval; //fromとtoのペアを保持
                                      // | ios::trunc);
    //ofstream out("topology.txt");
    while (1)
    {
        ofstream out("topology.txt");
        for (int i = 0; i <= M; i++)
        {
            int from = rnd(N);
            int to = rnd(N);
            double rate = rnd.randDoubleRange(0.5, 0.8);
            if (from < to && nodeval[{from, to}] == 0 && !(from == 0 && to == dst))
            {
                g2[from].push_back(Edge(to, rate));
                out << from << " " << to << " " << rate << endl;
                nodeval[{from, to}]++;
            }
            else
            {
                if (i >= 1)
                {
                    i--;
                }
            }
        }
        //shuffle(g.begin(), g.end(), engine);        //shuffleする
        //copy(g.begin(), g.begin() + M, g2.begin()); //g2(サイズMにコピー)
        //if (!g2.empty())
        //{
        dfs(g2, 0); //到達性チェック

        //}
        if (nodeval.size() > 0)
        {
            nodeval.clear();
        }
        if (g2.size() > 0)
        {
            g2.clear();
        }
        if (seen[dst] == true)
        {
            while (!bf_que.empty())
            {
                bf_que.pop();
            }
            vector<int> hopcheck = bfs(g2, N); //最大ホップ数かをチェック
            sort(hopcheck.begin(), hopcheck.end());
            int maxhop = hopcheck[hopcheck.size() - 1];
            if (hopcheck[dst] == maxhop)
            {
                out.close();
                break;
            }
            else
            {
                out.close();
                seen.assign(N, false);
            }
        }
        else
        {
            out.close();
            seen.assign(N, false);
        }
    }

    //ファイル書き込み
    /*double cost = rnd.randDoubleRange(0.5, 0.8); //通信成功確率
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
    */
    return 0;
}