#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>

std::string TranslateSpirvToGLES31(const uint32_t* spv, size_t size);
