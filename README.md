# ulci (untyped lambda calculus interpreter)
------------------------------------------

`ulci` is an interpreter for the untyped lambda calculus, written in c.
It parses expressions such as `(\x.x y) z` and `(\x. x) (\y.y y)` and evaluates
them. Includes an assignment statement:


```
set Name = Expression
```


For example, after assigning `set id = \x.x`, `id id id x` evaluates to `x`.


It interprets files containing lambda expressions (or assignments) evaluating
each line (`./ulci filename`). Run without parameters it starts an 
interactive shell.


## Dependencies:

It uses the following submodule:

https://github.com/sheredom/utest.h 

```
git clone --recursive https://github.com/nsanmartin/untyped-lambda-calculus.git
```

or `git submodule update --init --recursive` if already cloned.

This submodue is only used to run `utests`. Neither `lexer` nor `ulci` (the interpreter)
bot `itests` need that dependency.

## Build:


```
mkdir build
make build/ulci
```


