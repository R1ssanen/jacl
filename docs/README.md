# Formal grammar definition for JACLang

# \text{[unary\_expression]} &\to \text{unary\_op} \space \text{[expression]} \\

### Production Rules

$$\begin{align}
\text{[root]} &\to \text{[statement]}^* \\
\text{[statement]} &\to \begin{cases}
    exit \space \text{[expression]}; \\
    \text{type:} \space \text{identifier} := \text{[expression]}; \\
    \text{identifier} := \text{[expression]}; \\
\end{cases} \\
\text{[binary\_expression]} &\to \text{[expression]} \space \text{bin\_op} \space \text{[expression]} \\
\text{[expression]} &\to \begin{cases}
    \text{[binary\_expression]} \\
    \text{[literal]} \\
    \text{identifier} \\
    f"\{\text{identifier}\}^*" &= \text{format string}\\
\end{cases} \\
\text{[literal]} &\to \begin{cases}
    \text{int\_literal} \\
    \text{float\_literal} \\
    \text{str\_literal} \\
\end{cases} \\
\end{align}$$
