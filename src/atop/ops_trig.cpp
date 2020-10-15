#include "common_inc.h"
#include <cmath>
#include "invalids.h"

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang attribute push (__attribute__((target("avx2"))), apply_to=function)
#endif

#if defined(__GNUC__)
//#pragma GCC target "arch=core-avx2,tune=core-avx2"
#if __GNUC_PREREQ(4, 4) || (__clang__ > 0 && __clang_major__ >= 3) || !defined(__GNUC__)
/* GCC >= 4.4 or clang or non-GCC compilers */
#include <x86intrin.h>
#elif __GNUC_PREREQ(4, 1)
/* GCC 4.1, 4.2, and 4.3 do not have x86intrin.h, directly include SSE2 header */
#include <emmintrin.h>
#endif
#endif


//#define LOGGING printf
#define LOGGING(...)

static const inline __m256d LOADU(__m256d* x) { return _mm256_loadu_pd((double const*)x); };
static const inline __m256 LOADU(__m256* x) { return _mm256_loadu_ps((float const*)x); };
static const inline __m256i LOADU(__m256i* x) { return _mm256_loadu_si256((__m256i const*)x); };

static const inline void STOREU(__m256d* x, __m256d y) { _mm256_storeu_pd((double*)x, y); }
static const inline void STOREU(__m256* x, __m256 y) { _mm256_storeu_ps((float*)x, y); }
static const inline void STOREU(__m256i* x, __m256i y) { _mm256_storeu_si256((__m256i*)x, y); }

// For aligned loads which must be on 32 byte boundary
static const inline __m256d LOADA(__m256d* x) { return _mm256_load_pd((double const*)x); };
static const inline __m256 LOADA(__m256* x) { return _mm256_load_ps((float const*)x); };
static const inline __m256i LOADA(__m256i* x) { return _mm256_load_si256((__m256i const*)x); };

// Aligned stores
static const inline void STOREA(__m256d* x, __m256d y) { _mm256_store_pd((double*)x, y); }
static const inline void STOREA(__m256* x, __m256 y) { _mm256_store_ps((float*)x, y); }
static const inline void STOREA(__m256i* x, __m256i y) { _mm256_store_si256((__m256i*)x, y); }


template<typename T> static const inline long double LOG_OP(long double x) { return logl(x); }
template<typename T> static const inline double LOG_OP(double x) { return log(x); }
template<typename T> static const inline float LOG_OP(float x) { return logf(x); }

template<typename T> static const inline long double LOG2_OP(long double x) { return log2l(x); }
template<typename T> static const inline double LOG2_OP(double x) { return log2(x); }
template<typename T> static const inline float LOG2_OP(float x) { return log2f(x); }

template<typename T> static const inline long double LOG10_OP(long double x) { return log10l(x); }
template<typename T> static const inline double LOG10_OP(double x) { return log10(x); }
template<typename T> static const inline float LOG10_OP(float x) { return log10f(x); }

template<typename T> static const inline long double EXP_OP(long double x) { return expl(x); }
template<typename T> static const inline double EXP_OP(double x) { return exp(x); }
template<typename T> static const inline float EXP_OP(float x) { return expf(x); }

template<typename T> static const inline long double EXP2_OP(long double x) { return exp2l(x); }
template<typename T> static const inline double EXP2_OP(double x) { return exp2(x); }
template<typename T> static const inline float EXP2_OP(float x) { return exp2f(x); }

template<typename T> static const inline long double CBRT_OP(long double x) { return cbrtl(x); }
template<typename T> static const inline double CBRT_OP(double x) { return cbrt(x); }
template<typename T> static const inline float CBRT_OP(float x) { return cbrtf(x); }

template<typename T> static const inline long double TAN_OP(long double x) { return tanl(x); }
template<typename T> static const inline double TAN_OP(double x) { return tan(x); }
template<typename T> static const inline float TAN_OP(float x) { return tanf(x); }

template<typename T> static const inline long double COS_OP(long double x) { return cosl(x); }
template<typename T> static const inline double COS_OP(double x) { return cos(x); }
template<typename T> static const inline float COS_OP(float x) { return cosf(x); }

template<typename T> static const inline long double SIN_OP(long double x) { return sinl(x); }
template<typename T> static const inline double SIN_OP(double x) { return sin(x); }
template<typename T> static const inline float SIN_OP(float x) { return sinf(x); }

#if defined(RT_COMPILER_MSVC)

template<typename T> static const inline __m256  SIN_OP_256(__m256 x) { return _mm256_sin_ps(x); }
template<typename T> static const inline __m256d SIN_OP_256(__m256d x) { return _mm256_sin_pd(x); }

template<typename T> static const inline __m256  COS_OP_256(__m256 x) { return _mm256_cos_ps(x); }
template<typename T> static const inline __m256d COS_OP_256(__m256d x) { return _mm256_cos_pd(x); }

template<typename T> static const inline __m256  TAN_OP_256(__m256 x) { return _mm256_tan_ps(x); }
template<typename T> static const inline __m256d TAN_OP_256(__m256d x) { return _mm256_tan_pd(x); }

#endif

#if defined(__GNUC__)
// May require -lm for linker

__m256d _ZGVdN4v_cos(__m256d x);
__m256d _ZGVdN4v_exp(__m256d x);
__m256d _ZGVdN4v_log(__m256d x);
__m256d _ZGVdN4v_sin(__m256d x);
__m256d _ZGVdN4vv_pow(__m256d x, __m256d y);
void    _ZGVdN4vvv_sincos(__m256d x, __m256i ptrs, __m256i ptrc);

__m256  _ZGVdN8v_cosf(__m256 x);
__m256  _ZGVdN8v_expf(__m256 x);
__m256  _ZGVdN8v_logf(__m256 x);
__m256  _ZGVdN8v_sinf(__m256 x);
__m256  _ZGVdN8vv_powf(__m256 x, __m256 y);
void    _ZGVdN8vvv_sincosf(__m256 x, __m256i ptrs_lo, __m256i ptrs_hi, __m256i ptrc_lo, __m256i ptrc_hi);

template<typename T> static const inline __m256  SIN_OP_256(__m256 x) { return _ZGVdN8v_sinf(x); }
template<typename T> static const inline __m256d SIN_OP_256(__m256d x) { return _ZGVdN4v_sin(x); }

template<typename T> static const inline __m256  COS_OP_256(__m256 x) { return _ZGVdN8v_cosf(x); }
template<typename T> static const inline __m256d COS_OP_256(__m256d x) { return _ZGVdN4v_cos(x); }

#endif


//-------------------------------------------------------------------
// T = data type as input
// MathOp operation to perform
template<typename T, typename U256, const T MATH_OP(T), const U256 MATH_OP256(U256)>
static inline void UnaryOpFast(void* pDataIn, void* pDataOut, int64_t len, int64_t strideIn, int64_t strideOut) {

    T* pIn = (T*)pDataIn;
    T* pOut = (T*)pDataOut;
    T* pLastOut = (T*)((char*)pOut + (strideOut * len));

    int64_t chunkSize = sizeof(U256) / sizeof(T);
    LOGGING("unary op fast strides %lld %lld   sizeof: %lld\n", strideIn, strideOut, sizeof(T));
    if (sizeof(T) == strideOut && sizeof(T) == strideIn && len >= chunkSize) {

        // possible to align 32 bit boundary?
        //if (((int64_t)pDataOut & 31) != 0) {
        //   int64_t babylen = (32 - ((int64_t)pDataOut & 31)) / sizeof(T);
        //   if (babylen > len) babylen = len;
        //   for (int64_t i = 0; i < babylen; i++) {
        //      *pOut++ = MATH_OP(*pIn++);
        //   }
        //   len -= babylen;
        //}

        T* pEnd = &pOut[chunkSize * (len / chunkSize)];
        U256* pEnd_256 = (U256*)pEnd;

        U256* pIn1_256 = (U256*)pIn;
        U256* pOut_256 = (U256*)pOut;

        // possible to align?
        do {
            // Use 256 bit registers which hold 8 floats or 4 doubles
            // The first operand should allow unaligned loads
            STOREU(pOut_256, MATH_OP256(*pIn1_256));
            pIn1_256 += 1;
            pOut_256 += 1;

        } while (pOut_256 < pEnd_256);

        // update thin pointers to last location of wide pointers
        pIn = (T*)pIn1_256;
        pOut = (T*)pOut_256;

    }

    // Slow loop, handle 1 at a time
    while (pOut != pLastOut) {
        *pOut = MATH_OP(*pIn);
        pOut = STRIDE_NEXT(T, pOut, strideOut);
        pIn = STRIDE_NEXT(T, pIn, strideIn);
    }
}



extern "C"
UNARY_FUNC GetTrigOpFast(int func, int atopInType1, int* wantedOutType) {

    LOGGING("Looking for func %d  type:%d \n", func, atopInType1);

    switch (func) {
    case TRIG_OPERATION::SIN:
        *wantedOutType = atopInType1;
#if !defined(__clang__)
        switch (atopInType1) {
        case ATOP_FLOAT:  return UnaryOpFast<float, __m256, SIN_OP<float>, SIN_OP_256<__m256>>;
        case ATOP_DOUBLE: return UnaryOpFast<double, __m256d, SIN_OP<double>, SIN_OP_256<__m256d>>;
        }
#endif
        break;

    case TRIG_OPERATION::COS:
        *wantedOutType = atopInType1;
#if !defined(__clang__)
        switch (atopInType1) {
        case ATOP_FLOAT:  return UnaryOpFast<float, __m256, COS_OP<float>, COS_OP_256<__m256>>;
        case ATOP_DOUBLE: return UnaryOpFast<double, __m256d, COS_OP<double>, COS_OP_256<__m256d>>;
        }
#endif
        break;
    case TRIG_OPERATION::TAN:
        *wantedOutType = atopInType1;
#if defined(RT_COMPILER_MSVC)
        switch (atopInType1) {
        case ATOP_FLOAT:  return UnaryOpFast<float, __m256, TAN_OP<float>, TAN_OP_256<__m256>>;
        case ATOP_DOUBLE: return UnaryOpFast<double, __m256d, TAN_OP<double>, TAN_OP_256<__m256d>>;
        }
#endif
        break;

    }
    return NULL;
}



#if defined(__clang__)
#pragma clang attribute pop
#endif
