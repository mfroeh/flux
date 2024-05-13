parser grammar FluxParser;

options {
	tokenVocab = FluxLexer;
}

// module: classDefinition* functionDefinition*;
module: functionDefinition*;

classDefinition:
	KwClass Identifier '{' fieldDeclaration+ functionDefinition* '}';

fieldDeclaration: Identifier ':' type ';';

functionDefinition:
	Identifier '(' parameterList? ')' ':' type block					# BlockFunction
	| Identifier '(' parameterList? ')' (':' type)? '=>' expression ';'	# LambdaFunction;

parameterList: parameter (',' parameter)*;

block: '{' statement* '}';

parameter: Identifier ':' type;

statement:
	expressionStatement
	| variableDeclaration
	| returnStatement
	| ifStatement
	| loop;

expressionStatement: expression ';';

variableDeclaration:
	KwLet Identifier (':' type)? '=' expression ';'
	| KwLet Identifier ':' type ';';

returnStatement: KwReturn expression? ';';

loop:
	KwWhile '(' expression ')' (block | '->' statement) # WhileLoop
	| KwFor '(' statement ';' expression ';' statement ')' (
		block
		| '->' statement
	) # ForLoop;

interval: ('[' | ')') expression ',' expression (']' | ')');

ifStatement:
	KwIf '(' expression ')' (block | '->' statement) elseStatement* elseStatement?;

elseIfStatement:
	KwElseif '(' expression ')' (block | '->' statement);

elseStatement: KwElse (block | '->' statement);

expression:
	'(' expression ')'												# ParenExpr
	| literal														# LiteralExpr
	| Identifier													# IdentifierExpr
	| expression '[' expression ']'									# ArrayRefExpr
	| Identifier '(' expressionList? ')'							# CallExpr
	| ('-' | '!') expression										# PrefixUnaryExpr
	| expression ('*' | '/' | '%') expression						# BinaryExpr
	| expression ('+' | '-') expression								# BinaryExpr
	| expression ('<' | '<=' | '==' | '!=' | '>' | '>=') expression	# BinaryExpr
	| expression '&&' expression									# BinaryExpr
	| expression '||' expression									# BinaryExpr
	| expression '?' expression ':' expression						# TernaryExpr
	| expression KwIn interval										# InIntervalExpr
	// must be IdentifierExpr or ArrayRefExpr
	| expression '=' expression									# AssignmentExpr
	| expression ('+' | '-' | '*' | '/' | '%') '=' expression	# CompoundAssignmentExpr;

expressionList: expression (',' expression)*;

type: builtinType | arrayType;
// type: builtinType | arrayType | Identifier;

arrayType: builtinType '[' expression ']';

builtinType:
	KwInt64
	| KwInt32
	| KwFloat64
	| KwFloat32
	| KwBool
	| KwString;

// todo: array literal
literal:
	IntLiteral
	| FloatLiteral
	| BoolLiteral
	| StringLiteral;