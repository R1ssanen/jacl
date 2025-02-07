# Formal grammar definition for JACLang
<b>bold_text</b> = token \
[brackets] = production

$$
\begin{align}
\text{[program]} &\to \text{[statement]}^*  \\
\text{[statement]} &\to \begin{cases}
    \text{declaration} \space \text{:= [expr]};  &= \text{initialisation}  \\
    \textbf{type:} \space \textbf{qual}^* \space \textbf{ident};  &= \text{declaration}  \\
    \textbf{ident} := \text{[expr]};  &= \text{assignment}  \\
    \text{[scope]};  \\
\end{cases}  \\
\text{[scope]} &\to \begin{cases}
	\text{<\textbf{ident}>} \text{[scope]}  &= \text{namespace}  \\
	\{ \text{[statement]}^* \}  \\
\end{cases}  \\
\text{[expr]} &\to \begin{cases}
    \text{[expr]} \space \textbf{bin\_op} \space \text{[expr]}  \\
    \text{[lit]}  \\
    \textbf{ident}  &\to \text{[expr]}  \\
    \textbf{f}"\{\text{[expr]}\}^*"  &\to \text{str\_lit}
\end{cases} \\
\text{[lit]} &\to \begin{cases}
    \textbf{int\_lit}  \\
    \textbf{float\_lit}  \\
    \textbf{str\_lit}
\end{cases}  \\
\end{align}
$$
