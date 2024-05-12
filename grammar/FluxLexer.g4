lexer grammar FluxLexer;

// keywords
KwModule: 'mod';
KwFunctions: 'functions';
KwMain: 'main';
KwReturn: 'ret';
KwExtern: 'ext';
KwUsing: 'using';
KwIf: 'if';
KwElse: 'else';
KwElseif: 'elif';
KwWhile: 'while';

KwInt64: 'i64';
KwInt32: 'i32';
KwFloat64: 'f64';
KwFloat32: 'f32';
KwBool: 'bool';
KwString: 'str';


Init: ':=';
Assign: '=';

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
PlusPlus: '++';
MinusMinus: '--';

// bitwise logic
BitwiseAnd: '&';
BitwiseXor: '^';
BitwiseOr: '|';

// logic
Not: '!';
LogicalAnd: '&&';
LogicalOr: '||';
LogicalXor: '^^';

// surrounding
LeftParen: '(';
RightParent: ')';
LeftBrace: '{';
RightBrace: '}';
LeftBracket: '[';
RightBracket: ']';

// others
Colon: ':';
Semicolon: ';';
Comma: ',';
RightArrow: '->';
ModuleSeperator: '::';
TernaryOp: '?';

BoolLiteral: 'true' | 'false';
IntLiteral: '0' | [1-9][0-9]*;
FloatLiteral: IntLiteral '.' [0-9]* | '.' [0-9]+;
StringLiteral: '"' ~["\r\n]* '"';

Identifier: [a-zA-Z_][a-zA-Z0-9_]*;

// hidden
Comment: '//' ~[\r\n]* -> channel(HIDDEN);
Newline: ('\r\n' | [\r\n]) -> channel(HIDDEN);
Whitespace: [\t ] -> channel(HIDDEN);
