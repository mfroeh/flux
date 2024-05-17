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
	Identifier '(' parameterList? ')' ':' type block
	| Identifier '(' parameterList? ')' (':' type)? '=>' expression ';';

parameterList: parameter (',' parameter)*;

block: '{' statement* '}';

parameter: Identifier ':' type;

statement:
	expressionStatement
	| variableDeclaration
	| returnStatement
	| ifStatement
	| whileLoop
	| forLoop
	| standaloneBlock
	| functionDefinition;

standaloneBlock: block;

expressionStatement: expression ';';

variableDeclaration:
	KwLet Identifier (':' type)? '=' expression ';'
	| KwLet Identifier ':' type ';';

returnStatement: KwReturn expression? ';';

whileLoop: KwWhile '(' expression ')' (block | '->' statement);

forLoop:
	KwFor '(' statement expressionStatement statement ')' (
		block
		| '->' statement
	);

interval: ('[' | '(') expression ',' expression (']' | ')');

ifStatement:
	KwIf '(' expression ')' (block | '->' statement) elseIfStatement* elseBlock?;

elseIfStatement:
	KwElseif '(' expression ')' (block | '->' statement);

elseBlock: KwElse (block | '->' statement);

expression:
	'(' expression ')'												# ParenExpr
	| literal														# LiteralExpr
	| Identifier													# IdentifierExpr
	| expression '[' expression ']'									# ArrayRefExpr
	| Identifier '(' expressionList? ')'							# CallExpr
	| ('-' | '!' | '&' | '*') expression							# PrefixUnaryExpr
	| expression ('*' | '/' | '%') expression						# BinaryArithmeticExpr
	| expression ('+' | '-') expression								# BinaryArithmeticExpr
	| expression ('<' | '<=' | '==' | '!=' | '>' | '>=') expression	# BinaryCompExpr
	| expression '&&' expression									# BinaryLogicalExpr
	| expression '||' expression									# BinaryLogicalExpr
	| expression '?' expression ':' expression						# TernaryExpr
	| expression KwIn interval										# InIntervalExpr
	// must be IdentifierExpr or ArrayRefExpr
	| expression '=' expression									# AssignmentExpr
	| expression ('+' | '-' | '*' | '/' | '%') '=' expression	# CompoundAssignmentExpr;

expressionList: expression (',' expression)*;

type: pointerType | nonPointerType;

pointerType: nonPointerType '*'+;

nonPointerType: builtinType | arrayType;

arrayType: builtinType '[' IntLiteral ']';

builtinType:
	KwInt64
	| KwInt32
	| KwFloat64
	| KwFloat32
	| KwBool
	| KwString
	| KwVoid;

// todo: array literal
literal:
	IntLiteral
	| FloatLiteral
	| BoolLiteral
	| StringLiteral;