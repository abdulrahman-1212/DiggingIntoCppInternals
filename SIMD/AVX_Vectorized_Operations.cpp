#include "AVX_Vectorized_Operations.hpp"

#include <cmath>
#include <immintrin.h>
#include <limits>

void add_avx(const float* a, const float* b, float* result, std::size_t n) {
    // Precondition: n is divisible by 8.
    for (std::size_t i = 0; i < n; i += 8) {
        const __m256 va = _mm256_loadu_ps(a + i);
        const __m256 vb = _mm256_loadu_ps(b + i);
        const __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(result + i, vr);
    }
}

void sub_avx(const float* a, const float* b, float* result, std::size_t n) {
    // Precondition: n is divisible by 8.
    for (std::size_t i = 0; i < n; i += 8) {
        const __m256 va = _mm256_loadu_ps(a + i);
        const __m256 vb = _mm256_loadu_ps(b + i);
        const __m256 vr = _mm256_sub_ps(va, vb);
        _mm256_storeu_ps(result + i, vr);
    }
}

void add_avx_cleanup(const float* a, const float* b, float* result, std::size_t n) {
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        const __m256 va = _mm256_loadu_ps(a + i);
        const __m256 vb = _mm256_loadu_ps(b + i);
        const __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(result + i, vr);
    }

    for (; i < n; ++i) {
        result[i] = a[i] + b[i];
    }
}

void sub_avx_cleanup(const float* a, const float* b, float* result, std::size_t n) {
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        const __m256 va = _mm256_loadu_ps(a + i);
        const __m256 vb = _mm256_loadu_ps(b + i);
        const __m256 vr = _mm256_sub_ps(va, vb);
        _mm256_storeu_ps(result + i, vr);
    }

    for (; i < n; ++i) {
        result[i] = a[i] - b[i];
    }
}

void add_avx_overflow(const float* a, const float* b, float* out, std::size_t n)
{
    const __m256 inf = _mm256_set1_ps(INFINITY);
    const __m256 zero = _mm256_setzero_ps();

    std::size_t i = 0;

    for (; i + 8 <= n; i += 8){
        const __m256 va = _mm256_loadu_ps(a + i);
        const __m256 vb = _mm256_loadu_ps(b + i);

        const __m256 result = _mm256_add_ps(va, vb);

        _mm256_storeu_ps(out + i, result);

        // Check whether the result is infinity.
        const __m256 result_abs = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), result);

        const __m256 is_inf = _mm256_cmp_ps(result_abs, inf, _CMP_EQ_OQ);

        const __m256 a_abs = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), va);
        const __m256 b_abs = _mm256_andnot_ps(_mm256_set1_ps(-0.0f), vb);

        const __m256 a_finite = _mm256_cmp_ps(a_abs, inf, _CMP_LT_OQ);
        const __m256 b_finite = _mm256_cmp_ps(b_abs, inf, _CMP_LT_OQ);

        const __m256 overflow = _mm256_and_ps(
            is_inf, _mm256_and_ps(a_finite, b_finite)
        );

        if (_mm256_movemask_ps(overflow) != 0){
            std::cout << "Warning: Addition overflow detected!\n";
        }
    }

    // Scalar cleanup.
    for (; i < n; ++i){
        out[i] = a[i] + b[i];

        if (std::isfinite(a[i]) && std::isfinite(b[i]) &&
            std::isinf(out[i]))
        {
            std::cout << "Warning: Addition overflow detected!\n";
        }
    }
}

void sub_avx_overflow(const float *a, const float *b, float *result, std::size_t n) {
    const __m256 inf = _mm256_set1_ps(INFINITY);
    const __m256 sign_mask = _mm256_set1_ps(-0.0f);

    std::size_t i;

    for (i = 0; i + 8 < n; i += 8) {
        const __m256 va = _mm256_loadu_ps(a + i);
        const __m256 vb = _mm256_loadu_ps(b + i);

        const __m256 vr = _mm256_sub_ps(va, vb);
        
        _mm256_storeu_ps(result + i, vr);

        const __m256 abs_result = _mm256_andnot_ps(sign_mask, result);
        const __m256 a_abs = _mm256_andnot_ps(sign_mask, va);
        const __m256 b_abs = _mm256_andnot_ps(sign_mask, vb);

        const __m256 a_finite = _mm256_cmp_ps(a_abs, inf, _CMP_LT_OQ);
        const __m256 b_finite = _mm256_cmp_ps(b_abs, inf, _CMP_LT_OQ);
        const __m256 result_inf = _mm256_cmp_ps(result_abs, inf, _CMP_EQ_OQ);

        const __m256 overflow = _mm256_and_ps(result_inf, a_finite, b_finite);

        if (_mm256_movemask_ps(overflow) != 0) {
            std::cout << "Warning: Subtraction overflow detected!\n";
        }
    }

    // scalar cleanup
    for(; i < n; i++) {
        result[i] = a[i] - b[i];
        if(std::isfinite(a[i]) && std::isfinite(b[i]) && std::isinf(result[i])) {
            std::cout << "Warning: Subtraction overflow detected!\n";
        }
    }
}
