/**
 * @file types.hpp
 * @brief Structures for type stuff.
 */

#ifndef EXCERPT_TYPES_HPP
#define EXCERPT_TYPES_HPP

#include <cstdint>
#include <string>

namespace excerpt {
  /**
   * @brief Holds type information.
   */
  struct TypeInfo {
    /**
     * @brief Enum for data types.
     */
    enum class DataType : uint8_t {
      INTEGER,  /**< Integer type. */
      FLOAT,    /**< Floating point type. */
      DOUBLE,   /**< Double precision floating point type. */
      STRING,   /**< String type. */
      BOOL,     /**< Boolean type. */
      VOID,     /**< Void type. */
      UNKNOWN   /**< Unknown type. */
    } dataType; /**< Data type of the type. */

    std::string name; /**< Name of the type. */

    /**
     * @brief Default constructor.
     */
    TypeInfo() : dataType(DataType::UNKNOWN), name("UNKNOWN") {}

    /**
     * @brief Constructor.
     * @param dataType Data type.
     * @param name Name of the type.
     */
    TypeInfo(DataType dataType, const std::string &name)
        : dataType(dataType), name(name) {}

    /**
     * @brief Constructor.
     * @param dataType Data type.
     */
    explicit TypeInfo(DataType dataType)
        : dataType(dataType), name(toString(dataType)) {}

    /**
     * @brief Get the data type.
     * @return Data type.
     */
    inline DataType getDataType() const { return dataType; }

    /**
     * @brief Get the name of the type.
     * @return Name of the type.
     */
    inline const std::string &getName() const { return name; }

    /**
     * @brief Convert a string to a DataType.
     * @param type The string to convert.
     * @return The converted DataType.
     */
    static DataType toDataType(const std::string &type) {
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
     * @param dataType The DataType to convert.
     * @return The converted string.
     */
    static std::string toString(DataType dataType) {
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
          return "unknown";
      }

      return "unknown";
    }
  };

  /**
   * @brief Represents a parameter in a function declaration.
   */
  struct Parameter {
    std::string name;  /**< The name of the parameter. */
    TypeInfo typeInfo; /**< The type of the parameter. */

    /**
     * @brief Construct a new Parameter object.
     * @param name The name of the parameter.
     * @param type The type of the parameter.
     */
    Parameter(const std::string &name, const TypeInfo &typeInfo)
        : name(name), typeInfo(typeInfo){};
  };
} // namespace excerpt

#endif // EXCERPT_TYPES_HPP
