# research2020
ORの実装・信頼値測定についてのメモ
## シミュレーションにおける設定について
## To do
## パケット送受信に関して
- トポロジ生成に関して
    - edgeの設定
        - edgeを手動で設定(現時点)
        - リンクの通信成功率を重みとして設定
        - ノード0，ノードn(最終的なノード数)が接続になるようにする
        - 信頼値測定が行えるようにするにはトポロジ生成も少し工夫しなければならない？
        - 1hopのノードリスト，2hopのノードリスト…のようにやる？
- ノードの設定
    - 接続しているノードの情報を与える
    - 最終的には座標からトポロジを生成したい
- パケットの送信
    - 最初にETXに応じて送信元から1ホップの送信優先度を決定する
        - ETXについてはダイクストラ法で計算する
        - もっとも良い経路がETXを計算した時点で確定するのではないか？
        - priority_queueをノードごとに持たせる
    - edgeがあるノードにブロードキャスト
        - 転送優先度の高いノードがパケットを転送する
    - 転送優先度の決定について
        - 送信元から1hopの場合とそうでない場合に分ける？
    - 信頼値が閾値以下のノードには送信しない(後で)
    - キューのパケットをコピーする必要がある
        - 例えばエッジが2つあったとき112233みたいにコピーして送信するため
        - 実際には，123123の順に送信
    - パケットのドロップについて(実例で考える)
        - 優先度がもっとも高いノードが送信する
        - 送信に失敗したパケットを送信する
        - 送信に失敗していないパケットは送信しない->ドロップする
        - 
    - パケットが不達かどうかは気にせず，例えば100回送信元がブロードキャストしたらそれ以上のブロードキャストは行わない
- パケットの受信
    - 受信時に信頼値を見る？
    - すでに受信していたらdropする
- パケットについて
    - uidで識別する形で行う
- 2ホップ以上先のノードがパケットを受信する場合
    - TBD
## ノード信頼値測定について
- 直接的な観察
    - リンクのある1ホップノードのパケットのドロップなどの挙動を観察
    - エッジのあるノードを観察する
    - 逆方向へ通知するスキーム
- 間接的な観察
    - リンクのある1ホップノードに直接観察可能なノードの挙動を観察させる
    - リンクがある1ホップノードかつ観察対象のノードとリンクがある=>リンクのある1ホップノードから直接的な信頼値を収集
    - 「近傍ノードに観察させる」部分が難しそう
- 最終的な信頼値の決定方法
    - 重みづけ
- 提案手法の部分について
    - 後で

## 評価について
- 悪意のあるノードと検知率(誤検知率も？)
- 悪意のあるノードとパケット配送率の関係
- パケットをドロップする確率と検出率との関係？
    - グレーホール攻撃の考察もできそう
- ////////////以下はできたら//////////////
    - 悪意のあるノードとルーチングコストの関係
        - ルーチングコスト:各ノードと固定宛先へのORの期待コスト(ETXなど)
    - スループット(クロックを計算すればできる)
        - **正確な** スループットの計算はちょっと難しい？

## 参考にしたサイト・論文等(今までのやつもまとめる)
- 実装について
    - [Link Probability Based Opportunistic Routing Metric in Wireless Network](https://ieeexplore.ieee.org/document/4797138)
    - [DFS (深さ優先探索) 超入門！ 〜 グラフ・アルゴリズムの世界への入口 〜【前編】](https://qiita.com/drken/items/4a7869c5e304883f539b)
    - [DFS (深さ優先探索) 超入門！ 〜 グラフ・アルゴリズムの世界への入口 〜【後編】](https://qiita.com/drken/items/a803d4fc4a727e02f7ba#4-%E3%82%B0%E3%83%A9%E3%83%95%E4%B8%8A%E3%81%AE%E6%A7%98%E3%80%85%E3%81%AA%E4%BE%8B%E9%A1%8C)
    - [ダイクストラ法による単一始点最短経路を求めるアルゴリズム](https://algo-logic.info/dijkstra/)
    - [お手軽 乱数実装【C++11】](https://qiita.com/Gaccho/items/dc312fb5a056505f0a9f)
    - [BFS (幅優先探索) 超入門！ 〜 キューを鮮やかに使いこなす 〜](https://qiita.com/drken/items/996d80bcae64649a6580)
    - [C, C++でのファイル入力をまとめてみたよ](https://qiita.com/Reed_X1319RAY/items/098596cda78e9c1a6bad)
- matplotlib
    - [ ファイルから読み込まれた2次元データのプロット，可視化，matplotlib](https://qiita.com/sci_Haru/items/4c4fb511521c4f4e816d)
    - [2016-04-21matplotlibで軸の値が小数になったりオフセット表現になったりするのを止める方法](https://minus9d.hatenablog.com/entry/2016/04/21/215532)
- その他
    - [uniform_real_distribution](https://cpprefjp.github.io/reference/random/uniform_real_distribution.html)
    - [cpprefjp - C++日本語リファレンス](https://cpprefjp.github.io/)
    - [グラフ理論(北海道大学)](https://ocw.hokudai.ac.jp/wp-content/uploads/2016/01/GraphTheory-2007-Note-all.pdf)
    - [グラフ探索アルゴリズムとその応用](https://www.ioi-jp.org/camp/2011/hosaka-graph.pdf)
        - 隣接リストなどについて
    - [グラフ描画用のツール](https://kyopro.hateblo.jp/entry/2018/04/26/204335)
    - [【二値分類の評価指標】再現率は見逃さない確率、適合率は誤検出しない確率](https://qiita.com/lilacs/items/703314640ac790e4a6c5)
    - [【Linux】エイリアスの設定方法](https://qiita.com/_ydah/items/18d242b632e5486fe912)
## その他事項・疑問・メモ
- gitの設定をした(Ubuntu18.04LTSで):kushiyama@mycomputerから
- 攻撃ノードは通信成功率を0とするとETXが無限になってゼロ除算が発生する
    - 仮の値をセットする？
    - リンクの信頼性ではないのでカット
- 乱数について
    - 初期値はランダムに与えている
    - 乱数はメルセンヌ・ツイスタを使用している
    - パケットの送信成功、失敗はベルヌーイ分布に従う
    - double型の乱数はuniform_real_distributionによって取得
        - 指定された範囲の値が等確率で発生するよう離散分布
    - [参考](https://cpprefjp.github.io/reference/random/uniform_real_distribution.html)
- すべてパケットが到着するまで待つと，パケット配送率が100パーセントになる(当たり前)
<!--
- 信頼値の更新
    - 100ごとにインターバルとかでも良いかも
    - パケットが100到達したごとに更新
-->
- ExOR
    - バッチで送信している(現状だとパケット1こ)
- 深さ優先探索
    - ループがあっても可
- ETX
    - グローバル変数化してetxを求める？
    - 宛先が見つかるまで探索
    - ダイクストラ法を使って求めた
- 送信中止
- 重複パケットを減らしたい
    - 最初からベストなルートをある程度選んでおけば減らせる？
- 2ホップ以降の優先度の決定について
    - 中継ノードが転送優先度を決めるようにしたい
    - あらかじめ計算しておく
- BFS
    - hop数を取得するのに用いる
- routing1からの変更
    - BroadcastFromSource,SendlessとSendHighestを変更
## 実装に関するTo do とメモ
- To do(12/8)
    <!--
    - 理論計算
        - 特定のトポロジーを作って試す
    -->
    - 送受信の分離
        - 正しい引数さえ与えれば簡単にできそう
            - Highestとlowerの場合で異なるので注意
        - そこで下に書いているようなことをやればラウンドの定義は容易になるのではないか
    - トポロジのランダム生成
        - ~~ノード数を増やす~~
        - 通信成功率の設定
    - 攻撃の実装
        - 送信の関数のところでやる？
        - for文が増える::ちょっと面倒
        - 完全に宛先までのルートが遮断される場合は棄却する::ダメかも
        - 受信に失敗した場合...betaのカウントを増やす
        - Highとlessに書く
            - num_edge.to が受信
            - そのために送受信を分ける
    - 信頼値測定の実装
        - dtv...エッジのあるノードを観察
            - カウントにラウンドをもたせる必要がある
        - itv...1hopノード内で観察，あるノードから1hopノードは互いに通信できると仮定
        - 悪意のあるノードにはパケットを送らないようにする仕組みが必要
        - ブラックホール攻撃をしているときに観察をどうやるか
    - 時系列っぽいもの
        - ラウンドの更新を行うタイミングをどこにするか
        - スレッドでやってないので，送信元がnパケットを送信したらなどは不可
        - 宛先がnパケットを受信したらでやる（ブロードキャストのfor文の中には書きたくない）
            - 受信を関数にする
            - 分ける
        - 「宛先がnパケットを受信した」の部分はrecvmapで判別
        - ホップ数のfor文で回していたらラウンド数の受信を余裕で超えそう
        - ちょうどn個受信したみたいな仕組みを作るのは難しいかもしれない
        - 宛先の受信のたびに調べる（非効率な計算）
        - 
        - 
    - ルートを求める関数を作る
- To do(12/12)
    - PDRに応じてしか信頼値が更新されない(例:PDRが0.29->2回)
    - 送信の動作を観察する
    - そもそも検知してない
        - 周辺ノードのカウントをやっていなそう
        - Black...をキーに検索
        - 異常に低い値になってる
        - マイナスnanの理由はitvにありそう
    - 検出するには神視点が必要
        - 自律的に検出するのが難しい
        - modeで観察しているかしていないかを切り替えている
        - 
    - パケットは収束するまで（ラウンドが終了するまで送り続ける？）
        - 宛先への送信において失敗もカウントしてラウンド更新を行う？
        - 送信元が一回のブロードキャストのみだから問題になりそう
    - 検出したら優先度を変更するとかもやりたい
- 不具合
    - 優先度がもっとも高いノードが送信するとき，重複を避けないケース
- To do(12/14)
    - 12/12参照
    - on.itvは要求のたび書き換わるので問題ないはず
    - ラウンドの更新のタイミングがおかしい
    - 5のぶんを4でカウント？
        - よくよく考えたらalpha,betaをどちらも増やしている
        - それなら正しい？
        - dtvの測定自体はできていそう
        - itvで死んでる
            - 0/0をやっている
    - 周辺ノードの計算を入れてなかった？->入れる
        - CalTrust_and_Filteringの前半でやっていた
    - 送信のカウントもやっていない
- To do(12/15)
    - ds_allの計算はうまくいっていそう
    - ds_trustもすべてのノードから悪意のあるノードであると認識されていたら0になるからOKだと思われる
    - 信頼値測定のタイミングについて考察する
        - 上述のようにPDRによる更新しかなされない
        - 再送する？？？
    - ノードを増やす
- メモ(12/16)
    - 送信の部分もカウントすべき
    - おかしくなったら12/15の最後に戻す
        <!--
        - 送信追加(12/16)でも可
        -->
        - コミットメッセージ: 重複追加(12/16)，dtvによる悪意ノードの表追加でも可
        - 重複時なども追加した
        - ルーチングに参加していないノードを排除したい
    - 追加したけど結果は変わらず…
    - dtvに基づく結果は利用しない？
    - ルーチングに参加していないノードでも測定を行っているため
    - 検出したときに途中でフロー量を変える仕組みを作る
    - 当たり前ではあるけど，高い優先度のノードが攻撃ノードだとPDRが低下する
    - 受信時に測定がそもそもおかしい
    - Hop数ごとに測定
    - 最大ホップがラウンドになる？

- To do(12/17)
    - 信頼値測定を送信する側に変える
    - 限定的な測定に変更する
- メモ(12/18)
    - routing3.cppを作ってソースコードを分割する
    - headerを新しく作った
    - research2020/program2で作業する
    - 変更
    - 測定方法を変更することで変な検知はしなくなったけど検知しなくなった
    - 検出率の求め方が変なので辞書的に求めるとかに変更する
    - 送信中
        - 他のノードが測定対象のノードを観察する
        - インタラクションがなくても特に問題はないはず...xxx
        - この方法だとnhop目の優先度が中盤または最後のノードでないと信頼値を測定できない？
    - ホップ数ごとに変更する
        - 送り終わってから測定するような感じになってる
        - 送信中に測定がしたい
- メモ(12/19)
    - sendmapがtrueになってるから測定がされない
    - roundのリセットを送信終了後にやればよい？
    - リセットしたら検知した
    - BroadCastIntermediateでsend_roundをリセットするタイミングをいろいろ変更する
- メモ(12/20)
    - キューをコピーするのはおかしい？
    - パケットをコピーする
    - キューをコピーしないで送信する
    - 送信周りを修正した
        - 送信周りは正しくなったと思う
    - やばくなったら12/20の最初のコミットか12/19のコミットのバージョンを取得する
    - 提案手法で誤検知が減るっぽい動作を一応した
    - 提案手法なしだとほぼ確実に誤検知をする
    - この通知をするときのオーバーヘッドを考えてない(今後の課題)
    - いらないものを消したい
    - double -> long doubleにした
- メモ(12/21)
    - 家と研究室で挙動が変わった
    - doubleに戻した
    - 研究室: g++(Ubuntu 9.3.0-17ubuntu1~20.04)9.3.0
        - double,1e-7の場合だとうまく行く？
        - ↑関係なし:等しいかの比較と勘違いしていた
    - betaが多すぎ（バグ？）
    - WSL(家):g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
        - 共有ライブラリのバージョンの都合でgdbで実行できなかった
    - 1ホップに攻撃ノードを配置した場合で回数を増やすと正確になる
    - 2ホップの場合は誤った検知が起こる
    - 送信したあとも前ホップによる信頼値の測定が必要(提案手法のためには)
    - タイミング send_roundをリセットするところをちょっと変更
- メモ(12/22)
    - 重みが1にならない
    - 多分重みを1にできた(ds_all,ds_trustに重みを変更する仕組みを追加)
    - commit (12/21):lablast
        - 1hop目に悪意のあるノードがない場合は正しそうな動きをしている
        - 0からの測定が無理
    - commit (12/22): 送信中ノードの測定関数を作った
        - 完全にできた？
        - 攻撃ノードを変える
- メモ(12/23)
    - 信頼値測定あり，提案手法なし(悪意ノード:3)
        - ノード2が悪意ノードの判定

    ```
    Node 0: malicious 3
    Node 0: malicious 2
    Node 1: malicious 3
    Node 1: malicious 5
    Node 2: malicious 3
    Node 2: malicious 4
    Node 2: malicious 5
    Node 3: malicious 5
    ```

    - 信頼値測定あり，提案手法あり(悪意ノード:3)

    ```
    Node 0: malicious 3
    Node 1: malicious 3
    Node 2: malicious 3
    Node 2: malicious 4
    Node 3: malicious 5
    ```

    - 上は気にしない
    - 結果(前ホップのカウントを減らす関数を作ったとき)
    - 提案手法なし

    ```
    Node 0: malicious 3
    Node 1: malicious 3
    Node 1: malicious 4
    Node 2: malicious 3
    Node 2: malicious 4
    Node 3: malicious 5
    Node 4: malicious 5
    Node 5: malicious 4
    ``` 

    - あり

    ```
    Node 0: malicious 3
    Node 1: malicious 3
    Node 1: malicious 4
    Node 2: malicious 3
    Node 2: malicious 4
    Node 3: malicious 5
    Node 4: malicious 5
    Node 5: malicious 4
    ```

メモ(12/24)
- 提案手法なし(attacker=3)

```
Node 0: malicious 3
Node 0: malicious 2
Node 1: malicious 3
Node 1: malicious 2
Node 1: malicious 5
Node 2: malicious 3
Node 2: malicious 5
Node 3: malicious 2
Node 3: malicious 5
Node 4: malicious 5
```

- あり

```
Node 0: malicious 3
Node 1: malicious 3
Node 2: malicious 3
Node 3: malicious 5
```

- 提案手法ありとなしで検知に違いを作ることができた
    - PDRに違いが出るようにする
- 問題点
    - ルーチングに参加しないと検出しない/誤検知が起こる
    - 孤立しないようなトポロジーを作る？
    - この前考えたような方法でやる？
- commit(12/24)
    - トポロジーの生成方法を変更
- メモ(12/25)
    - 評価指標に関してFP(False Positive)を求める
    - 悪意ノードは検出率が100％になっている
        - 有効なエッジが少ない可能性
    - フローを変更/流れを変える仕組みが必要->PDRが向上しない
- メモ(12/26)
    - routing4.cppを作る
    - 送信を何パケットかで打ち切る，分ける(numberofpackets=10000なら1000など)
        - 送信元がn個パケットを送信したら次ホップへ移る
        - simulate関数のところでパケット数を制御できる？<-最初にここを書く
            - 関数化してどのmodeでも呼び出せるようにする
            - テスト(mode = 0,1)のときに動作しなかったら困るので2,3と分ける？->ソースコードがとても長くなる
        - グローバル変数で初期化する必要のあるものを考えればいけそう
            - 優先度キュー
            - seen
        - 「悪意ノードには送信しない」仕組みを外部関数から決めたい
            - あまりBroadcastFrom～を変更したくない
            - roundsetとかにも注意する
            - 普通にif文制御で実装できると考えられる(ただし送信を担う関数の変更の必要がある)
    - routeがなかった場合の例外処理が必要
        - その時点で止める(トポロジーを変更させる)<-良くなさそうなやり方
        - 途中でなくなった場合もどうするか
        - 深さ優先探索のルートが1通りだったら配置によってはPDR=0もありうる
        - 動的なエッジの追加とかも考えなくてはならないかも
            - エッジの削除ができなかったのでできるかは不明(constになってる？)
            - 削除はできなくても追加ならできそう
    - 良いトポロジーを実験結果に採用する（少し恣意的であると考えられる）
    - ノードの配置の問題かもしれないけれども，そこまで提案手法ありとなしで差が出ない
    - エッジを取り除くのができた
    - PDRに4~6パーセントの差(N=7,malnode=1)の場合
    - バグが出る可能性があるので注意する(norouteを消す)
- メモ(12/27)
    - 一応PDRについてそれらしい結果が出た
    - 信頼値測定のタイミングによって誤検出を頻発している？

- 使ったコマンド
    - node xの宛先までのETXのみを取り出す

    ```
    $ ./routing4 | grep -E 'Node x :ETX'
    ```

- メモ(12/28)
    - 4*前ホップノードのノード数
    - 2の時？ 9
    - 3の時 11
    - 2の時 14

    ```
    2
    PDR: 0.48556
    Detection Rate: 0.8
    Count Missed : 13

    3
    PDR: 0.68232
    Detection Rate: 0.8
    Count Missed : 10
    ```

    - データをひとまずとってみる 
        - 0,1,2,3で分ける
        - 0はとりあえず省略
        - 問題:悪意ノード数を変化させるのが手動でないとできない
            - グローバル変数を止める？
            - argvで受け取る形にすればシェル等で利用できる
            - 止めたらできなそう
            - 初め5にしておく
        - 完全にランダムにする？
        - PDR:0のデータが出るケースが考えられる
    - 結果の命名規則
    - PDR
        - PDR-(実験モード)-(悪意のあるノード数).txt(それぞれ100行)
        - PDR-(実験モード)-all.txt(5-25or30の6行)にcppを使って平均を書き込み，（上のファイルから）求める
    - 検出率
        - DETECT-(実験モード)-(悪意のあるノード数).txt(それぞれ100行)
        - DETECT-(実験モード)-all.txt(5-25or30の6行)にcppを使って(上のファイルから)平均を求める
    
    - グラフ作成...pythonを使って書く？
- メモ(12/29)
    - Node配列をuniqueポインタにした
    - 結果のファイル書き込みを追加
    - argvも使えるようにしたい
    - //coutの一括変換でコメントを切った
- メモ(12/30)
    - bashを介さないでやる
    - bashはファイルの移動のみに用いる
    - modeと悪意ノード数を書いたファイルを作る
    - 進捗をもう少し細かくわかるようにする
    - 悪意ノードの種類を固定にする
    - 命名規則 : attacker-x.txt
    - 攻撃ノードの番号がかぶったときの処理をしていない(想定よりも少ないことがある)
    - やること
        - attacker~のテスト(済)
        - routing4のファイルを使った場合のテスト(済)
        - 実験
- メモ(12/31)
    - 考察(ディレクトリexp2に保存予定)
    - PDRについて
        - 攻撃のみの場合
            - 誤検知がないためPDRが高い
        - 信頼値測定ありの場合
            - 誤検知があるためPDRが低下する？
        - 信頼値測定あり・提案手法ありの場合
            - 誤検知は軽減されるが，ややPDRが低下する
    - 検知率について
        - 信頼値測定ありの場合
        - 信頼値測定あり・提案手法ありの場合
            - どちらも（ほぼまったく）同じ検出率
            - 誤検知の様子を見ないと比較することができない？
    - 考察(exp3に保存予定:12/31/22:50~,1/1)
        - もう一回走らせてダメならアルゴリズムを見直す
- メモ(1/1)...exp3
    - 1,3,2（ブラックホール攻撃のみ，提案手法ありの測定，提案手法なし測定）の順にPDRが高くなる傾向が見られた．提案手法では，信頼値測定を行わなかった場合よりもやや性能が低くなる場合もあるが，これは多少の誤検知が起こっているため，このような結果になると考えられる．
    - しかし，提案手法なしにおける測定では，前ホップノードに通知を行わなかった場合のノードは悪意ノードとして判断されるため，提案手法を用いることで，Opportuinistic Routingの転送優先度の特性を考慮することができたと考えられる．
    - 検出率については，ルーチングに参加しなかったノードは基本的に検出できないことから，このような結果になっており，ルーチングに参加するように配置を考えなければならない．
- メモ(1/2)
    - exp4をやる前:変更点
    - エッジの数をノード数*5未満とした
    - gm=1.01->1.001にした
    - 検出率に変化が出そうな仕組みを考える
- メモ(1/3)
    - ノードが送信する度にインタラクションをリセットしている？
        - 問題ない（たぶん）
        - ブロードキャストするノード->受信するノード:測定
        - ブロードキャストするノードから前ホップのノード->ブロードキャストするノード:測定
        - 1000パケット送信する度にラウンドをリセットした
        - 前ホップノードのリセットも追加した
        - False Positiveを測定する仕組みを作った
- メモ(1/4)
    - malnodes_arrayのリセットができてなかった
    - To do: トポロジーを入れ替える,パラメータを調節する
- メモ(1/6)
    - キュー内のパケットを再送できる仕組みを考える
    - 閾値を変更すればROCカーブが書ける？
    - 
- メモ(1/7)
    - コンパイル時に最適化
    - 原因を分析
        - 転送優先度によるものか
        - 送信失敗によるものか
- メモ(1/8)
    - 