/**
 * @file pretty.cpp
 * @brief Tests for the pretty printer.
 */

#include "excerpt/visitors/pretty.hpp"
#include <gtest/gtest.h>

#include <vector>

using namespace excerpt;

TEST(PrettyPrinterTest, TestPrintProgramAST) {
  PrettyPrinter printer(std::cout);
  std::vector<NodePtr> body;

  body.push_back(std::make_unique<LiteralNode>(
      "100", TypeInfo(TypeInfo::Type::INTEGER, false)));

  body.push_back(std::make_unique<LiteralNode>(
      "200", TypeInfo(TypeInfo::Type::FLOAT, false)));

  ProgramAST program(std::move(body));

  testing::internal::CaptureStdout();
  program.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Program:\n  Literal: 100\n  Literal: 200\n");
}

TEST(PrettyPrinterTest, TestPrintLiteral) {
  PrettyPrinter printer(std::cout);
  LiteralNode literal("100", TypeInfo(TypeInfo::Type::FLOAT, false));

  testing::internal::CaptureStdout();
  literal.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Literal: 100\n");
}

TEST(PrettyPrinterTest, PrintVarDecl) {
  PrettyPrinter printer(std::cout);
  LiteralNode literal("100", TypeInfo(TypeInfo::Type::INTEGER, false));
  VarDeclNode varDecl("foo", TypeInfo(TypeInfo::Type::INTEGER, false),
                      std::make_unique<LiteralNode>(literal));

  testing::internal::CaptureStdout();
  varDecl.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "VarDecl: foo : int\n  Literal: 100\n");
}

TEST(PrettyPrinterTest, PrintIdentNode) {
  PrettyPrinter printer(std::cout);
  IdentNode ident("foo");

  testing::internal::CaptureStdout();
  ident.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Ident: foo\n");
}

TEST(PrettyPrinterTest, PrintBinaryNode) {
  PrettyPrinter printer(std::cout);
  LiteralNode left("10", TypeInfo(TypeInfo::Type::INTEGER, false));
  LiteralNode right("20", TypeInfo(TypeInfo::Type::INTEGER, false));
  BinaryNode binary(std::make_unique<LiteralNode>(left),
                    std::make_unique<LiteralNode>(right), "+");

  testing::internal::CaptureStdout();
  binary.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Binary: +\n  Literal: 10\n  Literal: 20\n");
}

TEST(PrettyPrinterTest, PrintUnaryNode) {
  PrettyPrinter printer(std::cout);
  LiteralNode operand("100", TypeInfo(TypeInfo::Type::INTEGER, false));
  UnaryNode unary("-", std::make_unique<LiteralNode>(operand));

  testing::internal::CaptureStdout();
  unary.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Unary: -\n  Literal: 100\n");
}

TEST(PrettyPrinterTest, PrintProtoNode) {
  PrettyPrinter printer(std::cout);
  Params params = {Parameter("x", TypeInfo(TypeInfo::Type::INTEGER, false)),
                   Parameter("y", TypeInfo(TypeInfo::Type::FLOAT, false))};

  ProtoNode proto("foo", params, TypeInfo(TypeInfo::Type::FLOAT, false));

  testing::internal::CaptureStdout();
  proto.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output,
            "Proto: foo\n  Arg: x : int\n  Arg: y : float\n  Return: float\n");
}

TEST(PrettyPrinterTest, PrintBlockNode) {
  PrettyPrinter printer(std::cout);

  std::vector<NodePtr> stmts;
  LiteralNode literal("100", TypeInfo(TypeInfo::Type::INTEGER, false));

  stmts.push_back(std::make_unique<VarDeclNode>(
      "foo", TypeInfo(TypeInfo::Type::INTEGER, false),
      std::make_unique<LiteralNode>(literal)));

  BlockNode block(std::move(stmts));

  testing::internal::CaptureStdout();
  block.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Block:\n  VarDecl: foo : int\n    Literal: 100\n");
}

TEST(PrettyPrinterTest, PrintFuncDeclNode) {
  PrettyPrinter printer(std::cout);
  Params params = {Parameter("x", TypeInfo(TypeInfo::Type::INTEGER, false)),
                   Parameter("y", TypeInfo(TypeInfo::Type::FLOAT, false))};

  ProtoPtr proto = std::make_unique<ProtoNode>(
      "foo", params, TypeInfo(TypeInfo::Type::FLOAT, false));

  std::vector<NodePtr> stmts;
  LiteralNode literal("100", TypeInfo(TypeInfo::Type::INTEGER, false));

  stmts.push_back(std::make_unique<VarDeclNode>(
      "bar", TypeInfo(TypeInfo::Type::INTEGER, false),
      std::make_unique<LiteralNode>(literal)));

  BlockPtr block = std::make_unique<BlockNode>(std::move(stmts));
  FuncDeclNode funcDecl(std::move(proto), std::move(block));

  testing::internal::CaptureStdout();
  funcDecl.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "FuncDecl:\n"
                    "  Proto: foo\n"
                    "    Arg: x : int\n"
                    "    Arg: y : float\n"
                    "    Return: float\n"
                    "  Block:\n"
                    "    VarDecl: bar : int\n"
                    "      Literal: 100\n");
}
