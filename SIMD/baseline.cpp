#include "baseline.hpp"

void add_scalar(const float* a, const float* b, float* result, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        result[i] = a[i] + b[i];
    }
}

void sub_scalar(const float* a, const float* b, float* result, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        result[i] = a[i] - b[i];
    }
}
