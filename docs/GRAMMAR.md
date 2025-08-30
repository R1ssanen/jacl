# Formal grammar
## Production rules

```ebnf
unit := unit_statement*;

unit_statement := function_definition
        | extern_block;

function_definition := function_header block;

extern_block := EXTERN '{' (function_header ';')* '}';

function_header := FUNC IDENTIFIER ('(' declaration_list ')')? ('->' type)?;

declaration_list := variable_declaration (',' variable_declaration)*;

expression_statement := IDENTIFIER '(' expression (',' expression)* ')'
        | IDENTIFIER ':=' expression
        | variable_declaration ':=' expression;
 
variable_declaration := type ':' IDENTIFIER;

block := '{' scope_statement* '}';

scope_statement := RETURN expression? ';'
        | expression_statement ';'
        | block;

type := '*' type
        | IDENTIFIER
        | INT32
        | UINT8;

expression := LITERAL
        | IDENTIFIER
        | '&' IDENTIFIER
        | expression_statement;
```
