#include "verte/backend/codegen/codegen.hpp"
#include "verte/frontend/lexer/lexer.hpp"
#include "verte/frontend/parser/parser.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing;
using namespace verte;

class CodegenTest : public ::testing::Test {
protected:
  void SetUp() override {}

  std::unique_ptr<codegen::Codegen> compileSource(const std::string &source) {
    auto codegen = std::make_unique<codegen::Codegen>("test");

    lexer::Lexer lexer(source);
    nodes::Parser parser(lexer.allTokens());
    auto ast = parser.parse();
    ast->accept(*codegen);

    return codegen;
  }
};

TEST_F(CodegenTest, TestIntegerLiteral) {
  auto codegen = compileSource("let x: @int = 1;");
  const auto &module = codegen->getModule();

  ASSERT_EQ(module.getName(), "test");
}

TEST_F(CodegenTest, TestFloatLiteral) {
  auto codegen = compileSource("let x: @double = 1.0;");
  const auto &module = codegen->getModule();

  ASSERT_EQ(module.getName(), "test");
}

TEST_F(CodegenTest, TestBoolLiterals) {
  auto codegen = compileSource("let x: @bool = true; let y: @bool = false;");
  const auto &module = codegen->getModule();

  ASSERT_EQ(module.getName(), "test");
}

TEST_F(CodegenTest, TestStringLiteral) {
  auto codegen = compileSource("let msg: @str = \"foo\";");
  const auto &module = codegen->getModule();

  ASSERT_EQ(module.getName(), "test");
}

TEST_F(CodegenTest, TestEmptyFunction) {
  auto codegen = compileSource("fun foo(): @void do end");
  const auto &module = codegen->getModule();

  ASSERT_TRUE(module.hasFunction("foo"));

  auto *func = module.getFunction("foo");
  ASSERT_NE(func, nullptr);
  ASSERT_EQ(func->getName(), "foo");
  ASSERT_EQ(func->getParams().size(), 0);
  ASSERT_EQ(func->getReturnType().dataType, types::TypeInfo::DataType::VOID);
  ASSERT_EQ(func->getBlocks().size(), 1);
}

TEST_F(CodegenTest, TestFunctionWithParams) {
  auto codegen = compileSource("fun add(x: @int, y: @int): @int do end");
  const auto &module = codegen->getModule();

  ASSERT_TRUE(module.hasFunction("add"));

  auto *func = module.getFunction("add");
  ASSERT_EQ(func->getParams().size(), 2);
  ASSERT_EQ(func->getParams()[0].name, "x");
  ASSERT_EQ(func->getParams()[1].name, "y");
  ASSERT_EQ(func->getReturnType().dataType, types::TypeInfo::DataType::INTEGER);
}

TEST_F(CodegenTest, TestFunctionWithReturn) {
  auto codegen = compileSource("fun getValue(): @int do return 1; end");
  const auto &module = codegen->getModule();

  auto *func = module.getFunction("getValue");
  auto *entry = func->getEntryBlock();

  ASSERT_TRUE(entry->hasTerminator());
  ASSERT_EQ(entry->getInstructions().back().getOpcode(), ir::Opcode::RET);
}

TEST_F(CodegenTest, TestLocalVariable) {
  auto codegen = compileSource("fun foo(): @int do x: @int = 1; return x; end");
  const auto &module = codegen->getModule();

  auto *func = module.getFunction("foo");
  auto *entry = func->getEntryBlock();

  bool hasStore = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::STORE) {
      hasStore = true;
      break;
    }
  }

  ASSERT_TRUE(hasStore);
}

TEST_F(CodegenTest, TestLocalAssignment) {
  auto codegen =
      compileSource("fun foo(): @int do x: @int = 1; x = 2; return x; end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("foo");
  auto *entry = func->getEntryBlock();

  int storeCount = 0;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::STORE) {
      storeCount++;
    }
  }

  ASSERT_EQ(storeCount, 2);
}

TEST_F(CodegenTest, TestBinaryAddition) {
  auto codegen =
      compileSource("fun add(x: @int, y: @int): @int do return x + y; end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("add");
  auto *entry = func->getEntryBlock();

  bool hasAdd = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::ADD) {
      hasAdd = true;
      ASSERT_TRUE(instr.isBinary());
      ASSERT_EQ(instr.getOperands().size(), 2);
      break;
    }
  }

  ASSERT_TRUE(hasAdd);
}

TEST_F(CodegenTest, TestBinarySubtraction) {
  auto codegen =
      compileSource("fun sub(x: @int, y: @int): @int do return x - y; end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("sub");
  auto *entry = func->getEntryBlock();

  bool hasSub = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::SUB) {
      hasSub = true;
      break;
    }
  }

  ASSERT_TRUE(hasSub);
}

TEST_F(CodegenTest, TestBinaryMultiplication) {
  auto codegen =
      compileSource("fun mul(x: @int, y: @int): @int do return x * y; end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("mul");
  auto *entry = func->getEntryBlock();

  bool hasMul = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::MUL) {
      hasMul = true;
      break;
    }
  }

  ASSERT_TRUE(hasMul);
}

TEST_F(CodegenTest, TestBinaryDivision) {
  auto codegen =
      compileSource("fun div(x: @int, y: @int): @int do return x / y; end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("div");
  auto *entry = func->getEntryBlock();

  bool hasDiv = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::DIV) {
      hasDiv = true;
      break;
    }
  }

  ASSERT_TRUE(hasDiv);
}

TEST_F(CodegenTest, TestComparison) {
  auto codegen =
      compileSource("fun cmp(x: @int, y: @int): @bool do return x == y; end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("cmp");
  auto *entry = func->getEntryBlock();

  bool hasCmp = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::ICMP_EQ) {
      hasCmp = true;
      break;
    }
  }

  ASSERT_TRUE(hasCmp);
}

TEST_F(CodegenTest, TestUnaryNegation) {
  auto codegen = compileSource("fun neg(x: @int): @int do return -x; end");
  const auto &module = codegen->getModule();

  auto *func = module.getFunction("neg");
  auto *entry = func->getEntryBlock();

  bool hasNeg = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::NEG) {
      hasNeg = true;
      ASSERT_TRUE(instr.isUnary());
      ASSERT_EQ(instr.getOperands().size(), 1);
      break;
    }
  }

  ASSERT_TRUE(hasNeg);
}

TEST_F(CodegenTest, TestUnaryNot) {
  auto codegen = compileSource("fun not(x: @bool): @bool do return !x; end");
  const auto &module = codegen->getModule();

  auto *func = module.getFunction("not");
  auto *entry = func->getEntryBlock();

  bool hasNot = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::NOT) {
      hasNot = true;
      break;
    }
  }

  ASSERT_TRUE(hasNot);
}

TEST_F(CodegenTest, TestIfStatement) {
  auto codegen = compileSource("fun foo(x: @int): @int do "
                               "match where when x > 0 -> return 1; end "
                               "return 0; "
                               "end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("foo");

  ASSERT_GT(func->getBlocks().size(), 1);

  bool hasCondBr = false;
  for (const auto &block : func->getBlocks()) {
    for (const auto &instr : block->getInstructions()) {
      if (instr.getOpcode() == ir::Opcode::CONDBR) {
        hasCondBr = true;
        ASSERT_TRUE(instr.isTerminator());
        break;
      }
    }
  }

  ASSERT_TRUE(hasCondBr);
}

TEST_F(CodegenTest, TestIfElseStatement) {
  auto codegen =
      compileSource("fun foo(x: @int): @int do "
                    "match where when x > 0 -> return 1; else -> return 0; end "
                    "end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("foo");

  ASSERT_GT(func->getBlocks().size(), 2);

  bool hasCondBr = false;
  for (const auto &block : func->getBlocks()) {
    for (const auto &instr : block->getInstructions()) {
      if (instr.getOpcode() == ir::Opcode::CONDBR) {
        hasCondBr = true;
        break;
      }
    }
  }

  ASSERT_TRUE(hasCondBr);
}

TEST_F(CodegenTest, TestFunctionCall) {
  auto codegen = compileSource("fun bar(): @int do return 1; end "
                               "fun foo(): @int do return bar(); end");

  const auto &module = codegen->getModule();
  ASSERT_TRUE(module.hasFunction("bar"));
  ASSERT_TRUE(module.hasFunction("foo"));

  auto *func = module.getFunction("foo");
  auto *entry = func->getEntryBlock();

  bool hasCall = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::CALL) {
      hasCall = true;
      ASSERT_EQ(instr.getFuncName(), "bar");
      break;
    }
  }

  ASSERT_TRUE(hasCall);
}

TEST_F(CodegenTest, TestFunctionCallWithArgs) {
  auto codegen =
      compileSource("fun add(x: @int, y: @int): @int do return x + y; end "
                    "fun foo(): @int do return add(1, 2); end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("foo");
  auto *entry = func->getEntryBlock();

  bool hasCall = false;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.getOpcode() == ir::Opcode::CALL) {
      hasCall = true;
      ASSERT_EQ(instr.getFuncName(), "add");
      ASSERT_EQ(instr.getOperands().size(), 2);
      break;
    }
  }

  ASSERT_TRUE(hasCall);
}

TEST_F(CodegenTest, TestCFGConstruction) {
  auto codegen =
      compileSource("fun foo(x: @int): @int do "
                    "match where when x > 0 -> return 1; else -> return 0; end "
                    "end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("foo");

  bool hasPredecessors = false;
  bool hasSuccessors = false;

  for (const auto &block : func->getBlocks()) {
    if (!block->getPredecessors().empty()) {
      hasPredecessors = true;
    }

    if (!block->getSuccessors().empty()) {
      hasSuccessors = true;
    }
  }

  ASSERT_TRUE(hasPredecessors);
  ASSERT_TRUE(hasSuccessors);
}

TEST_F(CodegenTest, TestComplexExpression) {
  auto codegen =
      compileSource("fun complex(a: @int, b: @int, c: @int): @int do "
                    "return (a + b) * c - 1; "
                    "end");

  const auto &module = codegen->getModule();
  auto *func = module.getFunction("complex");
  auto *entry = func->getEntryBlock();

  int arithCount = 0;
  for (const auto &instr : entry->getInstructions()) {
    if (instr.isBinary()) {
      arithCount++;
    }
  }

  ASSERT_GE(arithCount, 3);
}

TEST_F(CodegenTest, TestMultipleFunctions) {
  auto codegen = compileSource("fun foo(): @int do return 1; end "
                               "fun bar(): @int do return 2; end "
                               "fun baz(): @int do return 3; end");

  const auto &module = codegen->getModule();
  ASSERT_EQ(module.getFunctions().size(), 3);
  ASSERT_TRUE(module.hasFunction("foo"));
  ASSERT_TRUE(module.hasFunction("bar"));
  ASSERT_TRUE(module.hasFunction("baz"));
}
