# particle
自身の勉強のために作りはじめた自作プログラミング言語です。

言語としてできることが素粒子レベルなので「particle（粒）」と名付けました。

## 使い方
```
$ git clone git@github.com:corgi0901/particle.git
$ cd particle
$ make
$ ./particle
または
$ ./particle <ソースファイル>
```

## （現在の）言語仕様
### 変数
最大64文字。使用可能な文字は「a〜z, A〜Z, _, 0〜9」のみ。

---
### 型
符号付き整数（int）のみ。

---
### 演算子
=, +, -, *, /, %, +=, -=, *=, /=, %=

---
### 制御構文（ifとかwhileとか）
なし

---
### 組み込み関数
print（値の表示）のみ

---
### コメント記法
#による行コメントに対応。

## 実行例
```
$ ./particle
> apple = 150
> num = 15
> price = apple * num
> print price
2250
```
