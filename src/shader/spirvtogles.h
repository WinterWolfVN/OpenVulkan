#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>

std::string TranslateSpirvFull(const uint32_t* spv, size_t size);
