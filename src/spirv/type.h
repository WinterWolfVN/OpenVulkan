#pragma once
#include <string>
#include <vector>
#include <cstring>

inline bool TranslateTypeFunction(uint16_t op, uint16_t len, const uint32_t* spv, size_t i, std::vector<std::string>& tm, std::vector<std::string>& expr, std::string& cur_blk) {
        switch (op) {
            case SpvOpFunction: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "func_" + TSTR(spv[i+2]); 
                break;
            case SpvOpFunctionCall: {
                std::string args = ""; for(size_t j=4; j<len; ++j) args += (j==4?"":", ") + GET_EXPR(spv[i+j]);
                ENSURE(tm, spv[i+2]); cur_blk += GET_TM(spv[i+1]) + " v_" + TSTR(spv[i+2]) + " = func_" + TSTR(spv[i+3]) + "(" + args + ");\n"; 
                break;
                }
            case SpvOpFunctionEnd: 
                break;
            case SpvOpFunctionParameter: ENSURE(expr, spv[i+2]); expr[spv[i+2]] = "v_" + TSTR(spv[i+2]); 
                break;
            case SpvOpTypeArray: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = GET_TM(spv[i+2]) + "[" + TSTR(spv[i+3]) + "]"; 
                break;
            case SpvOpTypeBool: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "bool"; 
                break;
            case SpvOpTypeEvent: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "event"; 
                break;      
            case SpvOpTypeFloat: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "float"; 
                break;
            case SpvOpTypeImage: 
            case SpvOpTypeSampledImage: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "sampler2D"; 
                break;
            case SpvOpTypeInt: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = (spv[i+3] == 1) ? "int" : "uint"; 
                break;
            case SpvOpTypeMatrix: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "mat"+TSTR(spv[i+3]); 
                break;
            case SpvOpTypePipe: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "pipe"; 
                break;        
            case SpvOpTypePointer: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = GET_TM(spv[i+3]); 
                break;
            case SpvOpTypeQueue: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "queue"; 
                break;      
            case SpvOpTypeStruct: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "struct_" + TSTR(spv[i+1]); 
                break;
            case SpvOpTypeVector: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = (GET_TM(spv[i+2])=="float"?"vec":(GET_TM(spv[i+2])=="int"?"ivec":"uvec"))+TSTR(spv[i+3]); 
                break;
            case SpvOpTypeVoid: ENSURE(tm, spv[i+1]); tm[spv[i+1]] = "void"; 
                break;

        }
}
