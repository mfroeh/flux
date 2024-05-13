#include "ast/ast_creator.hh"

// module
any AstCreator::visitModule(FluxParser::ModuleContext *ctx) {}

// classes
any AstCreator::visitClassDefinition(FluxParser::ClassDefinitionContext *ctx) {}
any AstCreator::visitFieldDeclaration(
    FluxParser::FieldDeclarationContext *ctx) {}

// functions
any AstCreator::visitBlockFunction(FluxParser::BlockFunctionContext *ctx) {}
any AstCreator::visitLambdaFunction(FluxParser::LambdaFunctionContext *ctx) {}
any AstCreator::visitParameterList(FluxParser::ParameterListContext *ctx) {}
any AstCreator::visitParameter(FluxParser::ParameterContext *ctx) {}

// statements
any AstCreator::visitBlock(FluxParser::BlockContext *ctx) {}
any AstCreator::visitStatement(FluxParser::StatementContext *ctx) {}
any AstCreator::visitExpressionStatement(
    FluxParser::ExpressionStatementContext *ctx) {}
any AstCreator::visitVariableDeclaration(
    FluxParser::VariableDeclarationContext *ctx) {}
any AstCreator::visitReturnStatement(FluxParser::ReturnStatementContext *ctx) {}
any AstCreator::visitWhileLoop(FluxParser::WhileLoopContext *ctx) {}
any AstCreator::visitForLoop(FluxParser::ForLoopContext *ctx) {}
any AstCreator::visitIfStatement(FluxParser::IfStatementContext *ctx) {}
any AstCreator::visitElseIfStatement(FluxParser::ElseIfStatementContext *ctx) {}
any AstCreator::visitElseStatement(FluxParser::ElseStatementContext *ctx) {}

// expressions
any AstCreator::visitPrefixUnaryExpr(FluxParser::PrefixUnaryExprContext *ctx) {}
any AstCreator::visitInIntervalExpr(FluxParser::InIntervalExprContext *ctx) {}
any AstCreator::visitArrayRefExpr(FluxParser::ArrayRefExprContext *ctx) {}
any AstCreator::visitIdentifierExpr(FluxParser::IdentifierExprContext *ctx) {}
any AstCreator::visitBinaryExpr(FluxParser::BinaryExprContext *ctx) {}
any AstCreator::visitCompoundAssignmentExpr(
    FluxParser::CompoundAssignmentExprContext *ctx) {}
any AstCreator::visitLiteralExpr(FluxParser::LiteralExprContext *ctx) {}
any AstCreator::visitCallExpr(FluxParser::CallExprContext *ctx) {}
any AstCreator::visitAssignmentExpr(FluxParser::AssignmentExprContext *ctx) {}
any AstCreator::visitParenExpr(FluxParser::ParenExprContext *ctx) {}
any AstCreator::visitTernaryExpr(FluxParser::TernaryExprContext *ctx) {}
any AstCreator::visitFunctionCall(FluxParser::FunctionCallContext *ctx) {}
any AstCreator::visitLiteral(FluxParser::LiteralContext *ctx) {}

// misc
any AstCreator::visitExpressionList(FluxParser::ExpressionListContext *ctx) {}
any AstCreator::visitType(FluxParser::TypeContext *ctx) {}
any AstCreator::visitArrayType(FluxParser::ArrayTypeContext *ctx) {}
any AstCreator::visitBuiltinType(FluxParser::BuiltinTypeContext *ctx) {}
any AstCreator::visitInterval(FluxParser::IntervalContext *ctx) {}