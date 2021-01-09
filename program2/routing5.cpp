/*From 2021-1-07
ソースコードを分割(routing4.hを作った)
routing5.hを作った
信頼値測定を加えた,送信方法を変更したバージョン5
routing2にも書きかけあり
*/
#include "routing5.h"
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
double ds_trust(ONode on[], Graph &gr, int node_num_from, int node_num_to)
{
    /*bitset か，bit 全探索*/
    /*HHH ,HHU ... などの列挙をやる*/
    /*U を0 に，H を1 に対応させる*/
    double val = 1.0;
    double val2 = 0.0; //返り値
    //vector<bool> bitval(gr[node_num].size()); //bitsetの代わりに使いたい,size
    int observer_node_size = gr[node_num_from].size() - 1; //これでOK
    //どのノードともリンクがなかった場合、0.0を返す
    //証拠を収集するノードが悪意のノードのみだったときの対策/////
    for (auto node_num : gr[node_num_from])
    {
        if (FindFromMaltable(node_num_from, node_num.to) == true)
        {
            observer_node_size--;
        }
    }
    //リンクがない場合1.0を返す
    //各ノードに関してtheta(lambda)を変更する（注意）
    //observerノードが1つもなかったら重みを1.0に変更
    if (observer_node_size <= 0)
    {
        on[node_num_to].lambda[node_num_from] = 1.0;
        return 0.0;
    }
    else
    {
        on[node_num_to].lambda[node_num_from] = 0.5;
    }
    //////////////////////////////////////////////////////////

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
    vector<int> nb_nodes; //(observer_node_size);
    for (auto num_edge : gr[node_num_from])
    {
        if (num_edge.to != node_num_to && !FindFromMaltable(node_num_from, num_edge.to))
        {
            nb_nodes.push_back(num_edge.to);
        }
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
                //if (nb_nodes[j] != node_num_to) //nb_nodesが測定対象のノードでない場合
                //{
                if (binarray[j] == 0)
                {
                    val *= on[node_num_to].dsarray[nb_nodes[j]][3]; //0+3だからなくてもいい
                }
                else
                {
                    val *= on[node_num_to].dsarray[nb_nodes[j]][1];
                }
                //}
            }
            val2 += val;
        }
        val = 1.0;
    }
    nb_nodes.clear(); //配列のクリア
    return val2;
}

//すべての場合を計算する
double ds_all(ONode on[], Graph &gr, int node_num_from, int node_num_to)
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
    int observer_node_size = gr[node_num_from].size() - 1;
    //証拠を収集するノードが悪意のノードのみだったときの対策/////
    for (auto node_num : gr[node_num_from])
    {
        if (FindFromMaltable(node_num_from, node_num.to) == true)
        {
            observer_node_size--;
        }
    }
    //リンクがない場合1.0を返す
    //各ノードに関してtheta(lambda)を変更する（注意）
    //observerノードが1つもなかったら重みを1.0に変更
    if (observer_node_size <= 0)
    {
        on[node_num_to].lambda[node_num_from] = 1.0;
        return 1.0;
    }
    else
    {
        on[node_num_to].lambda[node_num_from] = 0.5;
    }
    ///////////////////////////////////////////////////////////
    vector<int> nb_nodes; //(observer_node_size);
    for (auto num_edge : gr[node_num_from])
    {
        if (num_edge.to != node_num_to && !FindFromMaltable(node_num_from, num_edge.to))
        {
            nb_nodes.push_back(num_edge.to);
        }
    }
    for (int i = 0; i < (int)(pow(3, observer_node_size)); i++)
    {
        fill(threearray, threearray + 18, 0);
        num_to_three(i);
        for (int j = 0; j < observer_node_size; j++)
        {
            //if (nb_nodes[j] != node_num_to) //観測対象のノード番号と等しくない場合
            //{
            setcount[threearray[j] + 1]++;
            //}
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
                //if (nb_nodes[j] != node_num_to) //観測対象のノード番号と等しくない場合
                //{
                val *= on[node_num_to].dsarray[nb_nodes[j]][threearray[j] + 1];
                //}
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
        on[node_num_to].alpha[node_num_from][send_round] += 1;
    }
    else if (ev_val == 1) //1...送信失敗などの動作
    {
        on[node_num_to].beta[node_num_from][send_round] += 1;
    }
    else if (ev_val == 2) //重複などはこっちへ
    {
        on[node_num_to].beta[node_num_from][send_round] -= 1;
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

void cntint_flush_nb(ONode on[], Graph &gr, int node_num_from)
{
    //from周辺のノードのインタラクション数をリセット
    for (auto num_edge : gr[node_num_from])
    {
        for (auto num_v : gr[node_num_from])
        {
            if (num_edge.to != num_v.to)
            {
                cntint_flush(on, num_edge.to, num_v.to);
            }
        }
    }
}

//前ホップノードのインタラクションをリセット
void cntint_flush_prevhop(ONode on[], Graph &gr, int node_num_to)
{
    //i->jかつi->node_num_to
    for (int i = 0; i < N; i++)
    {
        //node_num_toへのインタラクションをリセット
        if (IsLinked(gr, i, node_num_to))
        {
            cntint_flush(on, i, node_num_to);
            //node_num_toと同ホップのノードのインタラクションをリセット
            for (int j = 0; j < N; j++)
            {
                if (j != node_num_to && IsLinked(gr, i, j))
                {
                    cntint_flush(on, i, j);
                }
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
    //betaに重み付け
    double all_val = (double)(on[node_num_to].alpha[node_num_from][send_round]) + gm * (double)(on[node_num_to].beta[node_num_from][send_round]);
    //n[node_num].dtv
    //リンクのあるエッジを取得
    on[node_num_to].dtv[node_num_from] = (double)((double)on[node_num_to].alpha[node_num_from][send_round] / (double)all_val);
    //ここで返すか返さないか
    //return (double)(n[node_num].alpha / all_val);
}

//間接的なノード信頼値の計算
void caliculate_indirect_trust_value(ONode on[], Graph &g, int node_num_from, int node_num_to)
{
    //間接的にノードに観察させる
    on[node_num_to].set_itv_rel(on, g, node_num_from, node_num_to);
    //dempster-shafer理論の計算
    on[node_num_to].itv = ds_trust(on, g, node_num_from, node_num_to) / ds_all(on, g, node_num_from, node_num_to);
}

//最終的な信頼値測定
//重みは各ノードごとに異なる
double cal_get_trust_value(ONode on[], int node_num_from, int node_num_to)
{
    double trust_value;
    trust_value = on[node_num_to].lambda[node_num_from] * on[node_num_to].dtv[node_num_from] + (1.0 - on[node_num_to].lambda[node_num_from]) * on[node_num_to].itv;
    return trust_value;
}

//信頼値測定を行って悪意のあるノードをフィルタリングする
//dtvに基づく追加と最終的なtvに基づく追加を作る？
void CalTrust_and_Filtering(ONode on[], Graph &gr)
{
    //変更する
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i != j)
            {
                //直接的なノード信頼値の計算
                caliculate_and_set_dtv(on, i, j);
                //dtvがしきい値以下の場合
                //i,jが直接つながっているまたはあるノードの共通の1hopノードである場合
                if (on[j].dtv[i] <= threshold && IsOneHopNeighbor(gr, i, j) == true)
                {
                    RegistTable(j, i);
                }
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
                    //constを変更しようとしている
                    //RemoveEdgeToMal(gr, j, i); //悪意のあるノードのエッジを取り除く
                    RegistTable(j, i); //まだ登録されていない場合テーブルに登録する
                }
            }
        }
    }
}

//ループをすべてのノードではなく測定対象のノードのみを測定する
void CalTrust_and_Filtering_nb(ONode on[], Graph &gr, int node_num_from)
{
    //変更する
    //1hopの信頼値をセットする
    for (auto node_num : gr[node_num_from])
    {
        caliculate_and_set_dtv(on, node_num_from, node_num.to);
        //dtvで悪意ノードと認識する？
        if (on[node_num.to].dtv[node_num_from] + eps <= threshold)
        {
            RegistTable(node_num.to, node_num_from);
        }
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            //直接的なノード信頼値の計算
            //自分ではない周りのノード...i
            //測定対象...j
            if (node_num_from != i && node_num_from != j && i != j && IsLinked(gr, node_num_from, i) == true && IsLinked(gr, node_num_from, j) == true)
            {
                caliculate_and_set_dtv(on, i, j);
                //dtvがしきい値以下の場合
                //i,jが直接つながっているまたはあるノードの共通の1hopノードである場合//この条件は消した
                if (on[j].dtv[i] + eps <= threshold)
                {
                    RegistTable(j, i);
                }
            }
        }
    }
    //ここはOK
    for (int j = 0; j < N; j++)
    {
        //間接的なノード信頼値の計算
        //d-sでエラー出そう
        //i-j間で直接(1ホップの)リンクがあるかを判定する
        if (node_num_from != j && IsLinked(gr, node_num_from, j) == true)
        {
            //itv測定
            caliculate_indirect_trust_value(on, gr, node_num_from, j);
            //最終的な信頼値測定
            double tv = cal_get_trust_value(on, node_num_from, j);
            if (tv + eps <= threshold) //信頼値が閾値以下の場合
            {
                //constを変更しようとしている
                RemoveEdgeToMal(gr, j, node_num_from); //悪意のあるノードのエッジを取り除く
                RegistTable(j, node_num_from);         //まだ登録されていない場合テーブルに登録する
            }
        }
    }
}

//あるノードが送信中における，前ホップノードからの信頼値の測定
void CalTrustWhileSending(ONode on[], Graph &gr, int node_num_to)
{
    for (int i = 0; i < N; i++)
    {
        //i -> node_num_toへのリンクがある場合
        if (i != node_num_to && IsLinked(gr, i, node_num_to))
        {
            for (auto num_edge : gr[i])
            {
                //i->num_edge.toへの直接的な信頼値の測定
                caliculate_and_set_dtv(on, i, num_edge.to);
                if (on[num_edge.to].dtv[i] + eps <= threshold)
                {
                    RegistTable(num_edge.to, i);
                }
            }
            for (int j = 0; j < N; j++)
            {
                //直接的なノード信頼値の計算
                //自分ではない周りのノード...j
                //測定対象...node_num_to
                if (i != j && node_num_to != j && IsLinked(gr, i, j) == true)
                {
                    caliculate_and_set_dtv(on, j, node_num_to);
                    //dtvがしきい値以下の場合
                    if (on[node_num_to].dtv[j] + eps <= threshold)
                    {
                        RegistTable(node_num_to, j);
                    }
                }
            }
            //間接的なノード信頼値の計算
            //itv測定
            caliculate_indirect_trust_value(on, gr, i, node_num_to);
            //最終的な信頼値測定
            double tv = cal_get_trust_value(on, i, node_num_to);
            if (tv + eps <= threshold) //信頼値が閾値以下の場合
            {
                //constを変更しようとしている
                RemoveEdgeToMal(gr, node_num_to, i); //悪意のあるノードのエッジを取り除く
                RegistTable(node_num_to, i);         //まだ登録されていない場合テーブルに登録する
            }
        }
    }
}
//end

//直接的・間接的な信頼値を0.6で初期化
void init_itv(ONode on[], int node_num_to)
{
    on[node_num_to].itv = 0.6;
}

void init_dtv(ONode on[], int node_num_from, int node_num_to)
{
    on[node_num_to].dtv[node_num_from] = 0.6;
}

//ONodeの重みを初期化
void Init_lambda(ONode on[])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            on[j].lambda[i] = theta;
        }
    }
}
//ONodeのdtv/itv配列をリセットする
void array_ONodeinit(ONode on[])
{
    for (int i = 0; i < N; i++)
    {
        on[i].arrayresize();
        init_itv(on, i);
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            init_dtv(on, i, j);
        }
    }
    //重みをリセット
    Init_lambda(on);
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
    //測定ラウンドがmx_round未満なら増やす
    //そうでない場合0にする
    if (send_round < mx_round - 1)
    {
        send_round++;
    }
    else
    {
        send_round = 0;
    }
}

////////////////////////

///////検知時関連///////

//キューが空でない場合単純にパケットをドロップ
//検知したとき
void RegistTable(int mal_num, int detect_num)
{
    //まだ登録されていない場合登録する
    if (FindFromMaltable(detect_num, mal_num) == false && mal_num != d)
    {
        malnodes_array[detect_num].push_back(mal_num);
    }
}

//悪意ノードのリンクを除去
void RemoveEdgeToMal(Graph &gr, int mal_edge, int detect_num)
{
    //mal_edgeの要素を削除
    //普通1本だからforじゃなくて良いかも
    if (mal_edge == d)
    {
        return;
    }
    double mal_rate = 0.0;
    for (auto edge : gr[detect_num])
    {
        if (edge.to == mal_edge)
        {
            mal_rate = edge.tsuccess_rate;
        }
    }
    const Edge link_to_mal(mal_edge, mal_rate);
    gr[detect_num].erase(remove(gr[detect_num].begin(), gr[detect_num].end(), link_to_mal), gr[detect_num].end());
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

//モードに応じて信頼値測定を行うかどうかを判定する（ブラックホール攻撃）
void BlackholeAttackWithmode(Graph &gr, Node n[], ONode on[], int node_num, int num_edge_to, int packet_num)
{
    if (mode == 1)
    {
        //num_edge.to が攻撃ノードであれば，攻撃が行われる
        BlackholeAttack(n, num_edge_to); //追加
    }
    else if (mode >= 2) //ブラックホール攻撃・信頼値測定/提案手法
    {
        int tmp_packet_num = packet_num;

        //node_num(送信元ノードの観察)
        if (IsRegisteredAt(num_edge_to))
        {
            BlackholeAttack(n, num_edge_to);
            WhenRecvPacketFal(gr, n, on, num_edge_to, node_num, tmp_packet_num);
        }
        //周辺ノードについてもカウントする(to do)
        //二重にカウントしている: いらない
    }
}

//攻撃ノード指定
void AttackerSet()
{
    //attackerの設定
    if (attacker_array.size() > 0)
    {
        attacker_array.clear();
    }
    attacker_array.resize(number_of_malnodes); //攻撃ノード数に配列をリサイズする
    //攻撃ノードのノード番号を登録しておく
    //ランダムに変更
    //配列:1~48
    vector<int> arr(d - 1);
    for (int i = 0; i < d - 1; i++)
    {
        arr[i] = i + 1;
    }
    mt19937_64 mt_r(rnd());
    shuffle(arr.begin(), arr.end(), mt_r);
    string fname = "attacker-";
    fname += to_string(number_of_malnodes);
    fname += ".txt";
    ofstream ofs(fname, ios::app);
    for (int i = 0; i < number_of_malnodes; i++)
    {
        attacker_array[i] = arr[i];
        ofs << arr[i] << " ";
    }
    ofs << endl;
    ofs.close();
}

void AttackerSetFromFile()
{
    attacker_array.resize(number_of_malnodes); //攻撃ノード数に配列をリサイズする
    //攻撃ノードのノード番号をファイルから取得
    //ランダムに変更
    string s = "attacker-";
    s += to_string(number_of_malnodes);
    s += ".txt";
    ifstream ifs(s);
    int node_num;
    for (int i = 0; i < number_of_malnodes; i++)
    {
        ifs >> node_num;
        attacker_array[i] = node_num;
    }
    ifs.close();
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

//優先度付きキューをリセットする
void CleanPriorityQueue()
{
    if (!pq_onehop_fromsource.empty())
    {
        while (!pq_onehop_fromsource.empty())
        {
            pq_onehop_fromsource.pop();
        }
    }
    for (int i = 0; i < N; i++)
    {
        if (!pq_intermediate[i].empty())
        {
            while (!pq_intermediate[i].empty())
            {
                pq_intermediate[i].pop();
            }
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

//node_num1とnode_num2に共通の1hopノードがあるかどうか判定する関数
bool IsOneHopNeighbor(Graph &gr, int node_num1, int node_num2)
{
    for (int i = 0; i < N; i++)
    {
        if (IsLinked(gr, i, node_num1) && IsLinked(gr, i, node_num2))
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
            //cout << "Node " << num_edge.to << " :ETX = " << to_etx << endl;
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
            //cout << i << endl;
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
                    //cout << "Node " << num_edge.to << " :ETX = " << to_etx << endl;
                    //priority_queueのサイズ制限（信頼値測定時）
                    if (mode >= 2)
                    {
                        if (pq_intermediate[hop_num].size() <= 5 && !FindFromMaltable(i, num_edge.to))
                        {
                            pq_intermediate[hop_num].emplace(to_etx, num_edge.to);
                        }
                    }
                    else if (mode == 1)
                    {
                        if (pq_intermediate[hop_num].size() <= 5)
                        {
                            pq_intermediate[hop_num].emplace(to_etx, num_edge.to);
                        }
                    }
                    else
                    {
                        pq_intermediate[hop_num].emplace(to_etx, num_edge.to);
                    }
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
void BroadcastFromSource(Graph &gr, Node n[], ONode on[], int node_num, int p, int dst)
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
                WhenSendPacketSuc(gr, n, on, num_edge.to, node_num, p);
                //n[node_num].sendmap[p] = true;
                //n[num_edge.to].recvmap[p] = true; //toのrecvmapを更新
                //n[num_edge.to].q.push(p);         //toのキューにパケットをプッシュ
                WhenRecvPacketSuc(gr, n, on, num_edge.to, node_num, p);
                //cout << "Node " << num_edge.to << " received packet " << p << " from Node " << node_num << endl;
                //modeに応じて攻撃・攻撃+測定を切り替える
                BlackholeAttackWithmode(gr, n, on, node_num, num_edge.to, p);
            }
            else //失敗処理
            {
                WhenSendPacketFal(gr, n, on, num_edge.to, node_num, p);
                //n[node_num].sendmap[p] |= false; //orにする
                //n[num_edge.to].recvmap[p] = false; //
                WhenRecvPacketFal(gr, n, on, num_edge.to, node_num, p);
                //cout << "Node " << num_edge.to << " dropped packet " << p << " ((from  Node " << node_num << endl;
            }
            ////cout << num_edge.to << " ";
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
//packetごとに変更した
void SendFromlessPrior(Graph &gr, Node n[], ONode on[], priority_queue<P, vector<P>, greater<P>> tmp_pq_onehop_fromsource, int node_num, Edge num_edge, int packet_num) //queue<int> que)
{
    priority_queue<P, vector<P>, greater<P>> tmp2_pq_onehop_fromsource = tmp_pq_onehop_fromsource;
    //while (!que.empty())
    //{
    if (tmp2_pq_onehop_fromsource.top().second != node_num) //退避した優先度キューのインデックスを調べる
    {
        //自分より優先度の高いノード番号ごとにループするため，送信優先度のキューを退避
        //自分より優先度が高いノードが送信済みでないか
        //受信に失敗していたと考えられるとき
        //自分より優先度が高いノードが送信に失敗していたとき
        if (n[tmp2_pq_onehop_fromsource.top().second].sendmap[packet_num] == false)
        // || (n[num_edge.to].recvmap[que.front()] == false && n[tmp2_pq_onehop_fromsource.top().second].sendmap[que.front()] == false))
        {
            //そのパケットを宛先（最終的な宛先とは異なる）が受信していないとき
            if (n[num_edge.to].recvmap[packet_num] == false)
            {
                //送信成功
                if (rnd.randBool(num_edge.tsuccess_rate))
                {
                    //sendmapを成功に変える
                    WhenSendPacketSuc(gr, n, on, num_edge.to, node_num, packet_num);
                    //パケットの重複判定をする
                    WhenRecvPacketSuc(gr, n, on, num_edge.to, node_num, packet_num);
                    //modeで信頼値測定を行うか行わないかを切り替える
                    BlackholeAttackWithmode(gr, n, on, node_num, num_edge.to, packet_num);
                    //to do
                    //エッジを調べる
                    //成功or重複をnode_numに通知
                    //成功or重複をnode_numに通知
                }
                else //失敗
                {
                    //sendmapを失敗に変える
                    WhenSendPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
                    WhenRecvPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
                    //to do
                    //エッジを調べる
                    //失敗をnode_numに通知
                    //失敗を周辺ノードに通知
                }
            }
            else //重複を避けるためパケットをドロップ
            {
                //cout << "Node " << node_num << " Drop packet " << packet_num << " to prevent duplicate (to Node" << num_edge.to << endl;
                WhenSendPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
                if (mode == 3) //提案手法あり
                {
                    WhenSendPacketDup(gr, n, on, num_edge.to, node_num, packet_num);
                }
            }
        } //すでに優先度の高いノードが送信している場合
        else
        {
            //cout << "Node " << node_num << " Drop packet " << packet_num << " to prevent duplicate (to Node" << num_edge.to << endl;
            WhenSendPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
            //WhenRecvPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
            if (mode == 3) //提案手法あり
            {
                WhenSendPacketDup(gr, n, on, num_edge.to, node_num, packet_num);
                //WhenRecvPacketDup(gr, n, on, num_edge.to, node_num, packet_num);
            }
        }
        //que.pop();
        tmp2_pq_onehop_fromsource.pop(); //パケットについての処理終なのでノード番号を更新する
    }
    else //ノード番号が等しい場合はそのパケットについては調べ終わったのでcontinue
    {
        ////cout << "continue : queue " << que.front() << " node num " << tmp2_pq_onehop_fromsource.top().second << endl;
        //que.pop();
        tmp2_pq_onehop_fromsource = tmp_pq_onehop_fromsource; //番号をリセット(priorityqueueをもとに戻す)
    }                                                         //end while for pq

    //} //end while for que
}

//node_num...送信元
//num_edge.to...宛先
//優先度が高いノードからの送信
//packetごとのループに変更した
void SendFromHighestPrior(Graph &gr, Node n[], ONode on[], int node_num, Edge num_edge, int packet_num) //queue<int> que)
{

    //while (!que.empty())
    //{
    if (rnd.randBool(num_edge.tsuccess_rate))
    {
        //パケットの重複判定をする
        if (n[num_edge.to].recvmap[packet_num] == false) //まだキューの先頭のパケットを受信していない場合
        {
            //sendmapの更新
            WhenSendPacketSuc(gr, n, on, num_edge.to, node_num, packet_num);
            //受信時処理をWhenRecvに移動した
            WhenRecvPacketSuc(gr, n, on, num_edge.to, node_num, packet_num);
            //modeで攻撃時に観察を行うかを切り替える
            BlackholeAttackWithmode(gr, n, on, node_num, num_edge.to, packet_num);
        }
        else
        {
            //重複時のメッセージ
            //cout << "Node " << num_edge.to << " ignoring packet " << packet_num << " due to duplicate" << endl;
            //SendFromlessに書く
            WhenSendPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
            //WhenRecvPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
            if (mode == 3) //提案手法あり
            {
                WhenSendPacketDup(gr, n, on, num_edge.to, node_num, packet_num);
                //WhenRecvPacketDup(gr, n, on, num_edge.to, node_num, packet_num);
            }
        }
        //que.pop();
        //to do
        //エッジを調べる
        //成功or重複をnode_numに通知
        //成功or重複を周辺ノードへ通知
    }
    else
    {
        //sendmapを失敗に更新
        WhenSendPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
        //受信失敗時処理をRecvpacketFalに移動
        WhenRecvPacketFal(gr, n, on, num_edge.to, node_num, packet_num);
        //que.pop();
        //to do
        //エッジを調べる
        //失敗をnode_numに通知
        //失敗を周辺ノードに通知
    } //end if
    //}     //end while
}

//送信mapをtrueにする関数
//1hop後ろのノードに送信成功を通知する
void WhenSendPacketSuc(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num)
{
    n[node_num_send].sendmap[packet_num] = true; //送信マップをtrue
    //node_num_sendに接続しているノードとそれと接続している全ノードに送信成功を通知する
    for (int i = 0; i < N; i++)
    {
        if (i != node_num_send && IsLinked(gr, i, node_num_send))
        {
            cnt_inter(on, i, node_num_send, 0);
            for (int j = 0; j < N; j++)
            {
                if (IsLinked(gr, i, j))
                {
                    cnt_inter(on, j, node_num_send, 0);
                }
            }
        }
    }
    //パケットをある程度送信したら信頼値を測定する
    //キューサイズの1割送信したら測定する？
    //packet_step個送信したら更新
    if (mode >= 2)
    {
        //送信カウント
        //int send_cnt = count(n[node_num_send].sendmap, n[node_num_send].sendmap + numberofpackets, true);
        //if (send_cnt != 0 && send_cnt % packet_step == 0)
        if (count(n[node_num_send].sendmap, n[node_num_send].sendmap + numberofpackets, true) == packet_step * (send_round + 1))
        {
            CalTrust_and_Filtering_nb(on, gr, node_num_send); //信頼値の計算と結果によるフィルタリング
            CalTrustWhileSending(on, gr, node_num_send);      //前ホップノードからの信頼値測定
            round_set_next();                                 //ラウンドを1進める
            cntint_flush_nb(on, gr, node_num_send);           //インタラクション数のリセット
            cntint_flush_prevhop(on, gr, node_num_send);      //前ホップノードのインタラクションをリセットする
        }
    }
}

//送信mapをfalseにする関数
//1hop後ろのノードに送信失敗を通知する
void WhenSendPacketFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num)
{
    n[node_num_send].sendmap[packet_num] = false; //送信マップをfalse
    //node_num_sendに接続しているノードとそれと接続している全ノードに送信失敗を通知する
    for (int i = 0; i < N; i++)
    {
        if (i != node_num_send && IsLinked(gr, i, node_num_send))
        {
            cnt_inter(on, i, node_num_send, 1);
            for (int j = 0; j < N; j++)
            {
                if (IsLinked(gr, i, j))
                {
                    cnt_inter(on, j, node_num_send, 1);
                }
            }
        }
    }
}
//重複を避けるためにパケットを破棄した時(送信側)
void WhenSendPacketDup(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num)
{
    //node_num_sendに接続しているノードとそれと接続している全ノードに重複送信を避けたことを通知する
    for (int i = 0; i < N; i++)
    {
        if (i != node_num_send && IsLinked(gr, i, node_num_send))
        {
            cnt_inter(on, i, node_num_send, 2);
            for (int j = 0; j < N; j++)
            {
                if (IsLinked(gr, i, j))
                {
                    cnt_inter(on, j, node_num_send, 2);
                }
            }
        }
    }
}
//送信成功時(受信成功時)
//recvmapの状態を成功に変える
//変えた上でrecvmapを参照し，packet_step個増えたら信頼値関数を呼び出しラウンドを増やす
void WhenRecvPacketSuc(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num)
{
    //Recv
    n[node_num_recv].recvmap[packet_num] = true; //受信マップをfalseならtrue
    //受信成功時のメッセージ
    //cout << "Node " << node_num_recv << " received packet " << packet_num << " from Node " << node_num_send << endl;
    n[node_num_recv].q.push(packet_num);
    //宛先がpacket_step個パケットを受信したときの処理
    if (mode >= 2) //信頼値測定を行うかをモードで分岐する
    {
        CntSuc(gr, n, on, node_num_recv, node_num_send); //成功をカウント
    }
}

//失敗時
void WhenRecvPacketFal(Graph &gr, Node n[], ONode on[], int node_num_recv, int node_num_send, int packet_num)
{
    n[node_num_recv].recvmap[packet_num] |= false; //受信マップをfalse
    //cout << "Node " << node_num_recv << " dropped packet " << packet_num << " ((from Node " << node_num_send << endl;
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
void BroadcastFromIntermediatenode(Graph &gr, Node n[], ONode on[])
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
    //ラウンド，インタラクション数をリセットしておく
    //send_round = 0;
    //cntint_flush_all(on);
    //whileに変える
    while (cnt != one_hop_number) //送信元から1hopノードのブロードキャストが終了しているか
    {
        int highest = pq_onehop_fromsource.top().second; //最も優先度が高いノードのノード番号
        //優先度キューのループ
        while (!pq_onehop_fromsource.empty())
        {
            int node_num = pq_onehop_fromsource.top().second; //ノード番号(優先度順)
            send_round = 0;                                   //送信ラウンドのリセット
            cntint_flush_nb(on, gr, node_num);
            cntint_flush_prevhop(on, gr, node_num);
            //優先度を表示
            //数字(size)が大きいほど高い優先度
            //cout << "Node " << node_num << " priority " << pq_onehop_fromsource.size() << endl;
            //キューが空になるまでdo
            while (!n[node_num].q.empty())
            {
                int packet_num = n[node_num].q.front(); //パケット番号をコピー
                for (auto num_edge : gr[node_num])      //num_edge...接続しているエッジ
                {
                    //送信先のノードのETXを計算する
                    //priority_queueの配列から次の送信ノードを取得
                    //実際送信するところ
                    //queue<int> tmp = n[node_num].q; //キューの中身をいったん退避(ブロードキャストのため)
                    if (node_num != highest) //もっとも優先度の高いノードでない場合
                    {
                        //優先度がより低い場合
                        SendFromlessPrior(gr, n, on, tmp_pq_onehop_fromsource, node_num, num_edge, packet_num); //n[node_num].q);
                    }
                    else //最も優先度が高い場合
                    {
                        //優先度が高いノードから送信
                        SendFromHighestPrior(gr, n, on, node_num, num_edge, packet_num); //n[node_num].q);
                    }                                                                    //end if
                    //n[node_num].q = tmp; //退避していたキューの中身をもとに戻す
                } //end for
                n[node_num].q.pop();
            } //end while
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
    //送信元から2ホップ以上
    for (int i = 1; i < mxhop; i++)
    {
        //cout << i + 1 << " hop" << endl;
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
            send_round = 0;                                     //送信ラウンドのリセット
            cntint_flush_nb(on, gr, node_num_sev);
            cntint_flush_prevhop(on, gr, node_num_sev);
            //優先度を表示
            //数字(size)が大きいほど高い優先度
            //cout << "Node " << node_num_sev << " priority " << pq_intermediate[i].size() << endl;
            while (!n[node_num_sev].q.empty())
            {
                int pakcet_num = n[node_num_sev].q.front();
                for (auto num_edge : gr[node_num_sev]) //num_edge...接続しているエッジ
                {
                    //送信先のノードのETXを計算する
                    //priority_queueの配列から次の送信ノードを取得
                    //実際送信するところ
                    //queue<int> tmp = n[node_num_sev].q; //キューの中身をいったん退避(ブロードキャストのため)
                    if (node_num_sev != highest_sev) //もっとも優先度の高いノードでない場合
                    {
                        //優先度がより低い場合
                        SendFromlessPrior(gr, n, on, tmp_pq_intermediate, node_num_sev, num_edge, pakcet_num);
                    }
                    else //最も優先度が高い場合
                    {
                        //優先度が高いノードから送信
                        SendFromHighestPrior(gr, n, on, node_num_sev, num_edge, pakcet_num);
                    } //end if
                    //n[node_num_sev].q = tmp; //退避していたキューの中身をもとに戻す
                }
                n[node_num_sev].q.pop();
            }
            checked[pq_intermediate[i].top().second] = true;
            pq_intermediate[i].pop();
        }
    } //end for
}

//ルーチングを行う関数
void OpportunisticRouting4(Graph &g, Node node[], ONode obs_node[])
{
    edge_set(g); //エッジをセット
    //edge_set_from_file(g);
    //攻撃ノードの情報を追加
    //パケットはuID指定
    set_map(node);
    bfs(g);                      //幅優先探索によりホップ数計算
    int packet_step_send = 1000; //パケットのstep数(packet_stepと同名にしない)
    int packet_total_num = 0;    //パケットのカウンター
    if (mode >= 2)               //測定あり
    {
        array_ONodeinit(obs_node); //ONodeのdtv配列をリセットする
    }
    //悪意ノードのマップを初期化
    if (malnodes_array.size() > 0)
    {
        for (int i = 0; i < N; i++)
        {
            malnodes_array[i].clear();
        }
    }
    while (packet_total_num < numberofpackets) //パケットの合計数が総パケット数より小さい間 do
    {
        if (mode >= 2) //測定あり
        {
            send_round = 0;             //ラウンドをリセット
            cntint_flush_all(obs_node); //インタラクション数をリセット
        }
        seen.assign(N, false); //seen(訪問配列)をリセット
        Decidepriorityfromsource(g, node, 0, d);
        //packet_step_send個送信
        for (int i = packet_total_num; i < packet_total_num + packet_step_send; i++)
        {
            BroadcastFromSource(g, node, obs_node, 0, i, d);
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
        BroadcastFromIntermediatenode(g, node, obs_node);
        packet_total_num += packet_step_send;
        CleanPriorityQueue(); //優先度キューのリセット
    }
    show_map(node);
    show_pdr(node);
    if (mode >= 2)
    {
        get_detect_rate();
    }
    simulate_end(g, node);
    //FreeNodeArray(node);
}

////////////////////////シミュレーション・結果処理関連//////////////////////////////
//Input:設定したパラメータ
//Output:値を関数に渡す？

//単純な性能評価用...0
void simulate_without_Tv_without_at()
{
    //接続情報を入力
    Graph g(N);
    std::unique_ptr<Node[]> node(new Node[N]);
    ONode obs_node[N];
    //攻撃ノードの情報を追加
    //パケットはuID指定
    OpportunisticRouting4(g, node.get(), obs_node);
    node.release();
}

//測定なしかつ攻撃あり...1
void simulate_without_Tv_with_at()
{
    //接続情報を入力
    Graph g(N);
    std::unique_ptr<Node[]> node(new Node[N]);
    ONode obs_node[N];
    //攻撃ノードの情報を追加
    //パケットはuID指定
    //AttackerSet();
    //AttackerSetFromFile(); //攻撃ノード指定
    OpportunisticRouting4(g, node.get(), obs_node);
    node.release();
}
//測定ありかつ攻撃あり...2
void simulate_with_Tv_with_at()
{
    //ノードの位置を入力(あとで？)
    //ひとまずは考えない（手動でノードを接続）
    //接続情報を入力
    Graph g(N);
    std::unique_ptr<Node[]> node(new Node[N]);
    ONode obs_node[N];
    //攻撃ノードの情報を追加
    //パケットはuID指定
    //AttackerSet();
    //AttackerSetFromFile(); //攻撃ノード指定
    OpportunisticRouting4(g, node.get(), obs_node);
    node.release();
}
//提案手法ありかつ攻撃あり...3
void simulate_with_Suggest_with_attack()
{
    //ノードの位置を入力(あとで？)
    //ひとまずは考えない（手動でノードを接続）
    //接続情報を入力
    Graph g(N);
    std::unique_ptr<Node[]> node(new Node[N]);
    ONode obs_node[N];
    //攻撃ノードの情報を追加
    //パケットはuID指定
    //AttackerSet();
    //AttackerSetFromFile(); //攻撃ノード指定
    OpportunisticRouting4(g, node.get(), obs_node);
    node.release();
}
void FreeNodeArray(Node node[])
{
    delete[] node;
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
        //cout << "Invalid mode" << endl;
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
        //cout << "Node" << i << " ";
    }
    //cout << endl;
    for (int i = 0; i < numberofpackets; i++)
    {
        //cout << i << " ";
        if (node[0].sendmap[i])
        {
            //cout << "true ";
        }
        else
        {
            //cout << "false ";
        }
        for (int j = 1; j < N; j++)
        {
            if (node[j].recvmap[i])
            {
                //cout << "true ";
            }
            else
            {
                //cout << "false ";
            }
        }
        //cout << endl;
    }
}

//malnodes_arrayから検出率を求める
//malnodes_arrayの表示
void get_detect_rate()
{
    int cnt_of_detected = 0;
    map<int, int> mp;
    //検出したノード番号と出現回数を記録する
    for (int i = 0; i <= d; i++)
    {
        if (malnodes_array[i].size() > 0 && !IsRegisteredAt(i))
        {
            for (int j = 0; j < malnodes_array[i].size(); i++)
            {
                //攻撃ノードでない場合malnodes_arrayをカウント
                if (!IsRegisteredAt(i))
                {
                    mp[malnodes_array[i][j]]++;
                }
            }
            //cnt_of_detected += malnodes_array[i].size();
        }
    }
    map<int, int> missed; //誤検知したノード数
    for (auto i : mp)
    {
        for (int j = 0; j < attacker_array.size(); j++)
        {
            if (attacker_array[j] == i.first)
            {
                cnt_of_detected++;
            }
            else
            {
                missed[i.first]++;
            }
        }
    }
    double detection_rate = (double)((double)(cnt_of_detected) + eps) / (double)number_of_malnodes;
    //cout << "Detection Rate: " << detection_rate << endl;
    //cout << "Count Missed : " << missed.size() << " mode = " << mode << endl; //誤検知
    for (int i = 0; i < N; i++)
    {
        if (malnodes_array[i].size() > 0)
        {
            for (int j = 0; j < malnodes_array[i].size(); j++)
            {
                //cout << "Node " << i << ": malicious " << malnodes_array[i][j] << endl;
            }
        }
    }
}

//PDRの表示
void show_pdr(Node node[])
{
    double recv = count(node[N - 1].recvmap, node[N - 1].recvmap + numberofpackets, true);
    recv /= (double)(numberofpackets);
    //cout << "PDR: " << recv << endl;
}

//結果を書き込んでおく
void simulate_end(Graph &g, Node node[])
{
    WritePDR(node);
    if (mode >= 2)
    {
        WriteTopology(g, node);
        WriteDetect();
    }
    //string result = "xxx.csv";
}
void WriteTopology(Graph &g, Node node[])
{
    string s = "fin_topology.txt";
    ofstream ofs(s);
    for (int i = 0; i < N; i++)
    {
        if (g[i].size() > 0)
        {
            for (int j = 0; j < g[i].size(); j++)
            {
                int from = i;
                int to = g[i][j].to;
                double rate = g[i][j].tsuccess_rate;
                ofs << from << " " << to << " " << rate << endl;
            }
        }
    }
    ofs.close();
}
void WritePDR(Node node[])
{
    //ファイル名(PDR)
    string t1 = "PDR-";
    t1 += to_string(mode);
    t1 += "-";
    t1 += to_string(number_of_malnodes);
    t1 += ".txt";
    //終わり
    //PDRをファイルに書き込む
    ofstream ofs2(t1, ios::app);
    double recv = count(node[d].recvmap, node[d].recvmap + numberofpackets, true);
    recv /= (double)(numberofpackets);
    ofs2 << number_of_malnodes << " " << recv << endl; //悪意ノード数，PDR
    ofs2.close();
}
void WriteDetect()
{
    int cnt_of_detected = 0;
    map<int, int> mp;
    //検出したノード番号と出現回数を記録する
    for (int i = 0; i <= d; i++)
    {
        if (malnodes_array[i].size() > 0 && !IsRegisteredAt(i))
        {
            for (int j = 0; j < malnodes_array[i].size(); i++)
            {
                //攻撃ノードでない場合malnodes_arrayをカウント
                if (!IsRegisteredAt(i))
                {
                    mp[malnodes_array[i][j]]++;
                }
            }
            //cnt_of_detected += malnodes_array[i].size();
        }
    }
    map<int, int> missed; //誤検知したノード数
    for (auto i : mp)
    {
        for (int j = 0; j < attacker_array.size(); j++)
        {
            if (attacker_array[j] == i.first)
            {
                cnt_of_detected++;
            }
            else
            {
                missed[i.first]++;
            }
        }
    }
    double detection_rate = (double)((double)(cnt_of_detected) + eps) / (double)number_of_malnodes;
    //検出率
    string t2 = "DETECT-";
    t2 += to_string(mode);
    t2 += "-";
    t2 += to_string(number_of_malnodes);
    t2 += ".txt";
    ofstream ofs3(t2, ios::app);
    ofs3 << number_of_malnodes << " " << detection_rate << endl;
    ofs3.close();
    //誤検知率を求める
    string t3 = "FP-";
    t3 += to_string(mode);
    t3 += "-";
    t3 += to_string(number_of_malnodes);
    t3 += ".txt";
    //(誤検知した数(悪意ノードを除く))/通常ノードの数
    double fp_rate = (double)((double)(missed.size())) / (double)(N - number_of_malnodes); //誤検知率;
    ofstream ofs4(t3, ios::app);
    ofs4 << number_of_malnodes << " " << fp_rate << endl;
    ofs4.close();
}

void edge_set_from_file(Graph &gr)
{
    //ファイルから読み込む形に変更する
    if (gr.size() > 0)
    {
        for (int i = 0; i < N; i++)
        {
            if (gr[i].size() > 0)
            {
                gr[i].clear();
            }
        }
    }
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
    //グラフが空でないなら空にする
    if (gr.size() > 0)
    {
        for (int i = 0; i < N; i++)
        {
            if (gr[i].size() > 0)
            {
                gr[i].clear();
            }
        }
    }
    int cnt = 0;
    seen.assign(N, false);
    int dst = N - 1;
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
        ofstream out("topology.txt");
        for (int i = 1; i < mx_hop; i++) //mx_hop-1..dstのみ
        {
            //loop開始(エッジ数)
            if (i <= mx_hop - 2)
            {
                for (int j = 0; j < 10 * nodes_array[i].size(); j++)
                {
                    int from = nodes_array[i - 1][rnd(nodes_array[i - 1].size())]; //from
                    int to = nodes_array[i][rnd(nodes_array[i].size())];           //to
                    double rate = rnd.randDoubleRange(0.5, 0.8);                   //通信成功率
                    if (nodeval[{from, to}] == 0 && from != to)                    //まだ接続していないノードのみ接続する
                    {
                        gr[from].push_back(Edge(to, rate));
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
                    gr[from].push_back(Edge(to, rate));
                    out << from << " " << to << " " << rate << endl;
                }
            }
            //loop終
        }
        //dfsで接続性チェック
        dfs(gr, 0);
        nodeval.clear();
        //gr.clear();
        if (seen[dst] == true)
        {
            out.close();
            break;
        }
        else
        {
            gr.clear();
            out.close();
            seen.assign(N, false);
        }
    }
    //ファイル書き込み

    checked.resize(gr[0].size());
    //送信元から1hopをチェックしたかどうか
    fill(checked.begin(), checked.end(), false);
}

//ありうるルートを調べる
//vector<vector<int>> GetAllRoute()
//{
//}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////main/////////////////////////////////////////
int main(void)
{
    //0...単純な性能評価用
    //1...攻撃のみ
    //2...攻撃・信頼値測定あり
    //3...提案手法
    //ifstream ifs("simulate.txt", ios::in);
    int cnt_simulation = 1;
    //悪意ノードなしの場合
    set_simulate_mode(0);
    number_of_malnodes = 1;
    for (int i = 0; i < cnt_simulation; i++)
    {
        simulate();
    }
    cout << "mode 0 done" << endl;
    //悪意ノードありの場合
    for (int i = 5; i <= 30; i += 5)
    {
        //攻撃ノード数を変化
        number_of_malnodes = i;
        AttackerSet();
        //AttackerSetFromFile(); //攻撃ノード指定
        for (int j = 1; j <= 3; j++)
        {
            set_simulate_mode(j);
            for (int k = 0; k < cnt_simulation; k++)
            {
                simulate();
            }
            cout << "Done mode " << mode << " numberofmalnodes" << number_of_malnodes << endl;
        }
    }
    return 0;
}