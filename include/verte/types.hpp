/**
 * @brief Type definitions, and other related structures.
 * @file types.hpp
 */

#ifndef VERTE_TYPES_HPP
#define VERTE_TYPES_HPP

#include "verte/backend/ir/operand.hpp"
#include "verte/type_info.hpp"

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
} // namespace verte::types

#endif // VERTE_TYPES_HPP
