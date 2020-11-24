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
        - 
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
    - 
    - パケットが不達かどうかは気にせず，例えば100回送信元がブロードキャストしたらそれ以上のブロードキャストは行わない
- パケットの受信
    - 受信時に信頼値を見る？
    - すでに受信していたらdropする
- パケットについて
    - uidで識別する形で行う
- 2ホップ以上先のノードがパケットを受信する場合
    - 
## ノード信頼値測定について
- 直接的な観察
    - リンクのある1ホップノードのパケットのドロップなどの挙動を観察
    - 
    - 
- 間接的な観察
    - リンクのある1ホップノードに直接観察可能なノードの挙動を観察させる
    - リンクがある1ホップノードかつ観察対象のノードとリンクがある=>直接的な信頼値を収集
    - 
- 最終的な信頼値の決定方法
    - 重みづけ
- 提案手法の部分について
    - 後で

## 評価について
- 悪意のあるノードと検知率(誤検知率も？)
- 悪意のあるノードとパケット配送率の関係
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
- その他
    - 
## その他事項・疑問・メモ
- 攻撃ノードは通信成功率を0とするとETXが無限になってゼロ除算が発生する
    - 仮の値をセットする？
- すべてパケットが到着するまで待つと，パケット配送率が100パーセントになる(当たり前)
- クロックの設定（信頼値の更新を考えると必要そう）
    - 何型のクロック？
    - どこでクロックを進めるかを考える
    - 100ごとにインターバルとかでも良いかも
- STR(通信成功率)
    - 計算は難しくはなさそうだけど宛先までのETXで決める？
- ExOR
    - バッチで送信している(現状だとパケット1こ)
- 深さ優先探索
    - ループがあっても可
- ETX
    - グローバル変数化してetxを求める？
    - 宛先が見つかるまで探索
    - ダイクストラ法を使って求めた

- 重複パケットを減らしたい
    - 最初からベストなルートをある程度選んでおけば減らせる？
