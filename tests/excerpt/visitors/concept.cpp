/**
 * @file concept.cpp
 * @brief Test the visitor pattern.
 */

#include "excerpt/parser/ast.hpp"
#include <gtest/gtest.h>

using namespace excerpt;

class Foo : public ASTNode<Foo> {};
class Bar : public ASTNode<Bar> {};

class TestVisitor {
public:
  void visit(Foo &) { visitedFoo = true; }
  void visit(Bar &) { visitedBar = true; }

  bool visitedFoo = false;
  bool visitedBar = false;
};

class FakeVisitor {};

class VisitorPatternTest : public ::testing::Test {
protected:
  using Visitor = TestVisitor;

  // Create a visitor.
  Visitor visitor;
};

TEST_F(VisitorPatternTest, TestVisitorConcept) {
  EXPECT_TRUE((VisitorConcept<Visitor, Foo>));
  EXPECT_TRUE((VisitorConcept<Visitor, Bar>));
}

TEST_F(VisitorPatternTest, TestAcceptMethod) {
  Foo foo;
  Bar bar;

  foo.accept(visitor);
  bar.accept(visitor);

  // Check if the visitor visited each node.
  EXPECT_TRUE(visitor.visitedFoo);
  EXPECT_TRUE(visitor.visitedBar);
}

TEST_F(VisitorPatternTest, TestFakeVisitor) {
  EXPECT_FALSE((VisitorConcept<FakeVisitor, Foo>));
  EXPECT_FALSE((VisitorConcept<FakeVisitor, Bar>));
}
