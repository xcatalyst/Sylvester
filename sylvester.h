/*   Sylvester
 *
 *   ===========================================================================
 *
 *                                    IMPORTANT
 *
 *   DO NOT ACCESS VARIABLE OR FUNCTION THAT START WITH UNDERSCORE. THEY ARE
 *   INTERNAL ONLY AND CHANGING IT MIGHT BREAK THE LIBRARY OR CAUSE UNEXPECTED
 *   BEHAVIOURS !
 *
 *   ===========================================================================
 *
 */

#ifndef SYLVESTER_H
#define SYLVESTER_H

#if defined(SYL_ENABLE_AVX)
#include <immintrin.h>
#elif defined(SYL_ENABLE_SSE4)
#include <smmintrin.h>
#endif

#include <math.h>
#include <stdbool.h>

#if defined(__clang__)
#	define _SYL_SET_SPEC_ALIGN(x) __attribute__((aligned(x)))

#elif defined(__GNUC__) || defined(__GNUG__)
#	define _SYL_SET_SPEC_ALIGN(x) __attribute__((aligned(x)))

#elif defined(_MSC_VER)
#	define _SYL_SET_SPEC_ALIGN(x) __declspec(align(x))
#else
#	define _SYL_SET_SPEC_ALIGN(x)
#endif

#if defined(SYL_NO_INLINE)
#	define SYL_INLINE static
#elif defined (SYL_NO_STATIC)
#	define SYL_INLINE inline
#else
#	define SYL_INLINE static inline
#endif

#ifndef SYL_NO_STATIC
#define SYL_STATIC
#endif

#define SYL_PI 3.14159265359f

#define _SYL_SHUFFLE(a,b,c,d) (((a) << 6) | ((b) << 4) |	\
			       ((c) << 2) | ((d)))

#define _SYL_PERMUTE_PS( v, c ) _mm_shuffle_ps((v), (v), c )
#define _SYL_ADD_PS( a, b, c ) _mm_sub_ps((c), _mm_mul_ps((a), (b)))

#define _SYL_LOAD(a) _mm_load_ps((a))
#define _SYL_LOADV2(a) _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & (a))

#define _SYL_MAKE_SHUFFLE_MASK(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))
#define _SYL_VEC_SWIZZLE_MASK(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define _SYL_VEC_SWIZZLE(vec, x, y, z, w)        _SYL_VEC_SWIZZLE_MASK(vec, _SYL_MAKE_SHUFFLE_MASK(x,y,z,w))
#define _SYL_VEC_SWIZZLE1(vec, x)                _SYL_VEC_SWIZZLE_MASK(vec, _SYL_MAKE_SHUFFLE_MASK(x,x,x,x))
#define _SYL_VEC_SWIZZLE_0022(vec)               _mm_moveldup_ps(vec)
#define _SYL_VEC_SWIZZLE_1133(vec)               _mm_movehdup_ps(vec)

 // return (vec1[x], vec1[y], vec2[z], vec2[w])
#define _SYL_VEC_SHUFFLE(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, _SYL_MAKE_SHUFFLE_MASK(x,y,z,w))
// special shuffle
#define _SYL_VEC_SHUFFLE_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define _SYL_VEC_SHUFFLE_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)
#define _SYL_SMALL_NUMBER		(1.e-8f)


/* NOTE(xcatalyst): Doing it the other way fucks up emacs indentation*/
#ifndef _SYL_CPP_EXTER_STARTB
#define _SYL_CPP_EXTER_START extern "C" {
#endif

#ifndef _SYL_CPP_EXTERN_END
#define _SYL_CPP_EXTERN_END }
#endif

#ifdef __cplusplus
_SYL_CPP_EXTER_START
#endif

typedef union svec2
{
    struct { float x, y; };
    struct { float s; float t; };
    float e[2];
#if defined(SYL_ENABLE_SSE4)
    __m64 v;
#endif
} _SYL_SET_SPEC_ALIGN(8) svec2;

typedef union svec3
{
    struct { float x; float y; float z; };
    struct { float r; float g; float b; };
    struct { float s; float t; float p; };
    float e[3];
} svec3;

typedef union svec4
{
    struct { float x; float y; float z; float w; };
    struct { float r; float g; float b; float a; };
    struct { float s; float t; float m; float q; };
    float e[4];
#if defined(SYL_ENABLE_SSE4)
    __m128 v;
#endif
} _SYL_SET_SPEC_ALIGN(16) svec4;

/* We use column-major matricies */
typedef union smat4
{
    struct
    {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    float e[16];
    float e2[4][4];
    svec4 v4d[4];
#if defined(SYL_ENABLE_SSE4)
    __m128 v[4];
#endif
#if defined(SYL_ENABLE_AVX)
    __m256 v2[2];
#endif
} _SYL_SET_SPEC_ALIGN(16) smat4;

static smat4 _S_IDENT4X4 = { {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
    } };

#if !defined(SYL_ENABLE_SSE4) || !defined(SYL_ENABLE_AVX)
static smat4 _S_ZERO4X4 = { {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
    } };
#endif

static svec2 _SVEC2_ZERO = { { 0.0f, 0.0f } };

/* TODO(xcatalyst): Gives warning on GCC, unused variable */
static svec3 _SVEC3_ZERO = { { 0.0f, 0.0f, 0.0f } };
static svec4 _SVEC4_ZERO = { { 0.0f, 0.0f, 0.0f, 0.0f } };

#if defined(SYL_ENABLE_SSE4)
static __m128 _S_XMM_ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
static __m128 _S_IDENT4x4R0 = { 1.0f, 0.0f, 0.0f, 0.0f };
static __m128 _S_IDENT4x4R1 = { 0.0f, 1.0f, 0.0f, 0.0f };
static __m128 _S_IDENT4x4R2 = { 0.0f, 0.0f, 1.0f, 0.0f };
static __m128 _S_IDENT4x4R3 = { 0.0f, 0.0f, 0.0f, 1.0f };
static __m128 _S_XMM_MASK_3 = { (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0xFFFFFFFF, (float)0x00000000 };
static __m128 _S_XMM_MASK_Y = { 0x00000000, (float)0xFFFFFFFF, 0x00000000, 0x00000000 };
#endif
#if defined(SYL_ENABLE_AVX)
static __m256 _S_YMM_ZERO = { { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } };
#endif

SYL_INLINE float s_radian_to_degree(float Radian)
{
    return(Radian * (180 / SYL_PI));
}

SYL_INLINE float s_degree_to_radian(float Degree)
{
    return (Degree * (SYL_PI / 180));
}

SYL_INLINE float s_roundf(float A)
{
    return((int)(A + 0.5f));
}

SYL_INLINE double s_roundd(double A)
{
    return((int)(A + 0.5f));
}

SYL_INLINE float s_ceilf(float A)
{
    return((int)(A + 1.0f));
}

SYL_INLINE double s_ceild(double A)
{
    return((int)(A + 1.0f));
}

SYL_INLINE float s_floorf(float A)
{
    return((int)A);
}

SYL_INLINE double s_floord(double A)
{
    return((int)A);
}

/* Unpack four 8-bit BGRA values into vec4 */
SYL_INLINE svec4 s_bgra_unpack(int Color)
{
    svec4 Result = { {
        (float)((Color >> 16) & 0xFF),
        (float)((Color >> 8) & 0xFF),
        (float)((Color >> 0) & 0xFF),
        (float)((Color >> 24) & 0xFF)
    } };
    return(Result);
}

/* Pack four 8-bit RGB values */
SYL_INLINE unsigned int s_bgra_pack(svec4 Color)
{
    unsigned int Result =
        ((unsigned int)(Color.a) << 24) |
        ((unsigned int)(Color.r) << 16) |
        ((unsigned int)(Color.g) << 8) |
        ((unsigned int)(Color.b) << 0);
    return(Result);
}

/* Unpack four 8-bit RGBA values into vec4 */
SYL_INLINE svec4 s_rgba_unpack(unsigned int Color)
{
    svec4 Result = { {
        (float)((Color >> 24) & 0xFF),
        (float)((Color >> 16) & 0xFF),
        (float)((Color >> 8) & 0xFF),
        (float)((Color >> 0) & 0xFF)
    } };
    return(Result);
}

/* Pack four 8-bit RGBA values */
SYL_INLINE unsigned int s_rgba_pack(svec4 Color)
{
    unsigned int Result =
        ((unsigned int)(Color.a) << 24) |
        ((unsigned int)(Color.b) << 16) |
        ((unsigned int)(Color.g) << 8) |
        ((unsigned int)(Color.r) << 0);

    return(Result);

}

SYL_INLINE float s_clampf(float Value, float Min, float Max)
{
    float Result = Value;

    if (Result < Min)
    {
        Result = Min;
    }
    else if (Result > Max)
    {
        Result = Max;
    }

    return(Result);
}

SYL_INLINE float s_clampd(float Value, float Min, float Max)
{
    float Result = Value;

    if (Result < Min)
    {
        Result = Min;
    }
    else if (Result > Max)
    {
        Result = Max;
    }

    return(Result);
}

SYL_INLINE int s_clampi(int Value, int Min, int Max)
{
    int Result = Value;

    if (Result < Min)
    {
        Result = Min;
    }
    else if (Result > Max)
    {
        Result = Max;
    }

    return(Result);
}

SYL_INLINE float s_clamp01f(float Value)
{
    return(s_clampf(0.0f, Value, 1.0f));
}

SYL_INLINE double s_clamp01d(double Value)
{
    return(s_clampf(0.0f, Value, 1.0f));
}

SYL_INLINE float s_clamp_above_zero(float Value)
{
    return (Value < 0) ? 0.0f : Value;
}

SYL_INLINE float s_clamp_below_zero(float Value)
{
    return (Value > 0) ? 0.0f : Value;
}

SYL_INLINE bool s_is_in_range(float Value, float Min, float Max)
{
    return(((Min <= Value) && (Value <= Max)));
}

SYL_INLINE float s_lerp(float A, float t, float B)
{
    return (1.0f - t) * A + t * B;
}

/* It is handy to type square if the expression happens to be very long */
SYL_INLINE float s_square(float x)
{
    return(x * x);
}

/* Absoule value */
SYL_INLINE float s_abs(float x)
{
    return *((unsigned int*)(&x)) &= 0xffffffff >> 1;
}

/* Find the hypotenuse of a triangle given two other sides */
SYL_INLINE float s_pythagorean(float x, float y)
{
    return sqrt(x * x + y * y);
}

/* Maximum of two values */
SYL_INLINE float s_maxf(float x, float y)
{
    if (x > y)
        return(x);
    return(y);
}

SYL_INLINE int s_maxi(int x, int y)
{
    if (x > y)
        return(x);
    return(y);
}

SYL_INLINE int s_mini(int x, int y)
{
    if (x < y)
        return(x);
    return(y);
}

SYL_INLINE float s_minf(float x, float y)
{
    if (x < y)
        return(x);
    return(y);
}

SYL_INLINE float s_mod(float x, float y)
{
    return(x - (s_roundf(x / y) * y));
}

SYL_INLINE float s_pow(float Value, float Times)
{
    float pow = 1;
    for (int i = 0; i < Times; i++) {
        pow = pow * Value;
    }
    return(pow);
}

/* Places -> How many digits you want to keep remained */
SYL_INLINE float s_truncatef(float Value, float Remain)
{
    int Remove = s_pow(10, Remain);
    return(s_roundf(Value * Remove) / Remove);
}

SYL_INLINE double s_truncated(double Value, double Places)
{
    int Remove = pow(10, Places);
    return(s_roundf(Value * Remove) / Remove);
}

SYL_INLINE float s_normalize(float Value, float Min, float Max)
{
    return (Value - Min) / (Max - Min);
}

SYL_INLINE float s_map(float Value, float SourceMin, float SourceMax, float DestMin, float DestMax)
{
    return s_lerp(s_normalize(Value, SourceMin, SourceMax), DestMin, DestMax);
}

SYL_INLINE svec3 s_rgb_to_hsv(svec3 RGB)
{
    /* Range the values between 1 and 0*/
    RGB.r = RGB.r / 255.0;
    RGB.g = RGB.g / 255.0;
    RGB.b = RGB.b / 255.0;

    float MaxValue = s_maxf(RGB.r, s_maxf(RGB.g, RGB.b));
    float MinValue = s_maxf(RGB.r, s_minf(RGB.g, RGB.b));
    float Dif = MaxValue - MinValue;

    svec3 Result;
    Result.x = -1, Result.y = -1;

    if (MaxValue == MinValue)
        Result.x = 0;

    else if (MaxValue == RGB.r)
        Result.x = s_mod(60 * ((RGB.g - RGB.b) / Dif) + 360, 360);

    else if (MaxValue == RGB.g)
        Result.x = s_mod(60 * ((RGB.b - RGB.r) / Dif) + 120, 360);

    else if (MaxValue == RGB.b)
        Result.x = s_mod(60 * ((RGB.r - RGB.g) / Dif) + 240, 360);

    if (MaxValue == 0)
        Result.y = 0;
    else
        Result.y = (Dif / MaxValue) * 100;

    Result.z = MaxValue * 100;
    return(Result);
}

/*********************************************
 *                 VECTOR 2D		  *
 *********************************************/

SYL_INLINE svec2 S_VEC2F(float a, float b) // From individiaul valeus
{
    svec2 r = { { a, b } };
    return(r);
}

SYL_INLINE svec2 S_VEC2A(float* a) // From array
{
    svec2 r = { { a[0], a[1] } };
    return(r);
}

SYL_INLINE void s_vec2_zero(svec2* Vector)
{
    *Vector = _SVEC2_ZERO;
}

SYL_INLINE bool s_vec2_equal(svec2 Vec1, svec2 Vec2)
{
    bool result = false;
    if (Vec1.x == Vec2.x && Vec1.y == Vec2.y)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_equal_scalar(svec2 Vec1, float Value)
{
    bool result = false;
    if (Vec1.x == Value && Vec1.y == Value)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_not_equal(svec2 Vec1, svec2 Vec2)
{
    bool result = false;
    if (Vec1.x != Vec2.x && Vec1.y != Vec2.y)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_not_equal_scalar(svec2 Vec1, float Value)
{
    bool result = false;
    if (Vec1.x != Value && Vec1.y != Value)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_greater(svec2 Vec1, svec2 Vec2)
{
    bool result = false;
    if (Vec1.x > Vec2.x && Vec1.y > Vec2.y)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_greater_scalar(svec2 Vec1, float Value)
{
    bool result = false;
    if (Vec1.x > Value && Vec1.y > Value)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_greater_equal(svec2 Vec1, svec2 Vec2)
{
    bool result = false;
    if (Vec1.x >= Vec2.x && Vec1.y >= Vec2.y)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_greater_equal_scalar(svec2 Vec1, float Value)
{
    bool result = false;
    if (Vec1.x >= Value && Vec1.y >= Value)
        result = true;
    return(result);
}


SYL_INLINE bool s_vec2_less(svec2 Vec1, svec2 Vec2)
{
    bool result = false;
    if (Vec1.x < Vec2.x && Vec1.y < Vec2.y)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_less_scalar(svec2 Vec1, float Value)
{
    bool result = false;
    if (Vec1.x < Value && Vec1.y < Value)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_less_equal(svec2 Vec1, svec2 Vec2)
{
    bool result = false;
    if (Vec1.x <= Vec2.x && Vec1.y <= Vec2.y)
        result = true;
    return(result);
}

SYL_INLINE bool s_vec2_less_equal_scalar(svec2 Vec1, float Value)
{
    bool result = false;
    if (Vec1.x <= Value && Vec1.y <= Value)
        result = true;
    return(result);
}

SYL_INLINE svec2 s_vec2_add(svec2 Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    return *(svec2*)&r;
#else
    svec2 result = { { (Vec1.x + Vec2.x), (Vec1.y + Vec2.y) } };
    return(result);
#endif
}

SYL_INLINE svec2 s_vec2p_add(svec2* Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x + Vec2.x;
    Vec1->y = Vec1->y + Vec2.y;	// 
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_svec2_add_scalar(svec2 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_set1_ps(Value));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Vec1.x + Value), (Vec1.y + Value) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_svec2p_add_scalar(svec2* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x + Value;
    Vec1->y = Vec1->y + Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_vec2_sub(svec2 Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Vec1.x - Vec2.x), (Vec2.x - Vec2.y) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_vec2p_sub(svec2* Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x - Vec2.x;
    Vec1->y = Vec1->y - Vec2.y;
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_vec2_sub_scalar(svec2 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_set1_ps(Value));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Vec1.x - Value), (Vec1.y - Value) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_scalar_sub_vec2(float Value, svec2 Vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_set1_ps(Value), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Value - Vec1.x), (Value - Vec1.y) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_vec2p_sub_scalar(svec2* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x - Value;
    Vec1->y = Vec1->y - Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_scalar_sub_vec2p(float Value, svec2* Vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Value - Vec1->x;
    Vec1->y = Value - Vec1->y;
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_vec2_mul(svec2 Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Vec1.x * Vec2.x), (Vec1.y * Vec2.y) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_vec2p_mul(svec2* Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x * Vec2.x;
    Vec1->y = Vec1->y * Vec2.y;
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_vec2_mul_scalar(svec2 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_set1_ps(Value));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Vec1.x * Value), (Vec1.y * Value) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_vec2p_mul_scalar(svec2* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x * Value;
    Vec1->y = Vec1->y * Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_vec2_div(svec2 Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Vec1.x / Vec2.x), (Vec1.y / Vec2.y) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_vec2p_div(svec2* Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x / Vec2.x;
    Vec1->y = Vec1->y / Vec2.y;
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_vec2_div_scalar(svec2 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_set1_ps(Value));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Vec1.x / Value), (Vec1.y / Value) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_scalar_div_vec2(float Value, svec2 Vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_set1_ps(Value), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1));
    return *(svec2*)&r;
#else
    svec2 Result = { { (Value / Vec1.x), (Value / Vec1.y) } };
    return(Result);
#endif
}

SYL_INLINE svec2 s_vec2p_div_scalar(svec2* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x / Value;
    Vec1->y = Vec1->y / Value;;
    return(*Vec1);
#endif
}

SYL_INLINE svec2 s_scalar_div_vec2p(float Value, svec2* Vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_set1_ps(Value), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1->e));
    *Vec1 = *(svec2*)&r;
    return(*Vec1);
#else
    Vec1->x = Value / Vec1->x;
    Vec1->y = Value / Vec1->y;
    return(*Vec1);
#endif
}

/* Negate all components of the vector */
SYL_INLINE svec2 s_vec2_negate(svec2 a)
{
    svec2 Result = { { -a.x, -a.y } };
    return(Result);
}

SYL_INLINE svec2 s_vec2_floor(svec2 A)
{
    svec2 Result = { { s_floorf(A.x), s_floorf(A.y) } };
    return(Result);
}

/* Round all components of the vec3 to nearest integer*/
SYL_INLINE svec2 s_vec2_round(svec2 A)
{
    svec2 Result = { { s_roundf(A.x), s_roundf(A.y) } };
    return(Result);
}

SYL_INLINE float s_vec2_dot(svec2 Vec1, svec2 Vec2)
{
    return((Vec1.x * Vec2.x) + (Vec1.y * Vec2.y));
}

SYL_INLINE svec2 s_vec2_hadamard(svec2 Vec1, svec2 Vec2)
{
    svec2 Result = { { (Vec1.x * Vec2.x), (Vec1.y * Vec2.y) } };
    return(Result);
}

SYL_INLINE svec2 s_vec2_lerp(svec2 Vec1, svec2 Vec2, float t)
{
    svec2 r = { { Vec1.x + (Vec2.x - Vec1.x * t), Vec1.y + ((Vec2.y - Vec1.y * t)) } };
    return r;
}

SYL_INLINE svec2 s_vec2_clamp(svec2 Value, svec2 Min, svec2 Max)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_min_ps(_mm_max_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Value.v), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Min.v)), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Max.v));
    return *(svec2*)&Result;
#else
    svec2 Result = { { s_clampf(Value.x, Min.x, Max.x), s_clampf(Value.y, Min.y, Max.y) } };
    return(Result);
#endif
}

SYL_INLINE float s_vec2_length(svec2 Vec1)
{
    return(sqrt((Vec1.x * Vec1.x) + (Vec1.y * Vec1.y)));
}

SYL_INLINE float s_vec2_distance(svec2 Vec1, svec2 Vec2)
{
    return s_vec2_length(s_vec2_sub(Vec1, Vec2));
}

SYL_INLINE svec2 s_vec2_normalize(svec2 a)
{
    return(s_vec2_mul_scalar(a, (1.0f / s_vec2_length(a))));
}

/* Reflect a position to a normal plane */
SYL_INLINE svec2 s_vec2_reflect(svec2 Pos, svec2 N)
{
    svec2 Normal = s_vec2_normalize(N);
    float r = s_vec2_dot(Pos, Normal);
    svec2 t = { { (float)(Pos.e[0] - Normal.e[0] * 2.0 * r), (float)(Pos.e[1] - Normal.e[1] * 2.0 * r) } };
    return(t);
}

/* Project from a position along a vector on to a plane */
SYL_INLINE svec2 s_vec2_project(svec2 VectorToProject, svec2 ProjectionVector)
{
    float scale = s_vec2_dot(ProjectionVector, VectorToProject) / s_vec2_dot(ProjectionVector, ProjectionVector);
    return(s_vec2_mul_scalar(ProjectionVector, scale));
}

/* Flattens a position to a normal plane */
static inline svec2 s_vec2_flatten(svec2 Pos, svec2 Normal)
{
    float f = s_vec2_dot(Pos, Normal);
    svec2 result = { { (Pos.e[0] - Normal.e[0] * f), (Pos.e[1] - Normal.e[1] * f) } };
    return(result);
}

/* Per component comparsion to return a vector containing the largest components */
SYL_INLINE svec2 s_vec2_max_vector(svec2 Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_max_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1.e), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2.e));
    return *(svec2*)&r;
#else
    svec2 Result;
    if (Vec1.x > Vec2.x)
    {
        Result.x = Vec1.x;
    }
    else
    {
        Result.x = Vec2.x;
    }

    if (Vec1.y > Vec2.y)
    {
        Result.y = Vec1.y;
    }
    else
    {
        Result.y = Vec2.y;
    }
    return(Result);
#endif
}

/* Per component comparsion to return a vector containing the smollest components */
SYL_INLINE svec2 s_vec2_min_vector(svec2 Vec1, svec2 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_min_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1.e), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2.e));
    return *(svec2*)&r;
#else
    svec2 Result;
    if (Vec1.x < Vec2.x)
    {
        Result.x = Vec1.x;
    }
    else
    {
        Result.x = Vec2.x;
    }

    if (Vec1.y < Vec2.y)
    {
        Result.y = Vec1.y;
    }
    else
    {
        Result.y = Vec2.y;
    }
    return(Result);
#endif
}

/* Return the biggest element inside vec4 */
SYL_INLINE float s_vec2_max(svec2 A)
{
    if (A.e[0] > A.e[1])
    {
        return(A.e[0]);
    }
    else
    {
        return(A.e[1]);
    }
}

/* Return the smollest element inside vec4 */
SYL_INLINE float s_vec2_min(svec2 A)
{
    if (A.e[0] < A.e[1])
    {
        return(A.e[0]);
    }
    else
    {
        return(A.e[1]);
    }
}

/* Add all components of the vector together */
SYL_INLINE float s_vec2_sum(svec2 Vec1)
{
    return(Vec1.x + Vec1.y);
}

SYL_INLINE float s_triangle_area(svec2 Vec1, svec2 Vec2, svec2 Vec3)
{
    float r = ((Vec1.x * Vec2.y) + (Vec2.x * Vec3.y) + (Vec3.x * Vec1.y) - (Vec1.y * Vec2.x) - (Vec2.y * Vec3.x) - (Vec3.y * Vec1.x)) / 2;
    if (r < 0)
    {
        r = -r;
    }
    return(r);
}

/*********************************************
 *                 VECTOR 3D		     *
 *********************************************/


SYL_INLINE svec3 S_VEC3F(float a, float b, float c)
{
    svec3 r = { { a, b, c } };
    return(r);
}

SYL_INLINE svec3 S_VEC3A(float* a)
{
    svec3 r = { { a[0], a[1], a[2] } };
    return(r);
}

SYL_INLINE void s_vec3_zero(svec3* Vector)
{
    *Vector = _SVEC3_ZERO;
}

SYL_INLINE bool s_vec3_equal(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpeq_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x == Vec2.x) && (Vec1.y == Vec2.y) && (Vec1.z == Vec2.z))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec3_equal_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpeq_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x == Value) && (Vec1.y == Value) && (Vec1.z == Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }

#endif
}

SYL_INLINE bool s_vec3_not_equal(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)

    __m128 Result = _mm_cmpeq_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return (((_mm_movemask_ps(Result) & 7) == 7) == 0);
#else
    if ((Vec1.x != Vec2.x) && (Vec1.y != Vec2.y) && (Vec1.z == Vec2.z))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec3_not_equal_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpneq_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x != Value) && (Vec1.y != Value) && (Vec1.z == Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec3_greater(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)

    __m128 Result = _mm_cmpgt_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x > Vec2.x) && (Vec1.y > Vec1.y) && (Vec1.z > Vec1.z))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec3_less(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmplt_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x < Vec2.x) && (Vec1.y < Vec2.y) && (Vec1.z < Vec2.z))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec3_less_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmplt_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x < Value) && (Vec1.y < Value) && (Vec1.z < Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}


SYL_INLINE bool s_vec3_greater_equal(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpge_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x >= Vec2.x) && (Vec1.y >= Vec2.y) && (Vec1.z > Vec2.z))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec3_greater_equal_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpge_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x >= Value) && (Vec1.y >= Value) && (Vec1.z > Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec3_less_equal(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmple_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x <= Vec2.x) && (Vec1.y <= Vec2.y) && (Vec1.z > Vec2.z))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec3_less_equal_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmple_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return (((_mm_movemask_ps(Result) & 7) == 7) != 0);
#else
    if ((Vec1.x <= Value) && (Vec1.y <= Value) && (Vec1.z > Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE svec3 s_vec3_add(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(_mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec1), _mm_loadl_pi(_mm_setzero_ps(), (__m64*) & Vec2));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Vec1.x + Vec2.x), (Vec1.y + Vec2.y), (Vec1.z + Vec2.z) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3p_add(svec3* Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(_mm_load_ps(Vec1->e), _mm_load_ps(Vec2.e));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x + Vec2.x;
    Vec1->y = Vec1->y + Vec2.y;
    Vec1->z = Vec1->z + Vec2.z;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_vec3_add_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Vec1.x + Value), (Vec1.y + Value), (Vec1.z + Value) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3p_add_scalar(svec3* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(_mm_load_ps(Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x + Value;
    Vec1->y = Vec1->y + Value;
    Vec1->z = Vec1->z + Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_vec3_sub(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Vec1.x - Vec2.x), (Vec1.y - Vec2.y), (Vec1.z - Vec2.z) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3p_sub(svec3* Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_load_ps(Vec1->e), _mm_load_ps(Vec2.e));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x - Vec2.x;
    Vec1->y = Vec1->y - Vec2.y;
    Vec1->z = Vec1->z - Vec2.z;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_vec3_sub_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Vec1.x - Value), (Vec1.y - Value), (Vec1.z - Value) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3p_sub_saclar(svec3* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_load_ps(Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x - Value;
    Vec1->y = Vec1->y - Value;
    Vec1->z = Vec1->z - Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_scalar_sub_vec3(float value, svec3 vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_set1_ps(value), _mm_load_ps(vec1.e));
    return *(svec3*)&r;
#else
    svec3 result = { { (value - vec1.x), (value - vec1.y), (value - vec1.z) } };
    return(result);
#endif
}

SYL_INLINE svec3 s_scalar_sub_vec3p(float value, svec3* vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_set1_ps(value), _mm_load_ps(vec1->e));
    *vec1 = *(svec3*)&r;
    return(*vec1);
#else
    vec1->x = value - vec1->x;
    vec1->y = value - vec1->y;
    vec1->z = value - vec1->z;
    return(*vec1);
#endif
}

SYL_INLINE svec3 s_vec3_mul(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Vec1.x * Vec2.x), (Vec1.y * Vec2.y), (Vec1.z * Vec2.z) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3p_mul(svec3* Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(_mm_load_ps(Vec1->e), _mm_load_ps(Vec2.e));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x * Vec2.x;
    Vec1->y = Vec1->y * Vec2.y;
    Vec1->z = Vec1->z * Vec2.z;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_vec3_mul_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Vec1.x * Value), (Vec1.y * Value),  (Vec1.z * Value) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3p_mul_scalar(svec3* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(_mm_load_ps(Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x * Value;
    Vec1->y = Vec1->y * Value;
    Vec1->z = Vec1->z * Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_vec3_div(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Vec1.x / Vec2.x), (Vec1.y / Vec2.y), (Vec1.z / Vec2.z) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3p_div(svec3* Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_load_ps(Vec1->e), _mm_load_ps(Vec2.e));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x / Vec2.x;
    Vec1->y = Vec1->y / Vec2.y;
    Vec1->z = Vec1->z / Vec2.z;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_vec3_div_scalar(svec3 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Vec1.x / Value), (Vec1.y / Value), (Vec1.z / Value) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3p_div_scalar(svec3* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_load_ps(Vec1->e), _mm_set1_ps(Value));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x / Value;
    Vec1->y = Vec1->y / Value;
    Vec1->z = Vec1->z / Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_scalar_div_vec3(float Value, svec3 Vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_set1_ps(Value), _mm_load_ps(Vec1.e));
    return *(svec3*)&r;
#else
    svec3 Result = { { (Value / Vec1.x), (Value / Vec1.y), (Value / Vec1.z) } };
    return(Result);
#endif
}

SYL_INLINE svec3 s_scalar_div_vec3p(float Value, svec3* Vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(_mm_set1_ps(Value), _mm_load_ps(Vec1->e));
    *Vec1 = *(svec3*)&r;
    return(*Vec1);
#else
    Vec1->x = Value / Vec1->x;
    Vec1->y = Value / Vec1->y;
    Vec1->z = Value / Vec1->z;
    return(*Vec1);
#endif
}

SYL_INLINE svec3 s_vec3_floor(svec3 A)
{
    svec3 Result = { { s_floorf(A.x), s_floorf(A.y), s_floorf(A.z) } };
    return(Result);
}

/* Round all components of the vec3 to nearest integer*/
SYL_INLINE svec3 s_vec3_round(svec3 A)
{
    svec3 Result = { { s_roundf(A.x), s_roundf(A.y), s_roundf(A.z) } };
    return(Result);
}

/* Negate all components of the vector */
SYL_INLINE svec3 s_vec3_negate(svec3 a)
{
    svec3 r = { { -a.x, -a.y, -a.z } };
    return(r);
}

SYL_INLINE float s_vec3_dot(svec3 Vec1, svec3 Vec2)
{
    return((Vec1.x * Vec2.x) + (Vec1.y * Vec2.y) + (Vec1.z * Vec2.z));
}

SYL_INLINE svec3 s_vec3_hadamard(svec3 Vec1, svec3 Vec2)
{
    svec3 Result = { { (Vec1.x * Vec2.x), (Vec1.y * Vec2.y), (Vec1.z * Vec1.z) } };
    return(Result);
}

SYL_INLINE float s_vec3_length(svec3 Vec1)
{
    return(sqrt((Vec1.x * Vec1.x) + (Vec1.y * Vec1.y) + (Vec1.z * Vec1.z)));
}

SYL_INLINE float s_vec3_distance(svec3 Vec1, svec3 Vec2)
{
    return s_vec3_length(s_vec3_sub(Vec1, Vec2));
}

SYL_INLINE svec3 s_vec3_normalize(svec3 a)
{
    return(s_vec3_mul_scalar(a, (1.0f / s_vec3_length(a))));
}

/* Return the biggest element inside vec4 */
SYL_INLINE float s_vec3_max_value(svec3 A)
{
    /* NOTE(xcatalyst): Current SSE4 version doesn't work. It is removed for now. Reimplement it later.*/
    if (A.e[0] >= A.e[1] && A.e[0] >= A.e[2])
        return(A.e[0]);
    if (A.e[1] >= A.e[0] && A.e[1] >= A.e[2])
        return(A.e[1]);
    if (A.e[2] >= A.e[0] && A.e[2] >= A.e[1])
        return(A.e[2]);

    return(A.e[0]); // HMMMMMMMMMMMMMMMMMMMM
}

/* Return the smollest element inside vec4 */
SYL_INLINE float s_vec3_min_value(svec3 A)
{
    /* NOTE(xcatalyst): Current SSE4 version doesn't work. It is removed for now. Reimplement it later.*/
    if (A.e[0] <= A.e[1] && A.e[0] <= A.e[2])
        return(A.e[0]);
    if (A.e[1] <= A.e[0] && A.e[1] <= A.e[2])
        return(A.e[1]);
    if (A.e[2] <= A.e[0] && A.e[2] <= A.e[1])
        return(A.e[2]);

    return(A.e[0]); // HMMMMMMMMMMMMMMMMMMMM
}

/* Per component comparsion to return a vector containing the largest components */
SYL_INLINE svec3 s_vec3_max_vector(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_max_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return *(svec3*)&r;
#else
    svec3 Result;
    if (Vec1.x > Vec2.x)
    {
        Result.x = Vec1.x;
    }
    else
    {
        Result.x = Vec2.x;
    }

    if (Vec1.y > Vec2.y)
    {
        Result.y = Vec1.y;
    }
    else
    {
        Result.y = Vec2.y;
    }

    if (Vec1.z > Vec2.z)
    {
        Result.z = Vec1.z;
    }
    else
    {
        Result.z = Vec2.z;
    }

    return(Result);
#endif
}

/* Per component comparsion to return a vector containing the smollest components */
SYL_INLINE svec3 s_vec3_min_vector(svec3 Vec1, svec3 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_min_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return *(svec3*)&r;
#else
    svec3 Result;
    if (Vec1.x < Vec2.x)
    {
        Result.x = Vec1.x;
    }
    else
    {
        Result.x = Vec2.x;
    }

    if (Vec1.y < Vec2.y)
    {
        Result.y = Vec1.y;
    }
    else
    {
        Result.y = Vec2.y;
    }

    if (Vec1.z < Vec2.z)
    {
        Result.z = Vec1.z;
    }
    else
    {
        Result.z = Vec2.z;
    }

    return(Result);
#endif
}

SYL_INLINE svec3 s_vec3_clamp(svec3 Value, svec3 Min, svec3 Max)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_max_ps(_mm_setr_ps(Min.x, Min.y, Min.z, 0.0f), _mm_setr_ps(Value.x, Value.y, Value.z, 0.0f));
    Result = _mm_min_ps(_mm_setr_ps(Max.x, Max.y, Max.z, 0.0f), Result);
    return *(svec3*)&Result;
#else
    svec3 Result = s_vec3_min_vector(s_vec3_max_vector(Value, Min), Max);
    return(Result);
#endif
}

/* TODO(xcatalyst): Faster ! */
SYL_INLINE svec3 s_vec3_lerp(svec3 Vec1, svec3 Vec2, float t)
{
    svec3 Result = { { Vec1.e[0] + ((Vec2.e[0] - Vec1.e[0]) * t),
             Vec1.e[1] + ((Vec2.e[1] - Vec1.e[1]) * t),
             Vec1.e[2] + ((Vec2.e[2] - Vec1.e[2]) * t) } };
    return(Result);

}

SYL_INLINE svec3 s_vec3_project(svec3 VectorToProject, svec3 ProjectionVector)
{
    float scale = s_vec3_dot(ProjectionVector, VectorToProject) / s_vec3_dot(ProjectionVector, ProjectionVector);
    return(s_vec3_mul_scalar(ProjectionVector, scale));
}

SYL_INLINE svec3 s_vec3_cross(svec3 Vec1, svec3 Vec2)
{
#if defined(SYL_DEBUG)
    svec3 Result;
    Result.x = Vec1.y * Vec2.z - Vec1.z * Vec2.y;
    Result.y = Vec1.z * Vec2.x - Vec1.x * Vec2.z;
    Result.z = Vec1.x * Vec2.y - Vec1.y * Vec2.x;
    return(Result);
#else
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 V1 = _mm_load_ps(Vec1.e);
    __m128 V2 = _mm_load_ps(Vec2.e);
    __m128 A1 = _SYL_PERMUTE_PS(V1, _SYL_SHUFFLE(3, 0, 2, 1));
    __m128 A2 = _SYL_PERMUTE_PS(V2, _SYL_SHUFFLE(3, 1, 0, 2));
    __m128 r = _mm_mul_ps(A1, A2);
    A1 = _SYL_PERMUTE_PS(A1, _MM_SHUFFLE(3, 0, 2, 1));
    A2 = _SYL_PERMUTE_PS(A2, _MM_SHUFFLE(3, 1, 0, 2));
    r = _SYL_ADD_PS(A1, A2, r);
    __m128 re = _mm_and_ps(r, _S_XMM_MASK_3);
    return *(svec3*)&re;
#else
    svec3 Result = { { Vec1.e[1] * Vec2.e[2] - Vec1.e[2] * Vec2.e[1],
             Vec1.e[2] * Vec2.e[0] - Vec1.e[0] * Vec2.e[2],
             Vec1.e[0] * Vec2.e[1] - Vec1.e[1] * Vec2.e[0] } };
    return(Result);
#endif
#endif
}

/* TODO(xcatalyst): Typed this out of my mind, might be incorrect
   check the function later. Currently undocumented. */
SYL_INLINE float Slope(svec3 PointA, svec3 PointB)
{
    float Adjacent = sqrt(s_square(PointA.z - PointB.z) + s_square(PointA.x - PointB.x));
    float Opposite = s_abs(PointA.y - PointB.y);
    return(tanf(Opposite / Adjacent));
}

//SYL_INLINE float AreaOfTriangle(vec3 a, vec3 b, vec3 c)
//{
//	vec3 Vector = a - b;
//	float Area = sqrt(Dot(Vector, Vector));
//	vec3 Side = c - b;
//	float f = Dot(Vector, Side);
//	Side -= Vector * f;
//	Area *= sqrtf(Dot(Side, Side));
//	return(Area);
//}

/*********************************************
 *                   VECTOR 4D		         *
 *********************************************/

SYL_INLINE svec4 S_VEC4F(float a, float b, float c, float d)
{
    svec4 r = { { a, b, c, d } };
    return(r);
}

SYL_INLINE svec4 S_VEC4A(float* a)
{
    svec4 r = { { a[0], a[1], a[2], a[3] } };
    return(r);
}

SYL_INLINE svec4 S_VEC4VF(svec3 Vector, float Value)
{
    svec4 r = { { Vector.x, Vector.y, Vector.z, Value } };
    return(r);
}

SYL_INLINE void s_vector4_zero(svec4* Vector)
{
#if defined(SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    _mm_store_ps(Vector->e, _S_XMM_ZERO);
#else
    * Vector = _SVEC4_ZERO;
#endif
}

SYL_INLINE bool s_vec4_equal(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpeq_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x == Vec2.x) && (Vec1.y == Vec2.y) && (Vec1.z == Vec2.z))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec4_equal_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpeq_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x == Value) && (Vec1.y == Value) && (Vec1.z == Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec4_not_equal(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpneq_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x != Vec2.x) && (Vec1.y != Vec2.y) && (Vec1.z != Vec2.z))
    {
        return(true);
    }
    else
    {
        return(false);
    }

#endif
}

SYL_INLINE bool s_vec4_not_equal_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpneq_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x != Value) && (Vec1.y != Value) && (Vec1.z != Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec4_greater(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpgt_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x > Vec2.x) && (Vec1.y > Vec2.y) && (Vec1.z > Vec2.y))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec4_less(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmplt_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x < Vec2.x) && (Vec1.y < Vec2.y) && (Vec1.z < Vec2.y))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec4_less_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmplt_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x < Value) && (Vec1.y < Value) && (Vec1.z < Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}


SYL_INLINE bool s_vec4_greater_equal(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpge_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x >= Vec2.x) && (Vec1.y >= Vec2.y) && (Vec1.z >= Vec2.y))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec4_greater_equal_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmpge_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x >= Value) && (Vec1.y >= Value) && (Vec1.z >= Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE bool s_vec4_less_equal(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmple_ps(_mm_load_ps(Vec1.e), _mm_load_ps(Vec2.e));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x <= Vec2.x) && (Vec1.y <= Vec2.y) && (Vec1.z <= Vec2.y))
    {
        return(true);
    }
    else
    {
        return(false);
    }

#endif
}

SYL_INLINE bool s_vec4_less_equal_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_cmple_ps(_mm_load_ps(Vec1.e), _mm_set1_ps(Value));
    return ((_mm_movemask_ps(Result) == 0x0f) != 0);
#else
    if ((Vec1.x <= Value) && (Vec1.y <= Value) && (Vec1.z <= Value))
    {
        return(true);
    }
    else
    {
        return(false);
    }
#endif
}

SYL_INLINE svec4 s_vec4_add(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(Vec1.v, Vec2.v);
    return *(svec4*)&r;
#else
    svec4 Result = { { (Vec1.x + Vec2.x), (Vec1.y + Vec2.y), (Vec1.z + Vec2.z), (Vec1.w + Vec2.w) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4p_add(svec4* Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(Vec1->v, Vec2.v);
    *Vec1 = *(svec4*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x + Vec2.x;
    Vec1->y = Vec1->y + Vec2.y;
    Vec1->z = Vec1->z + Vec2.z;
    Vec1->w = Vec1->w + Vec2.w;
    return(*Vec1);
#endif
}

SYL_INLINE svec4 s_vec4_add_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(Vec1.v, _mm_set1_ps(Value));
    return *(svec4*)&r;
#else
    svec4 Result = { { (Vec1.x + Value), (Vec1.y + Value), (Vec1.z + Value), (Vec1.w + Value) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4p_add_scalar(svec4* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_add_ps(Vec1->v, _mm_set1_ps(Value));
    *Vec1 = *(svec4*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x + Value;
    Vec1->y = Vec1->y + Value;
    Vec1->z = Vec1->z + Value;
    Vec1->w = Vec1->w + Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec4 s_vec4_sub(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(Vec1.v, Vec2.v);
    return *(svec4*)&r;
#else
    svec4 Result = { { (Vec1.x - Vec2.x), (Vec1.y - Vec2.y), (Vec1.z - Vec2.z), (Vec1.w - Vec2.w) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4p_sub(svec4* Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(Vec1->v, Vec2.v);
    *Vec1 = *(svec4*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x - Vec2.x;
    Vec1->y = Vec1->y - Vec2.y;
    Vec1->z = Vec1->z - Vec2.z;
    Vec1->w = Vec1->w - Vec2.w;
    return(*Vec1);
#endif
}

SYL_INLINE svec4 s_vec4_sub_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(Vec1.v, _mm_set1_ps(Value));
    return *(svec4*)&r;
#else
    svec4 Result = { { (Vec1.x - Value), (Vec1.y - Value), (Vec1.z - Value), (Vec1.w - Value) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4p_sub_scalar(svec4* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(Vec1->v, _mm_set1_ps(Value));
    *Vec1 = *(svec4*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x - Value;
    Vec1->y = Vec1->y - Value;
    Vec1->z = Vec1->z - Value;
    Vec1->w = Vec1->w - Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec4 s_scalar_vec4_sub(float value, svec4 Vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_set1_ps(value), Vec1.v);
    return *(svec4*)&r;
#else
    svec4 Result = { { (value - Vec1.x), (value - Vec1.y), (value - Vec1.z), (value - Vec1.w) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_scalar_sub_vec4p(float value, svec4* Vec1)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_sub_ps(_mm_set1_ps(value), Vec1->v);
    *Vec1 = *(svec4*)&r;
    return(*Vec1);
#else
    Vec1->x = value - Vec1->x;
    Vec1->y = value - Vec1->y;
    Vec1->z = value - Vec1->z;
    Vec1->w = value - Vec1->w;
    return(*Vec1);
#endif
}


SYL_INLINE svec4 s_vec4_mul(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(Vec1.v, _mm_load_ps(Vec2.e));
    return *(svec4*)&r;
#else    
    svec4 Result = { { (Vec1.x - Vec2.x), (Vec1.y - Vec2.y), (Vec1.z - Vec2.z), (Vec1.w - Vec2.w) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4p_mul(svec4* Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(Vec1->v, _mm_load_ps(Vec2.e));
    *Vec1 = *(svec4*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x * Vec2.x;
    Vec1->y = Vec1->y * Vec2.y;
    Vec1->z = Vec1->z * Vec2.z;
    Vec1->w = Vec1->w * Vec2.w;
    return(*Vec1);
#endif
}

SYL_INLINE svec4 s_vec4_mul_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(Vec1.v, _mm_set1_ps(Value));
    return *(svec4*)&r;
#else
    svec4 Result = { { (Vec1.x * Value), (Vec1.y * Value), (Vec1.z * Value), (Vec1.w * Value) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4p_mul_scalar(svec4* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_mul_ps(Vec1->v, _mm_set1_ps(Value));
    *Vec1 = *(svec4*)&r;
    return(*Vec1);
#else
    Vec1->x = Vec1->x * Value;
    Vec1->y = Vec1->y * Value;
    Vec1->z = Vec1->z * Value;
    Vec1->w = Vec1->w * Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec4 s_vec4_div(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 r = _mm_div_ps(Vec1.v, Vec2.v);
    return *(svec4*)&r;
#else
    svec4 Result = { { (Vec1.x / Vec2.x), (Vec1.y / Vec2.y), (Vec1.z / Vec2.z), (Vec1.w / Vec2.w) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4p_div(svec4* Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_div_ps(Vec1->v, Vec2.v);
    *Vec1 = *(svec4*)&Result;
    return(*Vec1);
#else
    Vec1->x = Vec1->x / Vec2.x;
    Vec1->y = Vec1->y / Vec2.y;
    Vec1->z = Vec1->z / Vec2.z;
    Vec1->w = Vec1->w / Vec2.w;
    return(*Vec1);
#endif
}

SYL_INLINE svec4 s_vec4_div_scalar(svec4 Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_div_ps(Vec1.v, _mm_set1_ps(Value));
    return *(svec4*)&Result;
#else
    svec4 Result = { { (Vec1.x / Value), (Vec1.y / Value), (Vec1.z / Value), (Vec1.w / Value) } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4p_div_scalar(svec4* Vec1, float Value)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_div_ps(Vec1->v, _mm_set1_ps(Value));
    *Vec1 = *(svec4*)&Result;
    return(*Vec1);
#else
    Vec1->x = Vec1->x / Value;
    Vec1->y = Vec1->y / Value;
    Vec1->z = Vec1->z / Value;
    Vec1->w = Vec1->w / Value;
    return(*Vec1);
#endif
}

SYL_INLINE svec4 s_vec4_floor(svec4 A)
{
    svec4 Result = { { s_floorf(A.x), s_floorf(A.y), s_floorf(A.z), s_floorf(A.w) } };
    return(Result);
}

/* Round all components of the vec3 to nearest integer*/
SYL_INLINE svec4 s_vec4_round(svec4 A)
{
    svec4 Result = { { s_roundf(A.x), s_roundf(A.y), s_roundf(A.z), s_roundf(A.w) } };
    return(Result);
}

/* Negate all components of the vector */
SYL_INLINE svec4 s_vec4_negate(svec4 a)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_sub_ps(_mm_set1_ps(0), a.v);
    return *(svec4*)&Result;
#else
    svec4 Result = { { -a.x, -a.y, -a.z, -a.w } };
    return(Result);
#endif
}

SYL_INLINE float s_vec4_dot(svec4 Vec1, svec4 Vec2)
{
#if defined(SYL_DEBUG)
    return((Vec1.x * Vec2.x) + (Vec1.y * Vec2.y) + (Vec1.z * Vec2.z) + (Vec1.w * Vec2.w));
#else
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_dp_ps(Vec1.v, Vec2.v, 0xFF);
    return(*(float*)&Result);
#else
    return (Vec1.e[0] * Vec2.e[0]) + (Vec1.e[1] * Vec2.e[1]) + (Vec1.e[2] * Vec2.e[2]) + (Vec1.e[3] * Vec2.e[3]);
#endif
#endif
}

SYL_INLINE svec4 s_vec4_hadamard(svec4 Vec1, svec4 Vec2)
{
    svec4 Result = { { (Vec1.x * Vec2.x), (Vec1.y * Vec2.y), (Vec1.z * Vec2.z), (Vec1.w * Vec2.w) } };
    return(Result);
}

SYL_INLINE float s_vec4_length(svec4 Vec1)
{
#if defined(SYL_DEBUG)
    return(sqrt((Vec1.x * Vec1.x) + (Vec1.y * Vec1.y) + (Vec1.z * Vec1.z) + (Vec1.w * Vec1.w)));
#else
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 V = _mm_load_ps(Vec1.e);
    __m128 A1 = _mm_mul_ps(V, V);
    __m128 A2 = _mm_hadd_ps(A1, A1);
    __m128 A3 = _mm_hadd_ps(A2, A2);
    return sqrtf(_mm_cvtss_f32(A3));
#else
    return(sqrt((Vec1.x * Vec1.x) + (Vec1.y * Vec1.y) + (Vec1.z * Vec1.z) + (Vec1.w * Vec1.w)));
#endif
#endif
}

SYL_INLINE float s_vec4_distance(svec4 Vec1, svec4 Vec2)
{
    return s_vec4_length(s_vec4_sub(Vec1, Vec2));
}

SYL_INLINE svec4 s_vec4_normalize(svec4 a)
{
    return(s_vec4_mul_scalar(a, (1.0f / s_vec4_length(a))));
}

SYL_INLINE svec4 s_vec4_lerp(svec4 Vec1, svec4 Vec2, float t)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_add_ps(Vec1.v, _mm_mul_ps((_mm_sub_ps(Vec2.v, Vec1.v)), _mm_set1_ps(t)));
    return(*(svec4*)&Result);
#else
    svec4 Result = s_vec4_add(Vec1, (s_vec4_mul_scalar(s_vec4_sub(Vec2, Vec1), t)));
    // vec4 Result = Vec1 + ((Vec2 - Vec1) * t);
    return(Result);
#endif
}

SYL_STATIC svec4 s_vec4_cross(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 a1 = _SYL_PERMUTE_PS(Vec1.v, _SYL_SHUFFLE(3, 0, 2, 1));
    __m128 a2 = _SYL_PERMUTE_PS(Vec2.v, _SYL_SHUFFLE(3, 1, 0, 2));
    __m128 r = _mm_mul_ps(a1, a2);
    a1 = _SYL_PERMUTE_PS(a1, _SYL_SHUFFLE(3, 0, 2, 1));
    a2 = _SYL_PERMUTE_PS(a2, _SYL_SHUFFLE(3, 1, 0, 2));
    r = _SYL_ADD_PS(a1, a2, r);
    __m128 re = _mm_and_ps(r, _S_XMM_MASK_3);
    return *(svec4*)&re;
#else
    svec4 Result = { { Vec1.e[1] * Vec2.e[2] - Vec1.e[2] * Vec2.e[1],
             Vec1.e[2] * Vec2.e[0] - Vec1.e[0] * Vec2.e[2],
             Vec1.e[0] * Vec2.e[1] - Vec1.e[1] * Vec2.e[0], 0.0f } };
    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4_project(svec4 VectorToProject, svec4 ProjectionVector)
{
    float scale = s_vec4_dot(ProjectionVector, VectorToProject) / s_vec4_dot(ProjectionVector, ProjectionVector);
    return(s_vec4_mul_scalar(ProjectionVector, scale));
}

/* Per component comparsion to return a vector containing the largest components */
SYL_INLINE svec4 s_vec4_max_vector(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_max_ps(Vec1.v, Vec2.v);
    return *(svec4*)&Result;
#else
    svec4 Result;
    if (Vec1.x > Vec2.x)
    {
        Result.x = Vec1.x;
    }
    else
    {
        Result.x = Vec2.x;
    }

    if (Vec1.y > Vec2.y)
    {
        Result.y = Vec1.y;
    }
    else
    {
        Result.y = Vec2.y;
    }

    if (Vec1.z > Vec2.z)
    {
        Result.z = Vec1.z;
    }
    else
    {
        Result.z = Vec2.z;
    }

    if (Vec1.w > Vec2.w)
    {
        Result.w = Vec1.w;
    }
    else
    {
        Result.w = Vec2.w;
    }
    return(Result);
#endif
}

/* Per component comparsion to return a vector containing the smollest components */
SYL_INLINE svec4 s_vec4_min_vector(svec4 Vec1, svec4 Vec2)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Result = _mm_min_ps(Vec1.v, Vec2.v);
    return *(svec4*)&Result;
#else
    svec4 Result;
    if (Vec1.x < Vec2.x)
    {
        Result.x = Vec1.x;
    }
    else
    {
        Result.x = Vec2.x;
    }

    if (Vec1.y < Vec2.y)
    {
        Result.y = Vec1.y;
    }
    else
    {
        Result.y = Vec2.y;
    }

    if (Vec1.z < Vec2.z)
    {
        Result.z = Vec1.z;
    }
    else
    {
        Result.z = Vec2.z;
    }

    if (Vec1.w < Vec2.w)
    {
        Result.w = Vec1.w;
    }
    else
    {
        Result.w = Vec2.w;
    }

    return(Result);
#endif
}

SYL_INLINE svec4 s_vec4_clamp(svec4 Value, svec4 Min, svec4 Max)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    // __m128 Result = _mm_max_ps(Min.v, Max.v);
    // Result = _mm_max_ps(Max.v, Result);
    // return *(svec4*)&Result;

    __m128 Result = _mm_min_ps(_mm_max_ps(Value.v, Min.v), Max.v);
    return *(svec4*)&Result;
#else
    svec4 Result = s_vec4_min_vector(s_vec4_max_vector(Value, Min), Max);
    return(Result);
#endif
}

/* Return the biggest element inside vec4 */
SYL_INLINE float s_vec4_max(svec4 A)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Value = _mm_load_ps(A.e);
    __m128 r = _mm_max_ps(Value, Value);
    return *(float*)&r;
#else
    /* TODO(xcatalyst): Implement ! */
    return(A.e[0]);
#endif
}

/* Return the smollest element inside vec4 */
SYL_INLINE float s_vec4_min(svec4 A)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Value = _mm_load_ps(A.e);
    __m128 r = _mm_min_ps(Value, Value);
    return *(float*)&r;
#else
    /* TODO(xcatalyst): Implement ! */
    return(A.e[0]);
#endif
}

SYL_INLINE float s_vec4_sum(svec4 Vec1)
{
    return((Vec1.x + Vec1.y) + (Vec1.z + Vec1.w));
}

/*********************************************
 *                 MATRIX 4X4                 *
 *********************************************/

SYL_INLINE smat4 MAT4V(float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    smat4 R;
    R.v[0] = _mm_set_ps(m00, m01, m02, m03);
    R.v[1] = _mm_set_ps(m10, m11, m12, m13);
    R.v[2] = _mm_set_ps(m20, m21, m22, m23);
    R.v[3] = _mm_set_ps(m30, m31, m32, m33);
    return(R);
#else
    smat4 Result = { { m00, m01, m02, m03,
             m10, m11, m12, m13,
             m20, m21, m22, m23,
             m30, m31, m32, m33 } };
    return(Result);
#endif
}

SYL_INLINE smat4 MAT4A(float* a)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    smat4 R;
    R.v[0] = _mm_set_ps(a[0], a[1], a[2], a[3]);
    R.v[1] = _mm_set_ps(a[4], a[5], a[6], a[7]);
    R.v[2] = _mm_set_ps(a[8], a[9], a[10], a[11]);
    R.v[3] = _mm_set_ps(a[12], a[13], a[14], a[15]);
    return(R);
#else
    smat4 Result = { { a[0], a[1], a[2], a[3],
             a[4], a[5], a[6], a[7],
             a[8], a[9], a[10], a[11],
             a[12], a[13], a[14], a[15] } };
    return(Result);
#endif
}

SYL_INLINE void s_mat4_zero(smat4* Matrix)
{
#if defined(SYL_ENABLE_AVX)
    _mm256_store_ps(Matrix.e, _S_YMM_ZERO);
    _mm256_store_ps(Matrix.e + 8, _S_YMM_ZERO);
#elif defined(SYL_ENABLE_SSE4)
    _mm_store_ps(Matrix->e, _S_XMM_ZERO);
    _mm_store_ps(Matrix->e + 4, _S_XMM_ZERO);
    _mm_store_ps(Matrix->e + 8, _S_XMM_ZERO);
    _mm_store_ps(Matrix->e + 12, _S_XMM_ZERO);
#else
    * Matrix = _S_ZERO4X4;
#endif
}

SYL_INLINE smat4 s_mat4_identity()
{
    return(_S_IDENT4X4);
}

SYL_STATIC bool s_mat4_is_identity(smat4 Mat)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Temp1 = _mm_cmpeq_ps(Mat.v[0], _S_IDENT4x4R0);
    __m128 Temp2 = _mm_cmpeq_ps(Mat.v[1], _S_IDENT4x4R1);
    __m128 Temp3 = _mm_cmpeq_ps(Mat.v[2], _S_IDENT4x4R2);
    __m128 Temp4 = _mm_cmpeq_ps(Mat.v[3], _S_IDENT4x4R3);
    Temp1 = _mm_and_ps(Temp1, Temp2);
    Temp3 = _mm_and_ps(Temp3, Temp4);
    Temp1 = _mm_and_ps(Temp1, Temp3);
    return(_mm_movemask_ps(Temp1) == 0x0f);
#else
    /* Oh god don't use this on non intrinsic version... */
    /* 1278421 cycles incoming...  */
    if (Mat.e[0] == 1.0f && Mat.e[1] == 0.0f && Mat.e[2] == 0.0f && Mat.e[3] == 0.0f &&
        Mat.e[4] == 0.0f && Mat.e[5] == 1.0f && Mat.e[6] == 0.0f && Mat.e[7] == 0.0f &&
        Mat.e[8] == 0.0f && Mat.e[9] == 0.0f && Mat.e[10] == 1.0f && Mat.e[11] == 0.0f &&
        Mat.e[12] == 0.0f && Mat.e[13] == 0.0f && Mat.e[14] == 0.0f && Mat.e[15] == 1.0f)
    {
        /* Nice you spend couple of frames calculating this... */
        return(true);
    }
    else
    {
        /* Maybe even failed what a shame.. */
        return(false);
    }
#endif
}

/* Multiply two 4x4 Matricies */
SYL_STATIC smat4 s_mat4_mul(smat4 Matrix1, smat4 Matrix2)
{
#if defined(SYL_ENABLE_AVX)
    mat4 Result;
    __m256 Temp0 = _mm256_castps128_ps256(Matrix1.v[0]);
    Temp0 = _mm256_insertf128_ps(Temp0, Matrix1.v[1], 1);

    __m256 Temp1 = _mm256_castps128_ps256(Matrix1.v[2]);
    Temp1 = _mm256_insertf128_ps(Temp1, Matrix1.v[3], 1);

    __m256 Temp3 = _mm256_castps128_ps256(Matrix2.v[0]);
    Temp3 = _mm256_insertf128_ps(Temp3, Matrix2.v[1], 1);

    __m256 Temp4 = _mm256_castps128_ps256(Matrix2.v[2]);
    Temp4 = _mm256_insertf128_ps(Temp4, Matrix2.v[3], 1);

    __m256 CXA0 = _mm256_shuffle_ps(Temp0, Temp0, _SYL_SHUFFLE(0, 0, 0, 0));
    __m256 CXA1 = _mm256_shuffle_ps(Temp1, Temp1, _SYL_SHUFFLE(0, 0, 0, 0));

    __m256 CXB1 = _mm256_permute2f128_ps(Temp3, Temp3, 0x00);

    __m256 CXC0 = _mm256_mul_ps(CXA0, CXB1);
    __m256 CXC1 = _mm256_mul_ps(CXA1, CXB1);

    CXA0 = _mm256_shuffle_ps(Temp0, Temp0, _SYL_SHUFFLE(1, 1, 1, 1));
    CXA1 = _mm256_shuffle_ps(Temp1, Temp1, _SYL_SHUFFLE(1, 1, 1, 1));

    CXB1 = _mm256_permute2f128_ps(Temp3, Temp3, 0x11);

    __m256 CXC2 = _mm256_fmadd_ps(CXA0, CXB1, CXC0);
    __m256 CXC3 = _mm256_fmadd_ps(CXA1, CXB1, CXC1);

    CXA0 = _mm256_shuffle_ps(Temp0, Temp0, _SYL_SHUFFLE(2, 2, 2, 2));
    CXA1 = _mm256_shuffle_ps(Temp1, Temp1, _SYL_SHUFFLE(2, 2, 2, 2));

    __m256 CXCL1 = _mm256_permute2f128_ps(Temp4, Temp4, 0x00);

    __m256 CXC4 = _mm256_mul_ps(CXA0, CXCL1);
    __m256 CXC5 = _mm256_mul_ps(CXA1, CXCL1);

    CXA0 = _mm256_shuffle_ps(Temp0, Temp0, _SYL_SHUFFLE(3, 3, 3, 3));
    CXA1 = _mm256_shuffle_ps(Temp1, Temp1, _SYL_SHUFFLE(3, 3, 3, 3));

    CXCL1 = _mm256_permute2f128_ps(Temp4, Temp4, 0x11);

    __m256 RXR0 = _mm256_fmadd_ps(CXA0, CXCL1, CXC4);
    __m256 RXR1 = _mm256_fmadd_ps(CXA1, CXCL1, CXC5);

    Temp0 = _mm256_add_ps(CXC2, RXR0);
    Temp1 = _mm256_add_ps(CXC3, RXR1);

    Result.v[0] = _mm256_castps256_ps128(Temp0);
    Result.v[1] = _mm256_extractf128_ps(Temp0, 1);

    Result.v[2] = _mm256_castps256_ps128(Temp1);
    Result.v[3] = _mm256_extractf128_ps(Temp1, 1);

    return(Result);
#elif defined(SYL_ENABLE_SSE4)

    smat4 Result = { 0 };

    __m128 vW = Matrix1.v[0];
    __m128 vX = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(0, 0, 0, 0));
    __m128 vY = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(1, 1, 1, 1));
    __m128 vZ = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(2, 2, 2, 2));

    vW = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(3, 3, 3, 3));
    vX = _mm_mul_ps(vX, Matrix2.v[0]);
    vY = _mm_mul_ps(vY, Matrix2.v[1]);
    vZ = _mm_mul_ps(vZ, Matrix2.v[2]);
    vW = _mm_mul_ps(vW, Matrix2.v[3]);
    vX = _mm_add_ps(vX, vZ);
    vY = _mm_add_ps(vY, vW);
    vX = _mm_add_ps(vX, vY);
    Result.v[0] = vX;

    vW = Matrix1.v[1];
    vX = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(0, 0, 0, 0));
    vY = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(1, 1, 1, 1));
    vZ = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(2, 2, 2, 2));
    vW = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(3, 3, 3, 3));

    vX = _mm_mul_ps(vX, Matrix2.v[0]);
    vY = _mm_mul_ps(vY, Matrix2.v[1]);
    vZ = _mm_mul_ps(vZ, Matrix2.v[2]);
    vW = _mm_mul_ps(vW, Matrix2.v[3]);
    vX = _mm_add_ps(vX, vZ);
    vY = _mm_add_ps(vY, vW);
    vX = _mm_add_ps(vX, vY);

    Result.v[1] = vX;

    vW = Matrix1.v[2];
    vX = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(0, 0, 0, 0));
    vY = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(1, 1, 1, 1));
    vZ = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(2, 2, 2, 2));
    vW = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(3, 3, 3, 3));

    vX = _mm_mul_ps(vX, Matrix2.v[0]);
    vY = _mm_mul_ps(vY, Matrix2.v[1]);
    vZ = _mm_mul_ps(vZ, Matrix2.v[2]);
    vW = _mm_mul_ps(vW, Matrix2.v[3]);
    vX = _mm_add_ps(vX, vZ);
    vY = _mm_add_ps(vY, vW);
    vX = _mm_add_ps(vX, vY);

    Result.v[2] = vX;

    vW = Matrix1.v[3];
    vX = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(0, 0, 0, 0));
    vY = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(1, 1, 1, 1));
    vZ = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(2, 2, 2, 2));
    vW = _SYL_PERMUTE_PS(vW, _SYL_SHUFFLE(3, 3, 3, 3));

    vX = _mm_mul_ps(vX, Matrix2.v[0]);
    vY = _mm_mul_ps(vY, Matrix2.v[1]);
    vZ = _mm_mul_ps(vZ, Matrix2.v[2]);
    vW = _mm_mul_ps(vW, Matrix2.v[3]);

    vX = _mm_add_ps(vX, vZ);
    vY = _mm_add_ps(vY, vW);
    vX = _mm_add_ps(vX, vY);

    Result.v[3] = vX;

    return(Result);
#else
    smat4 Result;
    s_mat4_zero(&Result);

    for (int k = 0; k < 4; ++k)
    {
        for (int n = 0; n < 4; ++n)
        {
            for (int i = 0; i < 4; ++i)
            {
                Result.e2[k][n] += Matrix1.e2[k][i] * Matrix2.e2[i][n];
            }
        }
    }

    return(Result);
#endif
}

SYL_STATIC smat4 s_mat4_transpose(smat4 Mat)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 Temp1 = _mm_shuffle_ps(Mat.v[0], Mat.v[1], _SYL_SHUFFLE(1, 0, 1, 0));
    __m128 Temp2 = _mm_shuffle_ps(Mat.v[0], Mat.v[1], _SYL_SHUFFLE(3, 2, 3, 2));
    __m128 Temp3 = _mm_shuffle_ps(Mat.v[2], Mat.v[3], _SYL_SHUFFLE(1, 0, 1, 0));
    __m128 Temp4 = _mm_shuffle_ps(Mat.v[2], Mat.v[3], _SYL_SHUFFLE(3, 2, 3, 2));
    smat4 R;
    R.v[0] = _mm_shuffle_ps(Temp1, Temp2, _SYL_SHUFFLE(2, 0, 2, 0));
    R.v[1] = _mm_shuffle_ps(Temp1, Temp2, _SYL_SHUFFLE(3, 1, 3, 1));
    R.v[2] = _mm_shuffle_ps(Temp3, Temp4, _SYL_SHUFFLE(2, 0, 2, 0));
    R.v[3] = _mm_shuffle_ps(Temp3, Temp4, _SYL_SHUFFLE(3, 1, 3, 1));
    return(R);
#else
    smat4 Result;

    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            Result.e2[j][i] = Mat.e2[i][j];
        }
    }
    return(Result);
#endif
}

/* Inverse of a matrix but in the scale of this matrix should be 1 */
SYL_STATIC smat4 s_mat4_inverse_noscale(smat4 Matrix)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)

    __m128 Temp0 = _SYL_VEC_SHUFFLE_0101(Matrix.v[0], Matrix.v[1]); // 00, 01, 10, 11
    __m128 Temp1 = _SYL_VEC_SHUFFLE_2323(Matrix.v[0], Matrix.v[1]); // 02, 03, 12, 13

    smat4 Result;
    Result.v[0] = _SYL_VEC_SHUFFLE(Temp0, Matrix.v[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
    Result.v[1] = _SYL_VEC_SHUFFLE(Temp0, Matrix.v[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
    Result.v[2] = _SYL_VEC_SHUFFLE(Temp1, Matrix.v[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

    Result.v[3] = _mm_mul_ps(Result.v[0], _SYL_VEC_SWIZZLE1(Matrix.v[3], 0));
    Result.v[3] = _mm_add_ps(Result.v[3], _mm_mul_ps(Result.v[1], _SYL_VEC_SWIZZLE1(Matrix.v[3], 1)));
    Result.v[3] = _mm_add_ps(Result.v[3], _mm_mul_ps(Result.v[2], _SYL_VEC_SWIZZLE1(Matrix.v[3], 2)));
    Result.v[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), Result.v[3]);
    return(Result);
#else
    /* TODO(xcatalyst): This is temporary to prevent gcc warning, implemenet this later ! */
    return(Matrix);
#endif
}

/* Inverse of a matrix*/
SYL_STATIC smat4 Inverse(smat4 Matrix)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    __m128 a0 = _SYL_VEC_SHUFFLE_0101(Matrix.v[0], Matrix.v[1]); // 00, 01, 10, 11
    __m128 a1 = _SYL_VEC_SHUFFLE_2323(Matrix.v[0], Matrix.v[1]); // 02, 03, 12, 13

    smat4 Result;
    Result.v[0] = _SYL_VEC_SHUFFLE(a0, Matrix.v[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
    Result.v[1] = _SYL_VEC_SHUFFLE(a0, Matrix.v[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
    Result.v[2] = _SYL_VEC_SHUFFLE(a1, Matrix.v[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

    __m128 SizeSquared = _mm_mul_ps(Result.v[0], Result.v[0]);
    SizeSquared = _mm_add_ps(SizeSquared, _mm_mul_ps(Result.v[1], Result.v[1]));
    SizeSquared = _mm_add_ps(SizeSquared, _mm_mul_ps(Result.v[2], Result.v[2]));

    __m128 Sqr = _mm_blendv_ps(_mm_div_ps(_S_XMM_ZERO, SizeSquared), _S_XMM_ZERO, _mm_cmplt_ps(SizeSquared, _mm_set1_ps(_SYL_SMALL_NUMBER)));

    Result.v[0] = _mm_mul_ps(Result.v[0], Sqr);
    Result.v[1] = _mm_mul_ps(Result.v[1], Sqr);
    Result.v[2] = _mm_mul_ps(Result.v[2], Sqr);

    Result.v[3] = _mm_mul_ps(Result.v[0], _SYL_VEC_SWIZZLE1(Matrix.v[3], 0));
    Result.v[3] = _mm_add_ps(Result.v[3], _mm_mul_ps(Result.v[1], _SYL_VEC_SWIZZLE1(Matrix.v[3], 1)));
    Result.v[3] = _mm_add_ps(Result.v[3], _mm_mul_ps(Result.v[2], _SYL_VEC_SWIZZLE1(Matrix.v[3], 2)));
    Result.v[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), Result.v[3]);

    return(Result);
#else
    /* TODO(xcatalyst): This is temporary to prevent gcc warning, implemenet this later ! */
    return(Matrix);
#endif
}

SYL_STATIC svec4 s_mat4_transform(smat4 Matrix, svec4 Vector)
{
#if defined (SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    svec4 Result;
    Result.v = _SYL_PERMUTE_PS(Vector.v, _MM_SHUFFLE(3, 3, 3, 3));
    Result.v = _mm_mul_ps(Result.v, Matrix.v[3]);
    __m128 Temp = _SYL_PERMUTE_PS(Vector.v, _MM_SHUFFLE(2, 2, 2, 2));
    Result.v = _SYL_ADD_PS(Temp, Matrix.v[2], Result.v);
    Temp = _SYL_PERMUTE_PS(Vector.v, _MM_SHUFFLE(1, 1, 1, 1));
    Result.v = _SYL_ADD_PS(Temp, Matrix.v[1], Result.v);
    Temp = _SYL_PERMUTE_PS(Vector.v, _MM_SHUFFLE(0, 0, 0, 0));
    Result.v = _SYL_ADD_PS(Temp, Matrix.v[0], Result.v);
    return(Result);
#else
    svec4 Result;

    Result.x = Vector.x * Matrix.e2[0][0] + Vector.y * Matrix.e2[0][1] + Vector.z * Matrix.e2[0][2] + Vector.w * Matrix.e2[0][3];
    Result.y = Vector.x * Matrix.e2[1][0] + Vector.y * Matrix.e2[1][1] + Vector.z * Matrix.e2[1][2] + Vector.w * Matrix.e2[1][3];
    Result.z = Vector.x * Matrix.e2[2][0] + Vector.y * Matrix.e2[2][1] + Vector.z * Matrix.e2[2][2] + Vector.w * Matrix.e2[2][3];
    Result.w = Vector.x * Matrix.e2[3][0] + Vector.y * Matrix.e2[3][1] + Vector.z * Matrix.e2[3][2] + Vector.w * Matrix.e2[3][3];

    return(Result);
#endif
}

SYL_INLINE svec4 s_mat4_mul_vec4(smat4 Matrix1, svec4 Vector)
{
    return s_mat4_transform(Matrix1, Vector);
}

SYL_STATIC svec3 s_mat4_mul_vec3(smat4 Matrix1, svec3 Vector)
{
    svec4 Vec = s_mat4_transform(Matrix1, S_VEC4VF(Vector, 1.0f));
    svec3 Result = { { Vec.x, Vec.y, Vec.z } };
    return(Result);
}

SYL_STATIC smat4 s_mat4_xrotation(float Angle)
{
    float CosAngle = cos(Angle);
    float SinAngle = sin(Angle);

    smat4 Result = { {
        1, 0, 0, 0,
        0, CosAngle,-SinAngle, 0,
        0, SinAngle, CosAngle, 0,
        0, 0, 0, 1
    } };

    return(Result);
}

SYL_STATIC smat4 s_mat4_yrotation(float Angle)
{
    float CosAngle = cos(Angle);
    float SinAngle = sin(Angle);

    smat4 Result = { {
        CosAngle, 0, SinAngle, 0,
        0, 1, 0, 0,
        -SinAngle, 0, CosAngle, 0,
        0, 0, 0, 1
    } };

    return(Result);
}

SYL_STATIC smat4 s_mat4_zrotation(float Angle)
{
    float CosAngle = cos(Angle);
    float SinAngle = sin(Angle);

    smat4 Result = { {
        CosAngle, -SinAngle, 0, 0,
        SinAngle, CosAngle, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    } };

    return(Result);
}

SYL_STATIC smat4 s_mat4_translation(svec3 Vector)
{
#if defined(SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    smat4 Result;
    Result.v[0] = _S_IDENT4x4R0;
    Result.v[1] = _S_IDENT4x4R1;
    Result.v[2] = _S_IDENT4x4R2;
    Result.v[0] = _mm_set_ps(Vector.x, Vector.y, Vector.z, 1.0f);
    return(Result);
#else
    smat4 Result = { {
        1, 0, 0, Vector.x,
        0, 1, 0, Vector.y,
        0, 0, 1, Vector.z,
        0, 0, 0, 1,
    } };

    return(Result);
#endif
}

/* No LH version for now...  */
SYL_STATIC smat4 s_mat4_perspective_projection_rh(float Fov, float AspectRatio, float NearClipPlane, float FarClipPlane)
{
#if defined(SYL_ENABLE_SSE4) || defined(SYL_ENABLE_AVX)
    float Sin = sin(0.5f * Fov);
    float Cos = cos(0.5f * Fov);
    float Range = FarClipPlane / (NearClipPlane - FarClipPlane);
    float Height = Cos / Sin;
    __m128 Mem = { Height / AspectRatio, Height, Range, Range * NearClipPlane };
    __m128 Mem2 = Mem;
    __m128 Temp = _mm_setzero_ps();
    Temp = _mm_move_ss(Temp, Mem2);
    smat4 Result;
    Result.v[0] = Temp;
    Temp = Mem2;
    Temp = _mm_and_ps(Temp, _S_XMM_MASK_Y);
    Result.v[1] = Temp;
    Temp = _mm_setzero_ps();
    Mem2 = _mm_shuffle_ps(Mem2, _S_IDENT4x4R3, _MM_SHUFFLE(3, 2, 3, 2));
    Temp = _mm_shuffle_ps(Temp, Mem2, _MM_SHUFFLE(3, 0, 0, 0));
    Result.v[2] = Temp;
    Temp = _mm_shuffle_ps(Temp, Mem2, _MM_SHUFFLE(2, 1, 0, 0));
    Result.v[3] = Temp;
    return(Result);
#else
    //float Cotan = 1.0f / tanf(Fov / 2.0f);

    //mat4 Result = { 
    //	Cotan / AspectRatio, 0.0f, 0.0f, 0.0f,
    //	0.0f, Cotan, 0.0f, 0.0f,
    //	0.0f, 0.0f, (FarClipPlane + NearClipPlane) / (NearClipPlane - FarClipPlane), -1.0f,
    //	0.0f, 0.0f, (2.0f * FarClipPlane * NearClipPlane) / (NearClipPlane - FarClipPlane), 0.0f 
    //};

    //return(Result);

    float tanHalfFovy = tan(Fov / 2);

    smat4 Result;
    s_mat4_zero(&Result);
    Result.e2[0][0] = 1 / (AspectRatio * tanHalfFovy);
    Result.e2[1][1] = 1 / (tanHalfFovy);
    Result.e2[2][2] = -(FarClipPlane + NearClipPlane) / (FarClipPlane - NearClipPlane);
    Result.e2[2][3] = -1;
    Result.e2[3][2] = -(2 * FarClipPlane * NearClipPlane) / (FarClipPlane - NearClipPlane);
    return Result;
#endif
}

SYL_STATIC smat4 s_mat4_orthographic_projection_rh(float AspectRatio, float NearClipPlane, float FarClipPlane)
{
    float Ral = 1.0f;
    float Rsl = AspectRatio;
    float Fan = NearClipPlane;
    float Fsn = FarClipPlane;
    float Tab = 2.0f / (Fan - Fsn);
    float Tsb = (Fan + Fsn) / (Fan - Fsn);

    smat4 Result = { { 1 / Ral,   0,   0,    0,
             0, 1 / Rsl,   0,    0,
             0,   0, 1 / Tab, -Tsb / Tab,
             0,   0,   0,    1 } };
    return(Result);
}

#ifdef __cplusplus
_SYL_CPP_EXTERN_END
#endif

#endif // SYLVESTER_H
