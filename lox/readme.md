# lox

# Syntax(Calculator)

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


# Syntax(updated)
```shell
expression      →   equality
equality        →   comparison ( ( "!=" | "==" ) comparison ) *
comparison      →   addition ( ( ">" | ">=" | "<" | "<=" ) addition ) *
addition        →   multiplication ( ( "-" | "+" ) multiplication ) *
multiplication  →   unary ( ( "/" | "*" ) unary ) *
unary           →   ( "!" | "-" ) unary | primary
primary         →   NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
```



# reference
+ [Crafting Interpreters](http://www.craftinginterpreters.com/)