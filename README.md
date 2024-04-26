# ulci (untyped lambda calculus interpreter)
------------------------------------------

`ulci` is yet another interpreter for the untyped lambda calculus, written in c.
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

The only dependency needed is `readline`, but not strictly because the
interpreter can be built `#define`ing `NO_READLINE` or simply building
```
make build/ulci-no-readline
```

which will have the same behaviour when passing a file and obviously a diferent repl.


For (the ridiculously few) unit tests, it uses the following submodule:

https://github.com/sheredom/utest.h 

```
git clone --recursive https://github.com/nsanmartin/untyped-lambda-calculus.git
```

or `git submodule update --init --recursive` if already cloned.

This submodue is only used to run `build/utests`. Neither `lexer` nor `ulci` (the interpreter)
nor `itests` need that dependency.

## Build:


```
mkdir build
make build/ulci
```

## Run

You either pass expressions from `stdin` and use it interactively like:
$ ./build/ulci


```
> set apply = (\x.\y. x y)
\x.\y.x y
> set dup = (\x.x x)
\x.x x
> apply dup A
A A
>
```

and


```
$ echo "(\x.A) B" | ./build/ulci
> (\x.A) B
A
```


Or you can pass the paths of files containing valid ulc expressions

```
./ulci foo.ulc
$ ./build/ulci samples/0.ulc samples/1.ulc
\x.x
A
\t.y
```

