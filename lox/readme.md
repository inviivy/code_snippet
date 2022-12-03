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
program         →   declaration* EOF
declaration     →   varDecl | statement
varDecl         →   "var" IDENTIFIER ("=" expression) ? ";"
statement       →   exprStmt | printStmt
exprStmt        →   expression";"
printStmt       →   "print" expression ";"
expression      →   assignment
assignment      →   IDENTIFIER "=" assignment | equality
equality        →   comparison ( ( "!=" | "==" ) comparison ) *
comparison      →   addition ( ( ">" | ">=" | "<" | "<=" ) addition ) *
addition        →   multiplication ( ( "-" | "+" ) multiplication ) *
multiplication  →   unary ( ( "/" | "*" ) unary ) *
unary           →   ( "!" | "-" ) unary | primary
primary         →   NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER
```



# reference
+ [Crafting Interpreters](http://www.craftinginterpreters.com/)