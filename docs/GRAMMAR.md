# Formal grammar

## Production rules

```math
\begin{align}

\text{[Program]} &\to \text{[Statement]}* \\

\text{[Statement]} &\to
    \begin{cases}
        \text{identifier() -> [QualifiedType] [Scope]} \\
        \text{return [Literal]};
    \end{cases} \\
 
\text{[Scope]} &\to \{ \text{[Statement]}* \} \\

\text{[QualifiedType]} &\to
    \begin{cases}
        \text{type: qualifier}* \\
        \text{type}
    \end{cases} \\

\text{[Literal]} &\to
    \begin{cases}
        \text{integer}
    \end{cases} \\

\end{align}
```
