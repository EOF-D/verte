/**
 * @brief IR module representation.
 * @file module.hpp
 */

#ifndef VERTE_BACKEND_IR_MODULE_HPP
#define VERTE_BACKEND_IR_MODULE_HPP

#include "verte/backend/ir/function.hpp"

/**
 * @namespace verte::ir
 * @brief Intermediate representation namespace.
 */
namespace verte::ir {
  /**
   * @typedef FunctionPtr
   * @brief Unique pointer to a function.
   */
  using FunctionPtr = std::unique_ptr<Function>;

  /**
   * @class Module
   * @brief Represents an IR module.
   */
  class Module {
  public:
    /**
     * @brief Construct a new Module.
     * @param name The name of the module.
     */
    explicit Module(std::string name) : name(std::move(name)) {}

    /**
     * @brief Get the module name.
     * @return The module name.
     */
    [[nodiscard]] const std::string &getName() const noexcept { return name; }

    /**
     * @brief Get all functions in the module.
     * @return The functions.
     */
    [[nodiscard]] std::vector<FunctionPtr> &getFunctions() noexcept {
      return functions;
    }

    /**
     * @brief Get all functions in the module (const).
     * @return The functions.
     */
    [[nodiscard]] const std::vector<FunctionPtr> &
    getFunctions() const noexcept {
      return functions;
    }

    /**
     * @brief Create a new function.
     * @param name The function name.
     * @param params The function parameters.
     * @param returnType The return type.
     * @return Pointer to the created function.
     */
    Function *createFunction(const std::string &name,
                             std::vector<types::Parameter> params,
                             types::TypeInfo returnType) {
      auto func =
          std::make_unique<Function>(name, std::move(params), returnType);
      auto *ptr = func.get();

      functionMap[name] = ptr;
      functions.push_back(std::move(func));

      return ptr;
    }

    /**
     * @brief Get a function by name.
     * @param name The function name.
     * @return Pointer to the function.
     */
    [[nodiscard]] Function *getFunction(const std::string &name) const {
      auto iter = functionMap.find(name);
      if (iter == functionMap.end()) {
        throw std::runtime_error("Function not found: " + name);
      }

      return iter->second;
    }

    /**
     * @brief Check if a function with the given name exists.
     * @param name The function name.
     * @return True if function exists, otherwise false.
     */
    [[nodiscard]] bool hasFunction(const std::string &name) const noexcept {
      return functionMap.find(name) != functionMap.end();
    }

  private:
    std::string name; /**< The module name. */

    std::vector<FunctionPtr> functions; /**< The functions in the module. */
    std::unordered_map<std::string, Function *>
        functionMap; /**< Map of function names to function pointers. */
  };
} // namespace verte::ir

#endif // VERTE_BACKEND_IR_MODULE_HPP
