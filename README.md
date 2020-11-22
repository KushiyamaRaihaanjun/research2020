# research2020
ORの実装・信頼値測定についてのメモ
## シミュレーションにおける設定について
## To do

- クロックの設定（信頼値の更新を考えると必要そう）
    - int型のクロック
    - どこでクロックを進めるかを考える

- トポロジ生成に関して
    - edgeの設定
        - edgeを手動で設定(現時点)
        - リンクの通信成功率を重みとして設定
        - ノード0，ノードn(最終的なノード数)が接続になるようにする
        - 
- ノードの設定
    - 接続しているノードの情報を与える
    - 最終的には座標からトポロジを生成する
- パケットの送信
    - edgeがあるノードにブロードキャスト
    - 信頼値が閾値以下のノードには送信しない
    - キューのパケットをコピーする必要がある
        - 例えばエッジが2つあったとき112233みたいにコピーして送信するため
        - 実際には，123123の順に送信
    - パケットが不達かどうかは気にせず，例えば100回送信元がブロードキャストしたらそれ以上のブロードキャストは行わない
- パケットの受信
    - 受信時に信頼値を見る？
    - 
- パケットについて
    - uidで識別する形で行う
- ノード信頼値測定について
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
        - 
- 評価について
    - 悪意のあるノードと検知率(誤検知率も？)
    - 悪意のあるノードとパケット配送率の関係
    - 悪意のあるノードとルーチングコストの関係
        - ルーチングコスト:各ノードと固定宛先へのORの期待コスト(ETXなど)
    - スループット(クロックを計算すればできる)

- その他事項
    - すべてパケットが到着するまで待つと，パケット配送率が100パーセントになる(当たり前)
    -