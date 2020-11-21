#include <stdio.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <string>
#include <map>
#include <bitset>
#include <stack>
#include <queue>
#include "Rand.h"
//#include <bits/stdc++.h>
using namespace std;
typedef long long int lli;
#define urept(soeji, start, n) for (int soeji = start; soeji < n; soeji++)
#define drept(soeji, start, n) for (int soeji = start; soeji > n; soeji--)

const int N = 5; // number of nodes (observer)
//ノードのリンク情報(通信成功率等)を追加(初めは固定値)
double constant_suc_rate = 0.8;
double threshold = 0.6; // threshold
const int numberofpackets = 10;
struct Edge
{
    int to;
    double tsuccess_rate;
    Edge(int t, double rate) : to(t), tsuccess_rate(rate){};
};
using Graph = vector<vector<Edge>>;

struct Node
{
    //alpha...number of packets successfully received
    //beta .. all of packets transmitted

    //x,y...座標
    double x, y;
    uint32_t alpha;
    uint32_t beta;
    bool sendmap[numberofpackets];
    bool recvmap[numberofpackets];
    queue<int> q;
    int state;
    /*0(emptyset)
    1(trustee)
    2(untrustee)
    3(uncertain)*/
    double dtv;
    double itv;
};

struct ONode
{
    //alpha...number of packets successfully received
    //beta .. all of packets transmitted
    uint32_t alpha[1000];
    uint32_t beta[1000];
    double dsarray[1000][4];
    int state;
    /*0(emptyset)
    1(trustee)
    2(untrustee)
    3(uncertain)*/
    double dtv[1000];
    double itv;

    void set_itv(Node y[])
    {
        for (int i = 0; i < N; i++)
        {
            if (y[i].dtv > threshold && dtv[i] > threshold)
            {
                dsarray[i][1] = y[i].dtv;
                dsarray[i][2] = 0.0;
                dsarray[i][3] = 1.0 - y[i].dtv;
            }
            else if (y[i].dtv > threshold && dtv[i] <= threshold)
            {
                dsarray[i][1] = 0.0;
                dsarray[i][2] = y[i].dtv;
                dsarray[i][3] = 1.0 - y[i].dtv;
            }
            else
            {
                dsarray[i][1] = 0.0;
                dsarray[i][2] = 0.0;
                dsarray[i][3] = 1.0;
            }
        }
    }
};

void edge_set(Node x)
{
}

//三進法変換
int threearray[18];
void xtothree(int x)
{
    int power3 = 1;
    for (int i = 0; i < 18; i++)
    {
        threearray[i] = (x / power3) % 3;
        power3 *= 3;
    }
}
//乱数
/*uint32_t get_rand_range(int seed, uint32_t min_val, uint32_t max_val)
{
    // 乱数生成器
    static std::mt19937 mt32(seed);

    // [min_val, max_val] の一様分布整数 (int) の分布生成器
    std::uniform_int_distribution<uint32_t> get_rand_uni_int(min_val, max_val);

    // 乱数を生成
    return get_rand_uni_int(mt32);
}*/

double ds_trust(ONode x)
{
    /*bitset か，bit 全探索*/
    /*HHH ,HHU ... などの列挙をやる*/
    /*U を0 に，H を1 に対応させる*/
    double val = 1.0;
    double val2 = 0.0;
    for (int i = 0; i < (1 << N); i++)
    {
        bitset<N> state(i);
        if (i != 0)
        {
            for (int j = 0; j < N; j++)
            {
                if (state[j] == 0)
                {
                    val *= x.dsarray[j][state[j] + 3];
                }
                else
                {
                    val *= x.dsarray[j][state[j]];
                }
            }
            val2 += val;
        }

        val = 1.0;
    }
    return val2;
}

double ds_all(ONode x)
{
    double all_val = 0.0;
    double val = 1.0;
    double val2 = 0.0;
    //変更する
    //3 進数
    /*
    H...1
    not H...2
    U...3
    */
    //3 ^ N の全列挙をやる
    map<int, int> setcount;
    for (int i = 0; i < (int)(pow(3, N)); i++)
    {
        fill(threearray, threearray + 18, 0);
        xtothree(i);
        for (int j = 0; j < N; j++)
        {
            setcount[threearray[j] + 1]++;
        }
        if (setcount[1] > 0 && setcount[2] > 0)
        {
            //do nothing
        }
        else
        {
            for (int j = 0; j < N; j++)
            {
                val *= x.dsarray[j][threearray[j] + 1];
            }
            val2 += val;
        }
        val = 1.0;
        setcount.clear();
    }
    return val2;
}

void set_dtv()
{
}

//node_num:送信元
//num_edge.to:宛先
void broadcast(const Graph &gr, Node n[], int node_num, int p)
{
    //ブロードキャスト操作
    //edgeのあるノードにブロードキャストする
    //edgeのあるノードのrecvmapをリンクの確率でtrueにする
    //recvmapがtrueなら対象のパケットをqueueに入れる
    if (node_num == 0) //送信元
    {
        for (auto num_edge : gr[node_num]) //num_edge...接続しているエッジ
        {
            if (rnd.randBool(num_edge.tsuccess_rate))
            {
                n[node_num].sendmap[p] = true;
                n[num_edge.to].recvmap[p] = true; //toのrecvmapを更新
                n[num_edge.to].q.push(p);         //toのキューにパケットをプッシュ
                cout << "Node " << num_edge.to << " received packet " << p << " from Node " << node_num << endl;
            }
            else //失敗処理
            {
                n[node_num].sendmap[p] |= false;   //orにする
                n[num_edge.to].recvmap[p] = false; //
                cout << "Node " << num_edge.to << " dropped packet " << p << " ((from  Node " << node_num << endl;
            }
            //cout << num_edge.to << " ";
        }
    }
    else
    {
        for (auto num_edge : gr[node_num]) //num_edge...接続しているエッジ
        {
            while (!n[node_num].q.empty())
            {
                if (rnd.randBool(num_edge.tsuccess_rate))
                {
                    n[node_num].sendmap[n[node_num].q.front()] = true;    //送信マップをtrue
                    n[num_edge.to].recvmap[n[node_num].q.front()] = true; //キューの先頭をtrue
                    n[num_edge.to].q.push(n[node_num].q.front());
                    cout << "Node " << num_edge.to << " received packet " << n[node_num].q.front() << " from Node " << node_num << endl;
                    n[node_num].q.pop();
                }
                else
                {
                    n[node_num].sendmap[n[node_num].q.front()] = false;     //送信マップをfalse
                    n[num_edge.to].recvmap[n[node_num].q.front()] |= false; //キューの先頭をfalse
                    cout << "Node " << num_edge.to << " dropped packet " << n[node_num].q.front() << " ((from Node " << node_num << endl;
                    n[node_num].q.pop();
                }
            }
            /*if (n[node_num].recvmap[p] == true && rnd.randBool(num_edge.tsuccess_rate)) //受信に成功している条件を加える
            {
                n[node_num].sendmap[p] = true; //送信マップをtrue
                n[node_num].q.pop();
                n[num_edge.to].recvmap[p] = true;
                n[num_edge.to].q.push(p);
            }
            else
            {
                n[node_num].sendmap[p] = false;
                n[node_num].q.pop();
            }*/

            //cout << num_edge.to << " ";
        }
    }
    //cout << endl;
}

//Input:設定したパラメータ
//Output:値を関数に渡す？
void simulate()
{
    //broadcast(g, n[0], x, p);
}

void simulate_end()
{
}

int main(void)
{
    //int inputseed;
    //cout << "input seed: ";
    //ノードの位置を入力(あとで？)
    //ひとまずは考えない（手動でノードを接続）
    //接続情報を入力
    Graph g(N);
    int numberofedges = 8;
    //for (int i = 0; i < numberofedges; i++)
    //{
    //}

    Node node[5];
    //ノード番号，通信成功率の組
    g[0].push_back(Edge(1, 0.8));
    g[0].push_back(Edge(2, 0.8));
    g[0].push_back(Edge(3, 0.8));
    g[1].push_back(Edge(2, 0.8));
    g[1].push_back(Edge(4, 0.8));
    g[2].push_back(Edge(3, 0.8));
    g[2].push_back(Edge(4, 0.8));
    g[3].push_back(Edge(4, 0.8));

    //攻撃ノードの情報を追加
    //パケットはuID指定
    int packet[numberofpackets];
    for (int i = 0; i < numberofpackets; i++)
    {
        packet[i] = i + 1;
    }

    //for (int i = 0; i < N - 1; i++)
    //{
    //    for (int j = 0; j < numberofpackets; j++)
    //    {
    //        broadcast(g, node, i, j);
    //    }
    //}
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < numberofpackets; j++)
        {
            node[i].sendmap[j] = false;
            node[i].sendmap[j] = false;
        }
    }
    for (int i = 0; i < numberofpackets; i++)
    {
        broadcast(g, node, 0, i);
    }
    for (int i = 1; i <= 3; i++)
    {
        broadcast(g, node, i, 0);
    }
    cout << "Node0 Node1 Node2 Node3 Node4" << endl;
    for (int i = 0; i < numberofpackets; i++)
    {
        cout << i << " ";
        if (node[0].sendmap[i])
        {
            cout << "true ";
        }
        else
        {
            cout << "false ";
        }
        if (node[1].recvmap[i])
        {
            cout << "true ";
        }
        else
        {
            cout << "false ";
        }
        if (node[2].recvmap[i])
        {
            cout << "true ";
        }
        else
        {
            cout << "false ";
        }
        if (node[3].recvmap[i])
        {
            cout << "true ";
        }
        else
        {
            cout << "false ";
        }
        if (node[4].recvmap[i])
        {
            cout << "true ";
        }
        else
        {
            cout << "false ";
        }
        cout << endl;
    }
    //経路情報はvectorで管理
    vector<vector<int>>
        route;
    //シミュレーションを行う
    simulate();
    //終了処理
    simulate_end();
    //結果はcsv等に保存？
    string result = "xxx.csv";
    return 0;
}