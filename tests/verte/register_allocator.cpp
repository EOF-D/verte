#include "verte/backend/codegen/register_allocator.hpp"
#include "verte/backend/ir/function.hpp"
#include "verte/backend/ir/module.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing;
using namespace verte;

class RegisterAllocatorTest : public ::testing::Test {
protected:
  void SetUp() override { module = std::make_unique<ir::Module>("test"); }

  std::unique_ptr<ir::Module> module;
};

TEST_F(RegisterAllocatorTest, TestBasicAllocation) {
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);

  auto *func = module->createFunction("add", {}, intType);
  auto *entry = func->createBlock("entry");

  // v0 = add v1, v2
  auto v0 = ir::Operand::vreg(0, intType);
  auto v1 = ir::Operand::vreg(1, intType);
  auto v2 = ir::Operand::vreg(2, intType);

  entry->addInstruction(ir::Instruction(ir::Opcode::ADD, v0, v1, v2));
  entry->setTerminator(ir::Instruction(std::optional<ir::Operand>(v0)));

  codegen::RegisterAllocator allocator;
  allocator.allocate(*func);

  ASSERT_NE(allocator.getPhysReg(0), codegen::PhysReg::NONE);
  ASSERT_NE(allocator.getPhysReg(1), codegen::PhysReg::NONE);
  ASSERT_NE(allocator.getPhysReg(2), codegen::PhysReg::NONE);

  ASSERT_FALSE(allocator.isSpilled(0));
  ASSERT_FALSE(allocator.isSpilled(1));
  ASSERT_FALSE(allocator.isSpilled(2));
}

TEST_F(RegisterAllocatorTest, TestFloatAllocation) {
  types::TypeInfo floatType(types::TypeInfo::DataType::DOUBLE);

  auto *func = module->createFunction("fadd", {}, floatType);
  auto *entry = func->createBlock("entry");

  // v0 = fadd v1, v2
  auto v0 = ir::Operand::vreg(0, floatType);
  auto v1 = ir::Operand::vreg(1, floatType);
  auto v2 = ir::Operand::vreg(2, floatType);

  entry->addInstruction(ir::Instruction(ir::Opcode::FADD, v0, v1, v2));
  entry->setTerminator(ir::Instruction(std::optional<ir::Operand>(v0)));

  codegen::RegisterAllocator allocator;
  allocator.allocate(*func);

  auto r0 = allocator.getPhysReg(0);
  auto r1 = allocator.getPhysReg(1);
  auto r2 = allocator.getPhysReg(2);

  ASSERT_NE(r0, codegen::PhysReg::NONE);
  ASSERT_NE(r1, codegen::PhysReg::NONE);
  ASSERT_NE(r2, codegen::PhysReg::NONE);

  ASSERT_GE(static_cast<int>(r0), 16);
  ASSERT_GE(static_cast<int>(r1), 16);
  ASSERT_GE(static_cast<int>(r2), 16);
}

TEST_F(RegisterAllocatorTest, TestLiveIntervals) {
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);

  auto *func = module->createFunction("test", {}, intType);
  auto *entry = func->createBlock("entry");

  // v0 = add v1, v2
  // v3 = add v0, v1
  // ret v3
  auto v0 = ir::Operand::vreg(0, intType);
  auto v1 = ir::Operand::vreg(1, intType);
  auto v2 = ir::Operand::vreg(2, intType);
  auto v3 = ir::Operand::vreg(3, intType);

  entry->addInstruction(ir::Instruction(ir::Opcode::ADD, v0, v1, v2));
  entry->addInstruction(ir::Instruction(ir::Opcode::ADD, v3, v0, v1));
  entry->setTerminator(ir::Instruction(std::optional<ir::Operand>(v3)));

  codegen::RegisterAllocator allocator;
  allocator.allocate(*func);

  ASSERT_NE(allocator.getPhysReg(0), codegen::PhysReg::NONE);
  ASSERT_NE(allocator.getPhysReg(1), codegen::PhysReg::NONE);
  ASSERT_NE(allocator.getPhysReg(2), codegen::PhysReg::NONE);
  ASSERT_NE(allocator.getPhysReg(3), codegen::PhysReg::NONE);
}

TEST_F(RegisterAllocatorTest, TestSpilling) {
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);

  auto *func = module->createFunction("test", {}, intType);
  auto *entry = func->createBlock("entry");

  std::vector<ir::Operand> vregs;
  for (uint32_t i = 0; i < 20; i++) {
    vregs.push_back(ir::Operand::vreg(i, intType));
  }

  for (size_t i = 2; i < vregs.size(); i++) {
    entry->addInstruction(
        ir::Instruction(ir::Opcode::ADD, vregs[i], vregs[0], vregs[1]));
  }

  // This ensures all vregs have overlapping live intervals.
  for (size_t i = 2; i < vregs.size() - 1; i++) {
    entry->addInstruction(
        ir::Instruction(ir::Opcode::ADD, vregs[i], vregs[i], vregs[i + 1]));
  }

  entry->addInstruction(
      ir::Instruction(ir::Opcode::ADD, vregs[0], vregs[18], vregs[19]));

  entry->setTerminator(ir::Instruction(std::optional<ir::Operand>(vregs[0])));

  codegen::RegisterAllocator allocator;
  allocator.allocate(*func);

  int spillCount = 0;
  for (uint32_t i = 0; i < 20; i++) {
    if (allocator.isSpilled(i)) {
      spillCount++;
      ASSERT_NE(allocator.getSpillSlot(i), -1);
    }
  }

  ASSERT_GT(spillCount, 0);
  ASSERT_GT(allocator.getSpillStackSize(), 0);
}

TEST_F(RegisterAllocatorTest, TestCalleeSavedTracking) {
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);

  auto *func = module->createFunction("test", {}, intType);
  auto *entry = func->createBlock("entry");

  for (uint32_t i = 0; i < 10; i++) {
    auto v = ir::Operand::vreg(i, intType);
    auto v1 = ir::Operand::vreg(i + 1, intType);
    auto v2 = ir::Operand::vreg(i + 2, intType);
    entry->addInstruction(ir::Instruction(ir::Opcode::ADD, v, v1, v2));
  }

  entry->setTerminator(ir::Instruction(
      std::optional<ir::Operand>(ir::Operand::vreg(0, intType))));

  codegen::RegisterAllocator allocator;
  allocator.allocate(*func);

  const auto &usedCalleeSaved = allocator.getUsedCalleeSaved();
  ASSERT_GT(usedCalleeSaved.size(), 0);

  for (auto reg : usedCalleeSaved) {
    bool isCalleeSaved = false;
    for (auto cs : codegen::CALLEE_SAVED) {
      if (reg == cs) {
        isCalleeSaved = true;
        break;
      }
    }

    ASSERT_TRUE(isCalleeSaved);
  }
}

TEST_F(RegisterAllocatorTest, TestMixedIntFloat) {
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);
  types::TypeInfo floatType(types::TypeInfo::DataType::DOUBLE);

  auto *func = module->createFunction("test", {}, intType);
  auto *entry = func->createBlock("entry");

  auto v0 = ir::Operand::vreg(0, intType);
  auto v1 = ir::Operand::vreg(1, intType);
  auto v2 = ir::Operand::vreg(2, intType);

  auto v3 = ir::Operand::vreg(3, floatType);
  auto v4 = ir::Operand::vreg(4, floatType);
  auto v5 = ir::Operand::vreg(5, floatType);

  entry->addInstruction(ir::Instruction(ir::Opcode::ADD, v0, v1, v2));
  entry->addInstruction(ir::Instruction(ir::Opcode::FADD, v3, v4, v5));
  entry->setTerminator(ir::Instruction(std::optional<ir::Operand>(v0)));

  codegen::RegisterAllocator allocator;
  allocator.allocate(*func);

  auto r0 = allocator.getPhysReg(0);
  auto r1 = allocator.getPhysReg(1);
  auto r2 = allocator.getPhysReg(2);

  ASSERT_NE(r0, codegen::PhysReg::NONE);
  ASSERT_NE(r1, codegen::PhysReg::NONE);
  ASSERT_NE(r2, codegen::PhysReg::NONE);
  ASSERT_LT(static_cast<int>(r0), 16);
  ASSERT_LT(static_cast<int>(r1), 16);
  ASSERT_LT(static_cast<int>(r2), 16);

  auto r3 = allocator.getPhysReg(3);
  auto r4 = allocator.getPhysReg(4);
  auto r5 = allocator.getPhysReg(5);

  ASSERT_NE(r3, codegen::PhysReg::NONE);
  ASSERT_NE(r4, codegen::PhysReg::NONE);
  ASSERT_NE(r5, codegen::PhysReg::NONE);
  ASSERT_GE(static_cast<int>(r3), 16);
  ASSERT_GE(static_cast<int>(r4), 16);
  ASSERT_GE(static_cast<int>(r5), 16);
}

TEST_F(RegisterAllocatorTest, TestEmptyFunction) {
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);

  auto *func = module->createFunction("test", {}, intType);
  auto *entry = func->createBlock("entry");
  entry->setTerminator(ir::Instruction());

  codegen::RegisterAllocator allocator;
  ASSERT_NO_THROW(allocator.allocate(*func));

  ASSERT_EQ(allocator.getUsedCalleeSaved().size(), 0);
  ASSERT_EQ(allocator.getSpillStackSize(), 0);
}

TEST_F(RegisterAllocatorTest, TestSingleInstruction) {
  types::TypeInfo intType(types::TypeInfo::DataType::INTEGER);

  auto *func = module->createFunction("test", {}, intType);
  auto *entry = func->createBlock("entry");

  auto v0 = ir::Operand::vreg(0, intType);
  entry->setTerminator(ir::Instruction(std::optional<ir::Operand>(v0)));

  codegen::RegisterAllocator allocator;
  allocator.allocate(*func);

  ASSERT_NE(allocator.getPhysReg(0), codegen::PhysReg::NONE);
  ASSERT_FALSE(allocator.isSpilled(0));
}
