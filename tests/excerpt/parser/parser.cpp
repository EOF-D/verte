/**
 * @file parser.cpp
 * @brief Tests for the parser.
 */

#include "excerpt/parser/parser.hpp"
#include "excerpt/lexer/token.hpp"

#include <gtest/gtest.h>

using namespace excerpt;

// clang-format off
TEST(ParserTest, TestProgramAST) {
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

  Parser parser(tokens);
  std::unique_ptr<ProgramAST> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  FuncDeclNode &funcDecl = dynamic_cast<FuncDeclNode &>(*ast->getBody()[0]);
  ProtoNode &proto = funcDecl.getProto();

  EXPECT_EQ(proto.getName(), "main");
  EXPECT_EQ(proto.getParams().size(), 0);
  EXPECT_EQ(proto.getRet().type, TypeInfo::Type::INTEGER);

  BlockNode &block = funcDecl.getBody();
  ASSERT_EQ(block.getStmts().size(), 1);

  ReturnNode &retNode = dynamic_cast<ReturnNode &>(*block.getStmts()[0]);
  LiteralNode &literal = dynamic_cast<LiteralNode &>(*retNode.getExpr());

  LiteralValue value = literal.getValue();
  EXPECT_EQ(literal.getRaw(), "100");
  EXPECT_EQ(std::get<llvm::APSInt>(value).getSExtValue(), 100);
}

TEST(ParserTest, TestVarDecl) {
  std::vector<Token> tokens = {
    Token("foo", Token::Type::IDENTIFIER, {1, 1}),
    Token(":", Token::Type::COLON, {1, 2}),
    Token("int", Token::Type::IDENTIFIER, {1, 4}),
    Token("=", Token::Type::EQUAL, {1, 8}),
    Token("100", Token::Type::NUMBER, {1, 10}),
    Token(";", Token::Type::SEMICOLON, {1, 12}),
    Token("", Token::Type::END, {1, 13})
  };

  Parser parser(tokens);
  std::unique_ptr<ProgramAST> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  VarDeclNode &varDecl = dynamic_cast<VarDeclNode &>(*ast->getBody()[0]);

  EXPECT_EQ(varDecl.getName(), "foo");
  EXPECT_EQ(varDecl.getTypeInfo().type, TypeInfo::Type::INTEGER);

  LiteralNode &literal = dynamic_cast<LiteralNode &>(*varDecl.getValue());
  LiteralValue value = literal.getValue();

  EXPECT_EQ(literal.getRaw(), "100");
  EXPECT_EQ(std::get<llvm::APSInt>(value).getSExtValue(), 100);
}

TEST(ParserTest, TestBinaryExpr) {
  std::vector<Token> tokens = {
    Token("2", Token::Type::NUMBER, {1, 1}),
    Token("+", Token::Type::PLUS, {1, 3}),
    Token("3", Token::Type::NUMBER, {1, 5}),
    Token("*", Token::Type::STAR, {1, 7}),
    Token("4", Token::Type::NUMBER, {1, 9}),
    Token(";", Token::Type::SEMICOLON, {1, 10}),
    Token("", Token::Type::END, {1, 11})
  };

  Parser parser(tokens);
  std::unique_ptr<ProgramAST> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  BinaryNode &binaryNode1 = dynamic_cast<BinaryNode &>(*ast->getBody()[0]);
  EXPECT_EQ(binaryNode1.getOp(), "+");

  LiteralNode &literal1 = dynamic_cast<LiteralNode &>(*binaryNode1.getLHS());
  LiteralValue value1 = literal1.getValue();
  EXPECT_EQ(std::get<llvm::APSInt>(value1).getSExtValue(), 2);

  BinaryNode &binaryNode2 = dynamic_cast<BinaryNode &>(*binaryNode1.getRHS());
  EXPECT_EQ(binaryNode2.getOp(), "*");

  LiteralNode &literal2 = dynamic_cast<LiteralNode &>(*binaryNode2.getLHS());
  LiteralValue value2 = literal2.getValue();
  EXPECT_EQ(std::get<llvm::APSInt>(value2).getSExtValue(), 3);

  LiteralNode &literal3 = dynamic_cast<LiteralNode &>(*binaryNode2.getRHS());
  LiteralValue value3 = literal3.getValue();
  EXPECT_EQ(std::get<llvm::APSInt>(value3).getSExtValue(), 4);
}

TEST(ParserTest, TestUnaryExpr) {
  std::vector<Token> tokens = {
    Token("-", Token::Type::MINUS, {1, 1}),
    Token("100", Token::Type::NUMBER, {1, 3}),
    Token(";", Token::Type::SEMICOLON, {1, 5}),
    Token("", Token::Type::END, {1, 6})
  };

  Parser parser(tokens);
  std::unique_ptr<ProgramAST> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  UnaryNode &unaryNode = dynamic_cast<UnaryNode &>(*ast->getBody()[0]);
  EXPECT_EQ(unaryNode.getOp(), "-");

  LiteralNode &literal = dynamic_cast<LiteralNode &>(*unaryNode.getExpr());
  LiteralValue value = literal.getValue();
  EXPECT_EQ(std::get<llvm::APSInt>(value).getSExtValue(), 100);
}

TEST(ParserTest, TestCallExpr) {
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

  Parser parser(tokens);
  std::unique_ptr<ProgramAST> ast = parser.parse();
  ASSERT_EQ(ast->getBody().size(), 1);

  CallNode &callNode = dynamic_cast<CallNode &>(*ast->getBody()[0]);
  IdentNode &ident = dynamic_cast<IdentNode &>(*callNode.getCallee());
  EXPECT_EQ(ident.getName(), "foo");
  ASSERT_EQ(callNode.getArgs().size(), 2);

  LiteralNode &literal1 = dynamic_cast<LiteralNode &>(*callNode.getArgs()[0]);
  LiteralValue value1 = literal1.getValue();
  EXPECT_EQ(std::get<llvm::APSInt>(value1).getSExtValue(), 100);

  LiteralNode &literal2 = dynamic_cast<LiteralNode &>(*callNode.getArgs()[1]);
  LiteralValue value2 = literal2.getValue();
  EXPECT_EQ(std::get<std::string>(value2), "\"hello\"");
}
