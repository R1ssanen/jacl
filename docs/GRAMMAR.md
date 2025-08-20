# Formal grammar
## Production rules

```ebnf
unit := unit_statement*;

unit_statement := function_definition
        | function_declaration;

function_definition := function_header block;

function_declaration := EXTERN function_header ';';

function_header := FUNC IDENTIFIER '(' declaration_list ')' ('->' type);

declaration_list := variable_declaration (',' variable_declaration)*;

variable_declaration := type ':' IDENTIFIER;

block := '{' scope_statement* '}';

statement_expr := IDENTIFIER '(' expression (',' expression)* ')';

scope_statement := RETURN expression ';'
        | statement_expr ';'
        | block;

type := '*' type
        | INT32
        | U8;

expression := LITERAL
        | IDENTIFIER
        | '&' IDENTIFIER
        | statement_expr;
```
