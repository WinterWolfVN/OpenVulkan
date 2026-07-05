#pragma once
#include <string>
#include <unordered_map>
#include <string.h>
#include <vector>
#include "spirv.h"

#define TSTR std::to_string
#define OP1(op, sym) case op: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = "+sym+"v_"+TSTR(spv[i+3])+";\n"; break;
#define OP2(op, sym) case op: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = v_"+TSTR(spv[i+3])+" "+sym+" v_"+TSTR(spv[i+4])+";\n"; break;
#define OPF(op, fn)  case op: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = "+fn+"(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+");\n"; break;

std::string TranslateSpirvFull(const uint32_t* spv, size_t size) {
    std::unordered_map<uint32_t, std::string> tm, expr;
    std::unordered_map<uint32_t, uint32_t> loc, bind, blt;
    std::unordered_map<uint32_t, std::string> blocks;
    
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
            case SpvOpExecutionMode:
                if (spv[i+2] == SpvExecutionModeLocalSize) 
                    head += "layout(local_size_x="+TSTR(spv[i+3])+", local_size_y="+TSTR(spv[i+4])+", local_size_z="+TSTR(spv[i+5])+") in;\n";
                break;
            case SpvOpEntryPoint: exec_model = spv[i+1]; break;
            case SpvOpDecorate:
                if (spv[i+2] == SpvDecorationLocation) loc[spv[i+1]] = spv[i+3];
                else if (spv[i+2] == SpvDecorationBinding) bind[spv[i+1]] = spv[i+3];
                else if (spv[i+2] == SpvDecorationBuiltIn) blt[spv[i+1]] = spv[i+3];
                break;

            case SpvOpTypeVoid: tm[spv[i+1]] = "void"; break;
            case SpvOpTypeBool: tm[spv[i+1]] = "bool"; break;
            case SpvOpTypeFloat: tm[spv[i+1]] = "float"; break;
            case SpvOpTypeInt: tm[spv[i+1]] = (spv[i+3] == 1) ? "int" : "uint"; break;
            case SpvOpTypeVector: tm[spv[i+1]] = (tm[spv[i+2]]=="float"?"vec":(tm[spv[i+2]]=="int"?"ivec":"uvec"))+TSTR(spv[i+3]); break;
            case SpvOpTypeMatrix: tm[spv[i+1]] = "mat"+TSTR(spv[i+3]); break;
            case SpvOpTypePointer: tm[spv[i+1]] = tm[spv[i+3]]; break;
            case SpvOpTypeImage: case SpvOpTypeSampledImage: tm[spv[i+1]] = "sampler2D"; break;

            case SpvOpConstantTrue: expr[spv[i+2]] = "true"; break;
            case SpvOpConstantFalse: expr[spv[i+2]] = "false"; break;
            case SpvOpConstant: {
                if (tm[spv[i+1]] == "float") { float f; memcpy(&f, &spv[i+3], 4); expr[spv[i+2]] = TSTR(f); } 
                else expr[spv[i+2]] = TSTR(spv[i+3]);
                break;
            }

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

            case SpvOpLabel:
                if (first_label == 0) first_label = spv[i+1];
                if (cur_label != 0) blocks[cur_label] = cur_blk;
                cur_label = spv[i+1];
                cur_blk = "    case " + TSTR(cur_label) + ":\n";
                break;

            case SpvOpLoad: cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = "+expr.count(spv[i+3])?expr[spv[i+3]]:("v_"+TSTR(spv[i+3]))+";\n"; break;
            case SpvOpStore: cur_blk += (expr.count(spv[i+1])?expr[spv[i+1]]:("v_"+TSTR(spv[i+1])))+" = v_"+TSTR(spv[i+2])+";\n"; break;
            
            case SpvOpImageSampleImplicitLod: case SpvOpImageSampleExplicitLod:
                cur_blk += tm[spv[i+1]]+" v_"+TSTR(spv[i+2])+" = texture(v_"+TSTR(spv[i+3])+", v_"+TSTR(spv[i+4])+");\n"; break;

            case SpvOpBranch:
                cur_blk += "        _state = " + TSTR(spv[i+1]) + "; break;\n"; break;
            case SpvOpBranchConditional:
                cur_blk += "        if(v_"+TSTR(spv[i+1])+") _state = "+TSTR(spv[i+2])+"; else _state = "+TSTR(spv[i+3])+"; break;\n"; break;
            case SpvOpReturn: case SpvOpReturnValue:
                cur_blk += "        _state = 0; break;\n"; break;

            OP1(SpvOpSNegate, "-") OP1(SpvOpFNegate, "-") OP1(SpvOpLogicalNot, "!")
            OP2(SpvOpIAdd, "+") OP2(SpvOpFAdd, "+") OP2(SpvOpISub, "-") OP2(SpvOpFSub, "-")
            OP2(SpvOpIMul, "*") OP2(SpvOpFMul, "*") OP2(SpvOpUDiv, "/") OP2(SpvOpFDiv, "/")
            OP2(SpvOpIEqual, "==") OP2(SpvOpINotEqual, "!=") OP2(SpvOpSLessThan, "<") OP2(SpvOpSGreaterThan, ">")
            OP2(SpvOpLogicalOr, "||") OP2(SpvOpLogicalAnd, "&&") OP2(SpvOpBitwiseOr, "|") OP2(SpvOpBitwiseAnd, "&")
            OPF(SpvOpDot, "dot") OPF(SpvOpExtInst, "mix") 

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
