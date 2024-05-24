/**
 * @file base.cpp
 * @brief Testing for base visitor class.
 */

#include "excerpt/visitors/base.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <unordered_set>

using namespace excerpt;
using namespace testing;

// clang-format off
class TestASTVisitor : public ASTVisitor {
public:
  #define DEFINE_VISIT_METHOD(NodeType)                                        \
    void visit(NodeType& node) override {                                      \
      visitedNodes.insert(node.getType());                                     \
    }

  std::unordered_set<ASTNode::Type> visitedNodes;

  // Declaring visit methods for each AST node.
  DEFINE_VISIT_METHOD(ProgramAST)
  DEFINE_VISIT_METHOD(LiteralNode)
  DEFINE_VISIT_METHOD(VarDeclNode)
  DEFINE_VISIT_METHOD(IdentNode)
  DEFINE_VISIT_METHOD(BinaryNode)
  DEFINE_VISIT_METHOD(UnaryNode)
  DEFINE_VISIT_METHOD(ProtoNode)
  DEFINE_VISIT_METHOD(BlockNode)
  DEFINE_VISIT_METHOD(FuncDeclNode)
  DEFINE_VISIT_METHOD(ReturnNode)
  DEFINE_VISIT_METHOD(CallNode)

  #undef DEFINE_VISIT_METHOD
};

class BaseVisitorTest : public ::testing::Test {
protected:
  void SetUp() override {
    visitor.visitedNodes.clear();
  }

  TestASTVisitor visitor;
};

TEST_F(BaseVisitorTest, TestVisitProgramAST) {
  ProgramAST program({});

  program.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::PROGRAM));
}

TEST_F(BaseVisitorTest, TestVisitLiteralNode) {
  LiteralNode literal("100", TypeInfo(TypeInfo::Type::INTEGER, false));

  literal.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::LITERAL));
}

TEST_F(BaseVisitorTest, TestVisitVarDeclNode) {
  VarDeclNode varDecl(
    "foo", TypeInfo(TypeInfo::Type::INTEGER, false),
    std::make_unique<LiteralNode>("100", TypeInfo(TypeInfo::Type::INTEGER, false))
  );

  varDecl.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::VAR_DECL));
}

TEST_F(BaseVisitorTest, TestVisitIdentNode) {
  IdentNode ident("foo");

  ident.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::IDENT));
}

TEST_F(BaseVisitorTest, TestVisitBinaryNode) {
  BinaryNode binary(
    std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::Type::INTEGER, false)),
    std::make_unique<LiteralNode>("2", TypeInfo(TypeInfo::Type::INTEGER, false)),
    "+"
  );

  binary.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::BINARY));
}

TEST_F(BaseVisitorTest, TestVisitUnaryNode) {
  UnaryNode unary(
    "-",
    std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::Type::INTEGER, false))
  );

  unary.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::UNARY));
}

TEST_F(BaseVisitorTest, TestVisitProtoNode) {
  Params params = {
    Parameter("bar", TypeInfo(TypeInfo::Type::INTEGER, false)),
    Parameter("baz", TypeInfo(TypeInfo::Type::INTEGER, false))
  };

  ProtoNode proto("foo", params, TypeInfo(TypeInfo::Type::INTEGER, false));

  proto.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::PROTO));
}

TEST_F(BaseVisitorTest, TestVisitBlockNode) {
  std::vector<NodePtr> body;
  body.push_back(std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::Type::INTEGER, false)));
  body.push_back(std::make_unique<LiteralNode>("2", TypeInfo(TypeInfo::Type::INTEGER, false)));

  BlockNode block(std::move(body));
  block.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::BLOCK));
}

TEST_F(BaseVisitorTest, TestVisitFuncDeclNode) {
  std::vector<NodePtr> body;
  Params params = {
    Parameter("bar", TypeInfo(TypeInfo::Type::INTEGER, false)),
    Parameter("baz", TypeInfo(TypeInfo::Type::INTEGER, false))
  };

  body.push_back(std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::Type::INTEGER, false)));
  body.push_back(std::make_unique<LiteralNode>("2", TypeInfo(TypeInfo::Type::INTEGER, false)));

  ProtoNode proto("foo", params, TypeInfo(TypeInfo::Type::INTEGER, false));
  FuncDeclNode funcDecl(std::make_unique<ProtoNode>(proto), std::make_unique<BlockNode>(std::move(body)));

  funcDecl.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::FUNC_DECL));
}

TEST_F(BaseVisitorTest, TestVisitReturnNode) {
  ReturnNode returnNode(std::make_unique<LiteralNode>("100", TypeInfo(TypeInfo::Type::INTEGER, false)));

  returnNode.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::RETURN));
}

TEST_F(BaseVisitorTest, TestVisitCallNode) {
  std::vector<NodePtr> args;
  args.push_back(std::make_unique<LiteralNode>("1", TypeInfo(TypeInfo::Type::INTEGER, false)));
  args.push_back(std::make_unique<LiteralNode>("2", TypeInfo(TypeInfo::Type::INTEGER, false)));
  CallNode callNode(std::make_unique<IdentNode>("foo"),std::move(args));

  callNode.accept(visitor);
  EXPECT_TRUE(visitor.visitedNodes.count(ASTNode::Type::FUNC_CALL));
}
