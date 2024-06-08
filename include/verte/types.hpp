/**
 * @brief Type definitions, and other related structures.
 * @file types.hpp
 */

#ifndef VERTE_TYPES_HPP
#define VERTE_TYPES_HPP

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace verte::types {
  class TypeInfo; // Forward declaration.

  /**
   * @typedef RetT
   * @brief Return type for visitor functions.
   */
  // clang-format off
  using RetT = std::variant<
    std::monostate, /**< No return type. */
    std::string,   /**< String return type. */
    TypeInfo,     /**< Type information. */
    llvm::Value *, /**< LLVM value. */
    llvm::Function * /**< LLVM function. */
  >;
  // clang-format on

  /**
   * @struct TypeInfo
   * @brief Represents type information for a node.
   */
  struct TypeInfo {
    /**
     * @enum DataType
     * @brief Enum for the data type of the node.
     */
    enum class DataType : uint8_t {
      INTEGER,  /**< Integer type. */
      FLOAT,    /**< Floating-point type. */
      DOUBLE,   /**< Double type. */
      STRING,   /**< String type. */
      BOOL,     /**< Boolean type. */
      VOID,     /**< Void type. */
      UNKNOWN   /**< Unknown type. */
    } dataType; /**< The data type of the node. */

    const std::string name; /**< The name of the type. */

    /**
     * @brief Default constructor.
     */
    TypeInfo() noexcept : dataType(DataType::UNKNOWN), name("UNKNOWN") {}

    /**
     * @brief Construct a new TypeInfo.
     * @param dataType Data type to use.
     * @param name Name of the type.
     */
    TypeInfo(DataType dataType, const std::string &name) noexcept
        : dataType(dataType), name(name) {}

    /**
     * @brief Construct a new TypeInfo.
     * @param dataType Data type to use.
     */
    TypeInfo(DataType dataType) noexcept
        : dataType(dataType), name(toString(dataType)) {}

    /**
     * @brief Convert a string to a DataType.
     * @param type The string to convert.
     * @return The converted DataType.
     */
    static DataType toEnum(const std::string &type) {
      if (type == "int")
        return DataType::INTEGER;
      else if (type == "float")
        return DataType::FLOAT;
      else if (type == "double")
        return DataType::DOUBLE;
      else if (type == "string")
        return DataType::STRING;
      else if (type == "bool")
        return DataType::BOOL;
      else if (type == "void")
        return DataType::VOID;
      return DataType::UNKNOWN;
    }

    /**
     * @brief Convert a DataType to a string.
     * @param dataType Data type to convert.
     * @return The string representation of the data type.
     */
    static std::string toString(DataType dataType) noexcept {
      switch (dataType) {
        case DataType::INTEGER:
          return "int";
        case DataType::FLOAT:
          return "float";
        case DataType::DOUBLE:
          return "double";
        case DataType::STRING:
          return "string";
        case DataType::BOOL:
          return "bool";
        case DataType::VOID:
          return "void";
        case DataType::UNKNOWN:
        default:
          return "unknown";
      }
    }
  };

  /**
   * @struct Parameter
   * @brief Represents a parameter in a function declaration.
   */
  struct Parameter {
    std::string name; /**< The name of the parameter. */
    TypeInfo type;    /**< The type of the parameter. */

    /**
     * @brief Construct a new Parameter.
     * @param name The name of the parameter.
     * @param type The type of the parameter.
     */
    Parameter(const std::string &name, const TypeInfo &type)
        : name(name), type(type){};
  };

  /**
   * @struct Function
   * @brief Represents a function.
   */
  struct Function {
    std::string name;         /**< The name of the function. */
    llvm::Function *llvmFunc; /**< The LLVM function. */
    llvm::Type *retType;      /**< The return type of the function. */

    std::vector<llvm::Type *> paramTypes; /**< The types of the parameters. */
    std::unordered_map<std::string, llvm::Constant *>
        constants; /**< Constants. */

    std::unordered_map<std::string, llvm::AllocaInst *>
        locals; /**< Local variables. */

    /**
     * @brief Default constructor.
     */
    Function() = default;

    /**
     * @brief Construct a new Function.
     * @param name The name of the function.
     * @param paramTypes The types of the parameters.
     * @param retType The return type of the function.
     */
    Function(const std::string &name,
             const std::vector<llvm::Type *> &paramTypes, llvm::Type *retType)
        : name(name), retType(retType), paramTypes(paramTypes) {}
  };
} // namespace verte::types

#endif // VERTE_TYPES_HPP
