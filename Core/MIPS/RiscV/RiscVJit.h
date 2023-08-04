// Copyright (c) 2023- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#pragma once

#include <string>
#include <vector>
#include "Common/RiscVEmitter.h"
#include "Core/MIPS/IR/IRJit.h"
#include "Core/MIPS/IR/IRNativeCommon.h"
#include "Core/MIPS/JitCommon/JitState.h"
#include "Core/MIPS/JitCommon/JitCommon.h"
#include "Core/MIPS/RiscV/RiscVRegCache.h"
#include "Core/MIPS/RiscV/RiscVRegCacheFPU.h"

namespace MIPSComp {

// TODO: Separate.
class RiscVJit : public RiscVGen::RiscVCodeBlock, public IRNativeJit, public IRNativeBackend {
public:
	RiscVJit(MIPSState *mipsState);
	~RiscVJit();

	void RunLoopUntil(u64 globalticks) override;

	bool DescribeCodePtr(const u8 *ptr, std::string &name) override;
	bool CodeInRange(const u8 *ptr) const override;
	bool IsAtDispatchFetch(const u8 *ptr) const override;
	const u8 *GetDispatcher() const override;
	const u8 *GetCrashHandler() const override;

	void ClearCache() override;

	JitBlockCacheDebugInterface *GetBlockCacheDebugInterface() override;

protected:
	bool CompileTargetBlock(IRBlock *block, int block_num, bool preload) override;

private:
	void GenerateFixedCode(const JitOptions &jo);

	void RestoreRoundingMode(bool force = false);
	void ApplyRoundingMode(bool force = false);
	void MovFromPC(RiscVGen::RiscVReg r);
	void MovToPC(RiscVGen::RiscVReg r);

	void SaveStaticRegisters();
	void LoadStaticRegisters();

	// Note: destroys SCRATCH1.
	void FlushAll();

	void CompIR_Arith(IRInst inst) override;
	void CompIR_Assign(IRInst inst) override;
	void CompIR_Basic(IRInst inst) override;
	void CompIR_Bits(IRInst inst) override;
	void CompIR_Breakpoint(IRInst inst) override;
	void CompIR_Compare(IRInst inst) override;
	void CompIR_CondAssign(IRInst inst) override;
	void CompIR_CondStore(IRInst inst) override;
	void CompIR_Div(IRInst inst) override;
	void CompIR_Exit(IRInst inst) override;
	void CompIR_ExitIf(IRInst inst) override;
	void CompIR_FArith(IRInst inst) override;
	void CompIR_FAssign(IRInst inst) override;
	void CompIR_FCompare(IRInst inst) override;
	void CompIR_FCondAssign(IRInst inst) override;
	void CompIR_FCvt(IRInst inst) override;
	void CompIR_FLoad(IRInst inst) override;
	void CompIR_FRound(IRInst inst) override;
	void CompIR_FSat(IRInst inst) override;
	void CompIR_FSpecial(IRInst inst) override;
	void CompIR_FStore(IRInst inst) override;
	void CompIR_Generic(IRInst inst) override;
	void CompIR_HiLo(IRInst inst) override;
	void CompIR_Interpret(IRInst inst) override;
	void CompIR_Load(IRInst inst) override;
	void CompIR_LoadShift(IRInst inst) override;
	void CompIR_Logic(IRInst inst) override;
	void CompIR_Mult(IRInst inst) override;
	void CompIR_RoundingMode(IRInst inst) override;
	void CompIR_Shift(IRInst inst) override;
	void CompIR_Store(IRInst inst) override;
	void CompIR_StoreShift(IRInst inst) override;
	void CompIR_System(IRInst inst) override;
	void CompIR_Transfer(IRInst inst) override;
	void CompIR_VecArith(IRInst inst) override;
	void CompIR_VecAssign(IRInst inst) override;
	void CompIR_VecClamp(IRInst inst) override;
	void CompIR_VecHoriz(IRInst inst) override;
	void CompIR_VecLoad(IRInst inst) override;
	void CompIR_VecPack(IRInst inst) override;
	void CompIR_VecStore(IRInst inst) override;
	void CompIR_ValidateAddress(IRInst inst) override;

	void SetScratch1ToSrc1Address(IRReg src1);
	// Modifies SCRATCH regs.
	int32_t AdjustForAddressOffset(RiscVGen::RiscVReg *reg, int32_t constant, int32_t range = 0);
	void NormalizeSrc1(IRInst inst, RiscVGen::RiscVReg *reg, RiscVGen::RiscVReg tempReg, bool allowOverlap);
	void NormalizeSrc12(IRInst inst, RiscVGen::RiscVReg *lhs, RiscVGen::RiscVReg *rhs, RiscVGen::RiscVReg lhsTempReg, RiscVGen::RiscVReg rhsTempReg, bool allowOverlap);
	RiscVGen::RiscVReg NormalizeR(IRRegIndex rs, IRRegIndex rd, RiscVGen::RiscVReg tempReg);

	RiscVRegCache gpr;
	RiscVRegCacheFPU fpr;
	IRNativeBlockCacheDebugInterface debugInterface_;

	const u8 *enterDispatcher_ = nullptr;

	const u8 *outerLoop_ = nullptr;
	const u8 *outerLoopPCInSCRATCH1_ = nullptr;
	const u8 *dispatcherCheckCoreState_ = nullptr;
	const u8 *dispatcherPCInSCRATCH1_ = nullptr;
	const u8 *dispatcher_ = nullptr;
	const u8 *dispatcherNoCheck_ = nullptr;
	const u8 *dispatcherFetch_ = nullptr;
	const u8 *applyRoundingMode_ = nullptr;

	const u8 *saveStaticRegisters_ = nullptr;
	const u8 *loadStaticRegisters_ = nullptr;

	const u8 *crashHandler_ = nullptr;

	int jitStartOffset_ = 0;
};

} // namespace MIPSComp
