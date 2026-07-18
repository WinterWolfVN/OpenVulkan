#pragma once
#include <string>
#include <vector>
#include <cstring>
#include "spirv_math.h" 

#define APPLY_PHI \
    if (cur_label < phi_defs.size() && !phi_defs[cur_label].empty()) { \
        for (auto& p : phi_defs[cur_label]) cur_blk += "        " + GET_TM(p.first) + " _phi_" + TSTR(p.first) + " = " + p.second + ";\n        v_" + TSTR(p.first) + " = _phi_" + TSTR(p.first) + ";\n"; \
    }

inline std::string TranslateSpirvFull(const uint32_t* spv, size_t size) {
    std::vector<std::string> tm, expr;
    std::vector<uint32_t> loc, bind, blt, spec_id;
    std::vector<std::string> blocks;
    std::vector<std::vector<std::pair<uint32_t, std::string>>> phi_defs;   
    std::string head, vars, cur_blk;
    head.reserve(1024); vars.reserve(2048); cur_blk.reserve(4096); // Chống reallocate   
    uint32_t cur_label = 0, first_label = 0;
    size_t i = 5;

    while (i < size) {
        uint16_t op = spv[i] & 0xFFFF;
        uint16_t len = spv[i] >> 16;        
        if (len == 0 || i + len > size) break;         
        if (ProcessMathOpcodes(op, len, spv, i, cur_blk, tm, expr)) { i += len; continue; }
        switch (op) {
            case SpvOpAccessChain: case SpvOpInBoundsAccessChain: case SpvOpPtrAccessChain: {
                std::string res = GET_EXPR(spv[i+3]);
                for (size_t j=4; j<len; ++j) res += "[" + GET_EXPR(spv[i+j]) + "]";
                ENSURE(expr, spv[i+2]); expr[spv[i+2]] = res; break;
            }
            case SpvOpAtomicCompareExchange: if(len>=9) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = atomicCompSwap("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+7])+", "+GET_EXPR(spv[i+8])+");\n"; } break;
            case SpvOpBitFieldInsert: if(len>=7) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = bitfieldInsert("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+", "+GET_EXPR(spv[i+5])+", "+GET_EXPR(spv[i+6])+");\n"; } break;
            case SpvOpBitFieldSExtract: case SpvOpBitFieldUExtract: if(len>=6) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = bitfieldExtract("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+", "+GET_EXPR(spv[i+5])+");\n"; } break;
            case SpvOpBranch: if(len>=2) { APPLY_PHI; cur_blk += "        _state = " + TSTR(spv[i+1]) + "; break;\n"; } break;                
            case SpvOpBranchConditional: if(len>=4) { APPLY_PHI; cur_blk += "        if("+GET_EXPR(spv[i+1])+") _state = "+TSTR(spv[i+2])+"; else _state = "+TSTR(spv[i+3])+"; break;\n"; } break;
            case SpvOpCompositeConstruct: {
                std::string args = ""; for(size_t j=3; j<len; ++j) args += (j==3?"":", ") + GET_EXPR(spv[i+j]);
                ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = " + GET_TM(spv[i+1]) + "(" + args + ");\n"; break;
            }
            case SpvOpCompositeExtract: {
                std::string res = GET_EXPR(spv[i+3]); for (size_t j=4; j<len; ++j) res += "[" + TSTR(spv[i+j]) + "]";
                ENSURE(expr, spv[i+2]); expr[spv[i+2]] = res; break;
            }
            case SpvOpConstant: {
                ENSURE(expr, spv[i+2]);
                if (GET_TM(spv[i+1]) == "float") { float f; std::memcpy(&f, &spv[i+3], 4); expr[spv[i+2]] = TSTR(f); } 
                else expr[spv[i+2]] = TSTR(spv[i+3]); break;
            }
            case SpvOpConstantComposite: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "composite_v_" + TSTR(spv[i+2]); break; 
            case SpvOpConstantFalse: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "false"; break;
            case SpvOpConstantNull: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = GET_TM(spv[i+1]) + "(0)"; break;
            case SpvOpConstantTrue: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "true"; break;
            case SpvOpControlBarrier: cur_blk += "        barrier();\n"; break;
            case SpvOpCopyMemory: case SpvOpCopyObject: if(len>=4) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+GET_EXPR(spv[i+3])+";\n"; } break;
            case SpvOpDecorate:
                if (len >= 4) {
                    if (spv[i+2] == SpvDecorationLocation) { ENSURE(loc, spv[i+1]); loc[spv[i+1]] = spv[i+3]; }
                    else if (spv[i+2] == SpvDecorationBinding) { ENSURE(bind, spv[i+1]); bind[spv[i+1]] = spv[i+3]; }
                    else if (spv[i+2] == SpvDecorationBuiltIn) { ENSURE(blt, spv[i+1]); blt[spv[i+1]] = spv[i+3]; }
                    else if (spv[i+2] == SpvDecorationSpecId) { ENSURE(spec_id, spv[i+1]); spec_id[spv[i+1]] = spv[i+3]; }
                } break;
            case SpvOpEmitVertex: cur_blk += "        EmitVertex();\n"; break;
            case SpvOpEndPrimitive: cur_blk += "        EndPrimitive();\n"; break;
            case SpvOpEntryPoint: break;
            case SpvOpExecutionMode:
                if (len >= 6 && spv[i+2] == SpvExecutionModeLocalSize) 
                    head += "layout(local_size_x="+TSTR(spv[i+3])+", local_size_y="+TSTR(spv[i+4])+", local_size_z="+TSTR(spv[i+5])+") in;\n"; break;
            case SpvOpExtInst: {
                if (len < 5) break; uint32_t e = spv[i+4]; const char* fn = "unknown_ext_func";
                if(e==1) fn="round"; else if(e==3) fn="trunc"; else if(e==4||e==5) fn="abs"; else if(e==6||e==7) fn="sign";
                else if(e==8) fn="floor"; else if(e==9) fn="ceil"; else if(e==10) fn="fract"; else if(e==11) fn="radians";
                else if(e==12) fn="degrees"; else if(e==13) fn="sin"; else if(e==14) fn="cos"; else if(e==15) fn="tan";
                else if(e==16) fn="asin"; else if(e==17) fn="acos"; else if(e==18) fn="atan"; else if(e==26) fn="pow";
                else if(e==27) fn="exp"; else if(e==28) fn="log"; else if(e==29) fn="exp2"; else if(e==30) fn="log2";
                else if(e==31) fn="sqrt"; else if(e==32) fn="inversesqrt"; else if(e>=41&&e<=43) fn="min";
                else if(e>=44&&e<=46) fn="max"; else if(e>=47&&e<=49) fn="clamp"; else if(e==71||e==72) fn="mix";
                else if(e==73) fn="step"; else if(e==74) fn="smoothstep";
                std::string args = ""; for(size_t j=5; j<len; ++j) { if(j>5) args+=", "; args+=GET_EXPR(spv[i+j]); }
                ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = " + fn + "(" + args + ");\n"; break;
            }
            case SpvOpFunction: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "func_" + TSTR(spv[i+2]); break;
            case SpvOpFunctionCall: {
                std::string args = ""; for(size_t j=4; j<len; ++j) args += (j==4?"":", ") + GET_EXPR(spv[i+j]);
                ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = func_" + TSTR(spv[i+3]) + "(" + args + ");\n"; break;
            }
            case SpvOpFunctionEnd: break;
            case SpvOpFunctionParameter: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "v_" + TSTR(spv[i+2]); break;
            case SpvOpImageDrefGather: case SpvOpImageGather: if(len>=5) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = textureGather("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; } break;
            case SpvOpImageRead: if(len>=5) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = imageLoad("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; } break;
            case SpvOpImageSampleDrefExplicitLod: case SpvOpImageSampleDrefImplicitLod: if(len>=5) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = texture("+GET_EXPR(spv[i+3])+", vec3("+GET_EXPR(spv[i+4])+"));\n"; } break;
            case SpvOpImageSampleExplicitLod: case SpvOpImageSampleImplicitLod: if(len>=5) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = texture("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; } break;
            case SpvOpImageSampleProjDrefExplicitLod: case SpvOpImageSampleProjDrefImplicitLod: case SpvOpImageSampleProjExplicitLod: case SpvOpImageSampleProjImplicitLod: if(len>=5) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = textureProj("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; } break;
            case SpvOpImageWrite: if(len>=4) cur_blk += "        imageStore("+GET_EXPR(spv[i+1])+", "+GET_EXPR(spv[i+2])+", "+GET_EXPR(spv[i+3])+");\n"; break;
            case SpvOpKill: cur_blk += "        discard;\n"; break;
            case SpvOpLabel:
                if (len >= 2) {
                    if (first_label == 0) first_label = spv[i+1];
                    if (cur_label != 0 && cur_label < blocks.size()) blocks[cur_label] = cur_blk;
                    cur_label = spv[i+1]; ENSURE(blocks, cur_label); cur_blk = "    case " + TSTR(cur_label) + ":\n";
                } break;
            case SpvOpLoad: if(len>=4) { ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = " + GET_EXPR(spv[i+3]) + ";\n"; } break;
            case SpvOpLoopMerge: break;
            case SpvOpMemoryBarrier: cur_blk += "        memoryBarrierShared();\n"; break;
            case SpvOpPhi: {
                ENSURE(tm, spv[i+2]); ENSURE(phi_defs, spv[i+2]); tm[spv[i+2]] = GET_TM(spv[i+1]);
                vars += tm[spv[i+2]] + " v_" + TSTR(spv[i+2]) + ";\n";
                for (size_t j = 3; j < len; j += 2) if (j + 1 < len) { ENSURE(phi_defs, spv[i+j+1]); phi_defs[spv[i+j+1]].push_back({spv[i+2], GET_EXPR(spv[i+j])}); }
                break;
            }
            case SpvOpReturn: case SpvOpReturnValue: cur_blk += "        _state = 0; break;\n"; break;
            case SpvOpSelect: if(len>=6) { ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "(" + GET_EXPR(spv[i+3]) + " ? " + GET_EXPR(spv[i+4]) + " : " + GET_EXPR(spv[i+5]) + ")"; } break;
            case SpvOpSelectionMerge: break;
            case SpvOpSpecConstant: {
                ENSURE(expr, spv[i+2]); std::string val;
                if (GET_TM(spv[i+1]) == "float") { float f; std::memcpy(&f, &spv[i+3], 4); val = TSTR(f); } else val = TSTR(spv[i+3]);
                if (spv[i+2] < spec_id.size() && spec_id[spv[i+2]] != 0) head += "layout(constant_id=" + TSTR(spec_id[spv[i+2]]) + ") const " + GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = " + val + ";\n";
                else head += "const " + GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = " + val + ";\n";
                expr[spv[i+2]] = "v_" + TSTR(spv[i+2]); break;
            }
            case SpvOpSpecConstantTrue: head += "const bool v_"+TSTR(spv[i+2])+" = true;\n"; ENSURE(expr, spv[i+2]); expr[spv[i+2]]="v_"+TSTR(spv[i+2]); break;
            case SpvOpSpecConstantFalse: head += "const bool v_"+TSTR(spv[i+2])+" = false;\n"; ENSURE(expr, spv[i+2]); expr[spv[i+2]]="v_"+TSTR(spv[i+2]); break;
            case SpvOpSpecConstantComposite: case SpvOpSpecConstantOp: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "v_" + TSTR(spv[i+2]); break;
            case SpvOpStore: if(len>=3) cur_blk += GET_EXPR(spv[i+1])+" = "+GET_EXPR(spv[i+2])+";\n"; break;
            case SpvOpSwitch: {
                APPLY_PHI; cur_blk += "        switch("+GET_EXPR(spv[i+1])+") {\n";
                for(size_t j=3; j<len; j+=2) if(j+1<len) cur_blk += "            case "+TSTR(spv[i+j])+": _state = "+TSTR(spv[i+j+1])+"; break;\n";
                cur_blk += "            default: _state = "+TSTR(spv[i+2])+"; break;\n        }\n        break;\n"; break;
            }
            case SpvOpTypeArray: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = GET_TM(spv[i+2]) + "[" + TSTR(spv[i+3]) + "]"; break;
            case SpvOpTypeBool: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "bool"; break;
            case SpvOpTypeEvent: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "event"; break;      
            case SpvOpTypeFloat: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "float"; break;
            case SpvOpTypeImage: case SpvOpTypeSampledImage: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "sampler2D"; break;
            case SpvOpTypeInt: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = (spv[i+3] == 1) ? "int" : "uint"; break;
            case SpvOpTypeMatrix: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "mat"+TSTR(spv[i+3]); break;
            case SpvOpTypePipe: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "pipe"; break;        
            case SpvOpTypePointer: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = GET_TM(spv[i+3]); break;
            case SpvOpTypeQueue: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "queue"; break;      
            case SpvOpTypeStruct: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "struct_" + TSTR(spv[i+1]); break;
            case SpvOpTypeVector: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = (GET_TM(spv[i+2])=="float"?"vec":(GET_TM(spv[i+2])=="int"?"ivec":"uvec"))+TSTR(spv[i+3]); break;
            case SpvOpTypeVoid: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "void"; break;
            case SpvOpUnreachable: cur_blk += "        _state = 0; break;\n"; break;
            case SpvOpVariable: {
                uint32_t id = spv[i+2], st = spv[i+3]; ENSURE(expr, id);
                if (id < blt.size() && blt[id] != 0) {
                    if (blt[id] == SpvBuiltInPosition) expr[id] = "gl_Position";
                    else if (blt[id] == SpvBuiltInFragCoord) expr[id] = "gl_FragCoord";
                    else if (blt[id] == SpvBuiltInGlobalInvocationId) expr[id] = "gl_GlobalInvocationID";
                    break; 
                }
                expr[id] = "v_" + TSTR(id);
                if (st == SpvStorageClassInput) head += "layout(location="+TSTR(id < loc.size() ? loc[id] : 0)+") in "+GET_TM(spv[i+1])+" v_"+TSTR(id)+";\n";
                else if (st == SpvStorageClassOutput) head += "layout(location="+TSTR(id < loc.size() ? loc[id] : 0)+") out "+GET_TM(spv[i+1])+" v_"+TSTR(id)+";\n";
                else if (st == SpvStorageClassUniform || st == SpvStorageClassUniformConstant) 
                    head += "layout(binding="+TSTR(id < bind.size() ? bind[id] : 0)+") uniform "+GET_TM(spv[i+1])+" v_"+TSTR(id)+";\n";
                else vars += GET_TM(spv[i+1])+" v_"+TSTR(id)+";\n"; break;
            }
            case SpvOpVectorShuffle: if(len>=5) { ENSURE(expr, spv[i+2]); expr[spv[i+2]] = GET_TM(spv[i+1]) + "("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+")"; } break;
            default: break;
        }
        i += len;
    }
    if (cur_label != 0 && cur_label < blocks.size()) blocks[cur_label] = cur_blk;
    std::string main_func = "void main() {\n    int _state = " + TSTR(first_label) + ";\n" + vars + "    while(_state != 0) {\n        switch(_state) {\n";
    for (size_t b = 0; b < blocks.size(); ++b) if(!blocks[b].empty()) main_func += blocks[b];
    main_func += "        }\n    }\n}\n";
    return head + main_func;
}