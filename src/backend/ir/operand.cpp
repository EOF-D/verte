/**
 * @brief IR operand implementation.
 * @file operand.cpp
 */

#include "verte/backend/ir/operand.hpp"

#include <stdexcept>

namespace verte::ir {
  uint32_t Operand::getVRegID() const {
    if (type != OperandType::VREG) {
      throw std::runtime_error("Operand is not a virtual register");
    }

    return std::get<uint32_t>(value);
  }

  int64_t Operand::getImmediate() const {
    if (type != OperandType::IMMEDIATE) {
      throw std::runtime_error("Operand is not an immediate value");
    }

    return std::get<int64_t>(value);
  }

  double Operand::getFImmediate() const {
    if (type != OperandType::FIMMEDIATE) {
      throw std::runtime_error("Operand is not a floating-point immediate");
    }

    return std::get<double>(value);
  }

  const std::string &Operand::getGlobalName() const {
    if (type != OperandType::GLOBAL) {
      throw std::runtime_error("Operand is not a global reference");
    }

    return std::get<std::string>(value);
  }

  int32_t Operand::getStackOffset() const {
    if (type != OperandType::STACK_SLOT) {
      throw std::runtime_error("Operand is not a stack slot");
    }

    return std::get<int32_t>(value);
  }

  Operand Operand::vreg(uint32_t id, const types::TypeInfo &typeInfo) noexcept {
    return Operand(OperandType::VREG, typeInfo, id);
  }

  Operand Operand::imm(int64_t value) noexcept {
    types::TypeInfo typeInfo(types::TypeInfo::DataType::INTEGER);
    return Operand(OperandType::IMMEDIATE, typeInfo, value);
  }

  Operand Operand::fimm(double value) noexcept {
    types::TypeInfo typeInfo(types::TypeInfo::DataType::DOUBLE);
    return Operand(OperandType::FIMMEDIATE, typeInfo, value);
  }

  Operand Operand::global(const std::string &name) noexcept {
    types::TypeInfo typeInfo(types::TypeInfo::DataType::UNKNOWN);
    return Operand(OperandType::GLOBAL, typeInfo, name);
  }

  Operand Operand::stackSlot(int32_t offset,
                             const types::TypeInfo &typeInfo) noexcept {
    return Operand(OperandType::STACK_SLOT, typeInfo, offset);
  }
} // namespace verte::ir
