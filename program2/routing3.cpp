//From 2020-12-5
//信頼値測定を加えたバージョン2
//From 2020-12-11
//ファイル分割:ver3
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
typedef long long int lli;

#define INF 1e30
const int N = 50;                 // number of nodes
const int d = N - 1;              //宛先
int send_round = 0;               //ラウンド
const int mx_round = 10;          //ラウンドの最大
const int number_of_malnodes = 1; //悪意のあるノード数
int mode = 0;                     //実験モード
//ノードのリンク情報(通信成功率等)を追加(初めは固定値)
double constant_suc_rate = 0.8;                     //通信成功率(定数)
double threshold = 0.5000;                          // 信頼値の閾値
double theta = 0.5;                                 //直接的な信頼値の重み
const int packet_step = 100;                        //ラウンドで送信するパケット数
const int numberofpackets = packet_step * mx_round; //送信するパケット数
double tmpetx = 0.0;                                //etx計算用
vector<bool> seen;                                  // 到達可能かどうかを調べる
vector<bool> checked;                               // 送信元から1hopノードが送信しているか
vector<double> cs;                                  //宛先までのetxを求めるための配列
//エッジ型
struct Edge
{
    int to;
    double tsuccess_rate;
    Edge(int t, double rate) : to(t), tsuccess_rate(rate){};
};
using Graph = vector<vector<Edge>>;                            //グラフ型
using P = pair<double, int>;                                   //ETX,ノード番号のペア
priority_queue<P, vector<P>, greater<P>> pq_onehop_fromsource; //1hopノードの優先度付きキュー
priority_queue<P, vector<P>, greater<P>> pq_intermediate[N];   //各ノードの優先度付きキュー
vector<int> attacker_array;                                    //攻撃ノードの番号が入った配列(攻撃ノード用)
vector<vector<int>> malnodes_array;                            //悪意のあるノードを検知したときに使う配列(各ノードが保持)
vector<vector<double>> trust_value_array;                      //信頼値を格納する配列
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
//解体する？
//dtvをうまく使った形に変更する

//ONode : dsarrayの計算についてもう少し考える
struct ONode
{
    //alpha...number of packets successfully received
    //beta .. all of packets transmitted
    uint32_t alpha[1000][mx_round];
    uint32_t beta[1000][mx_round];
    double dsarray[1000][4]; //D-S理論計算
    int state;
    /*0(emptyset)
    1(trustee)
    2(untrustee)
    3(uncertain)*/
    //double dtv[1000];
    vector<double> dtv;
    double itv;
    //set_itv : 観察ノードの間接的な信頼値をセットする
    void set_itv_rel(ONode on[], const Graph &gr, int node_num_from, int node_num_to)
    {
        //ここを何とかする
        //for (int i = 0; i < N; i++) //Nを変更する，Nは本来オブザーバーの数だった:グラフで取得する
        /*node_num:観察を行うノード
        collect.to（1hopノード）から情報を収集
        node_num(観察を行うノード)がエッジのあるノードから情報を収集
        
        */
        /*使用例
        node_num_toはセットしない
       B[node_num_to].set_itv(n,g,n_from,n_to);

       //サンプルトポロジーで0が2の信頼値を測定
       dtv:
       caliculate_and_set_dtv(onode,0,2);
       itv: 
       onode[4].dtv[0]
       onode[5].dtv[0]
       caliculate_and_set_dtv(onode,4,2);//4->2
       caliculate_and_set_dtv(onode,5,2);//5->2
       onode[2].set_itv(n,g,0,2);
       dsall
       dstrust
       最終的な信頼値 = a*onode[2].dtv[0] + b*(d-sで求めたitv)
       */
        //サンプルトポロジー例を見て書き換える？
        for (auto collect : gr[node_num_from])
        {
            if (node_num_to != collect.to) //node_num_toがitvの測定で調べたいノードのノード番号
            {
                //on[collect.to].dtv[node_num_from]は証拠を聞くノードの信頼値,dtv[collect.to]はcollect.to->観測対象ノードにおける直接的な信頼値
                if (on[collect.to].dtv[node_num_from] > threshold && dtv[node_num_to] > threshold) //y[i]が信頼できるノードかつ観測対象ノードが信頼できるとき
                {
                    dsarray[collect.to][1] = on[collect.to].dtv[node_num_from];
                    dsarray[collect.to][2] = 0.0;
                    dsarray[collect.to][3] = 1.0 - on[collect.to].dtv[node_num_from];
                }
                else if (on[collect.to].dtv[node_num_from] > threshold && dtv[node_num_to] <= threshold) //y[i]が信頼できるノードかつかつ観測対象ノードが信頼できないとき
                {
                    dsarray[collect.to][1] = 0.0;
                    dsarray[collect.to][2] = on[collect.to].dtv[node_num_from];
                    dsarray[collect.to][3] = 1.0 - on[collect.to].dtv[node_num_from];
                }
                else
                {
                    dsarray[collect.to][1] = 0.0;
                    dsarray[collect.to][2] = 0.0;
                    dsarray[collect.to][3] = 1.0;
                }
            }
        }
    }
};

//三進法変換
int threearray[18];
void num_to_three(int x)
{
    int power3 = 1;
    for (int i = 0; i < 18; i++)
    {
        threearray[i] = (x / power3) % 3;
        power3 *= 3;
    }
}
int binarray[18]; //二進配列
//二進変換
void num_to_bin(int x)
{
    int power2 = 1;
    for (int i = 0; i < 18; i++)
    {
        binarray[i] = (x / power2) % 2;
        power2 *= 2;
    }
}
double ds_trust(ONode x, const Graph &gr, int node_num_from, int node_num_to)
{
    /*bitset か，bit 全探索*/
    /*HHH ,HHU ... などの列挙をやる*/
    /*U を0 に，H を1 に対応させる*/
    double val = 1.0;
    double val2 = 0.0;
    //vector<bool> bitval(gr[node_num].size()); //bitsetの代わりに使いたい,size
    int observer_node_size = gr[node_num_from].size(); //これでOK
    //どのノードともリンクがなかった場合
    if (observer_node_size == 0)
    {
        return 0.5;
    }
    //グラフからノード番号を取得する必要がありそう
    //添え字を何とかする
    /*
    観測ノードの番号... < observer_node_size未満
    xには観察対象ノードの番号付きで入っている
    */
    //bit全探索の計算は計算結果自体は同じになりそうだけど，メモリを節約しないと無駄な計算をしそう
    //ノード番号で管理するとオーバーフローが起こりそう(最悪2^49)
    //↑ノード3つだったら0~7を計算するからそんなに関係なかった
    //要するにbitで集合を管理しているからオーバーフローは気にしなくてもよい
    //jのfor文だけ変更
    //for文でノード番号を順番にプッシュしておく
    //0...(nodenum-1)でアクセスできるようにする
    vector<int> nb_nodes(observer_node_size);
    for (auto num_edge : gr[node_num_from])
    {
        nb_nodes.push_back(num_edge.to);
    }
    for (int i = 0; i < (1 << observer_node_size); i++) //N->変更
    {
        //bitset<observer_node_size> state(i);
        fill(binarray, binarray + 18, 0); //0をセット
        num_to_bin(i);                    //二進法変換(bitsetが使えないため)
        if (i != 0)                       //UU...以外を計算
        {
            for (int j = 0; j < observer_node_size; j++)
            {
                //binarrayが関係ないところの添え字を変える
                if (nb_nodes[j] != node_num_to) //nb_nodesが測定対象のノードでない場合
                {
                    if (binarray[j] == 0)
                    {
                        val *= x.dsarray[nb_nodes[j]][binarray[j] + 3]; //0+3だからなくてもいい
                    }
                    else
                    {
                        val *= x.dsarray[nb_nodes[j]][binarray[j]];
                    }
                }
            }
            val2 += val;
        }
        val = 1.0;
    }
    nb_nodes.clear(); //配列のクリア
    return val2;
}

//すべての場合を計算する
double ds_all(ONode x, const Graph &gr, int node_num_from, int node_num_to)
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
    //node_numは信頼値測定を行うノードのノード番号（のつもり）
    //オーバフロー起こりそう
    //ds_trustと同じ
    map<int, int> setcount;
    int observer_node_size = gr[node_num_from].size();
    //リンクがない場合1.0を返す
    if (observer_node_size == 0)
    {
        return 1.0;
    }
    vector<int> nb_nodes(observer_node_size);
    for (auto num_edge : gr[node_num_from])
    {
        nb_nodes.push_back(num_edge.to);
    }
    for (int i = 0; i < (int)(pow(3, observer_node_size)); i++)
    {
        fill(threearray, threearray + 18, 0);
        num_to_three(i);
        for (int j = 0; j < observer_node_size; j++)
        {
            if (nb_nodes[j] != node_num_to) //観測対象のノード番号と等しくない場合
            {
                setcount[threearray[j] + 1]++;
            }
        }
        if (setcount[1] > 0 && setcount[2] > 0)
        {
            //do nothing
        }
        else
        {
            //nb_nodesで順番に取得していく？
            for (int j = 0; j < observer_node_size; j++)
            {
                if (nb_nodes[j] != node_num_to) //観測対象のノード番号と等しくない場合
                {
                    val *= x.dsarray[nb_nodes[j]][threearray[j] + 1];
                }
            }
            val2 += val;
        }
        val = 1.0;
        setcount.clear();
    }
    nb_nodes.clear();
    return val2;
}

////////追加///////////

///////測定関連///////
//インタラクション数をイベントに応じてカウント増やす
//これだとすべてのノードにおいて信頼値が同じになる
//node_num_from...観察するノード
//node_num_to...観察されるノード
//0...送信成功など
//1...送信失敗など
//2...？
void cnt_inter(ONode on[], int node_num_from, int node_num_to, int ev_val) //ev_val...イベント種別
{
    if (ev_val == 0) //0...送信成功などの動作
    {
        on[node_num_to].alpha[node_num_from][send_round]++;
    }
    else if (ev_val == 1) //1...送信失敗などの動作
    {
        on[node_num_to].beta[node_num_from][send_round]++;
    }
    else if (ev_val == 2) //重複などはこっちへ
    {
        on[node_num_to].beta[node_num_from][send_round]--;
    }
}

//成功に関するインタラクション数を更新する
void CntSuc(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send)
{
    cnt_inter(on, node_num_send, node_num_recv, 0);
    //周辺ノードについて更新する
    for (auto edge : gr[node_num_send])
    {
        cnt_inter(on, edge.to, node_num_recv, 0);
    }
}

//失敗に関するインタラクション数を更新する
void CntFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send)
{
    cnt_inter(on, node_num_send, node_num_recv, 1);
    //周辺ノードについて更新する
    for (auto edge : gr[node_num_send])
    {
        cnt_inter(on, edge.to, node_num_recv, 1);
    }
}

//失敗に関するインタラクション数を減らす
void DecFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send)
{
    cnt_inter(on, node_num_send, node_num_recv, 2);
    //周辺ノードについて更新する
    for (auto edge : gr[node_num_send])
    {
        cnt_inter(on, edge.to, node_num_recv, 2);
    }
}

//インタラクション数をリセット
//最初にかならず呼び，ラウンドの更新ごとにも呼ぶ
//node_num_from...観察するノード
//node_num_to...観察されるノード
void cntint_flush(ONode on[], int node_num_from, int node_num_to)
{
    //ラウンドがゼロのときは初期値1をセット
    if (send_round == 0)
    {
        on[node_num_to].alpha[node_num_from][send_round] = 1;
        on[node_num_to].beta[node_num_from][send_round] = 1;
    } //そうでない場合は前のものを代入しておく
    else
    {
        on[node_num_to].alpha[node_num_from][send_round] = on[node_num_to].alpha[node_num_from][send_round - 1];
        on[node_num_to].beta[node_num_from][send_round] = on[node_num_to].beta[node_num_from][send_round - 1];
    }
}
//すべてのノードのインタラクションをリセット
void cntint_flush_all(ONode on[]) //, Graph &gr)
{
    //インタラクション数をリンクのある各ノードについて初期化
    //自分自身以外のやつを初期化
    for (int x = 0; x < N; x++)
    {
        for (int y = 0; y < N; y++)
        {
            if (x != y)
            {
                cntint_flush(on, x, y);
            }
        }
    }
}
//dtvを，そのノード(node_num_to)について計算し，セットする
//node_num_from...観察するノード
//node_num_to...観察されるノード
//dtvにはラウンドがあることに注意
void caliculate_and_set_dtv(ONode on[], int node_num_from, int node_num_to) //, const Graph &gr)
{
    uint32_t all_val = on[node_num_to].alpha[node_num_from][send_round] + on[node_num_to].beta[node_num_from][send_round];
    //n[node_num].dtv
    //リンクのあるエッジを取得
    on[node_num_to].dtv[node_num_from] = (double)(on[node_num_to].alpha[node_num_from][send_round] / all_val);

    //ここで返すか返さないか
    //return (double)(n[node_num].alpha / all_val);
}

//間接的なノード信頼値の計算
void caliculate_indirect_trust_value(ONode on[], const Graph &g, int node_num_from, int node_num_to)
{
    //間接的にノードに観察させる
    on[node_num_to].set_itv_rel(on, g, node_num_from, node_num_to);
    //dempster-shafer理論の計算
    on[node_num_to].itv = ds_trust(on[node_num_to], g, node_num_from, node_num_to) / ds_all(on[node_num_to], g, node_num_from, node_num_to);
}

//最終的な信頼値測定
double cal_get_trust_value(ONode on[], int node_num_from, int node_num_to)
{
    double trust_value;
    trust_value = theta * on[node_num_to].dtv[node_num_from] + (1 - theta) * on[node_num_to].itv;
    return trust_value;
}

//信頼値測定を行って悪意のあるノードをフィルタリングする
void CalTrust_and_Filtering(ONode on[], Graph &gr)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i != j)
            {
                //直接的なノード信頼値の計算
                caliculate_and_set_dtv(on, i, j);
            }
        }
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            //間接的なノード信頼値の計算
            //d-sでエラー出そう
            //i-j間で直接(1ホップの)リンクがあるかを判定する
            if (i != j && IsLinked(gr, i, j) == true)
            {
                //itv測定
                caliculate_indirect_trust_value(on, gr, i, j);
                //最終的な信頼値測定
                double tv = cal_get_trust_value(on, i, j);
                if (tv <= threshold) //信頼値が閾値以下の場合
                {
                    RemoveEdgeToMal(gr, j, i); //悪意のあるノードのエッジを取り除く
                    RegistTable(j, i);         //まだ登録されていない場合テーブルに登録する
                }
            }
        }
    }
}

//直接的・間接的な信頼値を0.6で初期化
void init_itv(ONode n[], int node_num_to)
{
    n[node_num_to].itv = 0.6;
}

void init_dtv(ONode n[], int node_num_from, int node_num_to)
{
    n[node_num_to].dtv[node_num_from] = 0.6;
}
//信頼値の更新をラウンドごとに行う関数を書く
//配列に格納しておく
//void update_trust_value(int node_num)
//{
//    if (send_round > 0)
//    {
//        trust_value_array[node_num][send_round] = trust_value_array[node_num][send_round - 1];
//    }
//}
///
//ラウンドを増やす
void round_set_next()
{
    send_round++;
}

////////////////////////

///////検知時関連///////

//キューが空でない場合単純にパケットをドロップ
//検知したとき
void RegistTable(int mal_num, int detect_num)
{
    //まだ登録されていない場合登録する
    if (FindFromMaltable(detect_num, mal_num) == false)
    {
        malnodes_array[detect_num].push_back(mal_num);
    }
}

//悪意ノードのリンクを除去
void RemoveEdgeToMal(Graph &gr, int mal_edge, int detect_num)
{
    //mal_edgeの要素を削除
    //普通1本だからforじゃなくて良いかも
    for (auto edge : gr[detect_num])
    {
        if (edge.to == mal_edge)
        {
            gr[detect_num].erase(remove(gr[detect_num].begin(), gr[detect_num].end(), edge), gr[detect_num].end());
        }
    }
}

//登録した攻撃ノードを検索
bool FindFromMaltable(int node_num, int key)
{
    //発見したらtrue
    for (auto x : malnodes_array[node_num])
    {
        if (x == key)
        {
            return true;
        }
    }
    //見つからなかったらfalse
    return false;
}

////攻撃関連///////////////////////////////////////////////////

//攻撃
void BlackholeAttack(Node node[], int node_num)
{
    //攻撃ノードならキューからポップする
    if (!node[node_num].q.empty() && IsRegisteredAt(node_num) == true)
    {
        node[node_num].q.pop();
    }
}

//攻撃ノード指定
void AttackerSet()
{
    attacker_array.resize(number_of_malnodes); //攻撃ノード数に配列をリサイズする
    //攻撃ノードのノード番号を登録しておく
    for (int i = 0; i < number_of_malnodes; i++)
    {
        attacker_array[i] = i + 1;
    }
}

//attackerの中に登録されているか調べる
bool IsRegisteredAt(int key)
{
    for (int i = 0; i < attacker_array.size(); i++)
    {
        if (attacker_array[i] == key)
        {
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////

////ルーチング関連////

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
vector<int> bf_dist(N, -1); // 全頂点を未訪問に初期化
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

//ホップ数を調べる
int GetMaxHop()
{
    vector<int> tmphp;
    tmphp = bf_dist;
    sort(tmphp.begin(), tmphp.end());
    return tmphp[tmphp.size() - 1];
}

//接続性チェック
bool IsLinked(Graph &gr, int from, int to)
{
    for (auto edge : gr[from])
    {
        if (edge.to == to)
        {
            return true;
        }
    }
    return false;
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
                    pq_intermediate[hop_num].emplace(to_etx, num_edge.to);
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
            if (n[tmp2_pq_onehop_fromsource.top().second].sendmap[que.front()] == false)
            // || (n[num_edge.to].recvmap[que.front()] == false && n[tmp2_pq_onehop_fromsource.top().second].sendmap[que.front()] == false))
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

//node_num...送信元
//num_edge.to...宛先
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
            //成功or重複を周辺ノードへ通知
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

//送信成功時
//recvmapの状態を成功に変える
//変えた上でrecvmapを参照し，packet_step個増えたら信頼値関数を呼び出しラウンドを増やす
void WhenRecvPacketSuc(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num)
{
    //Recv
    n[node_num_recv].recvmap[packet_num] = true;
    //宛先がpacket_step個パケットを受信したときの処理
    if (d == node_num_recv && count(n[d].recvmap, n[d].recvmap + numberofpackets, true) == packet_step * send_round)
    {
        CalTrust_and_Filtering(on, gr); //信頼値の計算と結果によるフィルタリング
        round_set_next();               //ラウンドを1進める
        cntint_flush_all(on);           //インタラクション数のリセット
    }
    else //そうでない場合インタラクション数を更新する
    {
        CntSuc(gr, n, on, node_num_recv, node_num_send);
    }
}

//失敗時
void WhenRecvPacketFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num)
{
    //dtv/itvにおける失敗回数を増やす
    CntFal(gr, n, on, node_num_recv, node_num_send);
}

//重複時
//提案手法
void WhenRecvPacketDup(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num)
{
    //dtv/itvにおける失敗回数を減らす
    DecFal(gr, n, on, node_num_recv, node_num_send);
}

//中継ノードからのブロードキャスト
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
        if (bf_dist[i] == 0 || bf_dist[i] == 1)
        {
            checked[i] = true;
            cnt++;
        }
    }
    checked[N - 1] = true;
    cnt++;
    int now_hopnum = 1;

    int mxhop = GetMaxHop(); //ホップ数(最大)
                             //while (cnt < N - 1)
                             //{
    //送信元から2ホップ以上
    for (int i = 1; i < mxhop; i++)
    {
        cout << i + 1 << " hop" << endl;
        //まだ調べていないノードがある場合
        //if (checked[pq_intermediate[i].top().second] == false) //ホップ数主体に書き換える
        //{
        //int node_num = tmp_pq_onehop_fromsource.top().second; //ノード番号(優先度順)
        //優先度キューを退避
        priority_queue<P, vector<P>, greater<P>> tmp_pq_intermediate = pq_intermediate[i];
        int highest_sev;
        highest_sev = tmp_pq_intermediate.top().second;
        while (!pq_intermediate[i].empty()) //ソースを中継ノードに置き換える
        {
            //if (checked[pq_intermediate[i].top().second] == false) //チェック済みでない
            //{
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
            }
            checked[pq_intermediate[i].top().second] = true;
            pq_intermediate[i].pop();
            //}
            //else
            //{
            //    pq_intermediate[i].pop();
            //}
            //cnt++;
        }
        //}
    } //end for
      //now_hopnum++; //調べるHop数を増やす
    //}                 //end while
}

////////////////////////シミュレーション・結果処理関連//////////////////////////////
//Input:設定したパラメータ
//Output:値を関数に渡す？

//単純な性能評価用
void simulate_without_Tv_without_at()
{
}
//測定なしかつ攻撃あり
void simulate_without_Tv_with_at()
{
    AttackerSet();
}
//測定ありかつ攻撃あり
void simulate_with_Tv_with_at()
{
    AttackerSet();
}
//提案手法ありかつ攻撃あり
void simulate_with_Suggest_with_attack()
{
    AttackerSet();
}

//シミュレーションモードを変更する
void set_simulate_mode(int m)
{
    mode = m;
}
//シミュレーションモードに応じたシミュレーションを行う
void simulate()
{
    if (mode == 0) //単純な性能評価用
    {
        simulate_without_Tv_without_at();
    }
    else if (mode == 1) //攻撃のみ
    {
        simulate_without_Tv_with_at();
    }
    else if (mode == 2) //攻撃・信頼値測定あり
    {
        simulate_with_Tv_with_at();
    }
    else if (mode == 3) //提案手法あり
    {
        simulate_with_Suggest_with_attack();
    }
    else
    {
        cout << "Invalid mode" << endl;
    }
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

//malnodes_arrayから検出率を求める
void get_detect_rate()
{
    int cnt_of_detected = 0;
    for (int i = 0; i < N; i++)
    {
        if (malnodes_array[i].size() > 0)
        {
            cnt_of_detected += malnodes_array[i].size();
        }
    }
    double detection_rate = (double)(cnt_of_detected / number_of_malnodes);
    cout << "Detection Rate: " << detection_rate << endl;
}

//PDRの表示
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

void edge_set_from_file(Graph &gr)
{
    //ファイルから読み込む形に変更する
    ifstream ifs("topology.txt", ios::in);
    if (!ifs)
    {
        cerr << "Error: file not opened" << endl;
        return;
    }
    int from, to;
    double cost;
    while (ifs >> from >> to >> cost)
    {
        gr[from].push_back(Edge(to, cost));
    }
    //加えてなかった
    checked.resize(gr[0].size());
    //送信元から1hopをチェックしたかどうか
    fill(checked.begin(), checked.end(), false);
    ifs.close();
}
void edge_set(Graph &gr)
{
    //gr[0].push_back(Edge(1, 0.8));
    //gr[0].push_back(Edge(2, 0.8));
    //gr[0].push_back(Edge(3, 0.8));
    //gr[1].push_back(Edge(4, 1.0));
    //gr[2].push_back(Edge(4, 1.0));
    //gr[3].push_back(Edge(4, 1.0));

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

//ありうるルートを調べる
vector<vector<int>> GetAllRoute()
{
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////main/////////////////////////////////////////
int main(void)
{
    //ノードの位置を入力(あとで？)
    //ひとまずは考えない（手動でノードを接続）
    //接続情報を入力
    Graph g(N);
    //edge_set(g);
    edge_set_from_file(g);
    Node node[N];
    ONode obs_node[N];
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

    //シミュレーションを行う
    //simulate();

    //終了処理
    //simulate_end();

    return 0;
}