#pragma once
#include <cstddef>

void add_avx(const float* a, const float* b, float* result, std::size_t n);
void sub_avx(const float* a, const float* b, float* result, std::size_t n);

void add_avx_cleanup(
    const float* a, const float* b, float* result, std::size_t n
);
void sub_avx_cleanup(
    const float* a, const float* b, float* result, std::size_t n
);

void add_avx_overflow(
    const float* a, const float* b, float* result, std::size_t n
);

void sub_avx_overflow(
    const float* a, const float* b, float* result, std::size_t n
);
