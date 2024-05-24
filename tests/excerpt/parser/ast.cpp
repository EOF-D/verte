/**
 * @file ast.cpp
 * @brief Tests for the AST.
 */

#include "excerpt/parser/ast.hpp"
#include <gtest/gtest.h>

using namespace excerpt;

TEST(NodeTest, TestProgramAST) {
  std::vector<NodePtr> body;

  // Create some sample nodes
  NodePtr stmt1 = std::make_unique<LiteralNode>(
      "1", TypeInfo(TypeInfo::Type::INTEGER, false));

  NodePtr stmt2 = std::make_unique<LiteralNode>(
      "2", TypeInfo(TypeInfo::Type::INTEGER, false));

  NodePtr stmt3 = std::make_unique<LiteralNode>(
      "3", TypeInfo(TypeInfo::Type::INTEGER, false));

  body.push_back(std::move(stmt1));
  body.push_back(std::move(stmt2));
  body.push_back(std::move(stmt3));

  // Create a ProgramAST instance
  ProgramAST program(std::move(body));

  // Check the program body
  const auto &programBody = program.getBody();
  EXPECT_EQ(programBody.size(), 3u);

  // Check that the nodes are not null
  EXPECT_NE(programBody[0], nullptr);
  EXPECT_NE(programBody[1], nullptr);
  EXPECT_NE(programBody[2], nullptr);

  // Check the types of the nodes
  EXPECT_EQ(programBody[0]->getType(), ASTNode::Type::LITERAL);
  EXPECT_EQ(programBody[1]->getType(), ASTNode::Type::LITERAL);
  EXPECT_EQ(programBody[2]->getType(), ASTNode::Type::LITERAL);
}

TEST(NodeTest, TestLiteralNode) {
  // Test integer literal
  LiteralNode intLiteral("42", TypeInfo(TypeInfo::Type::INTEGER, false));
  EXPECT_EQ(intLiteral.getRaw(), "42");
  EXPECT_EQ(intLiteral.getType().type, TypeInfo::Type::INTEGER);
  EXPECT_EQ(intLiteral.getValue().index(), 0); // Check if it's an APSInt

  // Test float literal
  LiteralNode floatLiteral("3.14", TypeInfo(TypeInfo::Type::FLOAT, false));
  EXPECT_EQ(floatLiteral.getRaw(), "3.14");
  EXPECT_EQ(floatLiteral.getType().type, TypeInfo::Type::FLOAT);
  EXPECT_EQ(floatLiteral.getValue().index(), 1); // Check if it's an APFloat

  // Test string literal
  LiteralNode stringLiteral("Hello", TypeInfo(TypeInfo::Type::STRING, false));
  EXPECT_EQ(stringLiteral.getRaw(), "Hello");
  EXPECT_EQ(stringLiteral.getType().type, TypeInfo::Type::STRING);
  EXPECT_EQ(stringLiteral.getValue().index(), 2); // Check if it's a string
}

TEST(NodeTest, TestVarDeclNode) {
  TypeInfo intType(TypeInfo::Type::INTEGER, false);
  NodePtr value = std::make_unique<LiteralNode>("42", intType);
  VarDeclNode varDecl("x", intType, std::move(value));

  EXPECT_EQ(varDecl.getName(), "x");
  EXPECT_EQ(varDecl.getType().type, TypeInfo::Type::INTEGER);
  EXPECT_NE(varDecl.getValue(), nullptr);
}

TEST(NodeTest, TesTIdentNode) {
  IdentNode ident("foo");
  EXPECT_EQ(ident.getName(), "foo");
}

TEST(NodeTest, TestBinaryNode) {
  NodePtr lhs = std::make_unique<LiteralNode>(
      "1", TypeInfo(TypeInfo::Type::INTEGER, false));

  NodePtr rhs = std::make_unique<LiteralNode>(
      "2", TypeInfo(TypeInfo::Type::INTEGER, false));

  BinaryNode binary(std::move(lhs), std::move(rhs), "+");
  EXPECT_NE(binary.getLHS(), nullptr);
  EXPECT_NE(binary.getRHS(), nullptr);
  EXPECT_EQ(binary.getOp(), "+");
}

TEST(NodeTest, TestUnaryNode) {
  NodePtr expr = std::make_unique<LiteralNode>(
      "42", TypeInfo(TypeInfo::Type::INTEGER, false));

  UnaryNode unary("!", std::move(expr));
  EXPECT_NE(unary.getExpr(), nullptr);
  EXPECT_EQ(unary.getOp(), "!");
}

TEST(NodeTest, TestProtoNode) {
  TypeInfo intType(TypeInfo::Type::INTEGER, false);
  Params params = {Parameter("x", intType), Parameter("y", intType)};
  ProtoNode proto("foo", params, intType);

  EXPECT_EQ(proto.getName(), "foo");
  EXPECT_EQ(proto.getParams().size(), 2u);
  EXPECT_EQ(proto.getRet().type, TypeInfo::Type::INTEGER);
}

TEST(NodeTest, TestBlockNode) {
  std::vector<NodePtr> stmts;
  NodePtr stmt1 = std::make_unique<LiteralNode>(
      "1", TypeInfo(TypeInfo::Type::INTEGER, false));

  NodePtr stmt2 = std::make_unique<LiteralNode>(
      "2", TypeInfo(TypeInfo::Type::INTEGER, false));

  stmts.push_back(std::move(stmt1));
  stmts.push_back(std::move(stmt2));

  BlockNode block(std::move(stmts));
  EXPECT_EQ(block.getStmts().size(), 2u);
}

TEST(NodeTest, TestFuncDeclNode) {
  std::vector<NodePtr> stmts;
  TypeInfo intType(TypeInfo::Type::INTEGER, false);
  Params params = {Parameter("x", intType)};

  ProtoPtr proto = std::make_unique<ProtoNode>("foo", params, intType);
  NodePtr stmt = std::make_unique<LiteralNode>("42", intType);

  stmts.push_back(std::move(stmt));

  BlockPtr body = std::make_unique<BlockNode>(std::move(stmts));
  FuncDeclNode funcDecl(std::move(proto), std::move(body));

  EXPECT_EQ(funcDecl.getProto().getName(), "foo");
  EXPECT_EQ(funcDecl.getBody().getStmts().size(), 1u);
}

TEST(NodeTest, TestReturnNode) {
  NodePtr expr = std::make_unique<LiteralNode>(
      "42", TypeInfo(TypeInfo::Type::INTEGER, false));

  ReturnNode ret(std::move(expr));
  EXPECT_NE(ret.getExpr(), nullptr);
}

TEST(NodeTest, TestCallNode) {
  NodePtr callee = std::make_unique<IdentNode>("foo");
  NodePtr arg1 = std::make_unique<LiteralNode>(
      "1", TypeInfo(TypeInfo::Type::INTEGER, false));

  NodePtr arg2 = std::make_unique<LiteralNode>(
      "2", TypeInfo(TypeInfo::Type::INTEGER, false));

  std::vector<NodePtr> args;
  args.push_back(std::move(arg1));
  args.push_back(std::move(arg2));

  CallNode call(std::move(callee), std::move(args));
  EXPECT_NE(call.getCallee(), nullptr);
  EXPECT_EQ(call.getArgs().size(), 2u);
}
