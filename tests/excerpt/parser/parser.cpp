/**
 * @file parser.cpp
 * @brief Tests for the parser.
 */

#include "excerpt/parser/parser.hpp"
#include <gtest/gtest.h>

using namespace excerpt;

TEST(ParserTest, TestProgramAST) {
  // clang-format off
  std::vector<Token> tokens = {
    Token("fn", Token::Type::FN, {1, 1}),
    Token("main", Token::Type::IDENTIFIER, {1, 4}),
    Token("(", Token::Type::LPAREN, {1, 8}),
    Token(")", Token::Type::RPAREN, {1, 9}),
    Token("-", Token::Type::MINUS, {1, 11}),
    Token(">", Token::Type::GREATER, {1, 13}),
    Token("int", Token::Type::IDENTIFIER, {1, 15}),
    Token("{", Token::Type::LBRACE, {2, 1}),
    Token("return", Token::Type::RETURN, {3, 3}),
    Token("100", Token::Type::NUMBER, {3, 10}),
    Token(";", Token::Type::SEMICOLON, {3, 12}),
    Token("}", Token::Type::RBRACE, {4, 1}),
    Token("", Token::Type::END, {4, 2})
  };
  // clang-format on

  Parser parser(tokens);
  std::unique_ptr<ProgramNode> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  FuncDeclNode &funcDecl = dynamic_cast<FuncDeclNode &>(
      *std::get<std::unique_ptr<FuncDeclNode>>(ast->getBody()[0]));

  ProtoNode &proto = dynamic_cast<ProtoNode &>(
      *std::get<std::unique_ptr<ProtoNode>>(funcDecl.getProto()));

  EXPECT_EQ(proto.getName(), "main");
  EXPECT_EQ(proto.getParams().size(), 0);
  EXPECT_EQ(proto.getRetType().dataType, TypeInfo::DataType::INTEGER);

  BlockNode &block = dynamic_cast<BlockNode &>(
      *std::get<std::unique_ptr<BlockNode>>(funcDecl.getBody()));

  ASSERT_EQ(block.getBody().size(), 1);

  ReturnNode &retNode = dynamic_cast<ReturnNode &>(
      *std::get<std::unique_ptr<ReturnNode>>(block.getBody()[0]));

  LiteralNode &literal = dynamic_cast<LiteralNode &>(
      *std::get<std::unique_ptr<LiteralNode>>(retNode.getValue()));

  EXPECT_EQ(literal.getValue(), "100");
  EXPECT_EQ(std::stoi(literal.getValue()), 100);
}

TEST(ParserTest, TestVarDecl) {
  // clang-format off
  std::vector<Token> tokens = {
    Token("foo", Token::Type::IDENTIFIER, {1, 1}),
    Token(":", Token::Type::COLON, {1, 2}),
    Token("int", Token::Type::IDENTIFIER, {1, 4}),
    Token("=", Token::Type::EQUAL, {1, 8}),
    Token("100", Token::Type::NUMBER, {1, 10}),
    Token(";", Token::Type::SEMICOLON, {1, 12}),
    Token("", Token::Type::END, {1, 13})
  };
  // clang-format on

  Parser parser(tokens);
  std::unique_ptr<ProgramNode> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  VarDeclNode &varDecl = dynamic_cast<VarDeclNode &>(
      *std::get<std::unique_ptr<VarDeclNode>>(ast->getBody()[0]));

  EXPECT_EQ(varDecl.getName(), "foo");
  EXPECT_EQ(varDecl.getTypeInfo().dataType, TypeInfo::DataType::INTEGER);

  LiteralNode &literal = dynamic_cast<LiteralNode &>(
      *std::get<std::unique_ptr<LiteralNode>>(varDecl.getValue()));

  EXPECT_EQ(literal.getValue(), "100");
  EXPECT_EQ(std::stoi(literal.getValue()), 100);
}

TEST(ParserTest, TestBinaryExpr) {
  // clang-format off
  std::vector<Token> tokens = {
    Token("2", Token::Type::NUMBER, {1, 1}),
    Token("+", Token::Type::PLUS, {1, 3}),
    Token("3", Token::Type::NUMBER, {1, 5}),
    Token("*", Token::Type::STAR, {1, 7}),
    Token("4", Token::Type::NUMBER, {1, 9}),
    Token(";", Token::Type::SEMICOLON, {1, 10}),
    Token("", Token::Type::END, {1, 11})
  };
  // clang-format on

  Parser parser(tokens);
  std::unique_ptr<ProgramNode> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  BinaryNode &binaryNode1 = dynamic_cast<BinaryNode &>(
      *std::get<std::unique_ptr<BinaryNode>>(ast->getBody()[0]));

  EXPECT_EQ(binaryNode1.getOp(), "+");

  LiteralNode &literal1 = dynamic_cast<LiteralNode &>(
      *std::get<std::unique_ptr<LiteralNode>>(binaryNode1.getLHS()));

  EXPECT_EQ(literal1.getValue(), "2");
  EXPECT_EQ(std::stoi(literal1.getValue()), 2);

  BinaryNode &binaryNode2 = dynamic_cast<BinaryNode &>(
      *std::get<std::unique_ptr<BinaryNode>>(binaryNode1.getRHS()));

  EXPECT_EQ(binaryNode2.getOp(), "*");

  LiteralNode &literal2 = dynamic_cast<LiteralNode &>(
      *std::get<std::unique_ptr<LiteralNode>>(binaryNode2.getLHS()));

  EXPECT_EQ(literal2.getValue(), "3");
  EXPECT_EQ(std::stoi(literal2.getValue()), 3);

  LiteralNode &literal3 = dynamic_cast<LiteralNode &>(
      *std::get<std::unique_ptr<LiteralNode>>(binaryNode2.getRHS()));

  EXPECT_EQ(literal3.getValue(), "4");
  EXPECT_EQ(std::stoi(literal3.getValue()), 4);
}

TEST(ParserTest, TestUnaryExpr) {
  // clang-format off
  std::vector<Token> tokens = {
    Token("-", Token::Type::MINUS, {1, 1}),
    Token("100", Token::Type::NUMBER, {1, 3}),
    Token(";", Token::Type::SEMICOLON, {1, 5}),
    Token("", Token::Type::END, {1, 6})
  };
  // clang-format on

  Parser parser(tokens);
  std::unique_ptr<ProgramNode> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  UnaryNode &unaryNode = dynamic_cast<UnaryNode &>(
      *std::get<std::unique_ptr<UnaryNode>>(ast->getBody()[0]));

  EXPECT_EQ(unaryNode.getOp(), "-");

  LiteralNode &literal = dynamic_cast<LiteralNode &>(
      *std::get<std::unique_ptr<LiteralNode>>(unaryNode.getOperand()));

  EXPECT_EQ(literal.getValue(), "100");
  EXPECT_EQ(std::stoi(literal.getValue()), 100);
}

TEST(ParserTest, TestCallExpr) {
  // clang-format off
  std::vector<Token> tokens = {
    Token("foo", Token::Type::IDENTIFIER, {1, 1}),
    Token("(", Token::Type::LPAREN, {1, 4}),
    Token("100", Token::Type::NUMBER, {1, 5}),
    Token(",", Token::Type::COMMA, {1, 7}),
    Token("\"hello\"", Token::Type::STRING, {1, 9}),
    Token(")", Token::Type::RPAREN, {1, 16}),
    Token(";", Token::Type::SEMICOLON, {1, 17}),
    Token("", Token::Type::END, {1, 18})
  };
  // clang-format on

  Parser parser(tokens);
  std::unique_ptr<ProgramNode> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  CallNode &callNode = dynamic_cast<CallNode &>(
      *std::get<std::unique_ptr<CallNode>>(ast->getBody()[0]));

  VariableNode &ident = dynamic_cast<VariableNode &>(
      *std::get<std::unique_ptr<VariableNode>>(callNode.getCallee()));

  EXPECT_EQ(ident.getName(), "foo");
  ASSERT_EQ(callNode.getArgs().size(), 2);

  LiteralNode &literal1 = dynamic_cast<LiteralNode &>(
      *std::get<std::unique_ptr<LiteralNode>>(callNode.getArgs()[0]));

  EXPECT_EQ(literal1.getValue(), "100");
  EXPECT_EQ(std::stoi(literal1.getValue()), 100);

  LiteralNode &literal2 = dynamic_cast<LiteralNode &>(
      *std::get<std::unique_ptr<LiteralNode>>(callNode.getArgs()[1]));

  EXPECT_EQ(literal2.getValue(), "\"hello\"");
}
