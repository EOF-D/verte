/**
 * @brief IR function representation.
 * @file function.hpp
 */

#ifndef VERTE_BACKEND_IR_FUNCTION_HPP
#define VERTE_BACKEND_IR_FUNCTION_HPP

#include "verte/backend/ir/basic_block.hpp"
#include "verte/types.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

/**
 * @namespace verte::ir
 * @brief Intermediate representation namespace.
 */
namespace verte::ir {
  /**
   * @class Function
   * @brief Represents a function in the IR.
   */
  class Function {
  public:
    /**
     * @brief Construct a new Function.
     * @param name The function name.
     * @param params The function parameters.
     * @param returnType The return type.
     */
    explicit Function(std::string name, std::vector<types::Parameter> params,
                      types::TypeInfo returnType)
        : name(std::move(name)), params(std::move(params)),
          returnType(returnType) {}

    /**
     * @brief Get the function name.
     * @return The function name.
     */
    [[nodiscard]] const std::string &getName() const noexcept { return name; }

    /**
     * @brief Get the function parameters.
     * @return The parameters.
     */
    [[nodiscard]] const std::vector<types::Parameter> &
    getParams() const noexcept {
      return params;
    }

    /**
     * @brief Get the return type of the function.
     * @return The return type.
     */
    [[nodiscard]] const types::TypeInfo &getReturnType() const noexcept {
      return returnType;
    }

    /**
     * @brief Get all basic blocks in this function.
     * @return The basic blocks.
     */
    [[nodiscard]] std::vector<BasicBlockPtr> &getBlocks() noexcept {
      return blocks;
    }

    /**
     * @brief Get all basic blocks in this function (const).
     * @return The basic blocks.
     */
    [[nodiscard]] const std::vector<BasicBlockPtr> &getBlocks() const noexcept {
      return blocks;
    }

    /**
     * @brief Create a new basic block for this function.
     * @param label The block label.
     * @return Pointer to the created block.
     */
    BasicBlock *createBlock(const std::string &label) {
      auto block = std::make_unique<BasicBlock>(label);
      auto *ptr = block.get();

      // Add to map and block list.
      blockMap[label] = ptr;
      blocks.push_back(std::move(block));

      return ptr;
    }

    /**
     * @brief Get the entry block of the function.
     * @return Pointer to the entry block.
     */
    [[nodiscard]] BasicBlock *getEntryBlock() const {
      if (blocks.empty()) {
        throw std::runtime_error("Function has no blocks");
      }

      return blocks[0].get();
    }

    /**
     * @brief Get a block by label.
     * @param label The block label.
     * @return Pointer to the block.
     */
    [[nodiscard]] BasicBlock *getBlock(const std::string &label) const {
      auto iter = blockMap.find(label);
      if (iter == blockMap.end()) {
        throw std::runtime_error("Block not found: " + label);
      }

      return iter->second;
    }

    /**
     * @brief Check if a block with the given label exists.
     * @param label The block label.
     * @return True if block exists, otherwise false.
     */
    [[nodiscard]] bool hasBlock(const std::string &label) const noexcept {
      return blockMap.find(label) != blockMap.end();
    }

  private:
    std::string name;                     /**< The function name. */
    std::vector<types::Parameter> params; /**< The function parameters. */
    types::TypeInfo returnType;           /**< The return type. */

    std::vector<BasicBlockPtr> blocks; /**< The basic blocks. */
    std::unordered_map<std::string, BasicBlock *>
        blockMap; /**< Map from label to block. */
  };
} // namespace verte::ir

#endif // VERTE_BACKEND_IR_FUNCTION_HPP
