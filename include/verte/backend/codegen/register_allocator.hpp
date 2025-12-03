/**
 * @brief Register allocator for x86-64 code generation.
 * @file register_allocator.hpp
 */

#ifndef VERTE_BACKEND_CODEGEN_REGISTER_ALLOCATOR_HPP
#define VERTE_BACKEND_CODEGEN_REGISTER_ALLOCATOR_HPP

#include "verte/backend/ir/function.hpp"
#include "verte/utils/logger.hpp"

/**
 * @namespace verte::codegen
 * @brief Code generation namespace. Contains all code generation related
 * classes and functions.
 */
namespace verte::codegen {
  /**
   * @enum PhysReg
   * @brief Physical x86-64 registers.
   */
  enum class PhysReg : uint8_t {
    // General purpose registers.
    RAX = 0,
    RBX,
    RCX,
    RDX,
    RSI,
    RDI,
    RBP,
    RSP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    // SSE registers for floating point.
    XMM0 = 16,
    XMM1,
    XMM2,
    XMM3,
    XMM4,
    XMM5,
    XMM6,
    XMM7,
    XMM8,
    XMM9,
    XMM10,
    XMM11,
    XMM12,
    XMM13,
    XMM14,
    XMM15,

    NONE = 255 /**< Spilled register (none). */
  };

  /**
   * @brief System V ABI integer argument registers.
   */
  inline constexpr PhysReg INT_ARG_REGS[] = {PhysReg::RDI, PhysReg::RSI,
                                             PhysReg::RDX, PhysReg::RCX,
                                             PhysReg::R8,  PhysReg::R9};

  /**
   * @brief System V ABI float argument registers.
   */
  inline constexpr PhysReg FLOAT_ARG_REGS[] = {
      PhysReg::XMM0, PhysReg::XMM1, PhysReg::XMM2, PhysReg::XMM3,
      PhysReg::XMM4, PhysReg::XMM5, PhysReg::XMM6, PhysReg::XMM7};

  /**
   * @brief System V ABI callee-saved integer registers.
   */
  inline constexpr PhysReg CALLEE_SAVED[] = {
      PhysReg::RBX, PhysReg::R12, PhysReg::R13, PhysReg::R14, PhysReg::R15};

  /**
   * @brief System V ABI caller-saved integer registers (excluding args).
   */
  inline constexpr PhysReg CALLER_SAVED[] = {PhysReg::R10, PhysReg::R11};

  /**
   * @brief System V ABI caller-saved float registers (excluding arg).
   */
  inline constexpr PhysReg FLOAT_CALLER_SAVED[] = {
      PhysReg::XMM8,  PhysReg::XMM9,  PhysReg::XMM10, PhysReg::XMM11,
      PhysReg::XMM12, PhysReg::XMM13, PhysReg::XMM14, PhysReg::XMM15};

  /**
   * @struct LiveInterval
   * @brief Represents the live range of a virtual register.
   */
  struct LiveInterval {
    PhysReg physReg;   /**< Assigned physical register. */
    uint32_t vreg;     /**< Virtual register ID. */
    size_t start;      /**< First instruction position. */
    size_t end;        /**< Last instruction position. */
    bool isFloat;      /**< True if floating-point register. */
    int32_t spillSlot; /**< Stack slot if spilled (-1 if not spilled). */

    /**
     * @brief Construct a new LiveInterval.
     * @param vreg Virtual register ID.
     * @param start Start position.
     * @param end End position.
     * @param isFloat Whether this is a float register or not.
     */
    LiveInterval(uint32_t vreg, size_t start, size_t end, bool isFloat)
        : physReg(PhysReg::NONE), vreg(vreg), start(start), end(end),
          isFloat(isFloat), spillSlot(-1) {}
  };

  /**
   * @class RegisterAllocator
   * @brief Linear scan register allocator for x86-64.
   */
  class RegisterAllocator {
  public:
    /**
     * @brief Construct a new RegisterAllocator.
     */
    RegisterAllocator() : logger("register-allocator") {}

    /**
     * @brief Allocate physical registers for a function.
     * @param func The IR function to allocate registers for.
     */
    void allocate(ir::Function &func);

    /**
     * @brief Get the physical register assigned to a virtual register.
     * @param vreg Virtual register ID.
     * @return Physical register, or PhysReg::NONE if spilled.
     */
    [[nodiscard]] PhysReg getPhysReg(uint32_t vreg) const;

    /**
     * @brief Check if a virtual register was spilled.
     * @param vreg Virtual register ID.
     * @return True if spilled, false otherwise.
     */
    [[nodiscard]] bool isSpilled(uint32_t vreg) const;

    /**
     * @brief Get the spill slot for a virtual register.
     * @param vreg Virtual register ID.
     * @return Stack offset for spill slot, or -1 if not spilled.
     */
    [[nodiscard]] int32_t getSpillSlot(uint32_t vreg) const;

    /**
     * @brief Get all callee-saved registers that were used.
     * @return Vector of used callee-saved registers.
     */
    [[nodiscard]] const std::vector<PhysReg> &getUsedCalleeSaved() const {
      return usedCalleeSaved;
    }

    /**
     * @brief Get the total stack space needed for spills.
     * @return Stack space in bytes (8-byte alignment).
     */
    [[nodiscard]] size_t getSpillStackSize() const { return nextSpillSlot * 8; }

  private:
    /**
     * @brief Compute live intervals for all virtual registers.
     * @param func The function to do liveness analysis on.
     * @return Vector of live intervals sorted by start position.
     */
    std::vector<LiveInterval> computeLiveIntervals(const ir::Function &func);

    /**
     * @brief Perform linear scan for register allocation.
     * @param intervals Live intervals sorted by start position.
     */
    void linearScan(std::vector<LiveInterval> &intervals);

    /**
     * @brief Expire old intervals that are no longer live.
     * @param intervals All live intervals.
     * @param index Current interval index.
     * @param active Currently active intervals.
     * @param freeIntRegs Free integer registers.
     * @param freeFloatRegs Free float registers.
     */
    void expireOldIntervals(const std::vector<LiveInterval> &intervals,
                            size_t index, std::vector<LiveInterval *> &active,
                            std::vector<PhysReg> &freeIntRegs,
                            std::vector<PhysReg> &freeFloatRegs);

    /**
     * @brief Spill a register to memory.
     * @param interval Interval to spill.
     * @param active Currently active intervals.
     */
    void spillAtInterval(LiveInterval &interval,
                         std::vector<LiveInterval *> &active);

    /**
     * @brief Allocate a spill slot.
     * @return Offset from frame pointer (negative).
     */
    int32_t allocateSpillSlot();

    std::unordered_map<uint32_t, PhysReg>
        vregToPhysReg; /**< Vreg to physical registers. */

    std::unordered_map<uint32_t, int32_t>
        vregToSpillSlot; /**< Vreg to spill slots. */

    std::vector<PhysReg> usedCalleeSaved; /**< Callee-saved regs used. */

    int32_t nextSpillSlot; /**< Next available spill slot. */

    utils::Logger logger; /**< The logger. */
  };
} // namespace verte::codegen

#endif // VERTE_BACKEND_CODEGEN_REGISTER_ALLOCATOR_HPP
