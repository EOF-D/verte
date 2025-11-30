/**
 * @brief IR operand implementation.
 * @file operand.cpp
 */

#include "verte/backend/ir/operand.hpp"

namespace verte::ir {
  Operand Operand::vregister(uint32_t id, types::TypeInfo::DataType type) {
    Operand op(OperandType::VREGISTER, type);
    op.data = id;

    return op;
  }

  Operand Operand::immediate(int64_t value) {
    Operand op(OperandType::IMMEDIATE, types::TypeInfo::DataType::INTEGER);
    op.data = value;

    return op;
  }

  Operand Operand::fimmediate(double value) {
    Operand op(OperandType::FIMMEDIATE, types::TypeInfo::DataType::FLOAT);
    op.data = value;

    return op;
  }

  Operand Operand::global(const std::string &name) {
    Operand op(OperandType::GLOBAL, types::TypeInfo::DataType::INTEGER);
    op.data = std::move(name);

    return op;
  }

  Operand Operand::stackSlot(int32_t offset, types::TypeInfo::DataType type) {
    Operand op(OperandType::STACK_SLOT, type);
    op.data = offset;

    return op;
  }

  uint32_t Operand::getVRegId() const {
    if (type != OperandType::VREGISTER) {
      throw std::runtime_error("Operand is not a virtual register");
    }

    return std::get<uint32_t>(data);
  }

  int64_t Operand::getImmediate() const {
    if (type != OperandType::IMMEDIATE) {
      throw std::runtime_error("Operand is not an immediate value");
    }

    return std::get<int64_t>(data);
  }

  double Operand::getFImmediate() const {
    if (type != OperandType::FIMMEDIATE) {
      throw std::runtime_error(
          "Operand is not a floating-point immediate value");
    }

    return std::get<double>(data);
  }

  const std::string &Operand::getGlobalName() const {
    if (type != OperandType::GLOBAL) {
      throw std::runtime_error("Operand is not a global variable/label");
    }

    return std::get<std::string>(data);
  }

  int32_t Operand::getStackOffset() const {
    if (type != OperandType::STACK_SLOT) {
      throw std::runtime_error("Operand is not a stack slot");
    }

    return std::get<int32_t>(data);
  }

  bool Operand::isFloatingPoint() const {
    return type == OperandType::FIMMEDIATE ||
           valueType == types::TypeInfo::DataType::FLOAT;
  }
} // namespace verte::ir
