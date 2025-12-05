/**
 * @brief Register allocator implementation.
 * @file register_allocator.cpp
 */

#include "verte/backend/codegen/register_allocator.hpp"

#include <set>

namespace verte::codegen {
  void RegisterAllocator::allocate(ir::Function &func) {
    logger.debug("Allocating registers for function: {}", func.getName());

    // Reset state.
    vregToPhysReg.clear();
    vregToSpillSlot.clear();
    usedCalleeSaved.clear();
    nextSpillSlot = 0;

    // Compute live intervals.
    // Sort by start position (required for linear scan).
    auto intervals = computeLiveIntervals(func);
    std::sort(intervals.begin(), intervals.end(),
              [](const LiveInterval &x, const LiveInterval &y) {
                return x.start < y.start;
              });

    linearScan(intervals);
    logger.debug("Register allocation complete. Spilled {} vregs",
                 vregToSpillSlot.size());
  }

  PhysReg RegisterAllocator::getPhysReg(uint32_t vreg) const {
    auto iter = vregToPhysReg.find(vreg);
    if (iter != vregToPhysReg.end()) {
      return iter->second;
    }

    return PhysReg::NONE;
  }

  bool RegisterAllocator::isSpilled(uint32_t vreg) const {
    return vregToSpillSlot.find(vreg) != vregToSpillSlot.end();
  }

  int32_t RegisterAllocator::getSpillSlot(uint32_t vreg) const {
    auto iter = vregToSpillSlot.find(vreg);
    if (iter != vregToSpillSlot.end()) {
      return iter->second;
    }

    return -1;
  }

  std::vector<LiveInterval>
  RegisterAllocator::computeLiveIntervals(const ir::Function &func) {
    std::unordered_map<uint32_t, size_t> firstDef;
    std::unordered_map<uint32_t, size_t> lastUse;
    std::unordered_map<uint32_t, bool> isFloat;

    // Number all instructions and track vreg usage.
    size_t position = 0;
    for (const auto &block : func.getBlocks()) {
      for (const auto &instr : block->getInstructions()) {
        // Track operands.
        for (const auto &operand : instr.getOperands()) {
          if (operand.isVReg()) {
            uint32_t vreg = operand.getVRegID();
            lastUse[vreg] = position;

            // Determine if float based on type.
            if (isFloat.find(vreg) == isFloat.end()) {
              using DataType = types::TypeInfo::DataType;
              isFloat[vreg] =
                  (operand.getTypeInfo().dataType == DataType::FLOAT ||
                   operand.getTypeInfo().dataType == DataType::DOUBLE);
            }
          }
        }

        // Track destination.
        if (instr.getDest().isVReg()) {
          uint32_t vreg = instr.getDest().getVRegID();

          if (firstDef.find(vreg) == firstDef.end()) {
            firstDef[vreg] = position;
          }

          lastUse[vreg] = position;

          // Determine if float based on type.
          if (isFloat.find(vreg) == isFloat.end()) {
            using DataType = types::TypeInfo::DataType;
            isFloat[vreg] =
                (instr.getDest().getTypeInfo().dataType == DataType::FLOAT ||
                 instr.getDest().getTypeInfo().dataType == DataType::DOUBLE);
          }
        }

        position++;
      }
    }

    // Build intervals.
    std::vector<LiveInterval> intervals;
    std::set<uint32_t> allVregs;

    for (const auto &[vreg, _] : firstDef) {
      allVregs.insert(vreg);
    }

    for (const auto &[vreg, _] : lastUse) {
      allVregs.insert(vreg);
    }

    for (uint32_t vreg : allVregs) {
      size_t start = firstDef.count(vreg) ? firstDef[vreg] : 0;
      size_t end = lastUse.count(vreg) ? lastUse[vreg] : 0;
      bool isFloatReg = isFloat.count(vreg) ? isFloat[vreg] : false;

      if (start <= end) {
        intervals.emplace_back(vreg, start, end, isFloatReg);
      }
    }

    logger.debug("Computed {} live intervals", intervals.size());
    return intervals;
  }

  void RegisterAllocator::linearScan(std::vector<LiveInterval> &intervals) {
    std::vector<PhysReg> freeIntRegs;
    std::vector<PhysReg> freeFloatRegs;

    // Add caller-saved registers first.
    // This ensures callee-saved are preferred.
    for (auto reg : CALLER_SAVED) {
      freeIntRegs.push_back(reg);
    }

    // Add callee-saved registers last.
    // These survive across calls, so we prefer them.
    for (auto reg : CALLEE_SAVED) {
      freeIntRegs.push_back(reg);
    }

    // Float registers.
    for (auto reg : FLOAT_CALLER_SAVED) {
      freeFloatRegs.push_back(reg);
    }

    // Active intervals sorted by end position.
    std::vector<LiveInterval *> active;

    // Process each interval.
    for (size_t i = 0; i < intervals.size(); i++) {
      auto &interval = intervals[i];

      // Expire old intervals.
      expireOldIntervals(intervals, i, active, freeIntRegs, freeFloatRegs);

      // Try to allocate a register.
      std::vector<PhysReg> &freeRegs =
          interval.isFloat ? freeFloatRegs : freeIntRegs;

      // Allocate register.
      if (!freeRegs.empty()) {
        interval.physReg = freeRegs.back();
        freeRegs.pop_back();

        vregToPhysReg[interval.vreg] = interval.physReg;

        // Track callee-saved usage.
        for (auto reg : CALLEE_SAVED) {
          if (interval.physReg == reg) {
            if (std::find(usedCalleeSaved.begin(), usedCalleeSaved.end(),
                          reg) == usedCalleeSaved.end()) {
              usedCalleeSaved.push_back(reg);
            }
            break;
          }
        }

        // Add to active list.
        active.push_back(&interval);

        // Keep active sorted by end position.
        std::sort(active.begin(), active.end(),
                  [](const LiveInterval *a, const LiveInterval *b) {
                    return a->end < b->end;
                  });
      } else {
        spillAtInterval(interval, active);
      }
    }
  }

  void RegisterAllocator::expireOldIntervals(
      const std::vector<LiveInterval> &intervals, size_t index,
      std::vector<LiveInterval *> &active, std::vector<PhysReg> &freeIntRegs,
      std::vector<PhysReg> &freeFloatRegs) {
    const auto &current = intervals[index];

    // Remove intervals that end before current starts.
    for (auto iter = active.begin(); iter != active.end();) {
      auto *interval = *iter;

      if (interval->end < current.start) {
        // Return register to free list.
        if (interval->physReg != PhysReg::NONE) {
          if (interval->isFloat) {
            freeFloatRegs.push_back(interval->physReg);
          }

          else {
            freeIntRegs.push_back(interval->physReg);
          }
        }

        iter = active.erase(iter);
      } else {
        ++iter;
      }
    }
  }

  void RegisterAllocator::spillAtInterval(LiveInterval &interval,
                                          std::vector<LiveInterval *> &active) {
    // Find interval with longest lifetime.
    LiveInterval *spill = active.back();

    if (spill->end > interval.end) {
      // Spill the active interval instead.
      interval.physReg = spill->physReg;
      vregToPhysReg[interval.vreg] = interval.physReg;

      spill->physReg = PhysReg::NONE;
      spill->spillSlot = allocateSpillSlot();
      vregToSpillSlot[spill->vreg] = spill->spillSlot;
      vregToPhysReg.erase(spill->vreg);

      active.pop_back();
      active.push_back(&interval);

      // Re-sort active list.
      std::sort(active.begin(), active.end(),
                [](const LiveInterval *a, const LiveInterval *b) {
                  return a->end < b->end;
                });
    } else {
      // Spill current interval.
      interval.physReg = PhysReg::NONE;
      interval.spillSlot = allocateSpillSlot();
      vregToSpillSlot[interval.vreg] = interval.spillSlot;
    }
  }

  int32_t RegisterAllocator::allocateSpillSlot() {
    // Return negative offset from RBP.
    // Start at -16 (after saved RBP and return address).
    return -(16 + (nextSpillSlot++ * 8));
  }
} // namespace verte::codegen
