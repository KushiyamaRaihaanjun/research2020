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
    /*
    方針3
    ホップ数を5~10で用意して，ホップ数の配置を考える
    */
    int mx_hop = rnd(7, 10);                 //最大Hop数(6~9)
    vector<vector<int>> nodes_array(mx_hop); //ホップ数，そのホップ数におけるノード番号
    //nodes_array.resize(mx_hop);               //サイズをホップ数にする
    nodes_array[0].push_back(0);              //0ホップは0
    nodes_array[mx_hop - 1].push_back(N - 1); //最終ホップはN-1
    map<int, int> node_map;
    int nodes_array_total_size = 0;
    while (1)
    {
        for (int i = 1; i < mx_hop - 1; i++)
        {
            int hop_node_number = rnd(6, 12);       //n hopのノード数
            nodes_array[i].resize(hop_node_number); //ノード数にリサイズする
            nodes_array_total_size += hop_node_number;
        }
        if (nodes_array_total_size != N - 2)
        {
            nodes_array_total_size = 0;
            continue;
        }
        for (int i = 1; i < mx_hop - 1; i++)
        {
            int j = 0;
            while (j < nodes_array[i].size())
            {
                int node_num = rnd(1, dst - 1); //ノード番号を生成
                if (node_map.find(node_num) == node_map.end())
                {
                    nodes_array[i][j] = node_num;
                    node_map[node_num]++;
                    j++;
                }
            }
            //rest_node_num -= hop_node_number; //ノード数だけ減らす
            if (node_map.size() == N - 2)
            {
                break;
            }
        }
        //終了条件はジャスト
        if (node_map.size() == N - 2)
        {
            break;
        }
        else
        {
            node_map.clear();
            for (int i = 1; i < mx_hop - 1; i++)
            {
                nodes_array[i].clear();
            }
        }
    }
    map<pair<int, int>, int> nodeval; //fromとtoのペアを保持
    int f_number = 0;
    string f_name = "topology";
    f_name += to_string(f_number);
    f_name += ".txt";
    while (1)
    {
        ofstream out("topology1.txt");
        for (int i = 1; i < mx_hop; i++) //mx_hop-1..dstのみ
        {
            //loop開始(エッジ数)
            if (i <= mx_hop - 2)
            {
                for (int j = 0; j < 2 * nodes_array[i].size(); j++)
                {
                    int from = nodes_array[i - 1][rnd(nodes_array[i - 1].size())]; //from
                    int to = nodes_array[i][rnd(nodes_array[i].size())];           //to
                    double rate = rnd.randDoubleRange(0.5, 0.8);                   //通信成功率
                    if (nodeval[{from, to}] == 0 && from != to)                    //まだ接続していないノードのみ接続する
                    {
                        g[from].push_back(Edge(to, rate));
                        out << from << " " << to << " " << rate << endl;
                        nodeval[{from, to}]++;
                    }
                }
            }
            else //i==mxhop-1のときのみの処理
            {
                for (int j = 0; j < nodes_array[i - 1].size(); j++)
                {
                    int from = nodes_array[i - 1][j];
                    int to = dst;
                    double rate = rnd.randDoubleRange(0.5, 0.8);
                    g[from].push_back(Edge(to, rate));
                    out << from << " " << to << " " << rate << endl;
                }
            }
            //loop終
        }
        //dfsで接続性チェック
        dfs(g, 0);
        nodeval.clear();
        g.clear();
        if (seen[dst] == true)
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

    /*while (1)
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
        dfs(g2, 0); //到達性チェック
        nodeval.clear();
        g2.clear();
        if (seen[dst] == true)
        {
            out.close();
            break; //終了
        }
        else
        {
            out.close();
            seen.assign(N, false); //seenをリセット
        }
    }
    */
    //ファイル書き込み

    return 0;
}