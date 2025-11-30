/**
 * @brief IR operand types.
 * @file operand.hpp
 */

#ifndef VERTE_BACKEND_IR_OPERAND_HPP
#define VERTE_BACKEND_IR_OPERAND_HPP

#include "verte/types.hpp"

#include <cstdint>
#include <variant>

/**
 * @namespace verte::ir
 * @brief Intermediate representation namespace.
 */
namespace verte::ir {
  /**
   * @brief Type of IR operand.
   */
  enum class OperandType : uint8_t {
    VREGISTER,  /**< Virtual register operand. */
    IMMEDIATE,  /**< Immediate value operand. */
    FIMMEDIATE, /**< Floating-point immediate value operand. */
    GLOBAL,     /**< Global variable/label operand. */
    STACK_SLOT  /**< Stack slot operand. */
  };

  /**
   * @class Operand
   * @brief Operand for intermediate representation.
   */
  class Operand {
  public:
    /**
     * @brief Create a virtual register operand.
     * @param id ID of the virtual register.
     * @param type Type of the value stored in register.
     * @return Virtual register operand.
     */
    static Operand vregister(uint32_t id, types::TypeInfo::DataType type);

    /**
     * @brief Create an immediate operand.
     * @param value Immediate integer value.
     * @return Immediate operand.
     */
    static Operand immediate(int64_t value);

    /**
     * @brief Create a floating-point immediate operand.
     * @param value Immediate floating-point value.
     * @return Floating-point immediate operand.
     */
    static Operand fimmediate(double value);

    /**
     * @brief Create a global variable/label operand.
     * @param name Name of the global variable/label.
     * @return Global operand.
     */
    static Operand global(const std::string &name);

    /**
     * @brief Create a stack slot operand.
     * @param offset Offset of the stack slot.
     * @param type Type of value in the stack slot.
     * @return Stack slot operand.
     */
    static Operand stackSlot(int32_t offset, types::TypeInfo::DataType);

    /**
     * @brief Get operand type.
     * @return The operand type.
     */
    OperandType getType() const { return type; }

    /**
     * @brief Get virtual register ID.
     * @return The virtual register ID.
     */
    uint32_t getVRegId() const;

    /**
     * @brief Get integer immediate value.
     * @return The immediate integer value.
     */
    int64_t getImmediate() const;

    /**
     * @brief Get floating-point immediate value.
     * @return The immediate floating-point value.
     */
    double getFImmediate() const;

    /**
     * @brief Get global variable/label name.
     * @return The name of the global variable/label.
     */
    const std::string &getGlobalName() const;

    /**
     * @brief Get stack slot offset.
     * @return The offset of the stack slot.
     */
    int32_t getStackOffset() const;

    /**
     * @brief Get the type of value this operand represents.
     * @return The value type.
     */
    const types::TypeInfo getValueType() const { return valueType; }

    /**
     * @brief Check if operand is a floating point value.
     * @return True if floating point, otherwise fale.
     */
    bool isFloatingPoint() const;

  private:
    /**
     * @brief Private constructor.
     * @param type Operand type.
     * @param valueType Value type.
     */
    Operand(OperandType type, types::TypeInfo::DataType valueType) noexcept
        : type(type), valueType(valueType) {}

    OperandType type; /**< Operand type. */
    types::TypeInfo::DataType
        valueType; /**< Type of value represented by operand. */

    std::variant<uint32_t, int64_t, double, std::string, int32_t>
        data; /**< Operand data. */
  };
} // namespace verte::ir

#endif // VERTE_BACKEND_IR_OPERAND_HPP
