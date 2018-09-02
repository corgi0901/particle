# particle
自身の勉強のために作りはじめた自作プログラミング言語です。豆粒程度のことしかできないので「particle（粒）」と名付けました。

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
=, +, -, *, /, %, >, <, !, +=, -=, *=, /=, %=, >=, <=, ==, !=,',(comma)'

---
### 制御構文（ifとかwhileとか）
if-else-fiによる条件分岐が可能です。
```
if (a < 10)
  print 1
else
  print 0
fi
```
---
### 組み込み関数
print（値の表示）のみ。

---
### 関数
関数定義に対応。再帰呼び出しもできるので、以下のようなフィボナッチ数を計算する関数くらいなら動きます。
```
func fib(n)
  if (n <= 2)
    return 1
  else
    return fib(n-1) + fib(n-2)
  fi
end
```

---
### コメント記法
#による行コメントに対応。

## 実行例
```
$ ./particle
> func fib(n)
>   if (n <= 2)
>     return 1
>   else
>     return fib(n-1) + fib(n-2)
>   fi
> end
>
> ret = fib(8)
> print ret
21
```
