parser grammar FluxParser;

options {
	tokenVocab = FluxLexer;
}

program:
	KwModule ':' Identifier ';' (
		functionDefinition
		| functionDeclaration
	)* mainBlock? EOF;

functionSignature: Identifier '(' parameterList? ')' '->' type;

functionDeclaration:
	KwExtern functionSignature
	| KwUsing Identifier '::' functionSignature;

functionDefinition:
	functionSignature statementBlock
	| functionSignature ':' expression ';';

parameterList: parameter (',' parameter)*;

parameter: Identifier ':' type;
mainBlock: KwMain ':' statementBlock;

statement:
	expressionStatement
	| returnStatement
	| variableDeclaration
	| loop
	| ifStatement;

expressionStatement: expression ';';

returnStatement: KwReturn expression? ';';

loop: KwWhile expression? ':' statementBlock;

ifStatement:
	KwIf expression ':' statementBlock elseIfStatement* elseStatement?;

elseIfStatement: KwElseif expression ':' statementBlock;

elseStatement: KwElse ':' statementBlock;

statementBlock: '{' statement* '}';

// TODO: either a: i32 = 2; or a := 2;
variableDeclaration: Identifier ':' type '=' expression ';';

expression:
	'(' expression ')'												# ParenExpr
	| literal														# LiteralExpr
	| Identifier													# IdentifierExpr
	| expression '[' expression ']'									# IndexExpr
	| functionCall													# CallExpr
	| ('-' | '!' | '++' | '--') expression							# PrefixUnaryExpr
	| expression ('++' | '--')										# PostfixUnaryExpr
	| expression ('*' | '/' | '%') expression						# BinaryExpr
	| expression ('+' | '-') expression								# BinaryExpr
	| expression '&' expression										# BinaryExpr
	| expression '^' expression										# BinaryExpr
	| expression '|' expression										# BinaryExpr
	| expression ('<' | '<=' | '==' | '!=' | '>' | '>=') expression	# BinaryExpr
	| expression '&&' expression									# BinaryExpr
	| expression '^^' expression									# BinaryExpr
	| expression '||' expression									# BinaryExpr
	| expression '?' expression ':' expression						# TernaryExpr
	| Identifier '=' expression										# AssignmentExpr;

functionCall: Identifier '(' expressionList? ')';

expressionList: expression (',' expression)*;

// TODO: custom types
type: builtinType | builtinType '[' IntLiteral ']';

builtinType:
	KwInt64
	| KwInt32
	| KwFloat64
	| KwFloat32
	| KwBool
	| KwString;

literal:
	IntLiteral
	| FloatLiteral
	| BoolLiteral
	| StringLiteral;