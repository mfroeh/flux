parser grammar FluxParser;

options {
	tokenVocab = FluxLexer;
}

module: (classDefinition | functionDefinition)*;

classDefinition:
	KwClass Identifier '{' fieldDeclaration* functionDefinition* '}';

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
	'(' expression ')'									# ParenExpr
	| literal											# LiteralExpr
	| Identifier										# VarRef
	| ('-' | '!' | '&' | '*') expression				# PrefixUnary
	| expression '.' Identifier							# FieldRef
	| expression '[' expression ']'						# ArrayRef
	| Identifier '(' expressionList? ')'				# FunctionCall
	| expression '.' Identifier '(' expressionList? ')'	# MethodCall
	| '[' expressionList ']'							# ArrayLiteral
	| Identifier '{' (Identifier ':' expression) (
		',' Identifier ':' expression
	)* '}'															# StructLiteral
	| expression ('*' | '/' | '%') expression						# BinaryArithmetic
	| expression ('+' | '-') expression								# BinaryArithmetic
	| expression ('<' | '<=' | '==' | '!=' | '>' | '>=') expression	# BinaryComp
	| expression '&&' expression									# BinaryLogical
	| expression '||' expression									# BinaryLogical
	| expression '?' expression ':' expression						# Ternary
	| expression KwIn interval										# InInterval
	| expression '=' expression										# Assignment
	| expression ('+' | '-' | '*' | '/' | '%') '=' expression		# CompoundAssignment;

expressionList: expression (',' expression);

type: scalarType | pointerType | arrayType;

pointerType: (scalarType | arrayType) '*'+;

arrayType: scalarType '*'* '[' IntLiteral ']';

scalarType: builtinType | Identifier;

builtinType:
	KwInt64
	| KwInt32
	| KwFloat64
	| KwFloat32
	| KwBool
	| KwString
	| KwVoid;

literal:
	IntLiteral
	| FloatLiteral
	| BoolLiteral
	| StringLiteral;