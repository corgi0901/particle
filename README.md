# particle
This is a self-made programming language for my own learning. As this language can do only a few things, I named it "particle"

## How to use
```
$ git clone git@github.com:corgi0901/particle.git
$ cd particle
$ make
$ ./particle
または
$ ./particle <source file>
```

## (Current) Language specification
### Variable
Maximum 64 characters. You can use only a〜z, A〜Z, _, 0〜9. Variable supports only signed integer.

Following words are reserved, so you can't use these words as variable.

`if, else, while, end, func, return`

---
### Operator
Following operators are available.

`=, +, -, *, /, %, >, <, !, +=, -=, *=, /=, %=, >=, <=, ==, !=, ,(comma)`

---
### Control syntax
Conditional branching by "if" and "else" is possible.
```
if (a < 10)
  print(1)
else
  print(0)
end
```

Repeating  by "while" is possible.
```
while (a < 10)
  a += 1
  print(a)
end
```
---
### Built-in function
| Function | Description |
----|----
| print() | Display value |
| exit()  | Exit |

---
### Function definition
You can define original function. Recursive calling is also possible.
Following function is an example calcurating the Fibonacci number.

```
func fib(n)
  if (n <= 2)
    return 1
  else
    return fib(n-1) + fib(n-2)
  end
end
```

---
### Comment
You can use line comment by "#"


---
## Example of use
```
$ ./particle
>>> func fib(n)
...   if (n <= 2)
...     return 1
...   else
...     return fib(n-1) + fib(n-2)
...   end
... end
>>>
>>> ret = fib(10)
>>> print(ret)
55
```
