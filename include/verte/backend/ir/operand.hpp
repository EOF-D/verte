/**
 * @brief IR operand representation.
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
   * @enum OperandType
   * @brief Types of the operand.
   */
  enum class OperandType : uint8_t {
    VREG,       /**< Virtual register. */
    IMMEDIATE,  /**< Integer immediate value. */
    FIMMEDIATE, /**< Floating-point immediate value. */
    GLOBAL,     /**< Global variable reference. */
    STACK_SLOT  /**< Stack slot reference. */
  };

  /**
   * @class Operand
   * @brief Represents an operand in the IR.
   */
  class Operand {
  public:
    /**
     * @brief Default constructor.
     */
    Operand() noexcept
        : type(OperandType::IMMEDIATE), typeInfo(types::TypeInfo()),
          value(int64_t(0)) {}

    /**
     * @brief Get the operand type.
     * @return The operand type.
     */
    [[nodiscard]] OperandType getType() const noexcept { return type; }

    /**
     * @brief Get the type information.
     * @return The type information.
     */
    [[nodiscard]] const types::TypeInfo &getTypeInfo() const noexcept {
      return typeInfo;
    }

    /**
     * @brief Check if this is a virtual register.
     * @return True if virtual register, otherwise false.
     */
    [[nodiscard]] bool isVReg() const noexcept {
      return type == OperandType::VREG;
    }

    /**
     * @brief Check if this is an immediate value.
     * @return True if immediate, otherwise false.
     */
    [[nodiscard]] bool isImmediate() const noexcept {
      return type == OperandType::IMMEDIATE;
    }

    /**
     * @brief Check if this is a floating-point immediate.
     * @return True if float immediate, otherwise false.
     */
    [[nodiscard]] bool isFImmediate() const noexcept {
      return type == OperandType::FIMMEDIATE;
    }

    /**
     * @brief Check if this is a global reference.
     * @return True if global, otherwise false.
     */
    [[nodiscard]] bool isGlobal() const noexcept {
      return type == OperandType::GLOBAL;
    }

    /**
     * @brief Check if this is a stack slot.
     * @return True if stack slot, otherwise false.
     */
    [[nodiscard]] bool isStackSlot() const noexcept {
      return type == OperandType::STACK_SLOT;
    }

    /**
     * @brief Get virtual register ID.
     * @return The virtual register ID.
     */
    [[nodiscard]] uint32_t getVRegID() const;

    /**
     * @brief Get integer immediate value.
     * @return The immediate value.
     */
    [[nodiscard]] int64_t getImmediate() const;

    /**
     * @brief Get floating-point immediate value.
     * @return The floating-point immediate value.
     */
    [[nodiscard]] double getFImmediate() const;

    /**
     * @brief Get global name.
     * @return The global variable name.
     */
    [[nodiscard]] const std::string &getGlobalName() const;

    /**
     * @brief Get stack slot offset.
     * @return The stack slot offset.
     */
    [[nodiscard]] int32_t getStackOffset() const;

    /**
     * @brief Create a virtual register operand.
     * @param id The virtual register ID.
     * @param typeInfo The type information.
     * @return The operand.
     */
    static Operand vreg(uint32_t id, const types::TypeInfo &typeInfo) noexcept;

    /**
     * @brief Create an integer immediate operand.
     * @param value The immediate value.
     * @return The operand.
     */
    static Operand imm(int64_t value) noexcept;

    /**
     * @brief Create a floating-point immediate operand.
     * @param value The floating-point value.
     * @return The operand.
     */
    static Operand fimm(double value) noexcept;

    /**
     * @brief Create a global variable operand.
     * @param name The global variable name.
     * @return The operand.
     */
    static Operand global(const std::string &name) noexcept;

    /**
     * @brief Create a stack slot operand.
     * @param offset The stack offset.
     * @param typeInfo The type information.
     * @return The operand.
     */
    static Operand stackSlot(int32_t offset,
                             const types::TypeInfo &typeInfo) noexcept;

  private:
    /**
     * @brief Private constructor for factory methods.
     * @param type The operand type.
     * @param typeInfo The type information.
     * @param value The operand value.
     */
    Operand(OperandType type, const types::TypeInfo &typeInfo,
            std::variant<uint32_t, int64_t, double, std::string, int32_t>
                value) noexcept
        : type(type), typeInfo(typeInfo), value(std::move(value)) {}

    OperandType type;         /**< The operand type. */
    types::TypeInfo typeInfo; /**< The type information. */

    std::variant<uint32_t, int64_t, double, std::string, int32_t>
        value; /**< The operand value. */
  };
} // namespace verte::ir

#endif // VERTE_BACKEND_IR_OPERAND_HPP
