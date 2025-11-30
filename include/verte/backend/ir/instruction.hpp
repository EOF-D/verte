/**
 * @brief IR instruction representation.
 * @file instruction.hpp
 */

#ifndef VERTE_BACKEND_IR_INSTRUCTION_HPP
#define VERTE_BACKEND_IR_INSTRUCTION_HPP

#include "verte/backend/ir/operand.hpp"

#include <cstdint>
#include <optional>
#include <vector>

/**
 * @namespace verte::ir
 * @brief Intermediate representation namespace.
 */
namespace verte::ir {
  /**
   * @enum Opcode
   * @brief IR instruction opcodes.
   */
  enum class Opcode : uint8_t {
    ADD, /**< Integer addition. */
    SUB, /**< Integer subtraction. */
    MUL, /**< Integer multiplication. */
    DIV, /**< Integer division. */
    MOD, /**< Integer modulo. */

    FADD, /**< Floating-point addition. */
    FSUB, /**< Floating-point subtraction. */
    FMUL, /**< Floating-point multiplication. */
    FDIV, /**< Floating-point division. */

    ICMP_EQ, /**< Integer equal. */
    ICMP_NE, /**< Integer not equal. */
    ICMP_LT, /**< Integer less than. */
    ICMP_LE, /**< Integer less than or equal. */
    ICMP_GT, /**< Integer greater than. */
    ICMP_GE, /**< Integer greater than or equal. */

    FCMP_EQ, /**< Float equal. */
    FCMP_NE, /**< Float not equal. */
    FCMP_LT, /**< Float less than. */
    FCMP_LE, /**< Float less than or equal. */
    FCMP_GT, /**< Float greater than. */
    FCMP_GE, /**< Float greater than or equal. */

    ITOF, /**< Integer to float. */
    FTOI, /**< Float to integer. */

    NEG,  /**< Integer negation. */
    FNEG, /**< Float negation. */
    NOT,  /**< Logical NOT. */

    LOAD,  /**< Load from memory. */
    STORE, /**< Store to memory. */

    BR,     /**< Unconditional branch. */
    CONDBR, /**< Conditional branch. */
    CALL,   /**< Function call. */
    RET     /**< Return from function. */
  };

  /**
   * @class Instruction
   * @brief Represents an IR instruction.
   */
  class Instruction {
  public:
    /**
     * @brief Create a binary instruction.
     * @param opcode The instruction opcode.
     * @param dest The destination operand.
     * @param lhs The left-hand side operand.
     * @param rhs The right-hand side operand.
     */
    Instruction(Opcode opcode, Operand dest, Operand lhs, Operand rhs) noexcept
        : opcode(opcode), dest(std::move(dest)),
          operands{std::move(lhs), std::move(rhs)} {}

    /**
     * @brief Create a unary instruction.
     * @param opcode The instruction opcode.
     * @param dest The destination operand.
     * @param operand The source operand.
     */
    Instruction(Opcode opcode, Operand dest, Operand operand) noexcept
        : opcode(opcode), dest(std::move(dest)), operands{std::move(operand)} {}

    /**
     * @brief Create a branch instruction.
     * @param opcode The instruction opcode.
     * @param label The target label.
     * @param cond Optional condition for CONDBR.
     * @param falseLabel Optional false label for CONDBR.
     */
    Instruction(Opcode opcode, std::string label,
                std::optional<Operand> cond = std::nullopt,
                std::optional<std::string> falseLabel = std::nullopt) noexcept
        : opcode(opcode), label(std::move(label)),
          falseLabel(std::move(falseLabel)) {
      // If conditional branch, add condition operand.
      if (cond.has_value()) {
        operands.push_back(std::move(*cond));
      }
    }

    /**
     * @brief Create a call instruction.
     * @param dest The destination operand for return value.
     * @param funcName The function name.
     * @param args The function arguments.
     */
    Instruction(Operand dest, std::string funcName,
                std::vector<Operand> args) noexcept
        : opcode(Opcode::CALL), dest(std::move(dest)),
          funcName(std::move(funcName)), operands(std::move(args)) {}

    /**
     * @brief Create a return instruction.
     * @param value Optional return value.
     */
    explicit Instruction(std::optional<Operand> value = std::nullopt) noexcept
        : opcode(Opcode::RET) {
      // Add return value operand if present.
      if (value.has_value()) {
        operands.push_back(std::move(*value));
      }
    }

    /**
     * @brief Get the instruction opcode.
     * @return The opcode.
     */
    [[nodiscard]] Opcode getOpcode() const noexcept { return opcode; }

    /**
     * @brief Get the destination operand.
     * @return The destination operand.
     */
    [[nodiscard]] const Operand &getDest() const noexcept { return dest; }

    /**
     * @brief Get the source operands.
     * @return The source operands.
     */
    [[nodiscard]] const std::vector<Operand> &getOperands() const noexcept {
      return operands;
    }

    /**
     * @brief Get the branch target label.
     * @return The target label.
     */
    [[nodiscard]] const std::string &getLabel() const noexcept { return label; }

    /**
     * @brief Get the false branch label (for CONDBR).
     * @return The false label.
     */
    [[nodiscard]] const std::optional<std::string> &
    getFalseLabel() const noexcept {
      return falseLabel;
    }

    /**
     * @brief Get the function name (CALL).
     * @return The function name.
     */
    [[nodiscard]] const std::string &getFuncName() const noexcept {
      return funcName;
    }

    /**
     * @brief Check if this is a terminator instruction.
     * @return True if terminator, otherwise false.
     */
    [[nodiscard]] bool isTerminator() const noexcept {
      return opcode == Opcode::BR || opcode == Opcode::CONDBR ||
             opcode == Opcode::RET;
    }

    /**
     * @brief Check if this is a binary instruction.
     * @return True if binary, otherwise false.
     */
    [[nodiscard]] bool isBinary() const noexcept {
      return operands.size() == 2 && !isTerminator() && opcode != Opcode::CALL;
    }

    /**
     * @brief Check if this is a unary instruction.
     * @return True if unary, otherwise false.
     */
    [[nodiscard]] bool isUnary() const noexcept {
      return operands.size() == 1 && !isTerminator() && opcode != Opcode::CALL;
    }

  private:
    Opcode opcode;                         /**< The instruction opcode. */
    Operand dest;                          /**< The destination operand. */
    std::vector<Operand> operands;         /**< Source operands. */
    std::string label;                     /**< Branch target label. */
    std::optional<std::string> falseLabel; /**< False branch label. */
    std::string funcName;                  /**< Function name for CALL. */
  };
} // namespace verte::ir

#endif // VERTE_BACKEND_IR_INSTRUCTION_HPP
