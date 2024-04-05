ulci (untyped lambda calculus interpreter)
------------------------------------------

`ulci` is an interpreter for the untyped lambda calculus, written in c.
It parses expressions such as `(\x.x y) z` and `(\x. x) (\y.y y)` and evaluates
them. Includes an assignment statement:
```
set Name = Expression
```

For example, after assigning `set id = \x.x`, `id id id x` evaluates to `x`.

It interprets files containing lambda expressions (or assignments) evaluating
each line (`./interpeter filename`). Run without parameters it starts an 
interactive shell.

Dependencies:

It uses the following submodules:

https://github.com/sheredom/utest.h 
https://github.com/ivmai/bdwgc 

```
git clone --recursive https://github.com/nsanmartin/untyped-lambda-calculus.git
```

or `git submodule update --init --recursive` if already cloned.

Build:

```
mkdir build
make build/interpreter
```


