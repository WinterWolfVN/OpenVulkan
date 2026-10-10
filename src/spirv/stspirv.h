#pragma once
#include <string>
#include <vector>
#include <cstdint>

#define TSTR std::to_string
#define ENSURE(v, id) if ((id) >= (v).size()) (v).resize((id) + 1)
#define GET_TM(id) (tm.size() > (id) && !tm[(id)].empty() ? tm[(id)] : "TYPE_" + TSTR(id))
#define GET_EXPR(id) (expr.size() > (id) && !expr[(id)].empty() ? expr[(id)] : "v_" + TSTR(id))
#define OP1(op, sym) case op: ENSURE(tm, spv[i+2]); if(len>=4) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+sym+GET_EXPR(spv[i+3])+";\n"; return true;
#define OP2(op, sym) case op: ENSURE(tm, spv[i+2]); if(len>=5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+GET_EXPR(spv[i+3])+" "+sym+" "+GET_EXPR(spv[i+4])+";\n"; return true;
#define OPF(op, fn)  case op: ENSURE(tm, spv[i+2]); if(len>=5) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+fn+"("+GET_EXPR(spv[i+3])+", "+GET_EXPR(spv[i+4])+");\n"; return true;
#define OPF1(op, fn) case op: ENSURE(tm, spv[i+2]); if(len>=4) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+fn+"("+GET_EXPR(spv[i+3])+");\n"; return true;
#define OP_CAST(op, cast) case op: ENSURE(tm, spv[i+2]); if(len>=4) cur_blk += GET_TM(spv[i+1])+" v_"+TSTR(spv[i+2])+" = "+cast+"("+GET_EXPR(spv[i+3])+");\n"; return true;

// [flow.h]
struct SpirvPhiValue {
    uint32_t result;
    uint32_t parent;
    std::string expr;
};

struct SpvFlowBag {
    std::vector<SpvPhiValue> phi;
    bool phi_loaded = false;
    uint32_t selection_merge = 0;
    uint32_t selection_control = 0;
    uint32_t loop_merge = 0;
    uint32_t continue_target = 0;
    uint32_t loop_control = 0;
};