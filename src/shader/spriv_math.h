#pragma once
#include <string>
#include <vector>
#include "spirv.h"

#define TSTR std::to_string
#define ENSURE(v, id) if ((id) >= (v).size()) (v).resize((id) + 1)
#define GET_TM(id) (tm.size() > (id) && !tm[(id)].empty() ? tm[(id)] : "TYPE_" + TSTR(id))
#define GET_EXPR(id) (expr.size() > (id) && !expr[(id)].empty() ? expr[(id)] : "v_" + TSTR(id))

#define OP1(op, sym) case op: ENSURE(tm, spv[i+2]); if(len>=4) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+sym+GET_EXPR(spv[i+3])+";\n"; return true;
#define OP2(op, sym) case op: ENSURE(tm, spv[i+2]); if(len>=5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+GET_EXPR(spv[i+3])+" "+sym+" "+GET_EXPR(spv[i+4])+";\n"; return true;
#define OPF(op, fn)  case op: ENSURE(tm, spv[i+2]); if(len>=5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+fn+"("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; return true;
#define OPF1(op, fn) case op: ENSURE(tm, spv[i+2]); if(len>=4) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+fn+"("+GET_EXPR(spv[i+3])+");\n"; return true;
#define OP_CAST(op, cast) case op: ENSURE(tm, spv[i+2]); if(len>=4) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+cast+"("+GET_EXPR(spv[i+3])+");\n"; return true;

inline bool ProcessMathOpcodes(uint16_t op, uint16_t len, const uint32_t* spv, size_t i, 
                               std::string& cur_blk, 
                               std::vector<std::string>& tm, 
                               std::vector<std::string>& expr) {
    switch (op) {
        OPF1(SpvOpAll, "all") OPF1(SpvOpAny, "any") OPF1(SpvOpArrayLength, "length")
        OPF(SpvOpAtomicAnd, "atomicAnd") OPF(SpvOpAtomicExchange, "atomicExchange") OPF(SpvOpAtomicIAdd, "atomicAdd")
        OPF(SpvOpAtomicOr, "atomicOr") OPF(SpvOpAtomicSMax, "atomicMax") OPF(SpvOpAtomicSMin, "atomicMin")
        OPF(SpvOpAtomicUMax, "atomicMax") OPF(SpvOpAtomicUMin, "atomicMin") OPF(SpvOpAtomicXor, "atomicXor")
        OP_CAST(SpvOpBitcast, GET_TM(spv[i+1])) OPF1(SpvOpBitCount, "bitCount") OPF1(SpvOpBitReverse, "bitfieldReverse")
        OP2(SpvOpBitwiseAnd, "&") OP2(SpvOpBitwiseOr, "|") OP2(SpvOpBitwiseXor, "^")
        OP_CAST(SpvOpConvertFToS, "int") OP_CAST(SpvOpConvertFToU, "uint") OP_CAST(SpvOpConvertSToF, "float") OP_CAST(SpvOpConvertUToF, "float")
        OPF(SpvOpDot, "dot") OPF1(SpvOpDPdx, "dFdx") OPF1(SpvOpDPdxCoarse, "dFdxCoarse") OPF1(SpvOpDPdxFine, "dFdxFine")
        OPF1(SpvOpDPdy, "dFdy") OPF1(SpvOpDPdyCoarse, "dFdyCoarse") OPF1(SpvOpDPdyFine, "dFdyFine")
        OP2(SpvOpFAdd, "+") OP2(SpvOpFDiv, "/") OPF(SpvOpFMod, "mod") OP2(SpvOpFMul, "*") OP1(SpvOpFNegate, "-")
        OP2(SpvOpFOrdEqual, "==") OP2(SpvOpFOrdGreaterThan, ">") OP2(SpvOpFOrdGreaterThanEqual, ">=")
        OP2(SpvOpFOrdLessThan, "<") OP2(SpvOpFOrdLessThanEqual, "<=") OP2(SpvOpFOrdNotEqual, "!=")
        OPF(SpvOpFRem, "mod") OP2(SpvOpFSub, "-") OPF1(SpvOpFwidth, "fwidth") OPF1(SpvOpFwidthCoarse, "fwidthCoarse") OPF1(SpvOpFwidthFine, "fwidthFine")
        OPF1(SpvOpGroupNonUniformAll, "subgroupAll") OPF1(SpvOpGroupNonUniformAllEqual, "subgroupAllEqual") OPF1(SpvOpGroupNonUniformAny, "subgroupAny")
        OPF1(SpvOpGroupNonUniformBallot, "subgroupBallot") OPF1(SpvOpGroupNonUniformBallotBitCount, "subgroupBallotBitCount")
        OPF(SpvOpGroupNonUniformBallotBitExtract, "subgroupBallotBitExtract") OPF1(SpvOpGroupNonUniformBallotFindLSB, "subgroupBallotFindLSB")
        OPF1(SpvOpGroupNonUniformBallotFindMSB, "subgroupBallotFindMSB") OPF(SpvOpGroupNonUniformBroadcast, "subgroupBroadcast")
        OPF1(SpvOpGroupNonUniformBroadcastFirst, "subgroupBroadcastFirst") OPF1(SpvOpGroupNonUniformElect, "subgroupElect")
        OPF1(SpvOpGroupNonUniformFAdd, "subgroupAdd") OPF1(SpvOpGroupNonUniformFMin, "subgroupMin") OPF1(SpvOpGroupNonUniformFMul, "subgroupMul")
        OPF1(SpvOpGroupNonUniformIAdd, "subgroupAdd") OPF1(SpvOpGroupNonUniformIMul, "subgroupMul") OPF1(SpvOpGroupNonUniformInverseBallot, "subgroupInverseBallot")
        OPF1(SpvOpGroupNonUniformSMin, "subgroupMin") OPF(SpvOpGroupNonUniformShuffle, "subgroupShuffle") OPF(SpvOpGroupNonUniformShuffleDown, "subgroupShuffleDown")
        OPF(SpvOpGroupNonUniformShuffleUp, "subgroupShuffleUp") OPF(SpvOpGroupNonUniformShuffleXor, "subgroupShuffleXor") OPF1(SpvOpGroupNonUniformUMin, "subgroupMin")
        OP2(SpvOpIAdd, "+") OPF(SpvOpIAddCarry, "uaddCarry") OP2(SpvOpIEqual, "==") OPF(SpvOpImageFetch, "texelFetch")
        OPF1(SpvOpImageQueryLevels, "textureQueryLevels") OPF(SpvOpImageQueryLod, "textureQueryLod") OPF1(SpvOpImageQuerySamples, "textureSamples") OPF1(SpvOpImageQuerySize, "textureSize")
        OPF(SpvOpImageSparseDrefGather, "sparseTextureGather") OPF(SpvOpImageSparseFetch, "sparseTexelFetch") OPF(SpvOpImageSparseGather, "sparseTextureGather")
        OPF(SpvOpImageSparseSampleDrefExplicitLod, "sparseTextureLod") OPF(SpvOpImageSparseSampleDrefImplicitLod, "sparseTexture")
        OPF(SpvOpImageSparseSampleExplicitLod, "sparseTextureLod") OPF(SpvOpImageSparseSampleImplicitLod, "sparseTexture")
        OP2(SpvOpIMul, "*") OP2(SpvOpINotEqual, "!=") OPF1(SpvOpIsInf, "isinf") OPF1(SpvOpIsNan, "isnan") OPF1(SpvOpIsNormal, "isNormal")
        OP2(SpvOpISub, "-") OPF(SpvOpISubBorrow, "usubBorrow") OP2(SpvOpLessOrGreater, "!=") OP2(SpvOpLogicalAnd, "&&")
        OP2(SpvOpLogicalEqual, "==") OP1(SpvOpLogicalNot, "!") OP2(SpvOpLogicalNotEqual, "!=") OP2(SpvOpLogicalOr, "||")
        OP2(SpvOpMatrixTimesMatrix, "*") OP2(SpvOpMatrixTimesScalar, "*") OP2(SpvOpMatrixTimesVector, "*") OP1(SpvOpNot, "~")
        OP2(SpvOpOrdered, "==") OPF(SpvOpOuterProduct, "outerProduct") OPF1(SpvOpRayQueryProceedKHR, "rayQueryProceedEXT")
        OP2(SpvOpSDiv, "/") OP2(SpvOpSGreaterThan, ">") OP2(SpvOpSGreaterThanEqual, ">=") OP2(SpvOpShiftLeftLogical, "<<")
        OP2(SpvOpShiftRightArithmetic, ">>") OP2(SpvOpShiftRightLogical, ">>") OPF1(SpvOpSignBitSet, "sign") OP2(SpvOpSLessThan, "<")
        OP2(SpvOpSLessThanEqual, "<=") OPF(SpvOpSMulExtended, "imulExtended") OP2(SpvOpSMod, "%") OP1(SpvOpSNegate, "-")
        OP2(SpvOpSRem, "%") OPF1(SpvOpTranspose, "transpose") OP2(SpvOpUDiv, "/") OP2(SpvOpUGreaterThan, ">")
        OP2(SpvOpUGreaterThanEqual, ">=") OP2(SpvOpULessThan, "<") OP2(SpvOpULessThanEqual, "<=") OPF(SpvOpUMulExtended, "umulExtended")
        OP2(SpvOpUMod, "%") OP2(SpvOpUnordered, "!=") OP2(SpvOpVectorTimesMatrix, "*") OP2(SpvOpVectorTimesScalar, "*")
        default: return false; 
    }
}