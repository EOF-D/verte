/**
 * @file ast.cpp
 * @brief Test for the AST nodes.
 */

#include "excerpt/parser/ast.hpp"
#include <gtest/gtest.h>

using namespace excerpt;

TEST(NodeTest, TestProgramNode) {
  std::vector<NodePtr> body;

  // Create sample nodes.
  NodePtr stmt1 =
      std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::DataType::INTEGER));

  NodePtr stmt2 =
      std::make_unique<LiteralNode>("2", TypeInfo(TypeInfo::DataType::INTEGER));

  NodePtr stmt3 =
      std::make_unique<LiteralNode>("3", TypeInfo(TypeInfo::DataType::INTEGER));

  body.push_back(std::move(stmt1));
  body.push_back(std::move(stmt2));
  body.push_back(std::move(stmt3));

  // Create a program node
  ProgramNode program(std::move(body));

  // Check the program body
  const auto &programBody = program.getBody();
  EXPECT_EQ(programBody.size(), 3u);

  // Check if the nodes are not null
  for (auto &stmtVariant : programBody) {
    std::visit([this](auto &stmtPtr) { EXPECT_NE(stmtPtr, nullptr); },
               stmtVariant);
  }
}

TEST(NodeTest, TestLiteralNode) {
  // Test integer literal
  LiteralNode intLiteral("100", TypeInfo(TypeInfo::DataType::INTEGER));
  EXPECT_EQ(intLiteral.getValue(), "100");
  EXPECT_EQ(intLiteral.getTypeInfo().dataType, TypeInfo::DataType::INTEGER);

  // Test float literal
  LiteralNode floatLiteral("100.0", TypeInfo(TypeInfo::DataType::FLOAT));
  EXPECT_EQ(floatLiteral.getValue(), "100.0");
  EXPECT_EQ(floatLiteral.getTypeInfo().dataType, TypeInfo::DataType::FLOAT);

  // Test string literal
  LiteralNode stringLiteral("hello", TypeInfo(TypeInfo::DataType::STRING));
  EXPECT_EQ(stringLiteral.getValue(), "hello");
  EXPECT_EQ(stringLiteral.getTypeInfo().dataType, TypeInfo::DataType::STRING);
}

TEST(NodeTest, TestVarDeclNode) {
  TypeInfo intType(TypeInfo::DataType::INTEGER);
  NodePtr value = std::make_unique<LiteralNode>("100", intType);
  VarDeclNode varDecl("foo", intType, std::move(value));

  EXPECT_EQ(varDecl.getName(), "foo");
  EXPECT_EQ(varDecl.getTypeInfo().dataType, TypeInfo::DataType::INTEGER);

  // Make sure the value is not nullptr
  std::visit([](const auto &valuePtr) { EXPECT_NE(valuePtr, nullptr); },
             varDecl.getValue());

  // Check the actual value inside the variant
  std::visit(
      [](const auto &valuePtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(valuePtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "100");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Value is not a LiteralNode";
        }
      },
      varDecl.getValue());
}

TEST(NodeTest, TestVariableNode) {
  VariableNode var("foo");
  EXPECT_EQ(var.getName(), "foo");
}

TEST(NodeTest, TestBinaryNode) {
  NodePtr lhs =
      std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::DataType::INTEGER));

  NodePtr rhs =
      std::make_unique<LiteralNode>("2", TypeInfo(TypeInfo::DataType::INTEGER));

  BinaryNode binary(std::move(lhs), std::move(rhs), "+");
  std::visit([](const auto &lhsPtr) { EXPECT_NE(lhsPtr, nullptr); },
             binary.getLHS());

  std::visit([](const auto &rhsPtr) { EXPECT_NE(rhsPtr, nullptr); },
             binary.getRHS());

  // Check the operator
  EXPECT_EQ(binary.getOp(), "+");

  // Check the actual values inside the variants
  std::visit(
      [](const auto &lhsPtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(lhsPtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "1");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Left-hand side is not a LiteralNode";
        }
      },
      binary.getLHS());

  std::visit(
      [](const auto &rhsPtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(rhsPtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "2");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Right-hand side is not a LiteralNode";
        }
      },
      binary.getRHS());
}

TEST(NodeTest, TestUnaryNode) {
  NodePtr expr = std::make_unique<LiteralNode>(
      "100", TypeInfo(TypeInfo::DataType::INTEGER));

  UnaryNode unary(std::move(expr), "!");
  std::visit([](const auto &operandPtr) { EXPECT_NE(operandPtr, nullptr); },
             unary.getOperand());

  // Check the operator
  EXPECT_EQ(unary.getOp(), "!");

  // Check the actual value inside the variant
  std::visit(
      [](const auto &operandPtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(operandPtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "100");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Operand is not a LiteralNode";
        }
      },
      unary.getOperand());
}

TEST(NodeTest, TestProtoNode) {
  TypeInfo intType(TypeInfo::DataType::INTEGER);
  std::vector<Parameter> params = {Parameter("bar", intType),
                                   Parameter("baz", intType)};

  ProtoNode proto("foo", params, intType);

  EXPECT_EQ(proto.getName(), "foo");
  EXPECT_EQ(proto.getParams().size(), 2u);
  EXPECT_EQ(proto.getRetType().dataType, TypeInfo::DataType::INTEGER);
}

TEST(NodeTest, TestBlockNode) {
  std::vector<NodePtr> stmts;
  NodePtr stmt1 =
      std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::DataType::INTEGER));

  NodePtr stmt2 =
      std::make_unique<LiteralNode>("2", TypeInfo(TypeInfo::DataType::INTEGER));

  stmts.push_back(std::move(stmt1));
  stmts.push_back(std::move(stmt2));

  BlockNode block(std::move(stmts));
  const auto &blockBody = block.getBody();
  EXPECT_EQ(blockBody.size(), 2u);

  // Check if the nodes are not nullptr
  for (const auto &stmtVariant : blockBody) {
    std::visit([](const auto &stmtPtr) { EXPECT_NE(stmtPtr, nullptr); },
               stmtVariant);
  }

  // Check the actual values inside the variants
  std::visit(
      [](const auto &stmtPtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(stmtPtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "1");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Statement is not a LiteralNode";
        }
      },
      blockBody[0]);

  std::visit(
      [](const auto &stmtPtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(stmtPtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "2");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Statement is not a LiteralNode";
        }
      },
      blockBody[1]);
}

TEST(NodeTest, TestFuncDeclNode) {
  std::vector<NodePtr> stmts;
  TypeInfo intType(TypeInfo::DataType::INTEGER);
  std::vector<Parameter> params = {Parameter("bar", intType)};

  NodePtr proto = std::make_unique<ProtoNode>("foo", params, intType);
  NodePtr stmt = std::make_unique<LiteralNode>("100", intType);

  stmts.push_back(std::move(stmt));

  NodePtr body = std::make_unique<BlockNode>(std::move(stmts));
  FuncDeclNode funcDecl(std::move(proto), std::move(body));

  // Check the prototype
  std::visit(
      [](const auto &protoPtr) {
        if (auto protoNodePtr = dynamic_cast<ProtoNode *>(protoPtr.get())) {
          EXPECT_EQ(protoNodePtr->getName(), "foo");
          EXPECT_EQ(protoNodePtr->getParams().size(), 1u);
          EXPECT_EQ(protoNodePtr->getRetType().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Prototype is not a ProtoNode";
        }
      },
      funcDecl.getProto());

  // Check the body
  std::visit(
      [](const auto &bodyPtr) {
        if (auto blockNodePtr = dynamic_cast<BlockNode *>(bodyPtr.get())) {
          const auto &bodyStmts = blockNodePtr->getBody();
          EXPECT_EQ(bodyStmts.size(), 1u);

          std::visit(
              [](const auto &stmtPtr) {
                if (auto literalPtr =
                        dynamic_cast<LiteralNode *>(stmtPtr.get())) {
                  EXPECT_EQ(literalPtr->getValue(), "100");
                  EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                            TypeInfo::DataType::INTEGER);
                } else {
                  FAIL() << "Statement is not a LiteralNode";
                }
              },
              bodyStmts[0]);
        } else {
          FAIL() << "Body is not a BlockNode";
        }
      },
      funcDecl.getBody());
}

TEST(NodeTest, TestCallNode) {
  NodePtr callee = std::make_unique<VariableNode>("foo");

  NodePtr arg1 =
      std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::DataType::INTEGER));

  NodePtr arg2 =
      std::make_unique<LiteralNode>("2", TypeInfo(TypeInfo::DataType::INTEGER));

  std::vector<NodePtr> args;
  args.push_back(std::move(arg1));
  args.push_back(std::move(arg2));

  CallNode call(std::move(callee), std::move(args));

  // Check the callee
  std::visit(
      [](const auto &calleePtr) {
        if (auto varNodePtr = dynamic_cast<VariableNode *>(calleePtr.get())) {
          EXPECT_EQ(varNodePtr->getName(), "foo");
        } else {
          FAIL() << "Callee is not a VariableNode";
        }
      },
      call.getCallee());

  // Check the arguments
  const auto &callArgs = call.getArgs();
  EXPECT_EQ(callArgs.size(), 2u);

  std::visit(
      [](const auto &argPtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(argPtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "1");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Argument is not a LiteralNode";
        }
      },
      callArgs[0]);

  std::visit(
      [](const auto &argPtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(argPtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "2");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Argument is not a LiteralNode";
        }
      },
      callArgs[1]);
}

TEST(NodeTest, TestReturnNode) {
  NodePtr expr = std::make_unique<LiteralNode>(
      "100", TypeInfo(TypeInfo::DataType::INTEGER));

  ReturnNode ret(std::move(expr));

  // Check the return value
  std::visit(
      [](const auto &exprPtr) {
        if (auto literalPtr = dynamic_cast<LiteralNode *>(exprPtr.get())) {
          EXPECT_EQ(literalPtr->getValue(), "100");
          EXPECT_EQ(literalPtr->getTypeInfo().dataType,
                    TypeInfo::DataType::INTEGER);
        } else {
          FAIL() << "Return value is not a LiteralNode";
        }
      },
      ret.getValue());
}
