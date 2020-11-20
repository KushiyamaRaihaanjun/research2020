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
#include <random>
//#include <bits/stdc++.h>
using namespace std;
typedef long long int lli;
#define urept(soeji, start, n) for (int soeji = start; soeji < n; soeji++)
#define drept(soeji, start, n) for (int soeji = start; soeji > n; soeji--)

const int N = 3;        // number of nodes observer
double threshold = 0.6; // threshold
struct Node
{
    //alpha...number of packets successfully received
    //beta .. all of packets transmi
    uint32_t alpha;
    uint32_t beta;

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
    //beta .. all of packets transmi
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
uint32_t get_rand_range(int seed, uint32_t min_val, uint32_t max_val)
{
    // 乱数生成器
    static std::mt19937 mt32(seed);

    // [min_val, max_val] の一様分布整数 (int) の分布生成器
    std::uniform_int_distribution<uint32_t> get_rand_uni_int(min_val, max_val);

    // 乱数を生成
    return get_rand_uni_int(mt32);
}

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

int main(void)
{
    int inputseed;
    cout << "input seed: ";
    cin >> inputseed;
    Node A;           //observer
    double gm = 1.05; //punishment factor
    /*
    A.alpha = 0;
    A.beta = 0;
    A.state = 1;
    A.dtv = 1.0;
    A.itv = 0.0;
    */

    Node j[N];
    //suppose that A transmits 100 packets
    for (int i = 0; i < N; i++)
    {
        j[i].alpha = 70;
        j[i].beta = 100 - j[i].alpha;
    }
    //1 hop neighbor direct trust value from A's viewpoint
    /*for (int i = 0; i < N; i++)
    {
        j[i].dtv = (double)(j[i].alpha / (j[i].alpha + gm * j[i].beta));
    }*/
    //for test value
    j[0].dtv = 0.8;
    j[1].dtv = 0.7;
    j[2].dtv = 0.8;
    j[3].dtv = 0.7;
    for (int i = 0; i < N; i++)
    {
        if (j[i].dtv > threshold)
        {
            cout << "Node" << i << ": T "
                 << " dtv..." << j[i].dtv << endl;
            j[i].state = 1;
        }
        else
        {
            cout << "Node" << i << ": not T " << endl;
            j[i].state = 2;
        }
    }
    //suppose that j[i] transmits 100 packet to B
    ONode B; //observed node
    for (int i = 0; i < N; i++)
    {
        B.alpha[i] = 70;
        B.beta[i] = 100 - B.alpha[i];
    }
    //direct value from j[i]'s viewpoint
    for (int i = 0; i < N; i++)
    {
        B.dtv[i] = (double)(B.alpha[i] / (B.alpha[i] + gm * B.beta[i]));
    }
    for (int i = 0; i < N; i++)
    {
        if (B.dtv[i] > threshold)
        {
            cout << "Node" << i << " thinks B is T " << endl;
        }
        else
        {
            cout << "Node" << i << " thinks B is not T " << endl;
        }
    }
    //indirect trust value calculation
    B.set_itv(j);
    double sumH = ds_trust(B);
    double sumall = ds_all(B);

    B.itv = sumH / sumall;
    //ノード B のノード A の観察に基づく信頼値
    double BdtvfromA = 0.4;

    double trustvalueofB = 0.5 * BdtvfromA + 0.5 * B.itv;
    cout << "bel(H):" << sumH << endl;
    cout << "K: " << sumall << endl;
    cout << "B's Trustvalue: " << trustvalueofB << endl;
    if (trustvalueofB > threshold)
    {
        cout << "B is trustworthy node" << endl;
    }
    else
    {
        cout << "B is malcious node" << endl;
    }
    return 0;
}