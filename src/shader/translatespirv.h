#pragma once
#include <string>
#include <unordered_map>
#include <string.h>
#include <vector>
#include <utility>
#include "spirv.h"

#define TSTR std::to_string
#define OP1(op, sym) case op: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = "+sym+"v_"+TSTR(spv[i+3])+";\n"; break;
#define OP2(op, sym) case op: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = v_"+TSTR(spv[i+3])+" "+sym+" v_"+TSTR(spv[i+4])+";\n"; break;
#define OPF(op, fn)  case op: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = "+fn+"(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+");\n"; break;
#define OPF1(op, fn) case op: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = "+fn+"(v_"+TSTR(spv[i+3])+");\n"; break;
#define OP_CAST(op, cast) case op: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = "+cast+"(v_"+TSTR(spv[i+3])+");\n"; break;

inline std::string TranslateSpirvFull(const uint32_t* spv, size_t size) {
    std::unordered_map<uint32_t, std::string> tm, expr;
    std::unordered_map<uint32_t, uint32_t> loc, bind, blt;
    std::unordered_map<uint32_t, std::string> blocks;
    std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, std::string>>> phi_defs;    
    #define APPLY_PHI \
    if (phi_defs.count(cur_label)) { \
        for (auto& p : phi_defs[cur_label]) cur_blk += "        " + tm[p.first] + " _phi_" + TSTR(p.first) + " = " + p.second + ";\n"; \
        for (auto& p : phi_defs[cur_label]) cur_blk += "        v_" + TSTR(p.first) + " = _phi_" + TSTR(p.first) + ";\n"; \
    }    
    std::string head = "#version 310 es\nprecision highp float;\nprecision highp int;\n\n";
    std::string vars = "";
    std::string cur_blk = "";
    uint32_t cur_label = 0;
    uint32_t first_label = 0;
    uint32_t exec_model = 0;
    size_t i = 5;
    while (i < size) {
        uint16_t op = spv[i] & 0xFFFF;
        uint16_t len = spv[i] >> 16;

        switch (op) {            
            OPF1(SpvOpAll, "all")
            OPF1(SpvOpAny, "any")
            OPF1(SpvOpArrayLength, "length")
            OPF(SpvOpAtomicAnd, "atomicAnd")
            OPF(SpvOpAtomicExchange, "atomicExchange")
            OPF(SpvOpAtomicIAdd, "atomicAdd")
            OPF(SpvOpAtomicOr, "atomicOr")
            OPF(SpvOpAtomicSMax, "atomicMax")
            OPF(SpvOpAtomicSMin, "atomicMin")
            OPF(SpvOpAtomicUMax, "atomicMax")
            OPF(SpvOpAtomicUMin, "atomicMin")
            OPF(SpvOpAtomicXor, "atomicXor")
            OP_CAST(SpvOpBitcast, tm[spv[i+1]])
            OPF1(SpvOpBitCount, "bitCount")
            OPF1(SpvOpBitReverse, "bitfieldReverse")
            OP2(SpvOpBitwiseAnd, "&")
            OP2(SpvOpBitwiseOr, "|")
            OP2(SpvOpBitwiseXor, "^")
            OP_CAST(SpvOpConvertFToS, "int")
            OP_CAST(SpvOpConvertFToU, "uint")
            OP_CAST(SpvOpConvertSToF, "float")
            OP_CAST(SpvOpConvertUToF, "float")
            OPF(SpvOpDot, "dot")
            OPF1(SpvOpDPdx, "dFdx")
            OPF1(SpvOpDPdxCoarse, "dFdxCoarse")
            OPF1(SpvOpDPdxFine, "dFdxFine")
            OPF1(SpvOpDPdy, "dFdy")
            OPF1(SpvOpDPdyCoarse, "dFdyCoarse")
            OPF1(SpvOpDPdyFine, "dFdyFine")
            OPF(SpvOpExtInst, "mix")
            OP2(SpvOpFAdd, "+")
            OP2(SpvOpFDiv, "/")
            OPF(SpvOpFMod, "mod")
            OP2(SpvOpFMul, "*")
            OP1(SpvOpFNegate, "-")
            OP2(SpvOpFOrdEqual, "==")
            OP2(SpvOpFOrdGreaterThan, ">")
            OP2(SpvOpFOrdGreaterThanEqual, ">=")
            OP2(SpvOpFOrdLessThan, "<")
            OP2(SpvOpFOrdLessThanEqual, "<=")
            OP2(SpvOpFOrdNotEqual, "!=")
            OPF(SpvOpFRem, "mod")
            OP2(SpvOpFSub, "-")
            OPF1(SpvOpFwidth, "fwidth")
            OPF1(SpvOpFwidthCoarse, "fwidthCoarse")
            OPF1(SpvOpFwidthFine, "fwidthFine")
            OP2(SpvOpIAdd, "+")
            OP2(SpvOpIEqual, "==")
            OPF(SpvOpImageFetch, "texelFetch")
            OPF1(SpvOpImageQueryLevels, "textureQueryLevels")
            OPF(SpvOpImageQueryLod, "textureQueryLod")
            OPF1(SpvOpImageQuerySamples, "textureSamples")
            OPF1(SpvOpImageQuerySize, "textureSize")
            OP2(SpvOpIMul, "*")
            OP2(SpvOpINotEqual, "!=")
            OPF1(SpvOpIsInf, "isinf")
            OPF1(SpvOpIsNan, "isnan")
            OP2(SpvOpISub, "-")
            OP2(SpvOpLogicalAnd, "&&")
            OP2(SpvOpLogicalEqual, "==")
            OP1(SpvOpLogicalNot, "!")
            OP2(SpvOpLogicalNotEqual, "!=")
            OP2(SpvOpLogicalOr, "||")
            OP2(SpvOpMatrixTimesMatrix, "*")
            OP2(SpvOpMatrixTimesScalar, "*")
            OP2(SpvOpMatrixTimesVector, "*")
            OP1(SpvOpNot, "~")
            OPF(SpvOpOuterProduct, "outerProduct")
            OP2(SpvOpSDiv, "/")
            OP2(SpvOpSGreaterThan, ">")
            OP2(SpvOpSGreaterThanEqual, ">=")
            OP2(SpvOpShiftLeftLogical, "<<")
            OP2(SpvOpShiftRightArithmetic, ">>")
            OP2(SpvOpShiftRightLogical, ">>")
            OPF1(SpvOpSignBitSet, "sign")
            OP2(SpvOpSLessThan, "<")
            OP2(SpvOpSLessThanEqual, "<=")
            OP2(SpvOpSMod, "%")
            OP1(SpvOpSNegate, "-")
            OP2(SpvOpSRem, "%")
            OPF1(SpvOpTranspose, "transpose")
            OP2(SpvOpUDiv, "/")
            OP2(SpvOpUGreaterThan, ">")
            OP2(SpvOpUGreaterThanEqual, ">=")
            OP2(SpvOpULessThan, "<")
            OP2(SpvOpULessThanEqual, "<=")
            OP2(SpvOpUMod, "%")
            OP2(SpvOpVectorTimesMatrix, "*")
            OP2(SpvOpVectorTimesScalar, "*")
            
            case SpvOpAccessChain: {
                std::string res = expr.count(spv[i+3]) ? expr[spv[i+3]] : "v_"+TSTR(spv[i+3]);
                for (size_t j=4; j<len; ++j) res += "[" + (expr.count(spv[i+j]) ? expr[spv[i+j]] : "v_"+TSTR(spv[i+j])) + "]";
                expr[spv[i+2]] = res;
                break;
            }
            case SpvOpAtomicCompareExchange: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = atomicCompSwap(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+7])+", v_"+TSTR(spv[i+8])+");\n"; break;
            case SpvOpBitFieldInsert: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = bitfieldInsert(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+", v_"+TSTR(spv[i+5])+", v_"+TSTR(spv[i+6])+");\n"; break;
            case SpvOpBitFieldSExtract:
            case SpvOpBitFieldUExtract: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = bitfieldExtract(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+", v_"+TSTR(spv[i+5])+");\n"; break;
            case SpvOpBranch: 
                APPLY_PHI 
                cur_blk += "        _state = " + TSTR(spv[i+1]) + "; break;\n"; 
                break;                
            case SpvOpBranchConditional: 
                APPLY_PHI 
                cur_blk += "        if(v_"+TSTR(spv[i+1])+") _state = "+TSTR(spv[i+2])+"; else _state = "+TSTR(spv[i+3])+"; break;\n"; 
                break;
            case SpvOpCompositeConstruct: {
                std::string args = "";
                for(size_t j=3; j<len; ++j) args += (j==3?"":", ") + (expr.count(spv[i+j])?expr[spv[i+j]]:"v_"+TSTR(spv[i+j]));
                cur_blk += tm[spv[i+1]] + " v_" + TSTR(spv[i+2]) + " = " + tm[spv[i+1]] + "(" + args + ");\n";
                break;
            }
            case SpvOpCompositeExtract: {
                std::string res = expr.count(spv[i+3]) ? expr[spv[i+3]] : "v_"+TSTR(spv[i+3]);
                for (size_t j=4; j<len; ++j) res += "[" + TSTR(spv[i+j]) + "]";
                expr[spv[i+2]] = res; 
                break;
            }
            case SpvOpConstant: {
                if (tm[spv[i+1]] == "float") { float f; memcpy(&f, &spv[i+3], 4); expr[spv[i+2]] = TSTR(f); } 
                else expr[spv[i+2]] = TSTR(spv[i+3]);
                break;
            }
            case SpvOpConstantFalse: expr[spv[i+2]] = "false"; break;
            case SpvOpConstantTrue: expr[spv[i+2]] = "true"; break;
            case SpvOpControlBarrier: cur_blk += "        barrier();\n"; break;
            case SpvOpCopyObject: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = v_"+TSTR(spv[i+3])+";\n"; break;
            case SpvOpDecorate:
                if (spv[i+2] == SpvDecorationLocation) loc[spv[i+1]] = spv[i+3];
                else if (spv[i+2] == SpvDecorationBinding) bind[spv[i+1]] = spv[i+3];
                else if (spv[i+2] == SpvDecorationBuiltIn) blt[spv[i+1]] = spv[i+3];
                break;                        
            case SpvOpExtInst: {
                uint32_t ext_op = spv[i+4];
                std::string fn = "unknown_ext_func";
                switch(ext_op) {
                    case 1: fn = "round"; break;
                    case 3: fn = "trunc"; break;
                    case 4: case 5: fn = "abs"; break;
                    case 6: case 7: fn = "sign"; break;
                    case 8: fn = "floor"; break;
                    case 9: fn = "ceil"; break;
                    case 10: fn = "fract"; break;
                    case 11: fn = "radians"; break;
                    case 12: fn = "degrees"; break;
                    case 13: fn = "sin"; break;
                    case 14: fn = "cos"; break;
                    case 15: fn = "tan"; break;
                    case 16: fn = "asin"; break;
                    case 17: fn = "acos"; break;
                    case 18: fn = "atan"; break;
                    case 26: fn = "pow"; break;
                    case 27: fn = "exp"; break;
                    case 28: fn = "log"; break;
                    case 29: fn = "exp2"; break;
                    case 30: fn = "log2"; break;
                    case 31: fn = "sqrt"; break;
                    case 32: fn = "inversesqrt"; break;
                    case 41: case 42: case 43: fn = "min"; break;
                    case 44: case 45: case 46: fn = "max"; break;
                    case 47: case 48: case 49: fn = "clamp"; break;
                    case 71: case 72: fn = "mix"; break;
                    case 73: fn = "step"; break;
                    case 74: fn = "smoothstep"; break;
                }                                
                std::string args = "";
                for(size_t j = 5; j < len; ++j) {
                    args += (j == 5 ? "" : ", ") + (expr.count(spv[i+j]) ? expr[spv[i+j]] : "v_" + TSTR(spv[i+j]));
                }                            
                cur_blk += tm[spv[i+1]] + " v_" + TSTR(spv[i+2]) + " = " + fn + "(" + args + ");\n";
                break;
                }
            case SpvOpEmitVertex: cur_blk += "        EmitVertex();\n"; break;
            case SpvOpEndPrimitive: cur_blk += "        EndPrimitive();\n"; break;
            case SpvOpEntryPoint: exec_model = spv[i+1]; break;
            case SpvOpExecutionMode:
                if (spv[i+2] == SpvExecutionModeLocalSize) 
                    head += "layout(local_size_x="+TSTR(spv[i+3])+", local_size_y="+TSTR(spv[i+4])+", local_size_z="+TSTR(spv[i+5])+") in;\n";
                break;
            case SpvOpImageDrefGather:
            case SpvOpImageGather: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = textureGather(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+");\n"; break;
            case SpvOpImageRead: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = imageLoad(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+");\n"; break;
            case SpvOpImageSampleDrefExplicitLod:
            case SpvOpImageSampleDrefImplicitLod: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = texture(v_"+TSTR(spv[i+3])+", vec3(v_"+TSTR(spv[i+4])+"));\n"; break;
            case SpvOpImageSampleExplicitLod:
            case SpvOpImageSampleImplicitLod: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = texture(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+");\n"; break;
            case SpvOpImageSampleProjDrefExplicitLod:
            case SpvOpImageSampleProjDrefImplicitLod:
            case SpvOpImageSampleProjExplicitLod:
            case SpvOpImageSampleProjImplicitLod: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = textureProj(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+");\n"; break;
            case SpvOpImageWrite: cur_blk += "        imageStore(v_"+TSTR(spv[i+1])+", v_"+TSTR(spv[i+2])+", v_"+TSTR(spv[i+3])+");\n"; break;
            case SpvOpKill: cur_blk += "        discard;\n"; break;
            case SpvOpLabel:
                if (first_label == 0) first_label = spv[i+1];
                if (cur_label != 0) blocks[cur_label] = cur_blk;
                cur_label = spv[i+1];
                cur_blk = "    case " + TSTR(cur_label) + ":\n";
                break;
            case SpvOpLoad: cur_blk += tm[spv[i+1]] + " v_" + TSTR(spv[i+2]) + " = " + (expr.count(spv[i+3]) ? expr[spv[i+3]] : ("v_" + TSTR(spv[i+3]))) + ";\n"; break;
            case SpvOpLoopMerge: break;
            case SpvOpMemoryBarrier: cur_blk += "        memoryBarrierShared();\n"; break;
            case SpvOpPhi: {
                uint32_t id_kq = spv[i+2];
                tm[id_kq] = tm[spv[i+1]];
                vars += tm[id_kq] + " v_" + TSTR(id_kq) + ";\n";
                for (size_t j = 3; j < len; j += 2) {
                    std::string val = expr.count(spv[i+j]) ? expr[spv[i+j]] : ("v_" + TSTR(spv[i+j]));
                    phi_defs[spv[i+j+1]].push_back({id_kq, val});
                }
                break;
            }
            case SpvOpReturn:
            case SpvOpReturnValue: cur_blk += "        _state = 0; break;\n"; break;
            case SpvOpSelect: {
                std::string cond = expr.count(spv[i+3]) ? expr[spv[i+3]] : "v_"+TSTR(spv[i+3]);
                std::string t_val = expr.count(spv[i+4]) ? expr[spv[i+4]] : "v_"+TSTR(spv[i+4]);
                std::string f_val = expr.count(spv[i+5]) ? expr[spv[i+5]] : "v_"+TSTR(spv[i+5]);
                expr[spv[i+2]] = "(" + cond + " ? " + t_val + " : " + f_val + ")";
                break;
            }
            case SpvOpSelectionMerge: break;
            case SpvOpStore: cur_blk += (expr.count(spv[i+1])?expr[spv[i+1]]:("v_"+TSTR(spv[i+1])))+" = v_"+TSTR(spv[i+2])+";\n"; break;
            case SpvOpSwitch: {
                APPLY_PHI
                cur_blk += "        switch(v_"+TSTR(spv[i+1])+") {\n";
                for(size_t j=3; j<len; j+=2) {
                    cur_blk += "            case "+TSTR(spv[i+j])+": _state = "+TSTR(spv[i+j+1])+"; break;\n";
                }
                cur_blk += "            default: _state = "+TSTR(spv[i+2])+"; break;\n        }\n        break;\n";
                break;
            }
            case SpvOpTypeArray: tm[spv[i+1]] = tm[spv[i+2]] + "[" + TSTR(spv[i+3]) + "]"; break;
            case SpvOpTypeBool: tm[spv[i+1]] = "bool"; break;
            case SpvOpTypeFloat: tm[spv[i+1]] = "float"; break;
            case SpvOpTypeImage: 
            case SpvOpTypeSampledImage: tm[spv[i+1]] = "sampler2D"; break;
            case SpvOpTypeInt: tm[spv[i+1]] = (spv[i+3] == 1) ? "int" : "uint"; break;
            case SpvOpTypeMatrix: tm[spv[i+1]] = "mat"+TSTR(spv[i+3]); break;
            case SpvOpTypePointer: tm[spv[i+1]] = tm[spv[i+3]]; break;
            case SpvOpTypeStruct: tm[spv[i+1]] = "struct_" + TSTR(spv[i+1]); break;
            case SpvOpTypeVector: tm[spv[i+1]] = (tm[spv[i+2]]=="float"?"vec":(tm[spv[i+2]]=="int"?"ivec":"uvec"))+TSTR(spv[i+3]); break;
            case SpvOpTypeVoid: tm[spv[i+1]] = "void"; break;
            case SpvOpUnreachable: cur_blk += "        _state = 0; break;\n"; break;
            case SpvOpVariable: {
                uint32_t id = spv[i+2], st = spv[i+3];
                if (blt.count(id)) {
                    if (blt[id] == SpvBuiltInPosition) expr[id] = "gl_Position";
                    else if (blt[id] == SpvBuiltInFragCoord) expr[id] = "gl_FragCoord";
                    else if (blt[id] == SpvBuiltInGlobalInvocationId) expr[id] = "gl_GlobalInvocationID";
                    break; 
                }
                expr[id] = "v_" + TSTR(id);
                if (st == SpvStorageClassInput) head += "layout(location="+TSTR(loc[id])+") in "+tm[spv[i+1]]+" v_"+TSTR(id)+";\n";
                else if (st == SpvStorageClassOutput) head += "layout(location="+TSTR(loc[id])+") out "+tm[spv[i+1]]+" v_"+TSTR(id)+";\n";
                else if (st == SpvStorageClassUniform || st == SpvStorageClassUniformConstant) 
                    head += "layout(binding="+TSTR(bind[id])+") uniform "+tm[spv[i+1]]+" v_"+TSTR(id)+";\n";
                else vars += tm[spv[i+1]]+" v_"+TSTR(id)+";\n";
                break;
            }
            case SpvOpVectorShuffle: expr[spv[i+2]] = tm[spv[i+1]] + "(v_" + TSTR(spv[i+3]) + ", v_" + TSTR(spv[i+4]) + ")"; break;

            default: break;
        }
        i += len;
    }
    if (cur_label != 0) blocks[cur_label] = cur_blk;
    std::string main_func = "void main() {\n    int _state = " + TSTR(first_label) + ";\n" + vars + "    while(_state != 0) {\n        switch(_state) {\n";
    for (auto& b : blocks) main_func += b.second;
    main_func += "        }\n    }\n}\n";
    return head + main_func;
} 
