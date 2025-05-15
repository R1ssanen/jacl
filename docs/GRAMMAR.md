# Formal grammar

## Production rules

```math
\begin{align}

&\text{[Program]} \to \text{[RootStatement]}* \\

&\text{[RootStatement]} \to
    \begin{cases}
        \text{qual* ident() -> [QualType] [Scope]} \\
        \text{qual* ident() -> [QualType]};
    \end{cases} \\

&\text{[Scope]} \to \text{\{ [Statement]* \}} \\

&\text{[Statement]} \to
    \begin{cases}
        \text{identifier()}; \\
        return \space \text{[Literal]}; \\
        \text{[Scope]}
    \end{cases} \\

&\text{[QualType]} \to
    \begin{cases}
        \text{type: qual}* \\
        \text{type}
    \end{cases} \\

&\text{[Literal]} \to
    \begin{cases}
        \text{integer}
    \end{cases} \\

\end{align}
```
