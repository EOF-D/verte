/**
 * @file pretty.cpp
 * @brief Tests for the pretty printer.
 */

#include "excerpt/visitors/pretty.hpp"
#include <gtest/gtest.h>

#include <vector>

using namespace excerpt;

TEST(PrettyPrinterTest, TestPrintProgramNode) {
  PrettyPrinter printer(std::cout);

  std::vector<NodePtr> body;
  body.push_back(std::make_unique<LiteralNode>(
      "100", TypeInfo(TypeInfo::DataType::INTEGER)));

  body.push_back(std::make_unique<LiteralNode>(
      "200", TypeInfo(TypeInfo::DataType::FLOAT)));

  ProgramNode program(std::move(body));

  testing::internal::CaptureStdout();
  program.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Program:\n  Literal: 100\n  Literal: 200\n");
}

TEST(PrettyPrinterTest, TestPrintLiteralNode) {
  PrettyPrinter printer(std::cout);
  LiteralNode literal("100", TypeInfo(TypeInfo::DataType::FLOAT));

  testing::internal::CaptureStdout();
  literal.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Literal: 100\n");
}

TEST(PrettyPrinterTest, TestPrintVarDeclNode) {
  PrettyPrinter printer(std::cout);
  LiteralNode literal("100", TypeInfo(TypeInfo::DataType::INTEGER));
  VarDeclNode varDecl("foo", TypeInfo(TypeInfo::DataType::INTEGER),
                      std::make_unique<LiteralNode>(literal));

  testing::internal::CaptureStdout();
  varDecl.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "VarDecl: foo : int\n  Literal: 100\n");
}

TEST(PrettyPrinterTest, TestPrintVariableNode) {
  PrettyPrinter printer(std::cout);
  VariableNode variable("foo");

  testing::internal::CaptureStdout();
  variable.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Variable: foo\n");
}

TEST(PrettyPrinterTest, TestPrintBinaryNode) {
  PrettyPrinter printer(std::cout);
  LiteralNode left("10", TypeInfo(TypeInfo::DataType::INTEGER));
  LiteralNode right("20", TypeInfo(TypeInfo::DataType::INTEGER));
  BinaryNode binary(std::make_unique<LiteralNode>(left),
                    std::make_unique<LiteralNode>(right), "+");

  testing::internal::CaptureStdout();
  binary.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Binary: +\n  Literal: 10\n  Literal: 20\n");
}

TEST(PrettyPrinterTest, TestPrintUnaryNode) {
  PrettyPrinter printer(std::cout);
  LiteralNode operand("100", TypeInfo(TypeInfo::DataType::INTEGER));
  UnaryNode unary(std::make_unique<LiteralNode>(operand), "-");

  testing::internal::CaptureStdout();
  unary.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Unary: -\n  Literal: 100\n");
}

TEST(PrettyPrinterTest, TestPrintProtoNode) {
  PrettyPrinter printer(std::cout);
  std::vector<Parameter> params = {
      Parameter("x", TypeInfo(TypeInfo::DataType::INTEGER)),
      Parameter("y", TypeInfo(TypeInfo::DataType::FLOAT))};

  ProtoNode proto("foo", params, TypeInfo(TypeInfo::DataType::FLOAT));

  testing::internal::CaptureStdout();
  proto.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output,
            "Proto: foo\n  Arg: x : int\n  Arg: y : float\n  Return: float\n");
}

TEST(PrettyPrinterTest, TestPrintBlockNode) {
  PrettyPrinter printer(std::cout);

  std::vector<NodePtr> stmts;
  LiteralNode literal("100", TypeInfo(TypeInfo::DataType::INTEGER));

  stmts.push_back(std::make_unique<VarDeclNode>(
      "foo", TypeInfo(TypeInfo::DataType::INTEGER),
      std::make_unique<LiteralNode>(literal)));

  BlockNode block(std::move(stmts));

  testing::internal::CaptureStdout();
  block.accept(printer);

  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Block:\n  VarDecl: foo : int\n    Literal: 100\n");
}

TEST(PrettyPrinterTest, TestPrintFuncDeclNode) {
  PrettyPrinter printer(std::cout);
  std::vector<Parameter> params = {
      Parameter("x", TypeInfo(TypeInfo::DataType::INTEGER)),
      Parameter("y", TypeInfo(TypeInfo::DataType::FLOAT))};

  NodePtr proto = std::make_unique<ProtoNode>(
      "foo", params, TypeInfo(TypeInfo::DataType::FLOAT));

  std::vector<NodePtr> stmts;
  LiteralNode literal("100", TypeInfo(TypeInfo::DataType::INTEGER));

  stmts.push_back(std::make_unique<VarDeclNode>(
      "bar", TypeInfo(TypeInfo::DataType::INTEGER),
      std::make_unique<LiteralNode>(literal)));

  NodePtr block = std::make_unique<BlockNode>(std::move(stmts));
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
