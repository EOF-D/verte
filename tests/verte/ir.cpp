#include "verte/backend/ir/basic_block.hpp"
#include "verte/backend/ir/function.hpp"
#include "verte/backend/ir/instruction.hpp"
#include "verte/backend/ir/module.hpp"
#include "verte/backend/ir/operand.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing;
using namespace verte;

class IRTest : public ::testing::Test {
protected:
  void SetUp() override {}
};

TEST_F(IRTest, TestVRegOperand) {
  auto operand =
      ir::Operand::vreg(1, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  ASSERT_TRUE(operand.isVReg());
  ASSERT_FALSE(operand.isImmediate());
  ASSERT_EQ(operand.getVRegID(), 1);
  ASSERT_EQ(operand.getTypeInfo().dataType, types::TypeInfo::DataType::INTEGER);
}

TEST_F(IRTest, TestImmediateOperand) {
  auto operand = ir::Operand::imm(1);

  ASSERT_TRUE(operand.isImmediate());
  ASSERT_FALSE(operand.isVReg());
  ASSERT_EQ(operand.getImmediate(), 1);
  ASSERT_EQ(operand.getTypeInfo().dataType, types::TypeInfo::DataType::INTEGER);
}

TEST_F(IRTest, TestFImmediateOperand) {
  auto operand = ir::Operand::fimm(1.0);

  ASSERT_TRUE(operand.isFImmediate());
  ASSERT_FALSE(operand.isImmediate());
  ASSERT_DOUBLE_EQ(operand.getFImmediate(), 1.0);
  ASSERT_EQ(operand.getTypeInfo().dataType, types::TypeInfo::DataType::DOUBLE);
}

TEST_F(IRTest, TestGlobalOperand) {
  auto operand = ir::Operand::global("foo");

  ASSERT_TRUE(operand.isGlobal());
  ASSERT_FALSE(operand.isVReg());
  ASSERT_EQ(operand.getGlobalName(), "foo");
}

TEST_F(IRTest, TestStackSlotOperand) {
  auto operand = ir::Operand::stackSlot(
      -8, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  ASSERT_TRUE(operand.isStackSlot());
  ASSERT_FALSE(operand.isVReg());
  ASSERT_EQ(operand.getStackOffset(), -8);
  ASSERT_EQ(operand.getTypeInfo().dataType, types::TypeInfo::DataType::INTEGER);
}

TEST_F(IRTest, TestBinaryInstruction) {
  auto dest =
      ir::Operand::vreg(1, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  auto lhs =
      ir::Operand::vreg(2, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  auto rhs =
      ir::Operand::vreg(3, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  ir::Instruction instr(ir::Opcode::ADD, dest, lhs, rhs);
  ASSERT_EQ(instr.getOpcode(), ir::Opcode::ADD);
  ASSERT_TRUE(instr.isBinary());
  ASSERT_FALSE(instr.isUnary());
  ASSERT_FALSE(instr.isTerminator());
  ASSERT_EQ(instr.getOperands().size(), 2);
}

TEST_F(IRTest, TestUnaryInstruction) {
  auto dest =
      ir::Operand::vreg(1, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  auto src =
      ir::Operand::vreg(2, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  ir::Instruction instr(ir::Opcode::NEG, dest, src);
  ASSERT_EQ(instr.getOpcode(), ir::Opcode::NEG);
  ASSERT_TRUE(instr.isUnary());
  ASSERT_FALSE(instr.isBinary());
  ASSERT_FALSE(instr.isTerminator());
  ASSERT_EQ(instr.getOperands().size(), 1);
}

TEST_F(IRTest, TestBranchInstruction) {
  ir::Instruction instr(ir::Opcode::BR, "foo");

  ASSERT_EQ(instr.getOpcode(), ir::Opcode::BR);
  ASSERT_TRUE(instr.isTerminator());
  ASSERT_FALSE(instr.isBinary());
  ASSERT_EQ(instr.getLabel(), "foo");
}

TEST_F(IRTest, TestConditionalBranchInstruction) {
  auto cond =
      ir::Operand::vreg(1, types::TypeInfo(types::TypeInfo::DataType::BOOL));

  ir::Instruction instr(ir::Opcode::CONDBR, "then", cond, "else");
  ASSERT_EQ(instr.getOpcode(), ir::Opcode::CONDBR);
  ASSERT_TRUE(instr.isTerminator());
  ASSERT_EQ(instr.getLabel(), "then");
  ASSERT_TRUE(instr.getFalseLabel().has_value());
  ASSERT_EQ(instr.getFalseLabel().value(), "else");
  ASSERT_EQ(instr.getOperands().size(), 1);
}

TEST_F(IRTest, TestCallInstruction) {
  auto dest =
      ir::Operand::vreg(1, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  std::vector<ir::Operand> args;
  args.push_back(ir::Operand::imm(1));
  args.push_back(ir::Operand::imm(2));

  ir::Instruction instr(dest, "foo", std::move(args));
  ASSERT_EQ(instr.getOpcode(), ir::Opcode::CALL);
  ASSERT_FALSE(instr.isTerminator());
  ASSERT_EQ(instr.getFuncName(), "foo");
  ASSERT_EQ(instr.getOperands().size(), 2);
}

TEST_F(IRTest, TestReturnInstruction) {
  auto value = ir::Operand::imm(1);

  ir::Instruction instr((std::optional<ir::Operand>(value)));
  ASSERT_EQ(instr.getOpcode(), ir::Opcode::RET);
  ASSERT_TRUE(instr.isTerminator());
  ASSERT_EQ(instr.getOperands().size(), 1);
}

TEST_F(IRTest, TestBasicBlockCreation) {
  ir::BasicBlock block("foo");

  ASSERT_EQ(block.getLabel(), "foo");
  ASSERT_TRUE(block.getInstructions().empty());
  ASSERT_TRUE(block.getPredecessors().empty());
  ASSERT_TRUE(block.getSuccessors().empty());
  ASSERT_FALSE(block.hasTerminator());
}

TEST_F(IRTest, TestBasicBlockAddInstruction) {
  ir::BasicBlock block("foo");

  auto dest =
      ir::Operand::vreg(1, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  auto lhs =
      ir::Operand::vreg(2, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  auto rhs =
      ir::Operand::vreg(3, types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  block.addInstruction(ir::Instruction(ir::Opcode::ADD, dest, lhs, rhs));
  ASSERT_EQ(block.getInstructions().size(), 1);
  ASSERT_FALSE(block.hasTerminator());
}

TEST_F(IRTest, TestBasicBlockSetTerminator) {
  ir::BasicBlock block("foo");

  block.setTerminator(ir::Instruction(ir::Opcode::BR, "bar"));
  ASSERT_TRUE(block.hasTerminator());
  ASSERT_EQ(block.getInstructions().size(), 1);
  ASSERT_TRUE(block.getInstructions()[0].isTerminator());
}

TEST_F(IRTest, TestBasicBlockCFGLinks) {
  ir::BasicBlock block1("foo");
  ir::BasicBlock block2("bar");

  block1.addSuccessor(&block2);
  block2.addPredecessor(&block1);

  ASSERT_EQ(block1.getSuccessors().size(), 1);
  ASSERT_EQ(block2.getPredecessors().size(), 1);
  ASSERT_EQ(block1.getSuccessors()[0], &block2);
  ASSERT_EQ(block2.getPredecessors()[0], &block1);
}

TEST_F(IRTest, TestFunctionCreation) {
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);
  std::vector<types::Parameter> params;
  params.emplace_back("foo", intType);
  params.emplace_back("bar", intType);

  ir::Function func("baz", std::move(params), intType);
  ASSERT_EQ(func.getName(), "baz");
  ASSERT_EQ(func.getParams().size(), 2);
  ASSERT_EQ(func.getReturnType().dataType, types::TypeInfo::DataType::INTEGER);
  ASSERT_TRUE(func.getBlocks().empty());
}

TEST_F(IRTest, TestFunctionCreateBlock) {
  ir::Function func("foo", {},
                    types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  auto *block = func.createBlock("bar");
  ASSERT_NE(block, nullptr);
  ASSERT_EQ(block->getLabel(), "bar");
  ASSERT_EQ(func.getBlocks().size(), 1);
  ASSERT_TRUE(func.hasBlock("bar"));
}

TEST_F(IRTest, TestFunctionGetBlock) {
  ir::Function func("foo", {},
                    types::TypeInfo(types::TypeInfo::DataType::INTEGER));

  auto *block1 = func.createBlock("foo");
  auto *block2 = func.createBlock("baz");

  ASSERT_EQ(func.getBlock("foo"), block1);
  ASSERT_EQ(func.getBlock("baz"), block2);
  ASSERT_EQ(func.getEntryBlock(), block1);
}

TEST_F(IRTest, TestModuleCreation) {
  ir::Module module("foo");

  ASSERT_EQ(module.getName(), "foo");
  ASSERT_TRUE(module.getFunctions().empty());
}

TEST_F(IRTest, TestModuleCreateFunction) {
  ir::Module module("foo");

  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);
  std::vector<types::Parameter> params;
  params.emplace_back("bar", intType);

  auto *func = module.createFunction("baz", std::move(params), intType);
  ASSERT_NE(func, nullptr);
  ASSERT_EQ(func->getName(), "baz");
  ASSERT_EQ(module.getFunctions().size(), 1);
  ASSERT_TRUE(module.hasFunction("baz"));
}

TEST_F(IRTest, TestModuleGetFunction) {
  ir::Module module("foo");
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);

  auto *func1 = module.createFunction("bar", {}, intType);
  auto *func2 = module.createFunction("baz", {}, intType);

  ASSERT_EQ(module.getFunction("bar"), func1);
  ASSERT_EQ(module.getFunction("baz"), func2);
}

TEST_F(IRTest, TestCompleteIRConstruction) {
  ir::Module module("foo");
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);

  std::vector<types::Parameter> params;
  params.emplace_back("foo", intType);
  params.emplace_back("bar", intType);

  auto *func = module.createFunction("add", std::move(params), intType);
  auto *entry = func->createBlock("entry");

  auto foo = ir::Operand::vreg(1, intType);
  auto bar = ir::Operand::vreg(2, intType);
  auto result = ir::Operand::vreg(3, intType);

  entry->addInstruction(ir::Instruction(ir::Opcode::ADD, result, foo, bar));
  entry->setTerminator(ir::Instruction(std::optional<ir::Operand>(result)));

  ASSERT_EQ(module.getFunctions().size(), 1);
  ASSERT_EQ(func->getBlocks().size(), 1);
  ASSERT_EQ(entry->getInstructions().size(), 2);
  ASSERT_TRUE(entry->hasTerminator());
  ASSERT_EQ(entry->getInstructions()[0].getOpcode(), ir::Opcode::ADD);
  ASSERT_EQ(entry->getInstructions()[1].getOpcode(), ir::Opcode::RET);
}
