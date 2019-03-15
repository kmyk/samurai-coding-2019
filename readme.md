# 第7回 情報処理学会 国際人工知能プログラミングコンテスト: SamurAI Coding 2018-19

## 問題

自分と相手で同時に馬を走らせ、先にゴールした方の勝ち。
それぞれのターンで速度を高々1増減させられる。
壁マスにぶつかったり相手に後ろからぶつかられると、そのターンの移動はキャンセルされ速度は0になる。
沼マスに入ると、その移動後に速度が0になる。
ただしコースはすべて見えているわけではなく、自分か相手の先頭にいる方のすこし先までのみが見える。

## 方針

### 予選

1人ゲームとしてビームサーチ (幅 100, 深さ 4)。
評価関数はゴールからの4近傍での距離。

### 本戦

予選のをすこし修正して相手を攻撃するようにした。

相手視点でビームサーチ (幅 100, 深さ 2) して相手の動きをひとつ確定させる。
その後に、その情報を見ながら自分視点でビームサーチ (幅 100, 深さ 4)。
評価関数を修正し、相手を邪魔するような動きに加点する。

予選の提出も含めて、深さが浅いのでこれはビームサーチでなくてDFSでも構わない。
使い回しのためにビームサーチを2回行なったが、適切に相手の手を含めて4手程度まで全探索 (あるいはalpha-beta法など) をし、その葉からそれぞれビームサーチをするのが最善に思える。

## 考察

### 予選中

視界が狭すぎるので運要素が大きい。
そのような場合はまじめに予測しても徒労で、見える範囲を貪欲にやるべきというのが典型なのでまずはこれをする。

見えている範囲の壁や沼の要素は探索が上手くやってくれるので気にしなくてよい。
相手との干渉もお互いに適切にやれば差が付く要素ではない。
つまり視界の外の壁の配置のみが重要である。しかしこれは運。
決勝は1戦のみのトーナメント方式なことを踏まえると2重の運ゲーなので、まじめにやっても労力に見合わないと判断して撤退した。

### 決勝中

問題の把握のために簡単に書いた解だったがとりあえず提出しておいたら学生枠で通ってしまった。
福岡は行ったことがないので旅行をすることにし、しかし行くからには勝ちたいので決勝の提出は頑張ろうとした。

視界の外の壁の配置をいい感じにするヒューリスティックを探した。
コースの中央付近に寄せるようにすると失敗時のリカバリがしやすいだろうかなどと試したがはずれだった。
他は思い付かなかったので早めに諦め、相手の情報を使う修正をしてお茶を濁したものを最終提出とした。

### 終了後

コース予測が正解だったらしい。
配布された `course_generator.py` は乱数でコースを生成するが、すべてランダムではなく、コースのパーツをランダムに選んで組み合わせるという仕様だったらしい。
これによりパーツの下部分だけが見えれば上部分も見え、十分な精度のコース予測が可能だったようだ。
これは去年のSamurAI Coding 2017-2018の決勝に行った人の間では周知の事実だったようだが、私は行っておらず知らなかった。

優先権を考慮してか左側に寄るAIが多かった。
視界の外が予測できていない状態では中央に寄せるのが比較して適切に思えるが、コース予測が成功する前提があるとそうなるのかもしれない。
ここはよく分かっていない。

決勝を見る限りどの参加者も速度には基本的に制限をかけていなかった。
これは正しい判断であるように感じられる。
速度の絶対値に関わらず速度を調整できる量は一定であることやx方向の速度は0と考えてよいことから、ぶつからないように見てから回避をするためにはかなりの低速が要求されるはずである。
比較すべきは「高速で移動する利益とぶつかった場合の損害」と「低速で移動する損害とぶつからないことの利益」であるが、(きちんと検証はしていないが) 視界が狭い場合でも前者の方が得であるように感じられる。
もし視界が広い場合やコース予測が成功している場合はなおさらである。
しかしこれは予想であり、確実なことは分かっていない。

## 反省

決勝トーナメントでは、相手にy座標を負にする動きをされ、これによるランタイムエラーで失格となった。
仕様を勘違いしていた。
テストが甘かった。

去年の1位がコース予測によるものというのは後から参加記を探せば見つかった。
この手の調査をする必要があった。
そうでなくても `course_generator.py` を読むことは必要であった。
あるいは、予選の1位が異様な強さであったという情報から自力でコース予測に辿り着けるべきであった。

## 要望

-   今年は対戦サーバが公開されていなかった。運営のリソースが足りてないので難しいのだろうけど、参加の楽しさにかなり大きく影響するので来年はなんとか頑張って提供してほしい。
-   以下の文章を読んで予選と同じコードを決勝に提出したところ、予選と決勝で制約が異なっていたことにより初手ランタイムエラーとなって失格となった人が複数いた。普段からコンテストに参加してる人間にとっては「入力の大きさの制約は問題やゲームルールの一部」というのが自然だが (入力の大きさが変わると適切である解法も変わるので) 、そうでない人間にとってはそうでないということらしい。これは不幸な事故であって回避は難しそう。

    >   ### 決勝ルール
    >
    >   ルールページのゲームルールおよび決勝ルールをご確認ください（ゲームルールは予選と同じです）

-   決勝の形式は「1戦のみのトーナメント」だった。決勝の中継の盛り上りを考えるとこうしたくはなるが、参加者のやる気や結果への納得感にかなり大きく影響するのでやめてほしい。参加者としては「16人の総当たりを134回やって累計2010試合の勝率で判定」などを希望したいが、中継の都合を考えると「9試合して5本先取のトーナメント」などが妥当な折衷案であるように思う。

(不満を多く書いてしまったように思いますが、少なくとも私は楽しかったです。ありがとうございました。)
