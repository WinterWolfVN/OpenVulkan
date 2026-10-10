#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "stspirv.h"

inline std::string SpvFlowBag(uint32_t source, uint32_t target, const std::vector<std::vector<SpvPhiValue>> spvflow_bag) {
    std::string out;
    if (target >= spvflow_bag.size()) return out;
    for (const auto& p : spvflow_bag[target])
        if (p.predecessor == source)
            out += "        v_" + std::to_string(p.result) + " = " + p.value + ";\n";
    return out;
}

inline std::string TranslateFlow(const uint32_t* spv, size_t size, std::vector<std::string>& tm, std::vector<std::string>& expr, std::string& vars) {
    std::vector<SpvFlowBag> spvflow_bag;
    std::vector<size_t> label_pos;
    std::vector<std::string> blocks;
    uint32_t cur_label = 0, first_label = 0;
    size_t i = 5;

    auto LoadPhi = [&](uint32_t target) {
        ENSURE(spvflow_bag, target);
        if (spvflow_bag[target].phi_loaded)
           return;
        spvflow_bag[target].phi_loaded = true;
        if (target >= label_pos.size())
           return;
        size_t p = label_pos[target];
        while (p < size) {
            uint16_t op2 = spv[p] & 0xFFFF;
            uint16_t len2 = spv[p] >> 16;
            if (len2 == 0 || p + len2 > size)
                break;
            if (p != label_pos[target] && op2 == SpvOpLabel)
                break;
            if (op2 != SpvOpPhi)
                break;
            uint32_t type = spv[p + 1];
            uint32_t result = spv[p + 2];
            ENSURE(tm, result);
            tm[result] = GET_TM(type);
            for (size_t j = 3; j + 1 < len2; j += 2) {
                spvflow_bag[target].phi.push_back({result, spv[p + j + 1], GET_EXPR(spv[p + j])});
            }
            p += len2;
        }
    };

        switch (op) {
            case SpvOpBranch:
                if (len >= 2 && cur_label < blocks.size()) {
                    uint32_t target = spv[i + 1];
                    LoadPhi(target);
                    for (const auto& p : spvflow_bag[target].phi) {
                        if (p.parent != cur_label)
                           continue;
                        blocks[cur_label] += "        v_" + TSTR(p.result) + " = " + p.expr + ";\n";
                    }
                    blocks[cur_label] += "        _state = " + TSTR(target) + ";\n" "        break;\n";
                }
                break;
            case SpvOpBranchConditional:
                if (len >= 4 && cur_label < blocks.size()) {
                    uint32_t cond = spv[i + 1];
                    uint32_t yes = spv[i + 2];
                    uint32_t no = spv[i + 3];
                    LoadPhi(yes);
                    LoadPhi(no);
                    blocks[cur_label] += "        if (" + GET_EXPR(cond) + ") {\n";
                    for (const auto& p : spvflow_bag[yes].phi) {
                        if (p.parent != cur_label)
                           continue;
                        blocks[cur_label] += "            v_" + TSTR(p.result) + " = " + p.expr + ";\n";
                    }
                    blocks[cur_label] += "            _state = " + TSTR(yes) + ";\n" "        } else {\n";
                    for (const auto& p : spvflow_bag[no].phi) {
                    if (p.parent != cur_label)
                       continue;
                    blocks[cur_label] += "            v_" + TSTR(p.result) + " = " + p.expr + ";\n";
                    }
                    blocks[cur_label] += "            _state = " + TSTR(no) + ";\n" "        }\n" "        break;\n";
                }
                break;
            case SpvOpPhi:
                if (len >= 5 && cur_label != 0) {
                    uint32_t type = spv[i + 1];
                    uint32_t result = spv[i + 2];
                    ENSURE(tm, result);
                    tm[result] = GET_TM(type);
                    ENSURE(spvflow_bag, cur_label);
                    bool exists = false;
                    for (const auto& p : spvflow_bag[cur_label].phi) {
                        if (p.result == result) {
                           exists = true;
                           break;
                        }
                    }
                    if (!exists)
                       vars += GET_TM(type) + " v_" + TSTR(result) + ";\n";
                    for (size_t j = 3; j + 1 < len; j += 2) {
                       spvflow_bag[cur_label].phi.push_back({result, spv[i + j + 1], GET_EXPR(spv[i + j])});
                    }
                }
                break;
            case SpvOpLabel:
               if (len >= 2) {
                   cur_label = spv[i + 1];
                   if (first_label == 0)
                      first_label = cur_label;
                      ENSURE(blocks, cur_label);
                      ENSURE(spvflow_bag, cur_label);
                      ENSURE(label_pos, cur_label);
                      label_pos[cur_label] = i;
                      blocks[cur_label] = "    case " + TSTR(cur_label) + ":\n";
               }
               break;
            case SpvOpSwitch:
                if (len >= 3 && cur_label < blocks.size()) {
                    uint32_t selector = spv[i + 1];
                    uint32_t def = spv[i + 2];
                    LoadPhi(def);
                    blocks[cur_label] += "        switch (" + GET_EXPR(selector) + ") {\n";
                    for (size_t j = 3; j + 1 < len; j += 2) {
                        uint32_t literal = spv[i + j];
                        uint32_t target = spv[i + j + 1];
                        LoadPhi(target);
                        blocks[cur_label] += "            case " + TSTR(literal) + ":\n";
                        for (const auto& p : spvflow_bag[target].phi) {
                            if (p.parent != cur_label)
                               continue;
                            blocks[cur_label] += "                v_" + TSTR(p.result) + " = " + p.expr + ";\n";
                        }
                        blocks[cur_label] += "                _state = " + TSTR(target) + ";\n" "                break;\n";
                    }
                    blocks[cur_label] += "            default:\n";
                    for (const auto& p : spvflow_bag[def].phi) {
                        if (p.parent != cur_label)
                           continue;
                        blocks[cur_label] += "                v_" + TSTR(p.result) + " = " + p.expr + ";\n";
                    }
                    blocks[cur_label] += "                _state = " + TSTR(def) + ";\n" "                break;\n" "        }\n" "        break;\n";
                }
                break;
            case SpvOpSelectionMerge:
                if (len >= 3 && cur_label < spvflow_bag.size()) {
                    spvflow_bag[cur_label].selection_merge = spv[i + 1];
                    spvflow_bag[cur_label].selection_control = spv[i + 2];
                }
                break;
            case SpvOpLoopMerge:
                if (len >= 4 && cur_label < spvflow_bag.size()) {
                    spvflow_bag[cur_label].loop_merge = spv[i + 1];
                    spvflow_bag[cur_label].continue_target = spv[i + 2];
                    spvflow_bag[cur_label].loop_control = spv[i + 3];
                }
                break;
            case SpvOpReturn:
            case SpvOpReturnValue:
                if (cur_label < blocks.size())
                    blocks[cur_label] += "        _state = 0;\n        break;\n";
                break;
            case SpvOpUnreachable:
                if (cur_label < blocks.size())
                    blocks[cur_label] += "        _state = 0;\n        break;\n";
                break;
        }
        i += len;
    }

    std::string out = "void main() {\n    int _state = " + TSTR(first_label) + ";\n" + vars + "    while (_state != 0) {\n        switch (_state) {\n";
    for (size_t b = 0; b < blocks.size(); ++b)
        if (!blocks[b].empty()) out += blocks[b];
    out += "        }\n    }\n}\n";
    return out;
}