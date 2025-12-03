/**
 * @brief Type information definition.
 * @file type_info.hpp
 */

#ifndef VERTE_TYPE_INFO_HPP
#define VERTE_TYPE_INFO_HPP

#include <cstdint>
#include <string>

/**
 * @namespace verte::types
 * @brief Types namespace.
 */
namespace verte::types {
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

    std::string name; /**< The name of the type. */

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
      else if (type == "str")
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
          return "str";
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
} // namespace verte::types

#endif // VERTE_TYPE_INFO_HPP
