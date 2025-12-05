/**
 * @brief Instruction selector for x86-64 code generation.
 * @file instruction_selector.hpp
 */

#ifndef VERTE_BACKEND_CODEGEN_INSTRUCTION_SELECTOR_HPP
#define VERTE_BACKEND_CODEGEN_INSTRUCTION_SELECTOR_HPP

#include "verte/backend/codegen/codegen.hpp"
#include "verte/backend/codegen/register_allocator.hpp"
#include "verte/backend/ir/module.hpp"
#include "verte/utils/logger.hpp"

#include <set>

/**
 * @namespace verte::codegen
 * @brief Code generation namespace. Contains all code generation related
 * classes and functions.
 */
namespace verte::codegen {
  /**
   * @struct X86Instruction
   * @brief Represents an x86-64 assembly instruction.
   */
  struct X86Instruction {
    std::string mnemonic;              /**< The instruction mnemonic. */
    std::vector<std::string> operands; /**< The instruction operands. */
    std::string comment;               /**< Optional comment. */

    /**
     * @brief Default constructor.
     */
    X86Instruction() noexcept = default;

    /**
     * @brief Construct with mnemonic only (no operands).
     * @param mnemonic The instruction mnemonic.
     */
    explicit X86Instruction(std::string mnemonic) noexcept
        : mnemonic(std::move(mnemonic)) {}

    /**
     * @brief Construct with mnemonic and one operand.
     * @param mnemonic The instruction mnemonic.
     * @param op The operand.
     */
    X86Instruction(std::string mnemonic, std::string op) noexcept
        : mnemonic(std::move(mnemonic)), operands{std::move(op)} {}

    /**
     * @brief Construct with mnemonic and two operands.
     * @param mnemonic The instruction mnemonic.
     * @param op1 The first operand.
     * @param op2 The second operand.
     */
    X86Instruction(std::string mnemonic, std::string op1,
                   std::string op2) noexcept
        : mnemonic(std::move(mnemonic)),
          operands{std::move(op1), std::move(op2)} {}

    /**
     * @brief Construct with mnemonic, operands, and comment.
     * @param mnemonic The instruction mnemonic.
     * @param op1 The first operand.
     * @param op2 The second operand.
     * @param comment The comment.
     */
    X86Instruction(std::string mnemonic, std::string op1, std::string op2,
                   std::string comment) noexcept
        : mnemonic(std::move(mnemonic)),
          operands{std::move(op1), std::move(op2)},
          comment(std::move(comment)) {}

    /**
     * @brief Convert instruction to assembly string.
     * @return The assembly string representation.
     */
    [[nodiscard]] std::string toString() const {
      std::ostringstream oss;

      // Emit mnemonic.
      oss << "    " << mnemonic;

      // Emit operands if present.
      if (!operands.empty()) {
        oss << " ";
        for (size_t i = 0; i < operands.size(); ++i) {
          if (i > 0) {
            oss << ", ";
          }

          oss << operands[i];
        }
      }

      // Emit comment if present.
      if (!comment.empty()) {
        oss << " ; " << comment;
      }

      return oss.str();
    }
  };

  /**
   * @struct FloatConstant
   * @brief Represents a floating-point constant for the data section.
   */
  struct FloatConstant {
    std::string label; /**< The label for the constant. */
    double value;      /**< The floating-point value. */

    /**
     * @brief Construct a new FloatConstant.
     * @param label The label.
     * @param value The value.
     */
    FloatConstant(std::string label, double value) noexcept
        : label(std::move(label)), value(value) {}
  };

  /**
   * @struct StringConstant
   * @brief Represents a string constant for the data section.
   */
  struct StringConstant {
    std::string label; /**< The label for the constant. */
    std::string value; /**< The string value. */

    /**
     * @brief Construct a new StringConstant.
     * @param label The label.
     * @param value The value.
     */
    StringConstant(std::string label, std::string value) noexcept
        : label(std::move(label)), value(std::move(value)) {}
  };

  /**
   * @struct FunctionCode
   * @brief Represents the generated code for a function.
   */
  struct FunctionCode {
    std::string name;                     /**< Function name. */
    std::vector<X86Instruction> prologue; /**< Prologue instructions. */
    std::vector<X86Instruction> body;     /**< Body instructions. */
    std::vector<X86Instruction> epilogue; /**< Epilogue instructions. */
    std::vector<PhysReg> usedCalleeSaved; /**< Callee-saved registers used. */
    size_t stackSize;                     /**< Total stack frame size. */

    /**
     * @brief Construct a new FunctionCode.
     * @param name The function name.
     */
    explicit FunctionCode(std::string name) noexcept
        : name(std::move(name)), stackSize(0) {}
  };

  /**
   * @class InstructionSelector
   * @brief Selects x86-64 instructions from IR.
   */
  class InstructionSelector {
  public:
    /**
     * @brief Construct a new InstructionSelector.
     */
    InstructionSelector() noexcept
        : nextFloatLabel(0), nextStringLabel(0), currentFunctionName(),
          logger("instr-selector") {}

    /**
     * @brief Select x86-64 instructions for the entire module.
     * @param module The IR module to select instructions for.
     * @param allocators Map of function names to their register allocators.
     * @param codegen The codegen instance to extract string constants from.
     */
    void
    select(const ir::Module &module,
           const std::unordered_map<std::string, RegisterAllocator> &allocators,
           const Codegen &codegen);

    /**
     * @brief Get the generated function code.
     * @return Vector of function code structures.
     */
    [[nodiscard]] const std::vector<FunctionCode> &getFunctions() const {
      return functions;
    }

    /**
     * @brief Get the floating-point constants.
     * @return Vector of float constants.
     */
    [[nodiscard]] const std::vector<FloatConstant> &getFloatConstants() const {
      return floatConstants;
    }

    /**
     * @brief Get the string constants.
     * @return Vector of string constants.
     */
    [[nodiscard]] const std::vector<StringConstant> &
    getStringConstants() const {
      return stringConstants;
    }

    /**
     * @brief Get the external function names used.
     * @return Set of external function names.
     */
    [[nodiscard]] const std::set<std::string> &getExternFunctions() const {
      return externFunctions;
    }

    /**
     * @brief Get the 64-bit register name for a physical register.
     * @param reg The physical register.
     * @return The register name string.
     */
    [[nodiscard]] static std::string getRegName64(PhysReg reg);

  private:
    /**
     * @brief Extract string constants from codegen.
     * @param codegen The codegen instance.
     */
    void extractStringConstants(const Codegen &codegen);

    /**
     * @brief Extract external functions from codegen.
     * @param codegen The codegen instance.
     */
    void extractExternalFunctions(const Codegen &codegen);

    /**
     * @brief Select instructions for a single function.
     * @param func The IR function.
     * @param allocator The register allocator for this function.
     * @return The generated function code.
     */
    FunctionCode selectFunction(const ir::Function &func,
                                const RegisterAllocator &allocator);

    /**
     * @brief Select instructions for a basic block.
     * @param block The basic block.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectBlock(const ir::BasicBlock &block,
                     const RegisterAllocator &allocator,
                     std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a single IR instruction.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectInstruction(const ir::Instruction &instr,
                           const RegisterAllocator &allocator,
                           std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a binary operation.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectBinaryOp(const ir::Instruction &instr,
                        const RegisterAllocator &allocator,
                        std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a unary operation.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectUnaryOp(const ir::Instruction &instr,
                       const RegisterAllocator &allocator,
                       std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a comparison operation.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectCompareOp(const ir::Instruction &instr,
                         const RegisterAllocator &allocator,
                         std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a load operation.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectLoad(const ir::Instruction &instr,
                    const RegisterAllocator &allocator,
                    std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a store operation.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectStore(const ir::Instruction &instr,
                     const RegisterAllocator &allocator,
                     std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a branch operation.
     * @param instr The IR instruction.
     * @param code Output vector for instructions.
     */
    void selectBranch(const ir::Instruction &instr,
                      std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a conditional branch operation.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectCondBranch(const ir::Instruction &instr,
                          const RegisterAllocator &allocator,
                          std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a function call.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectCall(const ir::Instruction &instr,
                    const RegisterAllocator &allocator,
                    std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for a return operation.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectReturn(const ir::Instruction &instr,
                      const RegisterAllocator &allocator,
                      std::vector<X86Instruction> &code);

    /**
     * @brief Select instructions for type conversion.
     * @param instr The IR instruction.
     * @param allocator The register allocator.
     * @param code Output vector for instructions.
     */
    void selectConversion(const ir::Instruction &instr,
                          const RegisterAllocator &allocator,
                          std::vector<X86Instruction> &code);

    /**
     * @brief Format an operand to its assembly string representation.
     * @param operand The IR operand.
     * @param allocator The register allocator.
     * @return The assembly string.
     */
    [[nodiscard]] std::string formatOperand(const ir::Operand &operand,
                                            const RegisterAllocator &allocator);

    /**
     * @brief Check if a type is floating-point.
     * @param type The type info.
     * @return True if float or double.
     */
    [[nodiscard]] static bool isFloatType(const types::TypeInfo &type) {
      return type.dataType == types::TypeInfo::DataType::FLOAT ||
             type.dataType == types::TypeInfo::DataType::DOUBLE;
    }

    /**
     * @brief Create a label for a float constant and store it.
     * @param value The float value.
     * @return The label string.
     */
    std::string createFloatConstant(double value);

    /**
     * @brief Create a label for a string constant and store it.
     * @param value The string value.
     * @return The label string.
     */
    std::string createStringConstant(const std::string &value);

    /**
     * @brief Generate function prologue.
     * @param func The function.
     * @param allocator The register allocator.
     * @param code Output function code.
     */
    void generatePrologue(const ir::Function &func,
                          const RegisterAllocator &allocator,
                          FunctionCode &code);

    /**
     * @brief Generate function epilogue.
     * @param func The function.
     * @param allocator The register allocator.
     * @param code Output function code.
     */
    void generateEpilogue(const ir::Function &func,
                          const RegisterAllocator &allocator,
                          FunctionCode &code);

    /**
     * @brief Make a label unique to the current function.
     * @param label The base label name.
     * @return The function unique label.
     */
    [[nodiscard]] std::string makeLabel(const std::string &label) const {
      return "." + currentFunctionName + "_" + label;
    }

    std::vector<FunctionCode> functions;       /**< Generated function code. */
    std::vector<FloatConstant> floatConstants; /**< Float constants. */
    std::vector<StringConstant> stringConstants; /**< String constants. */
    std::set<std::string> externFunctions;       /**< External functions. */

    uint32_t nextFloatLabel;         /**< Counter for float constant labels. */
    uint32_t nextStringLabel;        /**< Counter for string constant labels. */
    std::string currentFunctionName; /**< Current function being processed. */

    utils::Logger logger; /**< The logger. */
  };
} // namespace verte::codegen

#endif // VERTE_BACKEND_CODEGEN_INSTRUCTION_SELECTOR_HPP
