## Production Rules

### Expression
$$
\newcommand{\or}{\space | \space}
\begin{align}

Expression &\to id = Expression \or SimpleExpression
\\
SimpleExpression &\to SimpleExpression\ \{\&\&\ \or ||\}\ Comparison \or Comparison
\\
Comparison &\to Comparison\ CompOptr\ Term \or Term
\\
Term &\to Term+Factor \or Term-Factor \or Factor
\\
Factor &\to Factor*Primary \or Factor/Primary \or Primary
\\
Primary &\to -Primary \or !Primary \or (Expression) \or Identifier \or num
\\
CompOptr &\to\ < \or <= \or == \or >= \or >
\end{align}
$$

### Program
$$
\begin{align}

Program &\to Decleration \or \epsilon
\\
Decleration &\to VarDecleration \or FunDecleration
\\
VarDecleration &\to Keyword\ Identifier; \or Keyword\ Identifier = Expression;
\\
FunDecleration &\to Keyword\ Identifier(Params) Block
\\
Params &\to ParamList \or \epsilon
\\
ParamList &\to ParamList,Param \or Param
\\
Param &\to Keyword\ Identifier
\\
Block &\to StatementList
\\
StatementList &\to StatementList\ Statement \or \epsilon
\\
Statement &\to ExpressionStatement \or ReturnStatement
\\
ExpressionStatement &\to Expression; \or ;
\\
ReturnStatement &\to return\ ExpressionStatement

\end{align}
$$


### Function node
```
                     dec
                    /   \
              fn_dec     next
             /      \
      fn_sign        fn_body
     /       \
ret_type    params
```

### Function Call node
```
                 fn_call
                 /     \
        param_list     next
        /        \
param_list       arg_2
         \
         arg_1
```