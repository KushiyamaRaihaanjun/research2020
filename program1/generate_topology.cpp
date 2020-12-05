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
    //ofstream out;
    int N = atoi(argv[1]); //ノード数
    int M = atoi(argv[2]);
    //int M = atoi(argv[2]); //エッジ数
    //ノード番号,宛先ノード番号，確率の順に格納
    int cnt = 0;
    seen.assign(N, false);
    Graph g(N);
    Graph g2(N);
    int dst = N - 1;
    /*
    方針1
    配列に N*(N-1)/2の数の組み合わせを代入し，シャッフルして
    最初のM個を取る
    dfsで接続性を計算して接続でなかったら最初に戻る
    */
    /*
    方針2
    送信元と宛先の条件から乱数を使って生成
    */
    map<pair<int, int>, int> nodeval; //fromとtoのペアを保持
                                      // | ios::trunc);
    //ofstream out("topology.txt");
    while (1)
    {
        int one_hop_count = 0; //送信元から1hopのノードの数
        ofstream out("topology.txt");
        for (int i = 0; i < M; i++)
        {
            int from = rnd(N);
            int to = rnd(N);
            double rate = rnd.randDoubleRange(0.5, 0.8);
            if (from < to && nodeval[{from, to}] == 0 && !(from == 0 && to == dst))
            {
                if (from == 0 && one_hop_count < 10)
                {
                    g2[from].push_back(Edge(to, rate));
                    out << from << " " << to << " " << rate << endl;
                    nodeval[{from, to}]++;
                    one_hop_count++;
                }
                else if (from > 0 && nodeval[{0, from}] == 0) //fromが送信元と接続がない場合
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
            else
            {
                if (i >= 1)
                {
                    i--;
                }
            }
        }
        //if (!g2.empty())
        //{
        dfs(g2, 0); //到達性チェック
        //}
        nodeval.clear();
        g2.clear();
        if (seen[dst] == true)
        {
            //vector<int> hopcheck = bfs(g2, N); //最大ホップ数かをチェック
            //map<int, int> cnthop;              //ホップ数の出現回数をカウント
            //for (int i = 0; i < hopcheck.size(); i++)
            //{
            //    cnthop[hopcheck[i]]++;
            //}
            //vector<int> tmp_hopcheck = bfs(g2, N);
            //tmp_hopcheck = hopcheck;
            //sort(tmp_hopcheck.begin(), tmp_hopcheck.end());
            //ホップ数の最大値を調べる
            //int maxhop = tmp_hopcheck[tmp_hopcheck.size() - 1];
            //if (hopcheck[dst] == maxhop) //宛先までが最深のHop数の時
            //{
            out.close();
            break; //終了
            //}
            //else
            //{
            //    out.close();
            //    seen.assign(N, false); //seenをリセット
            //}
            //while (!bf_que.empty())
            //{
            //    bf_que.pop();
            //}
            //tmp_hopcheck.clear();
        }
        else
        {
            out.close();
            seen.assign(N, false); //seenをリセット
        }
    }
    //shuffle(g.begin(), g.end(), engine);        //shuffleする
    //copy(g.begin(), g.begin() + M, g2.begin()); //g2(サイズMにコピー)
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