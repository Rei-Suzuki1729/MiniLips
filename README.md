# MiniLips
 
MiniLisとはLipsというプログラミング言語の縮小版です
具体的にいえば、

- 数字のみ扱えます
- 四則演算ができます
- 数値比較できます(真偽判定)
- 変数、関数を定義、計算できます
 
# DEMO
そのまま返ってきます
```
> 1
1
```
前置記法で四則演算を書きます
```
> (+ 1 2)
3
```
変数に値を入れる(束縛する)
```
> (setq a 12)
t
>a
12
```
例にあるaをシンボルといいます
シンボルだけだと束縛された値が返ってきます
シンボルにクオート記号を付けるとシンボルそのものが返ってきます
```
> 'a
a
```
tは真偽の真を表します。またnilは偽を表しています。
```
> (> 2 1)
t
>(> 1 2)
nil
```
関数を定義。その次は計算された結果が返ってきます。
```
> (defun f(x) (+ x 10))
t
>(f 2)
12
```
条件分岐。例は階乗を再帰で計算しているところです
```
> (defun fact (n) (if (= n 0) 1(* n (fact (- n 1)))))
t
> (fact 3)
6
>
```
# Usage
## visual studioの場合
```
git clone https://github.com/Rei-Suzuki1729/MiniLips.git
```
MiniLips.slnをクリックするとソリューションエクスプローラーが開く
その状態でデバッグする。
## それ以外の場合
```
git clone https://github.com/Rei-Suzuki1729/MiniLips.git
cd MiniLips/MiniLips
gcc MiniLips.c -o MiniLips
```
> ~/MiniLips/MiniLipsというパスの中にcファイルとヘッダーファイルがあります

# Author
[Rei-Suzuki1729](https://github.com/Rei-Suzuki1729)