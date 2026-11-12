#include <iostream>
#include <immintrin.h>

using namespace std;


void sub_avx2(float* a, float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_sub_ps(va, vb);
        _mm256_storeu_ps(&result[i], vr);
    }
}

void add_avx2(float* a, float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&result[i], vr);
    }
}

void add_avx_scalar_cleanup(float* a, float* b, float* result, size_t n)  {
    size_t i;
    for (i = 0; i + 8 < n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&result[i], vr);
    }

    for (; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}

void sub_avx_scalar_cleanup(float* a, float* b, float* result, size_t n)  {
    size_t i;
    for (i = 0; i + 8 < n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_sub_ps(va, vb);
        _mm256_storeu_ps(&result[i], vr);
    }

    for (; i < n; i++) {
        result[i] = a[i] - b[i];
    }
}

void add_avx_scalar_cleanup_with_overflow(float* a, float* b, float* result, size_t n) {
    size_t i = 0;
    __m256 inf = _mm256_set1_ps(numeric_limits<float>::infinity());
    __m256 sign_mask =  _mm256_set1_ps(-0.0f);

    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_add_ps(va, vb);

        __m256 abs_result = _mm256_andnot_ps(sign_mask, vr);
        __m256 comparison = _mm256_cmp_ps(abs_result, inf, _CMP_EQ_OQ);

        int mask = _mm256_movemask_ps(comparison);

        if (mask != 0) {
            for (int j = 0; j < 8; j++) {
                if (mask & (1 << j)) {
                    std::cout << "Overflow at index: " << i + j << std::endl;
                }
            }
        }

        _mm256_storeu_ps(&result[i], vr);
    }

    // Scalar cleanup
    for (; i < n; i++) {
        result[i] = a[i] + b[i];

        if (std::isinf(result[i])) {
            std::cout << "Overflow at index " << i << '\n';
        }
    }
}

int main() {
    __m256 evens = _mm256_set_ps(2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0);
    __m256 odds = _mm256_set_ps(1.0, 3.0, 5.0, 7.0, 9.0, 11.0, 13.0, 15.0);

    __m256 result = _mm256_sub_ps(evens, odds);

    float *f = (float*)&result;
    cout << "Result: ";
    for (int i = 0; i < 8; i++) {
        cout << f[i] << " ";
    }

    cout << endl;

    return 0;
}
