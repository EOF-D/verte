/**
 * @brief Type definitions, and other related structures.
 * @file types.hpp
 */

#ifndef VERTE_TYPES_HPP
#define VERTE_TYPES_HPP

#include "verte/backend/ir/operand.hpp"
#include "verte/type_info.hpp"

#include <unordered_map>
#include <variant>
#include <vector>

// Forward declarations.
namespace verte::ir {
  class Function;
} // namespace verte::ir

/**
 * @namespace verte::types
 * @brief Types namespace.
 */
namespace verte::types {
  /**
   * @typedef RetT
   * @brief Return type for visitor functions.
   */
  // clang-format off
  using RetT = std::variant<
    std::monostate,   /**< No return type. */
    std::string,      /**< String return type. */
    TypeInfo,         /**< Type information. */
    ir::Operand,      /**< IR operand. */
    ir::Function*     /**< IR function. */
  >;
  // clang-format on

  /**
   * @struct Function
   * @brief Represents a function.
   */
  struct Function {
    std::string name;     /**< The name of the function. */
    ir::Function *irFunc; /**< The IR function. */
    TypeInfo returnType;  /**< The return type of the function. */

    std::vector<TypeInfo> paramTypes; /**< The types of the parameters. */
    std::unordered_map<std::string, ir::Operand> constants; /**< Constants. */
    std::unordered_map<std::string, ir::Operand>
        locals; /**< Local variables. */

    /**
     * @brief Default constructor.
     */
    Function() = default;

    /**
     * @brief Construct a new Function.
     * @param name The name of the function.
     * @param paramTypes The types of the parameters.
     * @param returnType The return type of the function.
     */
    Function(const std::string &name, const std::vector<TypeInfo> &paramTypes,
             const TypeInfo &returnType)
        : name(name), returnType(returnType), paramTypes(paramTypes) {}
  };
} // namespace verte::types

#endif // VERTE_TYPES_HPP
