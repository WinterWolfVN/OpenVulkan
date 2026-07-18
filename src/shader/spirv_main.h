#pragma once
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <utility>
#include "spirv_math.h" 

#define APPLY_PHI \
do { \
    if (phi_defs.count(cur_label)) { \
        for (auto& p : phi_defs[cur_label]) cur_blk += "        " + GET_TM(p.first) + " _phi_" + TSTR(p.first) + " = " + p.second + ";\n"; \
        for (auto& p : phi_defs[cur_label]) cur_blk += "        v_" + TSTR(p.first) + " = _phi_" + TSTR(p.first) + ";\n"; \
    } \
} while(0)

inline std::string TranslateSpirvFull(const uint32_t* spv, size_t size) {
    std::unordered_map<uint32_t, std::string> tm, expr;
    std::unordered_map<uint32_t, uint32_t> loc, bind, blt, spec_id;
    std::unordered_map<uint32_t, std::string> blocks;
    std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, std::string>>> phi_defs;    
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
        if (len == 0 || i + len > size) break; 
        if (ProcessMathOpcodes(op, len, spv, i, cur_blk, tm, expr)) {
            i += len;
            continue;
        }
        switch (op) {
            case SpvOpAccessChain:
            case SpvOpInBoundsAccessChain:     
            case SpvOpPtrAccessChain: {
                if (len < 4) break;
                std::string res = GET_EXPR(spv[i+3]);
                for (size_t j=4; j<len; ++j) res += "[" + GET_EXPR(spv[i+j]) + "]";
                expr[spv[i+2]] = res;
                break;
            }
            case SpvOpAtomicCompareExchange: if (len >= 9) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = atomicCompSwap("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+7])+", "+GET_EXPR(spv[i+8])+");\n"; break;
            case SpvOpBitFieldInsert: if (len >= 7) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = bitfieldInsert("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+", "+GET_EXPR(spv[i+5])+", "+GET_EXPR(spv[i+6])+");\n"; break;
            case SpvOpBitFieldSExtract:
            case SpvOpBitFieldUExtract: if (len >= 6) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = bitfieldExtract("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+", "+GET_EXPR(spv[i+5])+");\n"; break;
            case SpvOpBranch: if (len >= 2) { APPLY_PHI; cur_blk += "        _state = " + TSTR(spv[i+1]) + "; break;\n"; } break;                
            case SpvOpBranchConditional: if (len >= 4) { APPLY_PHI; cur_blk += "        if("+GET_EXPR(spv[i+1])+") _state = "+TSTR(spv[i+2])+"; else _state = "+TSTR(spv[i+3])+"; break;\n"; } break;
            case SpvOpCompositeConstruct: {
                if (len < 3) break;
                std::string args = "";
                for(size_t j=3; j<len; ++j) args += (j==3?"":", ") + GET_EXPR(spv[i+j]);
                cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = " + GET_TM(spv[i+1]) + "(" + args + ");\n";
                break;
            }
            case SpvOpCompositeExtract: {
                if (len < 4) break;
                std::string res = GET_EXPR(spv[i+3]);
                for (size_t j=4; j<len; ++j) res += "[" + TSTR(spv[i+j]) + "]";
                expr[spv[i+2]] = res; 
                break;
            }
            case SpvOpConstant: {
                if (len >= 4) {
                    if (GET_TM(spv[i+1]) == "float") { float f; std::memcpy(&f, &spv[i+3], 4); expr[spv[i+2]] = TSTR(f); } 
                    else expr[spv[i+2]] = TSTR(spv[i+3]);
                }
                break;
            }
            case SpvOpConstantComposite:       
                if (len >= 3) expr[spv[i+2]] = "composite_v_" + TSTR(spv[i+2]); break; 
            case SpvOpConstantFalse: if (len >= 3) expr[spv[i+2]] = "false"; break;
            case SpvOpConstantNull:            
                if (len >= 3) expr[spv[i+2]] = GET_TM(spv[i+1]) + "(0)"; break;
            case SpvOpConstantTrue: if (len >= 3) expr[spv[i+2]] = "true"; break;
            case SpvOpControlBarrier: cur_blk += "        barrier();\n"; break;
            case SpvOpCopyMemory:              
            case SpvOpCopyObject: 
                if (len >= 4) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+GET_EXPR(spv[i+3])+";\n"; break;
            case SpvOpDecorate:
                if (len >= 4) {
                    if (spv[i+2] == SpvDecorationLocation) loc[spv[i+1]] = spv[i+3];
                    else if (spv[i+2] == SpvDecorationBinding) bind[spv[i+1]] = spv[i+3];
                    else if (spv[i+2] == SpvDecorationBuiltIn) blt[spv[i+1]] = spv[i+3];
                    else if (spv[i+2] == SpvDecorationSpecId) spec_id[spv[i+1]] = spv[i+3];
                }
                break;
            case SpvOpEmitVertex: cur_blk += "        EmitVertex();\n"; break;
            case SpvOpEndPrimitive: cur_blk += "        EndPrimitive();\n"; break;
            case SpvOpEntryPoint: if (len >= 2) exec_model = spv[i+1]; break;
            case SpvOpExecutionMode:
                if (len >= 6 && spv[i+2] == SpvExecutionModeLocalSize) 
                    head += "layout(local_size_x="+TSTR(spv[i+3])+", local_size_y="+TSTR(spv[i+4])+", local_size_z="+TSTR(spv[i+5])+") in;\n";
                break;
            case SpvOpExtInst: {
                if (len < 5) break;
                uint32_t ext_op = spv[i+4];
                std::string fn = "unknown_ext_func";
                switch(ext_op) {
                    case 1: fn = "round"; break; case 3: fn = "trunc"; break;
                    case 4: case 5: fn = "abs"; break; case 6: case 7: fn = "sign"; break;
                    case 8: fn = "floor"; break; case 9: fn = "ceil"; break; case 10: fn = "fract"; break;
                    case 11: fn = "radians"; break; case 12: fn = "degrees"; break;
                    case 13: fn = "sin"; break; case 14: fn = "cos"; break; case 15: fn = "tan"; break;
                    case 16: fn = "asin"; break; case 17: fn = "acos"; break; case 18: fn = "atan"; break;
                    case 26: fn = "pow"; break; case 27: fn = "exp"; break; case 28: fn = "log"; break;
                    case 29: fn = "exp2"; break; case 30: fn = "log2"; break; case 31: fn = "sqrt"; break;
                    case 32: fn = "inversesqrt"; break; case 41: case 42: case 43: fn = "min"; break;
                    case 44: case 45: case 46: fn = "max"; break; case 47: case 48: case 49: fn = "clamp"; break;
                    case 71: case 72: fn = "mix"; break; case 73: fn = "step"; break; case 74: fn = "smoothstep"; break;
                }
                std::string args = "";
                for(size_t j=5; j<len; ++j) { args += (j==5 ? "" : ", ") + GET_EXPR(spv[i+j]); }
                cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = " + fn + "(" + args + ");\n";
                break;
            }
            case SpvOpFunction: if (len >= 3) expr[spv[i+2]] = "func_" + TSTR(spv[i+2]); break;
            case SpvOpFunctionCall: {
                if (len < 4) break;
                std::string args = "";
                for(size_t j=4; j<len; ++j) args += (j==4?"":", ") + GET_EXPR(spv[i+j]);
                cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = func_" + TSTR(spv[i+3]) + "(" + args + ");\n";
                break;
            }
            case SpvOpFunctionEnd: break;
            case SpvOpFunctionParameter: if (len >= 3) expr[spv[i+2]] = "v_" + TSTR(spv[i+2]); break;
            case SpvOpImageDrefGather:
            case SpvOpImageGather: if (len >= 5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = textureGather("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; break;
            case SpvOpImageRead: if (len >= 5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = imageLoad("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; break;
            case SpvOpImageSampleDrefExplicitLod:
            case SpvOpImageSampleDrefImplicitLod: if (len >= 5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = texture("+GET_EXPR(spv[i+3])+", vec3("+GET_EXPR(spv[i+4])+"));\n"; break;
            case SpvOpImageSampleExplicitLod:
            case SpvOpImageSampleImplicitLod: if (len >= 5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = texture("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; break;
            case SpvOpImageSampleProjDrefExplicitLod:
            case SpvOpImageSampleProjDrefImplicitLod:
            case SpvOpImageSampleProjExplicitLod:
            case SpvOpImageSampleProjImplicitLod: if (len >= 5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = textureProj("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; break;
            case SpvOpImageWrite: if (len >= 4) cur_blk += "        imageStore("+GET_EXPR(spv[i+1])+", "+GET_EXPR(spv[i+2])+", "+GET_EXPR(spv[i+3])+");\n"; break;
            case SpvOpKill: cur_blk += "        discard;\n"; break;
            case SpvOpLabel:
                if (len >= 2) {
                    if (first_label == 0) first_label = spv[i+1];
                    if (cur_label != 0) blocks[cur_label] = cur_blk;
                    cur_label = spv[i+1];
                    cur_blk = "    case " + TSTR(cur_label) + ":\n";
                }
                break;
            case SpvOpLoad: if (len >= 4) cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = " + GET_EXPR(spv[i+3]) + ";\n"; break;
            case SpvOpLoopMerge: break;
            case SpvOpMemoryBarrier: cur_blk += "        memoryBarrierShared();\n"; break;
            case SpvOpPhi: {
                if (len < 3) break;
                uint32_t id_kq = spv[i+2];
                tm[id_kq] = GET_TM(spv[i+1]);
                vars += tm[id_kq] + " v_" + TSTR(id_kq) + ";\n";
                for (size_t j = 3; j < len; j += 2) {
                    if (j + 1 < len) phi_defs[spv[i+j+1]].push_back({id_kq, GET_EXPR(spv[i+j])});
                }
                break;
            }
            case SpvOpReturn:
            case SpvOpReturnValue: cur_blk += "        _state = 0; break;\n"; break;
            case SpvOpSelect: { 
                if (len >= 6) expr[spv[i+2]] = "(" + GET_EXPR(spv[i+3]) + " ? " + GET_EXPR(spv[i+4]) + " : " + GET_EXPR(spv[i+5]) + ")"; 
                break; 
            }
            case SpvOpSelectionMerge: break;
            case SpvOpSpecConstant: {
                if (len >= 4) {
                    uint32_t id = spv[i+2];
                    std::string val;
                    if (GET_TM(spv[i+1]) == "float") { float f; std::memcpy(&f, &spv[i+3], 4); val = TSTR(f); } else val = TSTR(spv[i+3]);
                    if (spec_id.count(id)) head += "layout(constant_id=" + TSTR(spec_id[id]) + ") const " + GET_TM(spv[i+1]) + " v_" + TSTR(id) + " = " + val + ";\n";
                    else head += "const " + GET_TM(spv[i+1]) + " v_" + TSTR(id) + " = " + val + ";\n";
                    expr[id] = "v_" + TSTR(id);
                }
                break;
            }
            case SpvOpSpecConstantTrue: if (len >= 3) { head += "const bool v_"+TSTR(spv[i+2])+" = true;\n"; expr[spv[i+2]]="v_"+TSTR(spv[i+2]); } break;
            case SpvOpSpecConstantFalse: if (len >= 3) { head += "const bool v_"+TSTR(spv[i+2])+" = false;\n"; expr[spv[i+2]]="v_"+TSTR(spv[i+2]); } break;
            case SpvOpSpecConstantComposite:
            case SpvOpSpecConstantOp: if (len >= 3) expr[spv[i+2]] = "v_" + TSTR(spv[i+2]); break;
            case SpvOpStore: if (len >= 3) cur_blk += GET_EXPR(spv[i+1])+" = "+GET_EXPR(spv[i+2])+";\n"; break;
            case SpvOpSwitch: {
                if (len >= 3) {
                    APPLY_PHI;
                    cur_blk += "        switch("+GET_EXPR(spv[i+1])+") {\n";
                    for(size_t j=3; j<len; j+=2) { if (j+1 < len) cur_blk += "            case "+TSTR(spv[i+j])+": _state = "+TSTR(spv[i+j+1])+"; break;\n"; }
                    cur_blk += "            default: _state = "+TSTR(spv[i+2])+"; break;\n        }\n        break;\n";
                }
                break;
            }
            case SpvOpTypeArray: if (len >= 4) tm[spv[i+1]] = GET_TM(spv[i+2]) + "[" + TSTR(spv[i+3]) + "]"; break;
            case SpvOpTypeBool: if (len >= 2) tm[spv[i+1]] = "bool"; break;
            case SpvOpTypeEvent: if (len >= 2) tm[spv[i+1]] = "event"; break;      
            case SpvOpTypeFloat: if (len >= 2) tm[spv[i+1]] = "float"; break;
            case SpvOpTypeImage: 
            case SpvOpTypeSampledImage: if (len >= 2) tm[spv[i+1]] = "sampler2D"; break;
            case SpvOpTypeInt: if (len >= 4) tm[spv[i+1]] = (spv[i+3] == 1) ? "int" : "uint"; break;
            case SpvOpTypeMatrix: if (len >= 4) tm[spv[i+1]] = "mat"+TSTR(spv[i+3]); break;
            case SpvOpTypePipe: if (len >= 2) tm[spv[i+1]] = "pipe"; break;        
            case SpvOpTypePointer: if (len >= 4) tm[spv[i+1]] = GET_TM(spv[i+3]); break;
            case SpvOpTypeQueue: if (len >= 2) tm[spv[i+1]] = "queue"; break;      
            case SpvOpTypeStruct: if (len >= 2) tm[spv[i+1]] = "struct_" + TSTR(spv[i+1]); break;
            case SpvOpTypeVector: if (len >= 4) tm[spv[i+1]] = (GET_TM(spv[i+2])=="float"?"vec":(GET_TM(spv[i+2])=="int"?"ivec":"uvec"))+TSTR(spv[i+3]); break;
            case SpvOpTypeVoid: if (len >= 2) tm[spv[i+1]] = "void"; break;
            case SpvOpUnreachable: cur_blk += "        _state = 0; break;\n"; break;
            case SpvOpVariable: {
                if (len >= 4) {
                    uint32_t id = spv[i+2], st = spv[i+3];
                    if (blt.count(id)) {
                        if (blt[id] == SpvBuiltInPosition) expr[id] = "gl_Position";
                        else if (blt[id] == SpvBuiltInFragCoord) expr[id] = "gl_FragCoord";
                        else if (blt[id] == SpvBuiltInGlobalInvocationId) expr[id] = "gl_GlobalInvocationID";
                        break; 
                    }
                    expr[id] = "v_" + TSTR(id);
                    if (st == SpvStorageClassInput) head += "layout(location="+TSTR(loc[id])+") in "+GET_TM(spv[i+1])+" v_"+TSTR(id)+";\n";
                    else if (st == SpvStorageClassOutput) head += "layout(location="+TSTR(loc[id])+") out "+GET_TM(spv[i+1])+" v_"+TSTR(id)+";\n";
                    else if (st == SpvStorageClassUniform || st == SpvStorageClassUniformConstant) 
                        head += "layout(binding="+TSTR(bind[id])+") uniform "+GET_TM(spv[i+1])+" v_"+TSTR(id)+";\n";
                    else vars += GET_TM(spv[i+1])+" v_"+TSTR(id)+";\n";
                }
                break;
            }
            case SpvOpVectorShuffle: if (len >= 5) expr[spv[i+2]] = GET_TM(spv[i+1]) + "("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+")"; break;

            default: 
                break;
        }
        i += len;
    }
    if (cur_label != 0) blocks[cur_label] = cur_blk;
    std::string main_func = "void main() {\n    int _state = " + TSTR(first_label) + ";\n" + vars + "    while(_state != 0) {\n        switch(_state) {\n";
    for (auto& b : blocks) main_func += b.second;
    main_func += "        }\n    }\n}\n";
    return head + main_func;
}
