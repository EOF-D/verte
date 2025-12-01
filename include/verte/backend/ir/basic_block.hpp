/**
 * @brief IR basic block representation with control flow graph support.
 * @file basic_block.hpp
 */

#ifndef VERTE_BACKEND_IR_BASIC_BLOCK_HPP
#define VERTE_BACKEND_IR_BASIC_BLOCK_HPP

#include "verte/backend/ir/instruction.hpp"

#include <memory>
#include <vector>

/**
 * @namespace verte::ir
 * @brief Intermediate representation namespace.
 */
namespace verte::ir {
  // Forward declaration.
  class BasicBlock;

  /**
   * @typedef BasicBlockPtr
   * @brief Unique pointer to a basic block.
   */
  using BasicBlockPtr = std::unique_ptr<BasicBlock>;

  /**
   * @class BasicBlock
   * @brief Represents a basic block in the IR.
   */
  class BasicBlock {
  public:
    /**
     * @brief Construct a new BasicBlock.
     * @param label The block label.
     */
    explicit BasicBlock(std::string label) noexcept : label(std::move(label)) {}

    /**
     * @brief Get the block label.
     * @return The label.
     */
    [[nodiscard]] const std::string &getLabel() const noexcept { return label; }

    /**
     * @brief Get the instructions in this block.
     * @return The instructions of this block.
     */
    [[nodiscard]] std::vector<Instruction> &getInstructions() noexcept {
      return instructions;
    }

    /**
     * @brief Get the instructions in this block (const).
     * @return The instructions of this block.
     */
    [[nodiscard]] const std::vector<Instruction> &
    getInstructions() const noexcept {
      return instructions;
    }

    /**
     * @brief Get predecessor blocks.
     * @return The predecessor list.
     */
    [[nodiscard]] const std::vector<BasicBlock *> &
    getPredecessors() const noexcept {
      return predecessors;
    }

    /**
     * @brief Get successor blocks.
     * @return The successor list.
     */
    [[nodiscard]] const std::vector<BasicBlock *> &
    getSuccessors() const noexcept {
      return successors;
    }

    /**
     * @brief Add an instruction to this block.
     * @param instruction The instruction to add.
     */
    void addInstruction(Instruction instruction) {
      instructions.push_back(std::move(instruction));
    }

    /**
     * @brief Set the terminator instruction.
     * @param terminator The terminator instruction.
     */
    void setTerminator(Instruction terminator) {
      if (!terminator.isTerminator()) {
        throw std::runtime_error("Instruction is not a terminator");
      }

      // Remove existing terminator if present.
      if (!instructions.empty() && instructions.back().isTerminator()) {
        instructions.pop_back();
      }

      // Add the new terminator.
      instructions.push_back(std::move(terminator));
    }

    /**
     * @brief Add a predecessor block.
     * @param predecessor The predecessor block.
     */
    void addPredecessor(BasicBlock *predecessor) {
      if (predecessor == nullptr) {
        throw std::runtime_error("Predecessor cannot be null");
      }

      predecessors.push_back(predecessor);
    }

    /**
     * @brief Add a successor block.
     * @param successor The successor block.
     */
    void addSuccessor(BasicBlock *successor) {
      if (successor == nullptr) {
        throw std::runtime_error("Successor cannot be null");
      }

      successors.push_back(successor);
    }

    /**
     * @brief Check if this block has a terminator.
     * @return True if block has terminator, otherwise false.
     */
    [[nodiscard]] bool hasTerminator() const noexcept {
      return !instructions.empty() && instructions.back().isTerminator();
    }

  private:
    std::string label; /**< The block label. */
    std::vector<Instruction>
        instructions; /**< The instructions of the block. */

    std::vector<BasicBlock *> predecessors; /**< Predecessor blocks. */
    std::vector<BasicBlock *> successors;   /**< Successor blocks. */
  };
} // namespace verte::ir

#endif // VERTE_BACKEND_IR_BASIC_BLOCK_HPP
