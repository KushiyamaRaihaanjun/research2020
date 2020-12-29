//長過ぎるのでプロトタイプ宣言などを分割(12/18)
//送信方法を変更(12/26)
#ifndef routing4header
#define routing4header
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
#include <memory>
#include "Rand.h"
using namespace std;
typedef long long int lli;

#define INF 1e30
const int N = 50;            // ノード数
const int d = N - 1;         //宛先
int send_round = 0;          //ラウンド
int number_of_malnodes = 10; //悪意のあるノード数
int mode = 0;                //実験モード
//ノードのリンク情報(通信成功率等)を追加(初めは固定値)
double constant_suc_rate = 0.8;                            //通信成功率(定数)
double threshold = 0.5000;                                 // 信頼値の閾値
double theta = 0.5;                                        //直接的な信頼値の重み
double gm = 1.01;                                          //dtvを求める際の悪意のある動作betaの重み
const int packet_step = 5000;                              //ラウンドで送信するパケット数
const int numberofpackets = 50000;                         //送信するパケット数
const int mx_round = (int)(numberofpackets / packet_step); //ラウンドの最大
double tmpetx = 0.0;                                       //etx計算用
vector<bool> seen;                                         // 到達可能かどうかを調べる
vector<bool> checked;                                      // 送信元から1hopノードが送信しているか
vector<double> cs;                                         //宛先までのetxを求めるための配列
double eps = 1e-15;                                        //数値誤差
//エッジ型
struct Edge
{
    int to;
    double tsuccess_rate;
    //Edge(int t, double rate) : to(t), tsuccess_rate(rate){};
    Edge(int t, double tsuccess_rate)
    {
        this->to = t;
        this->tsuccess_rate = tsuccess_rate;
    }
    //演算子
    bool operator==(const Edge &e) const
    {
        return e.to == to;
    }
    Edge() {}
};
using Graph = vector<vector<Edge>>;                            //グラフ型
using P = pair<double, int>;                                   //ETX,ノード番号のペア
priority_queue<P, vector<P>, greater<P>> pq_onehop_fromsource; //1hopノードの優先度付きキュー
priority_queue<P, vector<P>, greater<P>> pq_intermediate[N];   //各ノードの優先度付きキュー
vector<int> attacker_array;                                    //攻撃ノードの番号が入った配列(攻撃ノード用)
vector<vector<int>> malnodes_array(N);                         //悪意のあるノードを検知したときに使う配列(各ノードが保持)
vector<vector<double>> trust_value_array;                      //信頼値を格納する配列
struct Node
{
    //alpha...number of packets successfully received
    //beta .. all of packets transmitted

    //x,y...座標
    double x, y;
    int alpha;
    int beta;
    bool *sendmap = new bool[numberofpackets];
    bool *recvmap = new bool[numberofpackets];
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
    int alpha[N][mx_round]; //変更:1000->N
    int beta[N][mx_round];  //変更:1000->N
    double dsarray[N][4];   //D-S理論計算(各ノードに対してサイズN)
    int state;
    /*0(emptyset)
    1(trustee)
    2(untrustee)
    3(uncertain)*/
    //double dtv[1000];
    vector<double> dtv; //サイズを決めてないとセグメンテーションフォルトになる(vector)
    double itv;
    double lambda[N];
    //dtv配列をリサイズする
    void arrayresize()
    {
        dtv.resize(N);
    }
    //set_itv : 観察ノードの間接的な信頼値をセットする
    void set_itv_rel(ONode on[], Graph &gr, int node_num_from, int node_num_to)
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
                //変更
                //代入するように変更した
                if (on[collect.to].dtv[node_num_from] > threshold && on[node_num_to].dtv[collect.to] > threshold) //collect.toが信頼できるノードかつ観測対象ノードが信頼できるとき
                {
                    on[node_num_to].dsarray[collect.to][1] = on[collect.to].dtv[node_num_from];
                    on[node_num_to].dsarray[collect.to][2] = (double)0.0;
                    on[node_num_to].dsarray[collect.to][3] = (double)1.0 - on[collect.to].dtv[node_num_from];
                }
                else if (on[collect.to].dtv[node_num_from] > threshold && on[node_num_to].dtv[collect.to] <= threshold) //collect.toが信頼できるノードかつかつ観測対象ノードが信頼できないとき
                {
                    on[node_num_to].dsarray[collect.to][1] = (double)0.0;
                    on[node_num_to].dsarray[collect.to][2] = on[collect.to].dtv[node_num_from];
                    on[node_num_to].dsarray[collect.to][3] = (double)1.0 - on[collect.to].dtv[node_num_from];
                }
                else
                {
                    on[node_num_to].dsarray[collect.to][1] = (double)0.0;
                    on[node_num_to].dsarray[collect.to][2] = (double)0.0;
                    on[node_num_to].dsarray[collect.to][3] = (double)1.0;
                }
            }
        }
    }
};

/*プロトタイプ宣言*/
void num_to_three(int x);
void num_to_bin(int x);
double ds_trust(ONode on[], Graph &gr, int node_num_from, int node_num_to);
double ds_all(ONode on[], Graph &gr, int node_num_from, int node_num_to);
void cnt_inter(ONode on[], int node_num_from, int node_num_to, int ev_val);
void CntSuc(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send);
void CntFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send);
void DecFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send);
void DecFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send);
void cntint_flush(ONode on[], int node_num_from, int node_num_to);
void cntint_flush_all(ONode on[]);
void cntint_flush_nb(ONode on[], Graph &gr, int node_num_from);
void cntint_flush_prevhop(ONode on[], Graph &gr, int node_num_to);
void caliculate_and_set_dtv(ONode on[], int node_num_from, int node_num_to);
void caliculate_indirect_trust_value(ONode on[], Graph &g, int node_num_from, int node_num_to);
double cal_get_trust_value(ONode on[], int node_num_from, int node_num_to);
void CalTrust_and_Filtering(ONode on[], Graph &gr);
void CalTrust_and_Filtering_nb(ONode on[], Graph &gr, int node_num_from);
void CalTrustWhileSending(ONode on[], Graph &gr, int node_num_to);
void init_itv(ONode n[], int node_num_to);
void init_dtv(ONode n[], int node_num_from, int node_num_to);
void array_ONodeinit(ONode on[]);
void round_set_next();
void RegistTable(int mal_num, int detect_num);
void RemoveEdgeToMal(Graph &gr, int mal_edge, int detect_num);
bool FindFromMaltable(int node_num, int key);
void BlackholeAttack(Node node[], int node_num);
void BlackholeAttackWithmode(Graph &gr, Node n[], ONode on[], int node_num, int num_edge_to, int packet_num);
void AttackerSet();
bool IsRegisteredAt(int key);
void dfs(const Graph &gr, int ver);
void bfs(const Graph &gr);
void CleanPriorityQueue();
int GetMaxHop();
bool IsLinked(Graph &gr, int from, int to);
bool IsOneHopNeighbor(Graph &gr, int node_num1, int node_num2);
void dijkstra_etx(const Graph &gr, int s, vector<double> &dis);
void Decidepriorityfromsource(const Graph &gr, Node n[], int node_num, int dst);
void DecidePriorityIntermediate(const Graph &gr, Node n[], int hop_num, int dst);
void BroadcastFromSource(Graph &gr, Node n[], ONode on[], int node_num, int p, int dst);
void SendFromlessPrior(Graph &gr, Node n[], ONode on[], priority_queue<P, vector<P>, greater<P>> tmp_pq_onehop_fromsource, int node_num, Edge num_edge, int packet_num); //queue<int> que);
void SendFromHighestPrior(Graph &gr, Node n[], ONode on[], int node_num, Edge num_edge, int packet_num);                                                                 //queue<int> que);
void WhenSendPacketSuc(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num);
void WhenSendPacketFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num);
void WhenSendPacketDup(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num);
void WhenRecvPacketSuc(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num);
void WhenRecvPacketFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num);
void WhenRecvPacketDup(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num);
void BroadcastFromIntermediatenode(Graph &gr, Node n[], ONode on[]);
void simulate_without_Tv_without_at();
void simulate_without_Tv_with_at();
void simulate_with_Tv_with_at();
void simulate_with_Suggest_with_attack();
void set_simulate_mode(int m);
void FreeNodeArray(Node node[]);
void simulate();
void set_map(Node node[]);
void show_map(Node node[]);
void get_detect_rate();
void show_pdr(Node node[]);
void simulate_end(Graph &g, Node node[]);
void edge_set_from_file(Graph &gr);
void edge_set(Graph &gr);
vector<vector<int>> GetAllRoute();
/* end */

#endif