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

#define INF 1e30
const int N = 7; // number of nodes (observer)
const int d = N - 1;
//ノードのリンク情報(通信成功率等)を追加(初めは固定値)
double constant_suc_rate = 0.8;
double threshold = 0.6; // trust value threshold
const int numberofpackets = 10;
double tmpetx = 0.0;  //etx計算用
vector<bool> seen;    // 到達可能かどうかを調べる
vector<bool> checked; // 送信元から1hopノードが送信しているか
vector<double> cs;    //宛先までのetxを求めるための配列
struct Edge
{
    int to;
    double tsuccess_rate;
    Edge(int t, double rate) : to(t), tsuccess_rate(rate){};
};
using Graph = vector<vector<Edge>>; //グラフ型
using P = pair<double, int>;        //ETX,ノード番号のペア

priority_queue<P, vector<P>, greater<P>> pq_onehop_fromsource; //1hopノードの優先度付きキュー
priority_queue<P, vector<P>, greater<P>> pq_intermediate[N];   //各ノードの優先度付きキュー
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
//深さ優先探索
//etx値を一つに決めなければならない？
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

//幅優先探索
//Hop数をトポロジから事前計算
vector<int> bf_dist(N, -1); // 全頂点を「未訪問」に初期化
queue<int> bf_que;
void bfs(const Graph &gr)
{

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
}

//ダイクストラ法
void dijkstra_etx(const Graph &gr, int s, vector<double> &dis)
{
    dis.resize(N, INF);
    priority_queue<P, vector<P>, greater<P>> pq; // 「仮の最短距離, 頂点」が小さい順に並ぶ
    dis[s] = 0;
    pq.emplace(dis[s], s);
    while (!pq.empty())
    {
        P p = pq.top();
        pq.pop();
        int v = p.second;
        if (dis[v] < p.first)
        { // 最短距離で無ければ無視
            continue;
        }
        //to do: 計算誤差を考える
        for (auto &e : gr[v])
        {
            if (dis[e.to] > dis[v] + (1.0 / e.tsuccess_rate))
            { // 最短距離候補なら priority_queue に追加
                dis[e.to] = dis[v] + (1.0 / e.tsuccess_rate);
                pq.emplace(dis[e.to], e.to);
            }
        }
    }
}

void Decidepriorityfromsource(const Graph &gr, Node n[], int node_num, int dst)
{
    //edgeに対するfor
    for (auto num_edge : gr[node_num])
    {
        //すべてのnum_edge.toに対して宛先へ到達できるかをdfsを使って探索
        //1/pを足し上げてETXを算出
        //dst～num_edge.toまでのETXを計算する
        //ここで深さ優先探索が使えそう
        //パケットの数だけキューにETXの組をpushしそう
        dfs(gr, num_edge.to);
        if (seen[dst] == true) //到達可能の場合
        {
            //ETXを使って優先度キューに入れるようなことをしたい
            double to_etx = 0.0;
            dijkstra_etx(gr, num_edge.to, cs);                 //num_edge.to から宛先までのetxを求めている
            to_etx = (1.0 / num_edge.tsuccess_rate) + cs[dst]; //sourceから宛先へのetx
            cout << "Node " << num_edge.to << " :ETX = " << to_etx << endl;
            pq_onehop_fromsource.emplace(to_etx, num_edge.to);
        }
        else
        {
            //到達不可能
        }
        seen.assign(N, false); //seenをリセット
    }
    //end for
}

//to do: 計算結果を再利用したい
//2 hop以降の優先度を選択する
void DecidePriorityIntermediate(const Graph &gr, Node n[], int hop_num, int dst)
{
    //優先度決定を関数化したい(後で)
    //edgeに対するfor
    //node_numの部分をHop数に置き換える
    for (int i = 1; i < N; i++)
    {
        if (bf_dist[i] == hop_num) //Hop数のノードについて
        {
            cout << i << endl;
            for (auto num_edge : gr[i])
            {
                //すべてのnum_edge.toに対して宛先へ到達できるかをdfsを使って探索
                //1/pを足し上げてETXを算出
                //dst～num_edge.toまでのETXを計算する
                //ここで深さ優先探索が使えそう
                //パケットの数だけキューにETXの組をpushしそう
                dfs(gr, num_edge.to);
                if (seen[dst] == true) //到達可能の場合
                {
                    //ETXを使って優先度キューに入れるようなことをしたい
                    double to_etx = 0.0;
                    dijkstra_etx(gr, num_edge.to, cs);                 //num_edge.to から宛先までのetxを求めている
                    to_etx = (1.0 / num_edge.tsuccess_rate) + cs[dst]; //source(送信元ではない)から宛先へのetx
                    cout << "Node " << num_edge.to << " :ETX = " << to_etx << endl;
                    pq_intermediate[i].emplace(to_etx, num_edge.to);
                }
                else
                {
                    //到達不可能
                }
                seen.assign(N, false); //seenをリセット
                cs.assign(N, INF);     //csをリセット(ETXを求め直すため)
            }
        }
    }
    //end for
}

//node_num:送信元
//num_edge.to:宛先
void BroadcastFromSource(const Graph &gr, Node n[], int node_num, int p, int dst)
{
    //ブロードキャスト操作
    //edgeのあるノードにブロードキャストする
    //edgeのあるノードのrecvmapをリンクの確率でtrueにする
    //recvmapがtrueなら対象のパケットをqueueに入れる
    //送信元から1ホップの優先度付けは別の関数で行った
    //低いETXのパスを選択するように1ホップノードの優先度を設定する
    if (node_num == 0) //送信元
    {
        //ブロードキャスト
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
        return;
    }
}

//優先度が2番目以下からのノードの送信
//node_num : 送信元ノード番号
//num_edge : 送信先を取得
//tmp_pq_onehop... 自分より優先度が高いノードの番号を取得する

void SendFromlessPrior(Node n[], priority_queue<P, vector<P>, greater<P>> tmp_pq_onehop_fromsource, int node_num, Edge num_edge, queue<int> que)
{
    priority_queue<P, vector<P>, greater<P>> tmp2_pq_onehop_fromsource = tmp_pq_onehop_fromsource;
    while (!que.empty())
    {
        if (tmp2_pq_onehop_fromsource.top().second != node_num) //退避した優先度キューのインデックスを調べる
        {
            //自分より優先度の高いノード番号ごとにループするため，送信優先度のキューを退避
            //自分より優先度が高いノードが送信済みでないか
            //受信に失敗していたと考えられるとき
            //自分より優先度が高いノードが送信に失敗していたとき
            if (n[tmp2_pq_onehop_fromsource.top().second].sendmap[que.front()] == false) // || (n[num_edge.to].recvmap[que.front()] == false && n[tmp2_pq_onehop_fromsource.top().second].sendmap[que.front()] == false))
            {
                //そのパケットを宛先（最終的な宛先とは異なる）が受信していないとき
                if (n[num_edge.to].recvmap[que.front()] == false)
                {
                    //送信成功
                    if (rnd.randBool(num_edge.tsuccess_rate))
                    {
                        //パケットの重複判定をする
                        n[node_num].sendmap[que.front()] = true;    //送信マップをtrue
                        n[num_edge.to].recvmap[que.front()] = true; //受信マップをfalseならtrue
                        //受信成功時のメッセージ
                        cout << "Node " << num_edge.to << " received packet " << que.front() << " from Node " << node_num << endl;
                        n[num_edge.to].q.push(que.front());

                        //to do
                        //エッジを調べる
                        //成功or重複をnode_numに通知
                        //成功or重複をnode_numに通知
                    }
                    else //失敗
                    {
                        n[node_num].sendmap[que.front()] = false;     //送信マップをfalse
                        n[num_edge.to].recvmap[que.front()] |= false; //受信マップをfalse
                        cout << "Node " << num_edge.to << " dropped packet " << que.front() << " ((from Node " << node_num << endl;
                        //to do
                        //エッジを調べる
                        //失敗をnode_numに通知
                        //失敗を周辺ノードに通知
                    }
                }
                else //重複を避けるためパケットをドロップ
                {
                    cout << "Node " << node_num << " Drop packet " << que.front() << " to prevent duplicate (to Node" << num_edge.to << endl;
                }
            } //すでに優先度の高いノードが送信している場合
            else
            {
                cout << "Node " << node_num << " Drop packet " << que.front() << " to prevent duplicate (to Node" << num_edge.to << endl;
            }
            que.pop();
            tmp2_pq_onehop_fromsource.pop(); //パケットについての処理終なのでノード番号を更新する
        }
        else //ノード番号が等しい場合はそのパケットについては調べ終わったのでcontinue
        {
            //cout << "continue : queue " << que.front() << " node num " << tmp2_pq_onehop_fromsource.top().second << endl;
            //que.pop();
            tmp2_pq_onehop_fromsource = tmp_pq_onehop_fromsource; //番号をリセット(priorityqueueをもとに戻す)
        }                                                         //end while for pq

    } //end while for que
}

//優先度が高いノードからの送信
void SendFromHighestPrior(Node n[], int node_num, Edge num_edge, queue<int> que)
{

    while (!que.empty())
    {
        if (rnd.randBool(num_edge.tsuccess_rate))
        {
            //パケットの重複判定をする
            if (n[num_edge.to].recvmap[que.front()] == false) //まだキューの先頭のパケットを受信していない場合
            {
                n[node_num].sendmap[que.front()] = true;    //送信マップをtrue
                n[num_edge.to].recvmap[que.front()] = true; //受信マップをfalseならtrue
                //受信成功時のメッセージ
                cout << "Node " << num_edge.to << " received packet " << que.front() << " from Node " << node_num << endl;
                n[num_edge.to].q.push(que.front());
            }
            else
            {
                //重複時のメッセージ
                cout << "Node " << num_edge.to << " ignoring packet " << que.front() << " due to duplicate" << endl;
            }
            que.pop();
            //to do
            //エッジを調べる
            //成功or重複をnode_numに通知

            //成功or重複をnode_numに通知
        }
        else
        {
            n[node_num].sendmap[que.front()] = false;     //送信マップをfalse
            n[num_edge.to].recvmap[que.front()] |= false; //受信マップをfalse
            cout << "Node " << num_edge.to << " dropped packet " << que.front() << " ((from Node " << node_num << endl;
            que.pop();
            //to do
            //エッジを調べる
            //失敗をnode_numに通知

            //失敗を周辺ノードに通知
        } //end if
    }     //end while
}

void BroadcastFromIntermediatenode(const Graph &gr, Node n[])
{
    //ノードの優先度付けは最初に行った
    //宛先までのETXを計算する
    //配列csからETXを取得する
    //priority_queueからノード番号を取得する
    //node_num->pq_onehop_fromsource.top().second(キューから取得した番号)に変更すれば良い
    //書き換える
    //送信元から1hopとそうでない場合で分ける？
    priority_queue<P, vector<P>, greater<P>> tmp_pq_onehop_fromsource; //優先度キューを退避？
    tmp_pq_onehop_fromsource = pq_onehop_fromsource;
    int one_hop_number = pq_onehop_fromsource.size();
    int cnt = 0; //送信元から1ホップを数えるための変数
    //whileに変える
    while (cnt != one_hop_number) //送信元から1hopノードのブロードキャストが終了しているか
    {
        int highest = pq_onehop_fromsource.top().second; //最も優先度が高いノードのノード番号
        //優先度キューのループ
        while (!pq_onehop_fromsource.empty())
        {
            int node_num = pq_onehop_fromsource.top().second; //ノード番号(優先度順)
            //優先度を表示
            //数字(size)が大きいほど高い優先度
            cout << "Node " << node_num << " priority " << pq_onehop_fromsource.size() << endl;
            for (auto num_edge : gr[node_num]) //num_edge...接続しているエッジ
            {
                //送信先のノードのETXを計算する
                //priority_queueの配列から次の送信ノードを取得
                //実際送信するところ
                queue<int> tmp = n[node_num].q; //キューの中身をいったん退避(ブロードキャストのため)
                if (node_num != highest)        //もっとも優先度の高いノードでない場合
                {
                    //優先度がより低い場合
                    SendFromlessPrior(n, tmp_pq_onehop_fromsource, node_num, num_edge, n[node_num].q);
                }
                else //最も優先度が高い場合
                {
                    //優先度が高いノードから送信
                    SendFromHighestPrior(n, node_num, num_edge, n[node_num].q);
                }                    //end if
                n[node_num].q = tmp; //退避していたキューの中身をもとに戻す
            }                        //end for
            //1hopの転送が完了したかどうかの配列をtrue
            checked[pq_onehop_fromsource.top().second] = true;
            pq_onehop_fromsource.pop();
        }
        cnt++;
    }

    //幅優先探索でノードのホップ数を求めてノードの番号を取得する
    //(送信元から2hop以上)

    //優先度キューのループ
    /*
     * BFSで2hop(nhopでも可)の頂点を取得してループ
     * nhopの場合の送信をやる
     * 
    */
    //1hopを調査済み

    //2hop以上//////
    cnt = 0; //カウントをリセット
    checked.resize(N);
    fill(checked.begin(), checked.end(), false);
    for (int i = 0; i < N; i++)
    {
        if (bf_dist[i] == 0)
        {
            checked[i] = true;
            cnt++;
        }
    }
    checked[N - 1] = true;
    cnt++;
    int now_hopnum = 1;
    //1~N-2を調べるまで
    cout << "2 hop" << endl;
    while (cnt <= N - 1)
    {
        for (int i = 1; i < d; i++)
        {
            //まだ調べていないノードがある場合
            if (bf_dist[i] == now_hopnum && checked[i] == false)
            {
                //int node_num = tmp_pq_onehop_fromsource.top().second; //ノード番号(優先度順)
                //優先度キューを退避
                priority_queue<P, vector<P>, greater<P>> tmp_pq_intermediate = pq_intermediate[i];
                int highest_sev = tmp_pq_intermediate.top().second;
                while (!pq_intermediate[i].empty()) //ソースを中継ノードに置き換える
                {
                    int node_num_sev = pq_intermediate[i].top().second; //ノード番号を取得
                    //優先度を表示
                    //数字(size)が大きいほど高い優先度
                    cout << "Node " << node_num_sev << " priority " << pq_intermediate[i].size() << endl;
                    for (auto num_edge : gr[node_num_sev]) //num_edge...接続しているエッジ
                    {
                        //送信先のノードのETXを計算する
                        //priority_queueの配列から次の送信ノードを取得
                        //実際送信するところ
                        queue<int> tmp = n[node_num_sev].q; //キューの中身をいったん退避(ブロードキャストのため)
                        if (node_num_sev != highest_sev)    //もっとも優先度の高いノードでない場合
                        {
                            //優先度がより低い場合
                            SendFromlessPrior(n, tmp_pq_intermediate, node_num_sev, num_edge, n[node_num_sev].q);
                        }
                        else //最も優先度が高い場合
                        {
                            //優先度が高いノードから送信
                            SendFromHighestPrior(n, node_num_sev, num_edge, n[node_num_sev].q);
                        }                        //end if
                        n[node_num_sev].q = tmp; //退避していたキューの中身をもとに戻す
                    }                            //end for
                    pq_intermediate[i].pop();
                }
                checked[i] = true;
                cnt++;
            }
        }
        now_hopnum++; //調べるHop数を増やす
    }
    /////
}

int GetMaxHop()
{
    vector<int> tmphp;
    tmphp = bf_dist;
    sort(tmphp.begin(), tmphp.end());
    return tmphp[bf_dist.size() - 1];
}

//Input:設定したパラメータ
//Output:値を関数に渡す？
void simulate_with_Tv()
{
    //broadcast(g, n[0], x, p);
}

void simulate_without_Tv()
{
}

//送受信マップのセット
void set_map(Node node[])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < numberofpackets; j++)
        {
            node[i].sendmap[j] = false;
            node[i].recvmap[j] = false;
        }
    }
}

//送受信マップの表示
void show_map(Node node[])
{
    for (int i = 0; i < N; i++)
    {
        cout << "Node" << i << " ";
    }
    cout << endl;
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
        for (int j = 1; j < N; j++)
        {
            if (node[j].recvmap[i])
            {
                cout << "true ";
            }
            else
            {
                cout << "false ";
            }
        }
        cout << endl;
    }
}

void show_pdr(Node node[])
{
    double recv = count(node[N - 1].recvmap, node[N - 1].recvmap + numberofpackets, true);
    recv /= (double)(numberofpackets);
    cout << "PDR: " << recv << endl;
}

void simulate_end()
{
    //結果はcsv等に保存？
    //string result = "xxx.csv";
}

void BlackholeAttack(queue<int> que)
{
    while (!que.empty())
    {
        que.pop();
    }
}

void edge_set(Graph &gr)
{
    //ノード番号，通信成功率の組
    gr[0].push_back(Edge(1, 0.8));
    gr[0].push_back(Edge(2, 0.8));
    gr[0].push_back(Edge(3, 0.8));
    //gr[1].push_back(Edge(2, 0.8));
    gr[1].push_back(Edge(4, 0.8));
    gr[1].push_back(Edge(5, 0.8));
    //gr[2].push_back(Edge(3, 0.8));
    gr[2].push_back(Edge(4, 0.8));
    gr[2].push_back(Edge(5, 0.8));
    gr[3].push_back(Edge(4, 0.8));
    gr[3].push_back(Edge(5, 0.8));
    gr[4].push_back(Edge(6, 0.9));
    gr[5].push_back(Edge(6, 0.8));
    checked.resize(gr[0].size());
    //送信元から1hopをチェックしたかどうか
    fill(checked.begin(), checked.end(), false);
}

int main(void)
{
    //ノードの位置を入力(あとで？)
    //ひとまずは考えない（手動でノードを接続）
    //接続情報を入力
    Graph g(N);
    int number_of_edges = 8;
    //for (int i = 0; i < numberofedges; i++)
    //{
    //}
    edge_set(g);
    Node node[N];
    //攻撃ノードの情報を追加
    //パケットはuID指定
    set_map(node);
    seen.assign(N, false);
    Decidepriorityfromsource(g, node, 0, d);
    bfs(g); //幅優先探索によりホップ数計算
    for (int i = 0; i < numberofpackets; i++)
    {
        BroadcastFromSource(g, node, 0, i, d);
    }
    //中継ノードの優先度を決定
    //各priorityqueueに優先度を入れている？
    //優先度決定を幅優先探索で求めたHop数ごとに行う
    //最大ホップ数を取得
    int mxhop = GetMaxHop();
    for (int i = 1; i < mxhop; i++)
    {
        DecidePriorityIntermediate(g, node, i, d);
    }
    BroadcastFromIntermediatenode(g, node);
    show_map(node);
    show_pdr(node);
    //経路情報はvectorで管理
    vector<vector<int>>
        route;
    //シミュレーションを行う
    //simulate();

    //終了処理
    //simulate_end();

    return 0;
}