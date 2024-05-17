lexer grammar FluxLexer;

// control flow
KwIf: 'if';
KwElseif: 'elif';
KwElse: 'else';
KwWhile: 'while';
KwFor: 'for';
KwIn: 'in';

// builtin types
KwInt64: 'i64';
KwInt32: 'i32';
KwFloat64: 'f64';
KwFloat32: 'f32';
KwBool: 'bool';
KwString: 'str';

// other keywords
KwClass: 'class';
KwReturn: 'ret';
KwUsing: 'using';
KwLet: 'let';

// comparison
Eq: '==';
Neq: '!=';
Less: '<';
Leq: '<=';
Greater: '>';
Geq: '>=';

// arithmetic
Plus: '+';
Minus: '-';
Mul: '*';
Div: '/';
Modolu: '%';

// increment/decrement
// PlusPlus: '++';
// MinusMinus: '--';

// bitwise logic
// BitwiseAnd: '&';
// BitwiseXor: '^';
// BitwiseOr: '|';

// logic
Not: '!';
LogicalAnd: '&&';
LogicalOr: '||';

// surrounding
LeftParen: '(';
RightParen: ')';
LeftBrace: '{';
RightBrace: '}';
LeftBracket: '[';
RightBracket: ']';

// others
Assign: '=';
Colon: ':';
Semicolon: ';';
Comma: ',';
RightArrow: '->';
RIGHTArrow: '=>';
ModuleSeperator: '::';
TernaryOp: '?';
MemberAccess: '.';
Reference: '&';

BoolLiteral: 'true' | 'false';
IntLiteral: '0' | [1-9][0-9]*;
FloatLiteral: IntLiteral '.' [0-9]* | '.' [0-9]+;
StringLiteral: '"' ~["\r\n]* '"';

Identifier: [a-zA-Z][a-zA-Z0-9]*[_]?;

// hidden
Comment: '//' ~[\r\n]* -> channel(HIDDEN);
Newline: ('\r\n' | [\r\n]) -> channel(HIDDEN);
Whitespace: [\t ] -> channel(HIDDEN);
