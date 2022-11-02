# lox

# Syntax

```shell
calculaterProgram → expression

expression → addition
# multiplication ( ( "-" | "+" ) multiplication ) *
addition → multiplication

# unary ( ( "/"  | "*" ) unary) *
multiplication → unary

unary → ( "!" | "-" ) unary | primary

primary → NUMBER | "(" expression ")"

```

# reference
+ [Crafting Interpreters](http://www.craftinginterpreters.com/)