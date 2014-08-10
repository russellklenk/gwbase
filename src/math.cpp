/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements a math library based on single-precision IEEE 754
/// floating-point numbers.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <math.h>
#include <float.h>
#include <assert.h>

#include "math.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
//static const uint32_t F32_MaskSgn  = 0x80000000; /* mask sign bit      */
//static const uint32_t F32_MaskExp  = 0x7F800000; /* mask exponent bits */
//static const uint32_t F32_MaskMnt  = 0x007FFFFF; /* mask mantissa bits */
//static const uint32_t F32_PZero    = 0x00000000;
//static const uint32_t F32_NZero    = 0x80000000;
static const uint32_t F32_PInf     = 0x7F800000;
static const uint32_t F32_NInf     = 0xFF800000;
static const uint32_t F32_QNaN     = 0x7FC00000;
//static const uint32_t F32_SNaN     = 0x7F800001;

/// @summary The number of uint32_t values that are part of the seed data.
#define WELL512_SEED_UNITS    (16U)

/// @summary The size of the seed data for the WELL512 PRNG, specified in bytes.
#define WELL512_SEED_SIZE     (16U * sizeof(uint32_t))

/// @summary The size of the state data for the WELL512 PRNG, specified in bytes.
#define WELL512_STATE_SIZE    (17U * sizeof(uint32_t))

/// @summary The maximum value that can be output by the WELL1024 PRNG.
#define WELL512_RAND_MAX      (4294967295U)

/// @summary The value 1.0 / (WELL512_RAND_MAX + 1.0) as an IEEE-754 double.
#define WELL512_RAND_SCALE    (2.32830643653869628906e-10)

/*///////////////////////
//   Local Functions   //
///////////////////////*/
/// @summary Determines the number of bytes of seed data required to seed an
/// instance of the WELL512 PRNG.
/// @return The number of bytes required to seed a WELL512 PRNG.
static inline size_t well512_seed_size(void)
{
    return WELL512_SEED_SIZE;
}

/// @summary Initializes a WELL512 PRNG instance to the default state. The RNG
/// remains unseeded; call well512_seed() before requesting data from the RNG.
/// @param rng The WELL512 PRNG state data to initialize.
static void well512_init(rng_state_t *rng)
{
    if (rng != NULL)
    {
        rng->index = 0;
        for (size_t i = 0; i < WELL512_SEED_UNITS; ++i)
        {
            rng->state[i]  = 0;
        }
    }
}

/// @summary Supplies seed data to seed (or re-seed) a WELL512 PRNG.
/// @param rng The WELL512 PRNG instance to seed (or re-seed.)
/// @param seed_data Pointer to the start of a memory block containing the seed data for the RNG.
/// @param seed_size The size of the seed data, in bytes. This value must be
/// at least the size specified by well512_seed_size().
static void well512_seed(rng_state_t *rng, void *seed_data, size_t seed_size)
{
    if (rng       == NULL) return;
    if (seed_data == NULL) return;
    if (seed_size  < WELL512_SEED_SIZE) return;
    uint32_t *seed = (uint32_t*) seed_data;
    for (size_t i  = 0; i < WELL512_SEED_UNITS; ++i)
    {
        rng->state[i] = seed[i];
    }
    rng->index = 0;
}

/// @summary Draws a single double-precision IEEE-754 floating point value from
/// a WELL512 PRNG. Values are uniformly distributed over the range [0, 1).
/// @param rng The WELL512 PRNG instance to draw from.
/// @return A value selected from the range [0, 1).
static inline double well512_draw(rng_state_t *rng)
{
    uint32_t &r = rng->index;
    uint32_t *s = rng->state;
    uint32_t  n = r;    // read the current value of rng->index
    uint32_t  a = s[n];
    uint32_t  b = 0;
    uint32_t  c = s[(n + 13) & 15];
    uint32_t  d = 0;
    b           = a ^ c ^ (a << 16) ^ (c << 15);
    c           = s[(n + 9)   & 15];
    c          ^= (c >> 11);
    a           = s[n] = b ^ c;
    d           = a ^ ((a << 5) & 0xDA442D24UL);
    r           = (n + 15) & 15; // update  rng->index
    n           = (n + 15) & 15; // updated rng->index; update cached
    a           = s[n];
    s[n]        = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
    return s[n] * WELL512_RAND_SCALE;
}

/// @summary Retrieves 32 random bits from a WELL512 PRNG. The bits are returned
/// without any transformation performed and the full range [0, UINT32_MAX] is possible.
/// @param rng The WELL512 PRNG instance to draw from.
/// @return A value selected from the range [0, 4294967295].
static inline uint32_t well512_bits(rng_state_t *rng)
{
    uint32_t &r = rng->index;
    uint32_t *s = rng->state;
    uint32_t  n = r;    // read the current value of rng->index
    uint32_t  a = s[n];
    uint32_t  b = 0;
    uint32_t  c = s[(n + 13) & 15];
    uint32_t  d = 0;
    b           = a ^ c ^ (a << 16) ^ (c << 15);
    c           = s[(n + 9)   & 15];
    c          ^= (c >> 11);
    a           = s[n] = b ^ c;
    d           = a ^ ((a << 5) & 0xDA442D24UL);
    r           = (n + 15) & 15; // update  rng->index
    n           = (n + 15) & 15; // updated rng->index; update cached
    a           = s[n];
    s[n]        = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
    return s[n];
}

/*///////////////////////
//  Public Functions   //
///////////////////////*/
float min2(float a, float b)
{
    return (a < b ? a : b);
}

float max2(float a, float b)
{
    return (a < b ? b : a);
}

float min3(float a, float b, float c)
{
    return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

float max3(float a, float b, float c)
{
    return (a > b ? (a > c ? a : c) : (b > c ? b : c));
}

float mix(float a, float b, float t)
{
    return (a + ((b - a) * t));
}

float clamp(float x, float a, float b)
{
    return max2(min2(x, b), a);
}

bool eq(float a, float b)
{
    return (fabsf(a-b) <= (FLT_EPSILON * max2(fabsf(a), fabsf(b))));
}

bool eq_abs(float a, float b, float tol)
{
    return (fabsf(a-b) <= tol);
}

bool eq_rel(float a, float b, float tol)
{
    return (fabsf(a-b) <= (tol * max2(fabsf(a), fabsf(b))));
}

bool eq_com(float a, float b, float t_a, float t_r)
{
    return (fabsf(a-b) <= max2(t_a, t_r*max2(fabsf(a), fabsf(b))));
}

bool is_nan(float a)
{
    const uint32_t   mask   = 0xFFC00000;  // sign + exponent
    const uint32_t   snan   = 0x7FC00000;  // all exponent + top-most mantissa
    uint32_t         value  = *(uint32_t*) &a;
    return ((value & mask) == snan);
}

bool is_inf(float a)
{
    const uint32_t   mask    = 0x7FFFFFFF;  // all exponent + all mantissa
    const uint32_t   inf     = 0x7F800000;  // all exponent; no mantissa
    uint32_t         value   = *(uint32_t*) &a;
    return ((value & mask)  == inf);
}

float rcp(float a)
{
    return 1.0f / a;
}

float rad(float degrees)
{
    // degrees * (PI/180)
    return (degrees * 0.017453292519943295769236907684886f);
}

float deg(float radians)
{
    // radians * (180/PI)
    return (radians * 57.29577951308232087679815481410500f);
}

float linear(float a, float b, float t)
{
    return (a + ((b - a) * t));
}

float bezier(float a, float b, float in_t, float out_t, float t)
{
    float d  =  b - a;
    float a2 = (d * 3.0f) - (in_t + (out_t * 2.0f));
    float a3 =   out_t    +  in_t - (d  * 2.0f);
    return a + ((out_t    +  (a2  + (a3 * t)) * t) * t);
}

float hermite(float a, float b, float in_t, float out_t, float t)
{
    float t2 =  t  * t;
    float t3 =  t2 * t;
    return ((+2.0f * t3   - 3.0f * t2  + 1.0f) * a     +
            (-2.0f * t3   + 3.0f * t2)         * b     +
            (  t3  - 2.0f *   t2 +  t)         * out_t +
            (  t3  -  t2)                      * in_t);
}

size_t random_seed_size(void)
{
    return well512_seed_size();
}

void random_init(rng_state_t *rng)
{
    well512_init(rng);
}

void random_seed(rng_state_t *rng, void *seed_data, size_t seed_size)
{
    well512_seed(rng, seed_data, seed_size);
}

void random_sequence(uint32_t *values, uint32_t start, size_t count)
{
    uint32_t n = (uint32_t)  count;
    for (uint32_t i = 0; i < n; ++i)
    {
        *values++ = start  + i;
    }
}

void random_shuffle(uint32_t *values, size_t count, rng_state_t *rng)
{
    // algorithm 3.4.2P of The Art of Computer Programming, Vol. 2
    uint32_t n = (uint32_t) count;
    while   (n > 1)
    {
        uint32_t k = random_range(0, n, rng); // k in [0, n)
        uint32_t t = values[k];               // swap values[k] and values[n-1]
        --n;                                  // n decreases every iteration
        values[k]  = values[n];
        values[n]  = t;
    }
}

void random_choose(uint64_t population_size, uint64_t sample_size, uint32_t *values, rng_state_t *rng)
{
    // algorithm 3.4.2S of The Art of Computer Programming, Vol. 2
    uint64_t n = sample_size;      // max allowable is UINT32_MAX + 1
    uint64_t N = population_size;  // max allowable is UINT32_MAX + 1
    uint32_t t = 0;                // total dealt with so far
    uint32_t m = 0;                // number selected so far
    while   (m < n)
    {
        double v = random_draw(rng);
        if ((N - t) * v >= (n-m))
        {
            ++t;
        }
        else
        {
            values[m++] = t++;
        }
    }
}

void random_choose_with_replacement(uint64_t population_size, uint64_t sample_size, uint32_t *values, rng_state_t *rng)
{
    for (uint64_t i = 0; i < sample_size; ++i)
    {
        *values++ = random_range(0, population_size, rng);
    }
}

double random_draw(rng_state_t *rng)
{
    return well512_draw(rng); // in [0, 1)
}

uint32_t random_range(uint64_t min_value, uint64_t max_value, rng_state_t *rng)
{
    // @note: max_value must be greater than min_value (or we divide by zero)
    // @note: the max value of max_value is UINT32_MAX + 1
    // see http://www.azillionmonkeys.com/qed/random.html
    // see http://en.wikipedia.org/wiki/Fisher-Yates_shuffle#Modulo_bias
    // remove the bias that can result when the range 'r'
    // does not divide evenly into the PRNG range 'n'.
    uint64_t r = max_value - min_value; // size of request range [min, max)
    uint64_t u = WELL512_RAND_MAX;      // PRNG inclusive upper bound
    uint64_t n = u + 1;                 // size of PRNG range [0, UINT32_MAX]
    uint64_t i = n / r;                 // # times whole of 'r' fits in 'n'
    uint64_t m = r * i;                 // largest integer multiple of 'r'<='n'
    uint64_t x = 0;                     // raw value from PRNG
    do
    {
        x = well512_bits(rng);          // x in [0, UINT32_MAX]
    } while (x >= m);
    x /= i;                             // x -> [0, r) and [0, UINT32_MAX]
    return uint32_t(x + min_value);     // x -> [min, max)
}

uint32_t random_bits(rng_state_t *rng)
{
    return well512_bits(rng);           // in [0, UINT32_MAX]
}

float* vec2_set_xy(float *dst_xy, float x, float y)
{
    dst_xy[0] = x;
    dst_xy[1] = y;
    return dst_xy;
}

float* vec2_set_vec2(float * restrict dst_xy, float const * restrict src_xy)
{
    dst_xy[0] = src_xy[0];
    dst_xy[1] = src_xy[1];
    return dst_xy;
}

float* vec2_set_vec3(float * restrict dst_xy, float const * restrict src_xyz)
{
    dst_xy[0] = src_xyz[0];
    dst_xy[1] = src_xyz[1];
    return dst_xy;
}

float* vec2_set_vec4(float * restrict dst_xy, float const * restrict src_xyzw)
{
    dst_xy[0] = src_xyzw[0];
    dst_xy[1] = src_xyzw[1];
    return dst_xy;
}

float* vec3_set_xyz(float *dst_xyz, float x, float y, float z)
{
    dst_xyz[0] = x;
    dst_xyz[1] = y;
    dst_xyz[2] = z;
    return dst_xyz;
}

float* vec3_set_vec2(float * restrict dst_xyz, float const * restrict src_xy, float z)
{
    dst_xyz[0] = src_xy[0];
    dst_xyz[1] = src_xy[1];
    dst_xyz[2] = z;
    return dst_xyz;
}

float* vec3_set_vec3(float * restrict dst_xyz, float const * restrict src_xyz)
{
    dst_xyz[0] = src_xyz[0];
    dst_xyz[1] = src_xyz[1];
    dst_xyz[2] = src_xyz[2];
    return dst_xyz;
}

float* vec3_set_vec4(float * restrict dst_xyz, float const * restrict src_xyzw)
{
    dst_xyz[0] = src_xyzw[0];
    dst_xyz[1] = src_xyzw[1];
    dst_xyz[2] = src_xyzw[2];
    return dst_xyz;
}

float* vec4_set(float *dst_xyzw, float x, float y, float z, float w)
{
    dst_xyzw[0] = x;
    dst_xyzw[1] = y;
    dst_xyzw[2] = z;
    dst_xyzw[3] = w;
    return dst_xyzw;
}

float* vec4_set_vec2(float * restrict dst_xyzw, float const * restrict src_xy, float z, float w)
{
    dst_xyzw[0] = src_xy[0];
    dst_xyzw[1] = src_xy[1];
    dst_xyzw[2] = z;
    dst_xyzw[3] = w;
    return dst_xyzw;
}

float* vec4_set_vec3(float * restrict dst_xyzw, float const * restrict src_xyz, float w)
{
    dst_xyzw[0] = src_xyz[0];
    dst_xyzw[1] = src_xyz[1];
    dst_xyzw[2] = src_xyz[2];
    dst_xyzw[3] = w;
    return dst_xyzw;
}

float* vec4_set_vec4(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    dst_xyzw[0] = src_xyzw[0];
    dst_xyzw[1] = src_xyzw[1];
    dst_xyzw[2] = src_xyzw[2];
    dst_xyzw[3] = src_xyzw[3];
    return dst_xyzw;
}

float* vec2_set_nan(float *dst_xy)
{
    float qnan = *(float*) &F32_QNaN;
    dst_xy[0]  = qnan;
    dst_xy[1]  = qnan;
    return dst_xy;
}

float* vec3_set_nan(float *dst_xyz)
{
    float qnan = *(float*) &F32_QNaN;
    dst_xyz[0] = qnan;
    dst_xyz[1] = qnan;
    dst_xyz[2] = qnan;
    return dst_xyz;
}

float* vec4_set_nan(float *dst_xyzw)
{
    float qnan  = *(float*) &F32_QNaN;
    dst_xyzw[0] = qnan;
    dst_xyzw[1] = qnan;
    dst_xyzw[2] = qnan;
    dst_xyzw[3] = qnan;
    return dst_xyzw;
}

float* vec2_set_one(float *dst_xy)
{
    dst_xy[0] = 1.0f;
    dst_xy[1] = 1.0f;
    return dst_xy;
}

float* vec3_set_one(float *dst_xyz)
{
    dst_xyz[0] = 1.0f;
    dst_xyz[1] = 1.0f;
    dst_xyz[2] = 1.0f;
    return dst_xyz;
}

float* vec4_set_one(float *dst_xyzw)
{
    dst_xyzw[0] = 1.0f;
    dst_xyzw[1] = 1.0f;
    dst_xyzw[2] = 1.0f;
    dst_xyzw[3] = 1.0f;
    return dst_xyzw;
}

float* vec2_set_zero(float *dst_xy)
{
    dst_xy[0] = 0.0f;
    dst_xy[1] = 0.0f;
    return dst_xy;
}

float* vec3_set_zero(float *dst_xyz)
{
    dst_xyz[0] = 0.0f;
    dst_xyz[1] = 0.0f;
    dst_xyz[2] = 0.0f;
    return dst_xyz;
}

float* vec4_set_zero(float *dst_xyzw)
{
    dst_xyzw[0] = 0.0f;
    dst_xyzw[1] = 0.0f;
    dst_xyzw[2] = 0.0f;
    dst_xyzw[3] = 0.0f;
    return dst_xyzw;
}

float* vec2_set_ninf(float *dst_xy)
{
    float ninf = *(float*) &F32_NInf;
    dst_xy[0]  = ninf;
    dst_xy[1]  = ninf;
    return dst_xy;
}

float* vec3_set_ninf(float *dst_xyz)
{
    float ninf = *(float*) &F32_NInf;
    dst_xyz[0] = ninf;
    dst_xyz[1] = ninf;
    dst_xyz[2] = ninf;
    return dst_xyz;
}

float* vec4_set_ninf(float *dst_xyzw)
{
    float ninf  = *(float*) &F32_NInf;
    dst_xyzw[0] = ninf;
    dst_xyzw[1] = ninf;
    dst_xyzw[2] = ninf;
    dst_xyzw[3] = ninf;
    return dst_xyzw;
}

float* vec2_set_pinf(float *dst_xy)
{
    float pinf = *(float*) &F32_PInf;
    dst_xy[0]  = pinf;
    dst_xy[1]  = pinf;
    return dst_xy;
}

float* vec3_set_pinf(float *dst_xyz)
{
    float pinf = *(float*) &F32_PInf;
    dst_xyz[0] = pinf;
    dst_xyz[1] = pinf;
    dst_xyz[2] = pinf;
    return dst_xyz;
}

float* vec4_set_pinf(float *dst_xyzw)
{
    float pinf  = *(float*) &F32_PInf;
    dst_xyzw[0] = pinf;
    dst_xyzw[1] = pinf;
    dst_xyzw[2] = pinf;
    dst_xyzw[3] = pinf;
    return dst_xyzw;
}

float* vec2_set_unit_x(float *dst_xy)
{
    dst_xy[0] = 1.0f;
    dst_xy[1] = 0.0f;
    return dst_xy;
}

float* vec3_set_unit_x(float *dst_xyz)
{
    dst_xyz[0] = 1.0f;
    dst_xyz[1] = 0.0f;
    dst_xyz[2] = 0.0f;
    return dst_xyz;
}

float* vec4_set_unit_x(float *dst_xyzw)
{
    dst_xyzw[0] = 1.0f;
    dst_xyzw[1] = 0.0f;
    dst_xyzw[2] = 0.0f;
    dst_xyzw[3] = 0.0f;
    return dst_xyzw;
}

float* vec2_set_unit_y(float *dst_xy)
{
    dst_xy[0] = 0.0f;
    dst_xy[1] = 1.0f;
    return dst_xy;
}

float* vec3_set_unit_y(float *dst_xyz)
{
    dst_xyz[0] = 0.0f;
    dst_xyz[1] = 1.0f;
    dst_xyz[2] = 0.0f;
    return dst_xyz;
}

float* vec4_set_unit_y(float *dst_xyzw)
{
    dst_xyzw[0] = 0.0f;
    dst_xyzw[1] = 1.0f;
    dst_xyzw[2] = 0.0f;
    dst_xyzw[3] = 0.0f;
    return dst_xyzw;
}

float* vec3_set_unit_z(float *dst_xyz)
{
    dst_xyz[0] = 0.0f;
    dst_xyz[1] = 0.0f;
    dst_xyz[2] = 1.0f;
    return dst_xyz;
}

float* vec4_set_unit_z(float *dst_xyzw)
{
    dst_xyzw[0] = 0.0f;
    dst_xyzw[1] = 0.0f;
    dst_xyzw[2] = 1.0f;
    dst_xyzw[3] = 0.0f;
    return dst_xyzw;
}

float* vec4_set_unit_w(float *dst_xyzw)
{
    dst_xyzw[0] = 0.0f;
    dst_xyzw[1] = 0.0f;
    dst_xyzw[2] = 0.0f;
    dst_xyzw[3] = 1.0f;
    return dst_xyzw;
}

bool vec2_eq(float const * restrict a_xy, float const * restrict b_xy)
{
    if (!eq(a_xy[0], b_xy[0])) return false;
    if (!eq(a_xy[1], b_xy[1])) return false;
    return true;
}

bool vec3_eq(float const * restrict a_xyz, float const * restrict b_xyz)
{
    if (!eq(a_xyz[0], b_xyz[0])) return false;
    if (!eq(a_xyz[1], b_xyz[1])) return false;
    if (!eq(a_xyz[2], b_xyz[2])) return false;
    return true;
}

bool vec4_eq(float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    if (!eq(a_xyzw[0], b_xyzw[0])) return false;
    if (!eq(a_xyzw[1], b_xyzw[1])) return false;
    if (!eq(a_xyzw[2], b_xyzw[2])) return false;
    if (!eq(a_xyzw[3], b_xyzw[3])) return false;
    return true;
}

float* vec2_add(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy)
{
    dst_xy[0] = a_xy[0] + b_xy[0];
    dst_xy[1] = a_xy[1] + b_xy[1];
    return dst_xy;
}

float* vec3_add(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz)
{
    dst_xyz[0] = a_xyz[0] + b_xyz[0];
    dst_xyz[1] = a_xyz[1] + b_xyz[1];
    dst_xyz[2] = a_xyz[2] + b_xyz[2];
    return dst_xyz;
}

float* vec4_add(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    dst_xyzw[0] = a_xyzw[0] + b_xyzw[0];
    dst_xyzw[1] = a_xyzw[1] + b_xyzw[1];
    dst_xyzw[2] = a_xyzw[2] + b_xyzw[2];
    dst_xyzw[3] = a_xyzw[3] + b_xyzw[3];
    return dst_xyzw;
}

float* vec2_sub(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy)
{
    dst_xy[0] = a_xy[0] - b_xy[0];
    dst_xy[1] = a_xy[1] - b_xy[1];
    return dst_xy;
}

float* vec3_sub(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz)
{
    dst_xyz[0] = a_xyz[0] - b_xyz[0];
    dst_xyz[1] = a_xyz[1] - b_xyz[1];
    dst_xyz[2] = a_xyz[2] - b_xyz[2];
    return dst_xyz;
}

float* vec4_sub(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    dst_xyzw[0] = a_xyzw[0] - b_xyzw[0];
    dst_xyzw[1] = a_xyzw[1] - b_xyzw[1];
    dst_xyzw[2] = a_xyzw[2] - b_xyzw[2];
    dst_xyzw[3] = a_xyzw[3] - b_xyzw[3];
    return dst_xyzw;
}

float* vec2_mul(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy)
{
    dst_xy[0] = a_xy[0] * b_xy[0];
    dst_xy[1] = a_xy[1] * b_xy[1];
    return dst_xy;
}

float* vec3_mul(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz)
{
    dst_xyz[0] = a_xyz[0] * b_xyz[0];
    dst_xyz[1] = a_xyz[1] * b_xyz[1];
    dst_xyz[2] = a_xyz[2] * b_xyz[2];
    return dst_xyz;
}

float* vec4_mul(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    dst_xyzw[0] = a_xyzw[0] * b_xyzw[0];
    dst_xyzw[1] = a_xyzw[1] * b_xyzw[1];
    dst_xyzw[2] = a_xyzw[2] * b_xyzw[2];
    dst_xyzw[3] = a_xyzw[3] * b_xyzw[3];
    return dst_xyzw;
}

float* vec2_div(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy)
{
    dst_xy[0] = a_xy[0] / b_xy[0];
    dst_xy[1] = a_xy[1] / b_xy[1];
    return dst_xy;
}

float* vec3_div(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz)
{
    dst_xyz[0] = a_xyz[0] / b_xyz[0];
    dst_xyz[1] = a_xyz[1] / b_xyz[1];
    dst_xyz[2] = a_xyz[2] / b_xyz[2];
    return dst_xyz;
}

float* vec4_div(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    dst_xyzw[0] = a_xyzw[0] / b_xyzw[0];
    dst_xyzw[1] = a_xyzw[1] / b_xyzw[1];
    dst_xyzw[2] = a_xyzw[2] / b_xyzw[2];
    dst_xyzw[3] = a_xyzw[3] / b_xyzw[3];
    return dst_xyzw;
}

float* vec2_scl(float * restrict dst_xy, float const * restrict a_xy, float b)
{
    dst_xy[0] = a_xy[0] * b;
    dst_xy[1] = a_xy[1] * b;
    return dst_xy;
}

float* vec3_scl(float * restrict dst_xyz, float const * restrict a_xyz, float b)
{
    dst_xyz[0] = a_xyz[0] * b;
    dst_xyz[1] = a_xyz[1] * b;
    dst_xyz[2] = a_xyz[2] * b;
    return dst_xyz;
}

float* vec4_scl(float * restrict dst_xyzw, float const * restrict a_xyzw, float b)
{
    dst_xyzw[0] = a_xyzw[0] * b;
    dst_xyzw[1] = a_xyzw[1] * b;
    dst_xyzw[2] = a_xyzw[2] * b;
    dst_xyzw[3] = a_xyzw[3] * b;
    return dst_xyzw;
}

float* vec4_scl3(float * restrict dst_xyzw, float const * restrict a_xyzw, float b)
{
    dst_xyzw[0] = a_xyzw[0] * b;
    dst_xyzw[1] = a_xyzw[1] * b;
    dst_xyzw[2] = a_xyzw[2] * b;
    dst_xyzw[3] = a_xyzw[3];
    return dst_xyzw;
}

float* vec2_neg(float * restrict dst_xy, float const * restrict src_xy)
{
    dst_xy[0] = -src_xy[0];
    dst_xy[1] = -src_xy[1];
    return dst_xy;
}

float* vec3_neg(float * restrict dst_xyz, float const * restrict src_xyz)
{
    dst_xyz[0] = -src_xyz[0];
    dst_xyz[1] = -src_xyz[1];
    dst_xyz[2] = -src_xyz[2];
    return dst_xyz;
}

float* vec4_neg(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    dst_xyzw[0] = -src_xyzw[0];
    dst_xyzw[1] = -src_xyzw[1];
    dst_xyzw[2] = -src_xyzw[2];
    dst_xyzw[3] = -src_xyzw[3];
    return dst_xyzw;
}

float* vec4_neg3(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    dst_xyzw[0] = -src_xyzw[0];
    dst_xyzw[1] = -src_xyzw[1];
    dst_xyzw[2] = -src_xyzw[2];
    dst_xyzw[3] =  src_xyzw[3];
    return dst_xyzw;
}

float vec2_dot(float &dst, float const * restrict a_xy, float const * restrict b_xy)
{
    dst = (a_xy[0] * b_xy[0] + a_xy[1] * b_xy[1]);
    return dst;
}

float vec3_dot(float &dst, float const * restrict a_xyz, float const * restrict b_xyz)
{
    dst = (a_xyz[0] * b_xyz[0] + a_xyz[1] * b_xyz[1] + a_xyz[2] * b_xyz[2]);
    return dst;
}

float vec4_dot(float &dst, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    dst = (a_xyzw[0] * b_xyzw[0] + a_xyzw[1] * b_xyzw[1] + a_xyzw[2] * b_xyzw[2] + a_xyzw[3] * b_xyzw[3]);
    return dst;
}

float vec4_dot3(float &dst, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    dst = (a_xyzw[0] * b_xyzw[0] + a_xyzw[1] * b_xyzw[1] + a_xyzw[2] * b_xyzw[2]);
    return dst;
}

float vec2_len(float &dst, float const *a_xy)
{
    dst  = sqrtf(a_xy[0] * a_xy[0] + a_xy[1] * a_xy[1]);
    return dst;
}

float vec3_len(float &dst, float const *a_xyz)
{
    dst = sqrtf(a_xyz[0] * a_xyz[0] + a_xyz[1] * a_xyz[1] + a_xyz[2] * a_xyz[2]);
    return dst;
}

float vec4_len(float &dst, float const *a_xyzw)
{
    dst = sqrtf(a_xyzw[0] * a_xyzw[0] + a_xyzw[1] * a_xyzw[1] + a_xyzw[2] * a_xyzw[2] + a_xyzw[3] * a_xyzw[3]);
    return dst;
}

float vec4_len3(float &dst, float const *a_xyzw)
{
    dst = sqrtf(a_xyzw[0] * a_xyzw[0] + a_xyzw[1] * a_xyzw[1] + a_xyzw[2] * a_xyzw[2]);
    return dst;
}

float vec2_len_sq(float &dst, float const *a_xy)
{
    dst = (a_xy[0] * a_xy[0] + a_xy[1] * a_xy[1]);
    return dst;
}

float vec3_len_sq(float &dst, float const *a_xyz)
{
    dst = (a_xyz[0] * a_xyz[0] + a_xyz[1] * a_xyz[1] + a_xyz[2] * a_xyz[2]);
    return dst;
}

float vec4_len_sq(float &dst, float const *a_xyzw)
{
    dst = (a_xyzw[0] * a_xyzw[0] + a_xyzw[1] * a_xyzw[1] + a_xyzw[2] * a_xyzw[2] + a_xyzw[3] * a_xyzw[3]);
    return dst;
}

float vec4_len3_sq(float &dst, float const *a_xyzw)
{
    dst = (a_xyzw[0] * a_xyzw[0] + a_xyzw[1] * a_xyzw[1] + a_xyzw[2] * a_xyzw[2]);
    return dst;
}

float* vec2_nrm(float * restrict dst_xy, float const * restrict src_xy)
{
    float len;
    if (!eq(vec2_len(len, src_xy), 0))
    {
        float rcp = 1.0f / len;
        dst_xy[0]  = src_xy[0] * rcp;
        dst_xy[1]  = src_xy[1] * rcp;
        return dst_xy;
    }
    else return vec2_set_pinf(dst_xy);
}

float* vec3_nrm(float * restrict dst_xyz, float const * restrict src_xyz)
{
    float len;
    if (!eq(vec3_len(len, src_xyz), 0))
    {
        float rcp = 1.0f / len;
        dst_xyz[0]  = src_xyz[0] * rcp;
        dst_xyz[1]  = src_xyz[1] * rcp;
        dst_xyz[2]  = src_xyz[2] * rcp;
        return dst_xyz;
    }
    else return vec3_set_pinf(dst_xyz);
}

float* vec4_nrm(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    float len;
    if (!eq(vec4_len(len, src_xyzw), 0))
    {
        float rcp = 1.0f / len;
        dst_xyzw[0]  = src_xyzw[0] * rcp;
        dst_xyzw[1]  = src_xyzw[1] * rcp;
        dst_xyzw[2]  = src_xyzw[2] * rcp;
        dst_xyzw[3]  = src_xyzw[3] * rcp;
        return dst_xyzw;
    }
    else return vec4_set_pinf(dst_xyzw);
}

float* vec4_nrm3(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    float len;
    if (!eq(vec4_len3(len, src_xyzw), 0))
    {
        float rcp = 1.0f / len;
        dst_xyzw[0]  = src_xyzw[0] * rcp;
        dst_xyzw[1]  = src_xyzw[1] * rcp;
        dst_xyzw[2]  = src_xyzw[2] * rcp;
        dst_xyzw[3]  = src_xyzw[3];
        return dst_xyzw;
    }
    else
    {
        vec3_set_pinf(dst_xyzw);
        dst_xyzw[3]  = src_xyzw[3];
        return dst_xyzw;
    }
}

float* vec2_perp(float * restrict dst_xy, float const * restrict src_xy)
{
    float x   = src_xy[0];
    float y   = src_xy[1];
    dst_xy[0] = -y;
    dst_xy[1] =  x;
    return dst_xy;
}

float* vec3_cross(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz)
{
    float ax   = a_xyz[0], ay = a_xyz[1], az = a_xyz[2];
    float bx   = b_xyz[0], by = b_xyz[1], bz = b_xyz[2];
    dst_xyz[0] = ay * bz - az * by;
    dst_xyz[1] = az * bx - ax * bz;
    dst_xyz[2] = ax * by - ay * bx;
    return dst_xyz;
}

float* vec4_cross(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    float ax    = a_xyzw[0], ay = a_xyzw[1], az = a_xyzw[2];
    float bx    = b_xyzw[0], by = b_xyzw[1], bz = b_xyzw[2];
    dst_xyzw[0] = ay * bz  - az * by;
    dst_xyzw[1] = az * bx  - ax * bz;
    dst_xyzw[2] = ax * by  - ay * bx;
    dst_xyzw[3] = 0.0f; // cross product always results in a vector
    return dst_xyzw;
}

float* vec2_swizzle(float * restrict dst_xy, float const * restrict src_xy, size_t x, size_t y)
{
    float a   = src_xy[x];
    float b   = src_xy[y];
    dst_xy[0] = a;
    dst_xy[1] = b;
    return dst_xy;
}

float* vec3_swizzle(float * restrict dst_xyz, float const * restrict src_xyz, size_t x, size_t y, size_t z)
{
    float a    = src_xyz[x];
    float b    = src_xyz[y];
    float c    = src_xyz[z];
    dst_xyz[0] = a;
    dst_xyz[1] = b;
    dst_xyz[2] = c;
    return dst_xyz;
}

float* vec4_swizzle(float * restrict dst_xyzw, float const * restrict src_xyzw, size_t x, size_t y, size_t z, size_t w)
{
    float a     = src_xyzw[x];
    float b     = src_xyzw[y];
    float c     = src_xyzw[z];
    float d     = src_xyzw[w];
    dst_xyzw[0] = a;
    dst_xyzw[1] = b;
    dst_xyzw[2] = c;
    dst_xyzw[3] = d;
    return dst_xyzw;
}

float* vec2_linear(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy, float t)
{
    dst_xy[0] = linear(a_xy[0], b_xy[0], t);
    dst_xy[1] = linear(a_xy[1], b_xy[1], t);
    return dst_xy;
}

float* vec3_linear(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz, float t)
{
    dst_xyz[0] = linear(a_xyz[0], b_xyz[0], t);
    dst_xyz[1] = linear(a_xyz[1], b_xyz[1], t);
    dst_xyz[2] = linear(a_xyz[2], b_xyz[2], t);
    return dst_xyz;
}

float* vec4_linear(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float t)
{
    dst_xyzw[0] = linear(a_xyzw[0], b_xyzw[0], t);
    dst_xyzw[1] = linear(a_xyzw[1], b_xyzw[1], t);
    dst_xyzw[2] = linear(a_xyzw[2], b_xyzw[2], t);
    dst_xyzw[3] = linear(a_xyzw[3], b_xyzw[3], t);
    return dst_xyzw;
}

float* vec4_linear3(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float t)
{
    dst_xyzw[0] = linear(a_xyzw[0], b_xyzw[0], t);
    dst_xyzw[1] = linear(a_xyzw[1], b_xyzw[1], t);
    dst_xyzw[2] = linear(a_xyzw[2], b_xyzw[2], t);
    dst_xyzw[3] = a_xyzw[3];
    return dst_xyzw;
}

float* vec2_bezier(
    float       * restrict dst_xy,
    float const * restrict a_xy,
    float const * restrict b_xy,
    float const * restrict itan_xy,
    float const * restrict otan_xy,
    float                  t)
{
    dst_xy[0] = bezier(a_xy[0], b_xy[0], itan_xy[0], otan_xy[0], t);
    dst_xy[1] = bezier(a_xy[1], b_xy[1], itan_xy[1], otan_xy[1], t);
    return dst_xy;
}

float* vec3_bezier(
    float       * restrict dst_xyz,
    float const * restrict a_xyz,
    float const * restrict b_xyz,
    float const * restrict itan_xyz,
    float const * restrict otan_xyz,
    float                  t)
{
    dst_xyz[0] = bezier(a_xyz[0], b_xyz[0], itan_xyz[0], otan_xyz[0], t);
    dst_xyz[1] = bezier(a_xyz[1], b_xyz[1], itan_xyz[1], otan_xyz[1], t);
    dst_xyz[2] = bezier(a_xyz[2], b_xyz[2], itan_xyz[2], otan_xyz[2], t);
    return dst_xyz;
}

float* vec4_bezier(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict itan_xyzw,
    float const * restrict otan_xyzw,
    float                  t)
{
    dst_xyzw[0] = bezier(a_xyzw[0], b_xyzw[0], itan_xyzw[0], otan_xyzw[0], t);
    dst_xyzw[1] = bezier(a_xyzw[1], b_xyzw[1], itan_xyzw[1], otan_xyzw[1], t);
    dst_xyzw[2] = bezier(a_xyzw[2], b_xyzw[2], itan_xyzw[2], otan_xyzw[2], t);
    dst_xyzw[3] = bezier(a_xyzw[3], b_xyzw[3], itan_xyzw[3], otan_xyzw[3], t);
    return dst_xyzw;
}

float* vec4_bezier3(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict itan_xyz,
    float const * restrict otan_xyz,
    float                  t)
{
    dst_xyzw[0] = bezier(a_xyzw[0], b_xyzw[0], itan_xyz[0], otan_xyz[0], t);
    dst_xyzw[1] = bezier(a_xyzw[1], b_xyzw[1], itan_xyz[1], otan_xyz[1], t);
    dst_xyzw[2] = bezier(a_xyzw[2], b_xyzw[2], itan_xyz[2], otan_xyz[2], t);
    dst_xyzw[3] = a_xyzw[3];
    return dst_xyzw;
}

float* vec2_hermite(
    float       * restrict dst_xy,
    float const * restrict a_xy,
    float const * restrict b_xy,
    float const * restrict itan_xy,
    float const * restrict otan_xy,
    float                  t)
{
    dst_xy[0] = hermite(a_xy[0], b_xy[0], itan_xy[0], otan_xy[0], t);
    dst_xy[1] = hermite(a_xy[1], b_xy[1], itan_xy[1], otan_xy[1], t);
    return dst_xy;
}

float* vec3_hermite(
    float       * restrict dst_xyz,
    float const * restrict a_xyz,
    float const * restrict b_xyz,
    float const * restrict itan_xyz,
    float const * restrict otan_xyz,
    float                  t)
{
    dst_xyz[0] = hermite(a_xyz[0], b_xyz[0], itan_xyz[0], otan_xyz[0], t);
    dst_xyz[1] = hermite(a_xyz[1], b_xyz[1], itan_xyz[1], otan_xyz[1], t);
    dst_xyz[2] = hermite(a_xyz[2], b_xyz[2], itan_xyz[2], otan_xyz[2], t);
    return dst_xyz;
}

float* vec4_hermite(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict itan_xyzw,
    float const * restrict otan_xyzw,
    float                  t)
{
    dst_xyzw[0] = hermite(a_xyzw[0], b_xyzw[0], itan_xyzw[0], otan_xyzw[0], t);
    dst_xyzw[1] = hermite(a_xyzw[1], b_xyzw[1], itan_xyzw[1], otan_xyzw[1], t);
    dst_xyzw[2] = hermite(a_xyzw[2], b_xyzw[2], itan_xyzw[2], otan_xyzw[2], t);
    dst_xyzw[3] = hermite(a_xyzw[3], b_xyzw[3], itan_xyzw[3], otan_xyzw[3], t);
    return dst_xyzw;
}

float* vec4_hermite3(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict itan_xyz,
    float const * restrict otan_xyz,
    float                  t)
{
    dst_xyzw[0] = hermite(a_xyzw[0], b_xyzw[0], itan_xyz[0], otan_xyz[0], t);
    dst_xyzw[1] = hermite(a_xyzw[1], b_xyzw[1], itan_xyz[1], otan_xyz[1], t);
    dst_xyzw[2] = hermite(a_xyzw[2], b_xyzw[2], itan_xyz[2], otan_xyz[2], t);
    dst_xyzw[3] = a_xyzw[3];
    return dst_xyzw;
}

float* quat_set(float *dst_xyzw, float x, float y, float z, float w)
{
    dst_xyzw[0] = x;
    dst_xyzw[1] = y;
    dst_xyzw[2] = z;
    dst_xyzw[3] = w;
    return dst_xyzw;
}

float* quat_set_quat(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    dst_xyzw[0] = src_xyzw[0];
    dst_xyzw[1] = src_xyzw[1];
    dst_xyzw[2] = src_xyzw[2];
    dst_xyzw[3] = src_xyzw[3];
    return dst_xyzw;
}

float* quat_set_nan(float *dst_xyzw)
{
    float qnan  = *(float*) &F32_QNaN;
    dst_xyzw[0] = qnan;
    dst_xyzw[1] = qnan;
    dst_xyzw[2] = qnan;
    dst_xyzw[3] = qnan;
    return dst_xyzw;
}

float* quat_set_one(float *dst_xyzw)
{
    dst_xyzw[0] = 1.0f;
    dst_xyzw[1] = 1.0f;
    dst_xyzw[2] = 1.0f;
    dst_xyzw[3] = 1.0f;
    return dst_xyzw;
}

float* quat_set_zero(float *dst_xyzw)
{
    dst_xyzw[0] = 0.0f;
    dst_xyzw[1] = 0.0f;
    dst_xyzw[2] = 0.0f;
    dst_xyzw[3] = 0.0f;
    return dst_xyzw;
}

float* quat_set_ninf(float *dst_xyzw)
{
    float ninf  = *(float*) &F32_NInf;
    dst_xyzw[0] = ninf;
    dst_xyzw[1] = ninf;
    dst_xyzw[2] = ninf;
    dst_xyzw[3] = ninf;
    return dst_xyzw;
}

float* quat_set_pinf(float *dst_xyzw)
{
    float pinf  = *(float*) &F32_PInf;
    dst_xyzw[0] = pinf;
    dst_xyzw[1] = pinf;
    dst_xyzw[2] = pinf;
    dst_xyzw[3] = pinf;
    return dst_xyzw;
}

float* quat_set_ident(float *dst_xyzw)
{
    dst_xyzw[0] = 0.0f;
    dst_xyzw[1] = 0.0f;
    dst_xyzw[2] = 0.0f;
    dst_xyzw[3] = 1.0f;
    return dst_xyzw;
}

bool quat_eq(float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    if (!eq(a_xyzw[0], b_xyzw[0]))
        return 0;
    if (!eq(a_xyzw[1], b_xyzw[1]))
        return 0;
    if (!eq(a_xyzw[2], b_xyzw[2]))
        return 0;
    if (!eq(a_xyzw[3], b_xyzw[3]))
        return 0;
    return 1;
}

float* quat_add(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    dst_xyzw[0] = a_xyzw[0] + b_xyzw[0];
    dst_xyzw[1] = a_xyzw[1] + b_xyzw[1];
    dst_xyzw[2] = a_xyzw[2] + b_xyzw[2];
    dst_xyzw[3] = a_xyzw[3] + b_xyzw[3];
    return dst_xyzw;
}

float* quat_sub(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    dst_xyzw[0] = a_xyzw[0] - b_xyzw[0];
    dst_xyzw[1] = a_xyzw[1] - b_xyzw[1];
    dst_xyzw[2] = a_xyzw[2] - b_xyzw[2];
    dst_xyzw[3] = a_xyzw[3] - b_xyzw[3];
    return dst_xyzw;
}

float* quat_mul(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    float ax = a_xyzw[0], ay = a_xyzw[1], az = a_xyzw[2], aw = a_xyzw[3];
    float bx = b_xyzw[0], by = b_xyzw[1], bz = b_xyzw[2], bw = b_xyzw[3];
    dst_xyzw[0] = ((aw * bx) + (ax * bw) + (ay * bz) - (az * by));
    dst_xyzw[1] = ((aw * by) - (ax * bz) + (ay * bw) + (az * bx));
    dst_xyzw[2] = ((aw * bz) + (ax * by) - (ay * bx) + (az * bw));
    dst_xyzw[3] = ((aw * bw) - (ax * bx) - (ay * by) - (az * bz));
    return dst_xyzw;
}

float* quat_scl(float * restrict dst_xyzw, float const * restrict a_xyzw, float b)
{
    dst_xyzw[0] = a_xyzw[0] * b;
    dst_xyzw[1] = a_xyzw[1] * b;
    dst_xyzw[2] = a_xyzw[2] * b;
    dst_xyzw[3] = a_xyzw[3] * b;
    return dst_xyzw;
}

float* quat_scl3(float * restrict dst_xyzw, float const * restrict a_xyzw, float b)
{
    dst_xyzw[0] = a_xyzw[0] * b;
    dst_xyzw[1] = a_xyzw[1] * b;
    dst_xyzw[2] = a_xyzw[2] * b;
    dst_xyzw[3] = a_xyzw[3];
    return dst_xyzw;
}

float* quat_neg(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    dst_xyzw[0] = -src_xyzw[0];
    dst_xyzw[1] = -src_xyzw[1];
    dst_xyzw[2] = -src_xyzw[2];
    dst_xyzw[3] = -src_xyzw[3];
    return dst_xyzw;
}

float* quat_neg3(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    dst_xyzw[0] = -src_xyzw[0];
    dst_xyzw[1] = -src_xyzw[1];
    dst_xyzw[2] = -src_xyzw[2];
    dst_xyzw[3] =  src_xyzw[3];
    return dst_xyzw;
}

float* quat_conj(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    dst_xyzw[0] = -src_xyzw[0];
    dst_xyzw[1] = -src_xyzw[1];
    dst_xyzw[2] = -src_xyzw[2];
    dst_xyzw[3] =  src_xyzw[3];
    return dst_xyzw;
}

float quat_dot(float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    // equivalent to selection(a * conjugate(b))
    return (a_xyzw[0] * b_xyzw[0] + a_xyzw[1] * b_xyzw[1] + a_xyzw[2] * b_xyzw[2] + a_xyzw[3] * b_xyzw[3]);
}

float quat_norm(float const *src_xyzw)
{
    return (src_xyzw[0] * src_xyzw[0] + src_xyzw[1] * src_xyzw[1] + src_xyzw[2] * src_xyzw[2] + src_xyzw[3] * src_xyzw[3]);
}

float quat_len(float const *src_xyzw)
{
    return sqrtf(src_xyzw[0] * src_xyzw[0] + src_xyzw[1] * src_xyzw[1] + src_xyzw[2] * src_xyzw[2] + src_xyzw[3] * src_xyzw[3]);
}

float quat_len_sq(float const *src_xyzw)
{
    return (src_xyzw[0] * src_xyzw[0] + src_xyzw[1] * src_xyzw[1] + src_xyzw[2] * src_xyzw[2] + src_xyzw[3] * src_xyzw[3]);
}

float quat_sel(float const *src_xyzw)
{
    return src_xyzw[3];
}

float* quat_inv(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    float norm = quat_norm(src_xyzw);
    if (!eq(norm, 0))
    {
        float rcp  = 1.0f / norm;
        dst_xyzw[0] = -src_xyzw[0] * rcp;
        dst_xyzw[1] = -src_xyzw[1] * rcp;
        dst_xyzw[2] = -src_xyzw[2] * rcp;
        dst_xyzw[3] =  src_xyzw[3] * rcp;
        return dst_xyzw;
    }
    else
    {
        quat_set_pinf(dst_xyzw);
        return dst_xyzw;
    }
}

float* quat_nrm(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    float length = quat_len(src_xyzw);
    if (!eq(length, 0))
    {
        float rcp  = 1.0f / length;
        dst_xyzw[0] = src_xyzw[0] * rcp;
        dst_xyzw[1] = src_xyzw[1] * rcp;
        dst_xyzw[2] = src_xyzw[2] * rcp;
        dst_xyzw[3] = src_xyzw[3] * rcp;
        return dst_xyzw;
    }
    else
    {
        quat_set_pinf(dst_xyzw);
        return dst_xyzw;
    }
}

float* quat_exp(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    float ax = src_xyzw[0], ay = src_xyzw[1], az = src_xyzw[2];
    float t  = sqrtf(ax * ax + ay * ay + az * az);
    float st = sinf(t);
    float w  = cosf(t);

    if (!eq(st, 0))
    {
        float c     = st / t;
        dst_xyzw[0] = ax * c;
        dst_xyzw[1] = ay * c;
        dst_xyzw[2] = az * c;
        dst_xyzw[3] = w;
        return dst_xyzw;
    }
    else
    {
        dst_xyzw[0] = ax;
        dst_xyzw[1] = ay;
        dst_xyzw[2] = az;
        dst_xyzw[3] = w;
        return dst_xyzw;
    }
}

float* quat_log(float * restrict dst_xyzw, float const * restrict src_xyzw)
{
    float ax = src_xyzw[0], ay = src_xyzw[1], az = src_xyzw[2], aw = src_xyzw[3];

    dst_xyzw[0] = ax;
    dst_xyzw[1] = ay;
    dst_xyzw[2] = az;
    dst_xyzw[3] = 0;

    if (fabsf(aw) < 1.0f)
    {
        float t   = acosf(aw);
        float st  = sinf(t);
        if (!eq(st, 0))
        {
            float c     = t  / st;
            dst_xyzw[0] = ax * c;
            dst_xyzw[1] = ay * c;
            dst_xyzw[2] = az * c;
        }
    }
    return dst_xyzw;
}

float* quat_closest(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw)
{
    if (quat_dot(a_xyzw, b_xyzw) < 0.0f)
    {
        return quat_neg(dst_xyzw, a_xyzw);
    }
    else
    {
        dst_xyzw[0] = a_xyzw[0];
        dst_xyzw[1] = a_xyzw[1];
        dst_xyzw[2] = a_xyzw[2];
        dst_xyzw[3] = a_xyzw[3];
        return dst_xyzw;
    }
}

float* quat_orient_vec3(float * restrict dst_xyzw, float const * restrict from_xyz, float const * restrict to_xyz)
{
    float unit_from[3];
    float unit_to[3];
    float x_from_to[3];
    float dp_from_to;
    float rcp_scale;

    vec3_nrm(unit_to,     to_xyz);
    vec3_nrm(unit_from,   from_xyz);
    vec3_cross(x_from_to, unit_from, unit_to);
    vec3_dot(dp_from_to,  unit_from, unit_to);
    rcp_scale   = 1.0f / (sqrtf((1.0f + dp_from_to) * 2.0f));
    dst_xyzw[0] = x_from_to[0] * rcp_scale;
    dst_xyzw[1] = x_from_to[1] * rcp_scale;
    dst_xyzw[2] = x_from_to[2] * rcp_scale;
    dst_xyzw[3] = 0.5f         * rcp_scale;
    return dst_xyzw;
}

float* quat_orient_vec4(float * restrict dst_xyzw, float const * restrict from_xyzw, float const * restrict to_xyzw)
{
    float unit_from[3];
    float unit_to[3];
    float x_from_to[3];
    float dp_from_to;
    float rcp_scale;

    vec3_nrm(unit_to,    to_xyzw);
    vec3_nrm(unit_from,  from_xyzw);
    vec3_cross(x_from_to,unit_from, unit_to);
    vec3_dot(dp_from_to, unit_from, unit_to);
    rcp_scale   = 1.0f / sqrtf((1.0f + dp_from_to) * 2.0f);
    dst_xyzw[0] = x_from_to[0] * rcp_scale;
    dst_xyzw[1] = x_from_to[1] * rcp_scale;
    dst_xyzw[2] = x_from_to[2] * rcp_scale;
    dst_xyzw[3] = 0.5f         * rcp_scale;
    return dst_xyzw;
}

float* quat_set_mat4(float * restrict dst_xyzw, float const * restrict m16)
{
    float trace_plus_one = 1.0f + m16[0] + m16[5] + m16[10];
    if   (trace_plus_one > 0.0f)
    {
        float s     = 2.0f  * sqrtf(trace_plus_one);
        float rcp_s = 1.0f  / s;
        dst_xyzw[0] = rcp_s * (m16[6] - m16[9]);
        dst_xyzw[1] = rcp_s * (m16[8] - m16[2]);
        dst_xyzw[2] = rcp_s * (m16[1] - m16[4]);
        dst_xyzw[3] = 0.25f * s;
        return dst_xyzw;
    }
    else
    {
        if (m16[0] > m16[5] && m16[0] > m16[10])
        {
            float s     = 2.0f  * sqrtf(1.0f + m16[0] - m16[5] - m16[10]);
            float rcp_s = 1.0f  / s;
            dst_xyzw[0] = 0.25f * s;
            dst_xyzw[1] = rcp_s * (m16[1] + m16[4]);
            dst_xyzw[2] = rcp_s * (m16[8] + m16[2]);
            dst_xyzw[3] = rcp_s * (m16[6] - m16[9]);
            return dst_xyzw;
        }
        else if (m16[5] > m16[10])
        {
            float s     = 2.0f  * sqrtf(1.0f + m16[5] - m16[0] - m16[10]);
            float rcp_s = 1.0f  / s;
            dst_xyzw[0] = rcp_s * (m16[1] + m16[4]);
            dst_xyzw[1] = 0.25f * s;
            dst_xyzw[2] = rcp_s * (m16[6] + m16[9]);
            dst_xyzw[3] = rcp_s * (m16[8] - m16[2]);
            return dst_xyzw;
        }
        else
        {
            float s     = 2.0f  * sqrtf(1.0f + m16[10] - m16[0] - m16[5]);
            float rcp_s = 1.0f  / s;
            dst_xyzw[0] = rcp_s * (m16[8] + m16[2]);
            dst_xyzw[1] = rcp_s * (m16[6] + m16[9]);
            dst_xyzw[2] = 0.25f * s;
            dst_xyzw[3] = rcp_s * (m16[1] - m16[4]);
            return dst_xyzw;
        }
    }
}

float* quat_set_euler_degree(float *dst_xyzw, float deg_x, float deg_y, float deg_z)
{
    return quat_set_euler_radian(dst_xyzw, rad(deg_x), rad(deg_y), rad(deg_z));
}

float* quat_set_euler_radian(float *dst_xyzw, float rad_x, float rad_y, float rad_z)
{
    float qx[4], qy[4], qz[4], qxy[4];
    float half_x = 0.5f * rad_x;
    float half_y = 0.5f * rad_y;
    float half_z = 0.5f * rad_z;
    float sin_x  = sinf(half_x);
    float cos_x  = cosf(half_x);
    float sin_y  = sinf(half_y);
    float cos_y  = cosf(half_y);
    float sin_z  = sinf(half_z);
    float cos_z  = cosf(half_z);

    // angle-axis to quat: x-axis
    qx[0] = sin_x;
    qx[1] = 0.0f;
    qx[2] = 0.0f;
    qx[3] = cos_x;

    // angle-axis to quat: y-axis
    qy[0] = 0.0f;
    qy[1] = sin_y;
    qy[2] = 0.0f;
    qy[3] = cos_y;

    // angle-axis to quat: z-axis
    qz[0] = 0.0f;
    qz[1] = 0.0f;
    qz[2] = sin_z;
    qz[3] = cos_z;

    quat_mul(qxy, qx, qy);
    return quat_mul(dst_xyzw, qxy, qz);
}

float* quat_set_angle_axis_degree_n(float * restrict dst_xyzw, float angle_deg, float const *axis_xyz)
{
    return quat_set_angle_axis_radian_n(dst_xyzw, rad(angle_deg), axis_xyz);
}

float* quat_set_angle_axis_radian_n(float * restrict dst_xyzw, float angle_rad, float const * restrict axis_xyz)
{
    float half_angle = 0.5f * angle_rad;
    float sin_angle  = sinf(half_angle);
    float cos_angle  = cosf(half_angle);
    dst_xyzw[0] = axis_xyz[0] * sin_angle;
    dst_xyzw[1] = axis_xyz[1] * sin_angle;
    dst_xyzw[2] = axis_xyz[2] * sin_angle;
    dst_xyzw[3] = cos_angle;
    return dst_xyzw;
}

float* quat_set_angle_axis_degree_u(float * restrict dst_xyzw, float angle_deg, float const * restrict axis_xyz)
{
    return quat_set_angle_axis_radian_u(dst_xyzw, rad(angle_deg), axis_xyz);
}

float* quat_set_angle_axis_radian_u(float * restrict dst_xyzw, float angle_rad, float const * restrict axis_xyz)
{
    float half_angle  = 0.5f * angle_rad;
    float sin_angle   = sinf(half_angle);
    float cos_angle   = cosf(half_angle);
    float unit_axis[3];
    vec3_nrm(unit_axis, axis_xyz);
    dst_xyzw[0] = unit_axis[0] * sin_angle;
    dst_xyzw[1] = unit_axis[1] * sin_angle;
    dst_xyzw[2] = unit_axis[2] * sin_angle;
    dst_xyzw[3] = cos_angle;
    return dst_xyzw;
}

float* quat_linear(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float t)
{
    dst_xyzw[0] = linear(a_xyzw[0], b_xyzw[0], t);
    dst_xyzw[1] = linear(a_xyzw[1], b_xyzw[1], t);
    dst_xyzw[2] = linear(a_xyzw[2], b_xyzw[2], t);
    dst_xyzw[3] = linear(a_xyzw[3], b_xyzw[3], t);
    return dst_xyzw;
}

float* quat_slerp(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float t)
{
    float ax  = a_xyzw[0], ay = a_xyzw[1], az = a_xyzw[2], aw = a_xyzw[3];
    float bx  = b_xyzw[0], by = b_xyzw[1], bz = b_xyzw[2], bw = b_xyzw[3];
    float omt = 1.0f - t;
    float co  = cosf(ax * bx + ay * by + az * bz + aw * bw);
    float s1  = 0.0f;
    float s2  = 0.0f;
    float q[4];

    quat_set_quat(q, b_xyzw);

    if (co < 0.0f)
    {
        co   = -co;
        q[0] = -bx;
        q[1] = -by;
        q[2] = -bz;
        q[3] = -bw;
    }

    if (!eq(1.0f - co, 0))
    {
        float om = acosf(co);
        float so = sinf(om);
        s1 = sinf(omt * om) / so;
        s2 = sinf(t   * om) / so;
    }
    else
    {
        // a and b are close; perform linear interpolation.
        s1 = omt;
        s2 = t;
    }

    dst_xyzw[0] = ax * s1 + q[0] * s2;
    dst_xyzw[1] = ay * s1 + q[1] * s2;
    dst_xyzw[2] = az * s1 + q[2] * s2;
    dst_xyzw[3] = aw * s1 + q[3] * s2;
    return dst_xyzw;
}

float* quat_squad(
    float       * restrict dst_xyzw,
    float const * restrict p_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict q_xyzw,
    float                  t)
{
    float at[4];
    float bt[4];
    quat_slerp(at,      p_xyzw,  q_xyzw, t);
    quat_slerp(bt,      a_xyzw,  b_xyzw, t);
    quat_slerp(dst_xyzw, at,     bt,     2.0f * t * (1.0f * t));
    return dst_xyzw;
}

float* quat_spline(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict c_xyzw)
{
    float ab[4];
    float ac[4];
    float inv_a[4];
    float inv_ab[4];
    float inv_ac[4];
    float log_iab[4];
    float log_iac[4];
    float sum_labac[4];
    float scl_labac[4];
    float exp_labac[4];
    quat_inv(inv_a, a_xyzw);
    quat_mul(ab, inv_a, b_xyzw);
    quat_mul(ac, inv_a, c_xyzw);
    quat_nrm(inv_ab, ab);
    quat_nrm(inv_ac, ac);
    quat_log(log_iab, inv_ab);
    quat_log(log_iac, inv_ac);
    quat_add(sum_labac, log_iac,  log_iab);
    quat_scl(scl_labac, sum_labac, -0.25f);
    quat_exp(exp_labac, scl_labac);
    quat_mul(dst_xyzw, a_xyzw, exp_labac);
    return dst_xyzw;
}

float* mat4_set(
    float *dst16,
    float  r0c0, float r0c1, float r0c2, float r0c3,
    float  r1c0, float r1c1, float r1c2, float r1c3,
    float  r2c0, float r2c1, float r2c2, float r2c3,
    float  r3c0, float r3c1, float r3c2, float r3c3)
{
    dst16[0]  = r0c0; dst16[1]  = r1c0; dst16[2]  = r2c0; dst16[3]  = r3c0;
    dst16[4]  = r0c1; dst16[5]  = r1c1; dst16[6]  = r2c1; dst16[7]  = r3c1;
    dst16[8]  = r0c2; dst16[9]  = r1c2; dst16[10] = r2c2; dst16[11] = r3c2;
    dst16[12] = r0c3; dst16[13] = r1c3; dst16[14] = r2c3; dst16[15] = r3c3;
    return dst16;
}

float* mat4_set_mat4(float * restrict dst16, float const * restrict src16)
{
    for (size_t i = 0; i < 16; ++i)
    {
        dst16[i] = src16[i];
    }
    return dst16;
}

float* mat4_set_nan(float *dst16)
{
    float qnan = *(float*) &F32_QNaN;
    for (size_t i = 0; i < 16; ++i)
    {
        dst16[i] = qnan;
    }
    return dst16;
}

float* mat4_set_one(float *dst16)
{
    for (size_t i = 0; i < 16; ++i)
    {
        dst16[i] = 1.0f;
    }
    return dst16;
}

float* mat4_set_zero(float *dst16)
{
    for (size_t i = 0; i < 16; ++i)
    {
        dst16[i] = 0.0f;
    }
    return dst16;
}

float* mat4_set_ninf(float *dst16)
{
    float ninf = *(float*) &F32_NInf;
    for (size_t i = 0; i < 16; ++i)
    {
        dst16[i] = ninf;
    }
    return dst16;
}

float* mat4_set_pinf(float *dst16)
{
    float pinf = *(float*) &F32_PInf;
    for (size_t i = 0; i < 16; ++i)
    {
        dst16[i] = pinf;
    }
    return dst16;
}

float* mat4_set_ident(float *dst16)
{
    dst16[0]  = 1.0f; dst16[1]  = 0.0f; dst16[2]  = 0.0f; dst16[3]  = 0.0f;
    dst16[4]  = 0.0f; dst16[5]  = 1.0f; dst16[6]  = 0.0f; dst16[7]  = 0.0f;
    dst16[8]  = 0.0f; dst16[9]  = 0.0f; dst16[10] = 1.0f; dst16[11] = 0.0f;
    dst16[12] = 0.0f; dst16[13] = 0.0f; dst16[14] = 0.0f; dst16[15] = 1.0f;
    return dst16;
}

bool mat4_eq(float const * restrict a16, float const * restrict b16)
{
    for (size_t i = 0; i < 16; ++i)
    {
        if (!eq(a16[i], b16[i]))
            return false;
    }
    return true;
}

bool mat4_is_identity(float const *src16)
{
    if (!eq(src16[0],  1.0f))
        return false;
    if (!eq(src16[1],  0.0f))
        return false;
    if (!eq(src16[2],  0.0f))
        return false;
    if (!eq(src16[3],  0.0f))
        return false;
    if (!eq(src16[4],  0.0f))
        return false;
    if (!eq(src16[5],  1.0f))
        return false;
    if (!eq(src16[6],  0.0f))
        return false;
    if (!eq(src16[7],  0.0f))
        return false;
    if (!eq(src16[8],  0.0f))
        return false;
    if (!eq(src16[9],  0.0f))
        return false;
    if (!eq(src16[10], 1.0f))
        return false;
    if (!eq(src16[11], 0.0f))
        return false;
    if (!eq(src16[12], 0.0f))
        return false;
    if (!eq(src16[13], 0.0f))
        return false;
    if (!eq(src16[14], 0.0f))
        return false;
    if (!eq(src16[15], 1.0f))
        return false;

    return true;
}

float* mat4_set_rows(
    float       * restrict dst16,
    float const * restrict r0_xyzw,
    float const * restrict r1_xyzw,
    float const * restrict r2_xyzw,
    float const * restrict r3_xyzw)
{
    dst16[0]  = r0_xyzw[0]; dst16[1] = r1_xyzw[0]; dst16[2] = r2_xyzw[0]; dst16[3] = r3_xyzw[0];
    dst16[4]  = r0_xyzw[1]; dst16[5] = r1_xyzw[1]; dst16[6] = r2_xyzw[1]; dst16[7] = r3_xyzw[1];
    dst16[8]  = r0_xyzw[2]; dst16[9] = r1_xyzw[2]; dst16[10]= r2_xyzw[2]; dst16[11]= r3_xyzw[2];
    dst16[12] = r0_xyzw[3]; dst16[13]= r1_xyzw[3]; dst16[14]= r2_xyzw[3]; dst16[15]= r3_xyzw[3];
    return dst16;
}

float* mat4_set_cols(
    float       * restrict dst16,
    float const * restrict c0_xyzw,
    float const * restrict c1_xyzw,
    float const * restrict c2_xyzw,
    float const * restrict c3_xyzw)
{
    dst16[0]  = c0_xyzw[0]; dst16[1] = c0_xyzw[1]; dst16[2] = c0_xyzw[2]; dst16[3] = c0_xyzw[3];
    dst16[4]  = c1_xyzw[0]; dst16[5] = c1_xyzw[1]; dst16[6] = c1_xyzw[2]; dst16[7] = c1_xyzw[3];
    dst16[8]  = c2_xyzw[0]; dst16[9] = c2_xyzw[1]; dst16[10]= c2_xyzw[2]; dst16[11]= c2_xyzw[3];
    dst16[12] = c3_xyzw[0]; dst16[13]= c3_xyzw[1]; dst16[14]= c3_xyzw[2]; dst16[15]= c3_xyzw[3];
    return dst16;
}

float* mat4_get_row(float * restrict dst_xyzw, size_t row, float const * restrict src16)
{
    dst_xyzw[0] = src16[row +  0];
    dst_xyzw[1] = src16[row +  4];
    dst_xyzw[2] = src16[row +  8];
    dst_xyzw[3] = src16[row + 12];
    return dst_xyzw;
}

float* mat4_set_row(float * restrict dst16, size_t row, float const * restrict src_xyzw)
{
    dst16[row +  0] = src_xyzw[0];
    dst16[row +  4] = src_xyzw[1];
    dst16[row +  8] = src_xyzw[2];
    dst16[row + 12] = src_xyzw[3];
    return dst16;
}

float* mat4_get_col(float * restrict dst_xyzw, size_t col, float const * restrict src16)
{
    dst_xyzw[0] = src16[col * 4 + 0];
    dst_xyzw[1] = src16[col * 4 + 1];
    dst_xyzw[2] = src16[col * 4 + 2];
    dst_xyzw[3] = src16[col * 4 + 3];
    return dst_xyzw;
}

float* mat4_set_col(float * restrict dst16, size_t col, float const * restrict src_xyzw)
{
    dst16[col * 4 + 0] = src_xyzw[0];
    dst16[col * 4 + 1] = src_xyzw[1];
    dst16[col * 4 + 2] = src_xyzw[2];
    dst16[col * 4 + 3] = src_xyzw[3];
    return dst16;
}

float mat4_trace(float const *src16)
{
    return (src16[0] + src16[5] + src16[10] + src16[15]);
}

float mat4_det(float const *src16)
{
    float c0  = src16[5] * src16[10] - src16[6] * src16[9];
    float c4  = src16[2] * src16[9]  - src16[1] * src16[10];
    float c8  = src16[1] * src16[6]  - src16[2] * src16[5];
    return src16[0] * c0 + src16[4]  * c4 + src16[8] * c8;
}

float* mat4x4_transpose(float * restrict dst16, float const * restrict src16)
{
    float src0   = src16[0];
    float src1   = src16[1];
    float src2   = src16[2];
    float src3   = src16[3];
    float src4   = src16[4];
    float src5   = src16[5];
    float src6   = src16[6];
    float src7   = src16[7];
    float src8   = src16[8];
    float src9   = src16[9];
    float src10  = src16[10];
    float src11  = src16[11];
    float src12  = src16[12];
    float src13  = src16[13];
    float src14  = src16[14];
    float src15  = src16[15];

    dst16[0]  = src0;
    dst16[1]  = src4;
    dst16[2]  = src8;
    dst16[3]  = src12;

    dst16[4]  = src1;
    dst16[5]  = src5;
    dst16[6]  = src9;
    dst16[7]  = src13;

    dst16[8]  = src2;
    dst16[9]  = src6;
    dst16[10] = src10;
    dst16[11] = src14;

    dst16[12] = src3;
    dst16[13] = src7;
    dst16[14] = src11;
    dst16[15] = src15;

    return dst16;
}

float* mat4_concat(float * restrict dst16, float const * restrict a16, float const * restrict b16)
{
    // transformation 'a' is applied first, followed by transformation 'b'.
    // result is the dot product of the columns of 'a' and the rows of 'b'.
    float a0   = a16[0];
    float a1   = a16[1];
    float a2   = a16[2];
    float a3   = a16[3];
    float a4   = a16[4];
    float a5   = a16[5];
    float a6   = a16[6];
    float a7   = a16[7];
    float a8   = a16[8];
    float a9   = a16[9];
    float a10  = a16[10];
    float a11  = a16[11];
    float a12  = a16[12];
    float a13  = a16[13];
    float a14  = a16[14];
    float a15  = a16[15];
    float b0   = b16[0];
    float b1   = b16[1];
    float b2   = b16[2];
    float b3   = b16[3];
    float b4   = b16[4];
    float b5   = b16[5];
    float b6   = b16[6];
    float b7   = b16[7];
    float b8   = b16[8];
    float b9   = b16[9];
    float b10  = b16[10];
    float b11  = b16[11];
    float b12  = b16[12];
    float b13  = b16[13];
    float b14  = b16[14];
    float b15  = b16[15];

    dst16[0]   = b0 * a0  + b4 * a1  + b8  * a2  + b12 * a3;
    dst16[1]   = b1 * a0  + b5 * a1  + b9  * a2  + b13 * a3;
    dst16[2]   = b2 * a0  + b6 * a1  + b10 * a2  + b14 * a3;
    dst16[3]   = b3 * a0  + b7 * a1  + b11 * a2  + b15 * a3;

    dst16[4]   = b0 * a4  + b4 * a5  + b8  * a6  + b12 * a7;
    dst16[5]   = b1 * a4  + b5 * a5  + b9  * a6  + b13 * a7;
    dst16[6]   = b2 * a4  + b6 * a5  + b10 * a6  + b14 * a7;
    dst16[7]   = b3 * a4  + b7 * a5  + b11 * a6  + b15 * a7;

    dst16[8]   = b0 * a8  + b4 * a9  + b8  * a10 + b12 * a11;
    dst16[9]   = b1 * a8  + b5 * a9  + b9  * a10 + b13 * a11;
    dst16[10]  = b2 * a8  + b6 * a9  + b10 * a10 + b14 * a11;
    dst16[11]  = b3 * a8  + b7 * a9  + b11 * a10 + b15 * a11;

    dst16[12]  = b0 * a12 + b4 * a13 + b8  * a14 + b12 * a15;
    dst16[13]  = b1 * a12 + b5 * a13 + b9  * a14 + b13 * a15;
    dst16[14]  = b2 * a12 + b6 * a13 + b10 * a14 + b14 * a15;
    dst16[15]  = b3 * a12 + b7 * a13 + b11 * a14 + b15 * a15;

    return dst16;
}

float* mat4_inv_affine(float * restrict dst16, float const * restrict src16)
{
    float c0  = src16[5] * src16[10] - src16[6] * src16[9];
    float c4  = src16[2] * src16[9]  - src16[1] * src16[10];
    float c8  = src16[1] * src16[6]  - src16[2] * src16[5];
    float det = src16[0] * c0 + src16[4] * c4 + src16[8] * c8;

    if (!eq(det, 0))
    {
        float rcp  = 1.0f / det;
        float r0c0 = rcp  * c0;
        float r1c0 = rcp  * c4;
        float r2c0 = rcp  * c8;
        float r0c1 = rcp  * (src16[6] * src16[8]  - src16[4]    * src16[10]);
        float r1c1 = rcp  * (src16[0] * src16[10] - src16[2]    * src16[8]);
        float r2c1 = rcp  * (src16[2] * src16[4]  - src16[0]    * src16[6]);
        float r0c2 = rcp  * (src16[4] * src16[9]  - src16[5]    * src16[8]);
        float r1c2 = rcp  * (src16[1] * src16[8]  - src16[0]    * src16[9]);
        float r2c2 = rcp  * (src16[0] * src16[5]  - src16[1]    * src16[4]);
        float r0c3 =-r0c0 * src16[12] - r0c1 * src16[13] - r0c2 * src16[14];
        float r1c3 =-r1c0 * src16[12] - r1c1 * src16[13] - r1c2 * src16[14];
        float r2c3 =-r2c0 * src16[12] - r2c1 * src16[13] - r2c2 * src16[14];

        dst16[0]   = r0c0;  dst16[1]  = r1c0;  dst16[2]  = r2c0;  dst16[3]  = 0.0f;
        dst16[4]   = r0c1;  dst16[5]  = r1c1;  dst16[6]  = r2c1;  dst16[7]  = 0.0f;
        dst16[8]   = r0c2;  dst16[9]  = r1c2;  dst16[10] = r2c2;  dst16[11] = 0.0f;
        dst16[12]  = r0c3;  dst16[13] = r1c3;  dst16[14] = r2c3;  dst16[15] = 1.0f;
        return dst16;
    }
    else
    {
        // singlular matrix.
        return mat4_set_zero(dst16);
    }
}

float* mat4_set_quat(float * restrict dst16, float const * restrict src_xyzw)
{
    float xx  = src_xyzw[0] * src_xyzw[0];
    float xy  = src_xyzw[0] * src_xyzw[1];
    float xz  = src_xyzw[0] * src_xyzw[2];
    float xw  = src_xyzw[0] * src_xyzw[3];
    float yy  = src_xyzw[1] * src_xyzw[1];
    float yz  = src_xyzw[1] * src_xyzw[2];
    float yw  = src_xyzw[1] * src_xyzw[3];
    float zz  = src_xyzw[2] * src_xyzw[2];
    float zw  = src_xyzw[2] * src_xyzw[3];
    dst16[0]  = 1.0f - 2.0f * (yy + zz);
    dst16[1]  = 2.0f * (xy  +  zw);
    dst16[2]  = 2.0f * (xz  -  yw);
    dst16[3]  = 0.0f;
    dst16[4]  = 2.0f * (xy  -  zw);
    dst16[5]  = 1.0f - 2.0f * (xx + zz);
    dst16[6]  = 2.0f * (yz  +  xw);
    dst16[7]  = 0.0f;
    dst16[8]  = 2.0f * (xz  +  yw);
    dst16[9]  = 2.0f * (yz  -  xw);
    dst16[10] = 1.0f - 2.0f * (xx + yy);
    dst16[11] = 0.0f;
    dst16[12] = 0.0f;
    dst16[13] = 0.0f;
    dst16[14] = 0.0f;
    dst16[15] = 1.0f;
    return dst16;
}

float* mat4_set_euler_degree_x(float *dst16, float deg_x)
{
    return mat4_set_euler_radian_x(dst16, rad(deg_x));
}

float* mat4_set_euler_radian_x(float *dst16, float rad_x)
{
    float sa  = sinf(rad_x);
    float ca  = cosf(rad_x);
    dst16[0]  = 1.0f;  dst16[1]  = 0.0f;  dst16[2]  = 0.0f;  dst16[3]  = 0.0f;
    dst16[4]  = 0.0f;  dst16[5]  = ca;    dst16[6]  = sa;    dst16[7]  = 0.0f;
    dst16[8]  = 0.0f;  dst16[9]  =-sa;    dst16[10] = ca;    dst16[11] = 0.0f;
    dst16[12] = 0.0f;  dst16[13] = 0.0f;  dst16[14] = 0.0f;  dst16[15] = 1.0f;
    return dst16;
}

float* mat4_set_euler_degree_y(float *dst16, float deg_y)
{
    return mat4_set_euler_radian_y(dst16, rad(deg_y));
}

float* mat4_set_euler_radian_y(float *dst16, float rad_y)
{
    float sa  = sinf(rad_y);
    float ca  = cosf(rad_y);
    dst16[0]  = ca;    dst16[1]  = 0.0f;  dst16[2]  =-sa;    dst16[3]  = 0.0f;
    dst16[4]  = 0.0f;  dst16[5]  = 1.0f;  dst16[6]  = 0.0f;  dst16[7]  = 0.0f;
    dst16[8]  = sa;    dst16[9]  = 0.0f;  dst16[10] = ca;    dst16[11] = 0.0f;
    dst16[12] = 0.0f;  dst16[13] = 0.0f;  dst16[14] = 0.0f;  dst16[15] = 1.0f;
    return dst16;
}

float* mat4_set_euler_degree_z(float *dst16, float deg_z)
{
    return mat4_set_euler_radian_z(dst16, rad(deg_z));
}

float* mat4_set_euler_radian_z(float *dst16, float rad_z)
{
    float sa  = sinf(rad_z);
    float ca  = cosf(rad_z);
    dst16[0]  = ca;    dst16[1]  = sa;    dst16[2]  = 0.0f;  dst16[3]  = 0.0f;
    dst16[4]  =-sa;    dst16[5]  = ca;    dst16[6]  = 0.0f;  dst16[7]  = 0.0f;
    dst16[8]  = 0.0f;  dst16[9]  = 0.0f;  dst16[10] = 1.0f;  dst16[11] = 0.0f;
    dst16[12] = 0.0f;  dst16[13] = 0.0f;  dst16[14] = 0.0f;  dst16[15] = 1.0f;
    return dst16;
}

float* mat4_set_euler_degree(float *dst16, float deg_x, float deg_y, float deg_z)
{
    return mat4_set_euler_radian(dst16, rad(deg_x), rad(deg_y), rad(deg_z));
}

float* mat4_set_euler_radian(float *dst16, float rad_x, float rad_y, float rad_z)
{
    float sx  = sinf(rad_x);
    float cx  = cosf(rad_x);
    float sy  = sinf(rad_y);
    float cy  = cosf(rad_y);
    float sz  = sinf(rad_z);
    float cz  = cosf(rad_z);
    dst16[0]  = (cy * cz);  dst16[1]  = (sx * sy * cz) + (cx * sz);  dst16[2]  = -(cx * sy * cz) + (sx * sz);  dst16[3]  = 0.0f;
    dst16[4]  =-(cy * sz);  dst16[5]  =-(sx * sy * sz) + (cx * cz);  dst16[6]  =  (cx * sy * sz) + (sx * cz);  dst16[7]  = 0.0f;
    dst16[8]  = (sy);       dst16[9]  =-(sx * cy);                   dst16[10] =  (cx * cy);                   dst16[11] = 0.0f;
    dst16[12] = 0.0f;       dst16[13] = 0.0f;                        dst16[14] =  0.0f;                        dst16[15] = 1.0f;
    return dst16;
}

float* mat4_set_angle_axis_degree_n(float * restrict dst16, float angle_deg, float const * restrict axis_xyz)
{
    return mat4_set_angle_axis_radian_n(dst16, rad(angle_deg), axis_xyz);
}

float* mat4_set_angle_axis_radian_n(float * restrict dst16, float angle_rad, float const * restrict axis_xyz)
{
    float sa  = sinf(angle_rad);
    float ca  = cosf(angle_rad);
    float t   = 1.0f- ca;
    float tx  = t   * axis_xyz[0];
    float ty  = t   * axis_xyz[1];
    float tz  = t   * axis_xyz[2];
    float sx  = sa  * axis_xyz[0];
    float sy  = sa  * axis_xyz[1];
    float sz  = sa  * axis_xyz[2];
    float txy = tx  * axis_xyz[1];
    float tyz = ty  * axis_xyz[2];
    float txz = tx  * axis_xyz[2];
    dst16[0]  = tx  * axis_xyz[0] + ca;
    dst16[1]  = txy + sz;
    dst16[2]  = txz - sy;
    dst16[3]  = 0.0f;
    dst16[4]  = txy - sz;
    dst16[5]  = ty  * axis_xyz[1] + ca;
    dst16[6]  = tyz + sx;
    dst16[7]  = 0.0f;
    dst16[8]  = txz + sy;
    dst16[9]  = tyz - sx;
    dst16[10] = tz  * axis_xyz[2] + ca;
    dst16[11] = 0.0f;
    dst16[12] = 0.0f;
    dst16[13] = 0.0f;
    dst16[14] = 0.0f;
    dst16[15] = 1.0f;
    return dst16;
}

float* mat4_set_angle_axis_degree_u(float * restrict dst16, float angle_deg, float const * restrict axis_xyz)
{
    float n[3];
    vec3_nrm(n, axis_xyz);
    return mat4_set_angle_axis_radian_n(dst16, rad(angle_deg), n);
}

float* mat4_set_angle_axis_radian_u(float * restrict dst16, float angle_rad, float const * restrict axis_xyz)
{
    float n[3];
    vec3_nrm(n, axis_xyz);
    return mat4_set_angle_axis_radian_n(dst16, angle_rad, n);
}

float* mat4_look_at(float * restrict dst16, float const * restrict pos_xyz, float const * restrict target_xyz, float const * restrict up_xyz)
{
    float d1, d2, d3;
    float tx, ty, tz;
    float z[3], x[3], y[3], zn[3], xn[3];
    vec3_sub(z, pos_xyz, target_xyz); // left-handed would be sub(target, pos).
    vec3_nrm(zn, z);
    vec3_cross(x, up_xyz, zn);
    vec3_nrm(xn, x);
    vec3_cross(y, zn, xn);
    tx = -vec3_dot(d1, xn, pos_xyz);
    ty = -vec3_dot(d2, y,  pos_xyz);
    tz = -vec3_dot(d3, zn, pos_xyz);
    dst16[0]  = x[0];  dst16[1]  = y[0];  dst16[2]  = z[0];  dst16[3]  = 0.0f;
    dst16[4]  = x[1];  dst16[5]  = y[1];  dst16[6]  = z[1];  dst16[7]  = 0.0f;
    dst16[8]  = x[2];  dst16[9]  = y[2];  dst16[10] = z[2];  dst16[11] = 0.0f;
    dst16[12] = tx;    dst16[13] = ty;    dst16[14] = tz;    dst16[15] = 1.0f;
    return dst16;
}

float* mat4_scale(float *dst16, float s_x, float s_y, float s_z)
{
    dst16[0]  = s_x;   dst16[1]  = 0.0f;  dst16[2]  = 0.0f;  dst16[3]  = 0.0f;
    dst16[4]  = 0.0f;  dst16[5]  = s_y;   dst16[6]  = 0.0f;  dst16[7]  = 0.0f;
    dst16[8]  = 0.0f;  dst16[9]  = 0.0f;  dst16[10] = s_z;   dst16[11] = 0.0f;
    dst16[12] = 0.0f;  dst16[13] = 0.0f;  dst16[14] = 0.0f;  dst16[15] = 1.0f;
    return dst16;
}

float* mat4_trans(float *dst16, float t_x, float t_y, float t_z)
{
    dst16[0]  = 1.0f;  dst16[1]  = 0.0f;  dst16[2]  = 0.0f;  dst16[3]  = 0.0f;
    dst16[4]  = 0.0f;  dst16[5]  = 1.0f;  dst16[6]  = 0.0f;  dst16[7]  = 0.0f;
    dst16[8]  = 0.0f;  dst16[9]  = 0.0f;  dst16[10] = 1.0f;  dst16[11] = 0.0f;
    dst16[12] = t_x;   dst16[13] = t_y;   dst16[14] = t_z;   dst16[15] = 1.0f;
    return dst16;
}

float* mat4_ortho(float *dst16, float left, float right, float bottom, float top, float near, float far)
{
    float rcpx = 1.0f / (right - left);
    float rcpy = 1.0f / (top   - bottom);
    float rcpz = 1.0f / (near  - far);
    float rpl  = right+ left;
    float tpb  = top  + bottom;
    float npf  = near + far;
    dst16[0]   = 2.0f * rcpx;  dst16[1]  = 0.0f;         dst16[2]  = 0.0f;        dst16[3]  = 0.0f;
    dst16[4]   = 0.0f;         dst16[5]  = 2.0f * rcpy;  dst16[6]  = 0.0f;        dst16[7]  = 0.0f;
    dst16[8]   = 0.0f;         dst16[9]  = 0.0f;         dst16[10] = 2.0f * rcpz; dst16[11] = 0.0f;
    dst16[12]  =-rpl  * rcpx;  dst16[13] =-tpb  * rcpy;  dst16[14] = npf  * rcpz; dst16[15] = 1.0f;
    return dst16;
}

float* mat4_persp_degree(float *dst16, float fov_deg, float aspect, float near, float far)
{
    return mat4_persp_radian(dst16, rad(fov_deg), aspect, near, far);
}

float* mat4_persp_radian(float *dst16, float fov_rad, float aspect, float near, float far)
{
    float a   = aspect;
    float d   = 1.0f / tanf(fov_rad * 0.5f);
    float r   = 1.0f / (near - far);
    float nf  = near * far;
    float npf = near + far;
    dst16[0]  = d / a;  dst16[1]  = 0.0f;  dst16[2]  = 0.0f;          dst16[3]  = 0.0f;
    dst16[4]  = 0.0f;   dst16[5]  = d;     dst16[6]  = 0.0f;          dst16[7]  = 0.0f;
    dst16[8]  = 0.0f;   dst16[9]  = 0.0f;  dst16[10] = npf * r;       dst16[11] =-1.0f;
    dst16[12] = 0.0f;   dst16[13] = 0.0f;  dst16[14] = 2.0f * nf * r; dst16[15] = 0.0f;
    return dst16;
}

float* mat4_2d(float *dst16, float width, float height)
{
    float s[16], t[16];
    float s_x = 1.0f / (width  * 0.5f);
    float s_y = 1.0f / (height * 0.5f);
    mat4_scale(s, s_x, -s_y, 1.0f);
    mat4_trans(t, -1.0f, 1.0f, 0.0f);
    return mat4_concat(dst16, s, t);
}

void mat4_extract_frustum_n(
    float       * restrict left_xyzD,
    float       * restrict right_xyzD,
    float       * restrict top_xyzD,
    float       * restrict bottom_xyzD,
    float       * restrict near_xyzD,
    float       * restrict far_xyzD,
    float const * restrict src16)
{
    float l[4], r[4], t[4], b[4], n[4], f[4];
    mat4_extract_frustum_u(l, r, t, b, n, f, src16);
    vec3_nrm(left_xyzD,    l);
    vec3_nrm(right_xyzD,   r);
    vec3_nrm(top_xyzD,     t);
    vec3_nrm(bottom_xyzD,  b);
    vec3_nrm(near_xyzD,    n);
    vec3_nrm(far_xyzD,     f);
}

void mat4_extract_frustum_u(
    float       * restrict left_xyzD,
    float       * restrict right_xyzD,
    float       * restrict top_xyzD,
    float       * restrict bottom_xyzD,
    float       * restrict near_xyzD,
    float       * restrict far_xyzD,
    float const * restrict src16)
{
    left_xyzD[0]   = src16[3]  + src16[0];
    left_xyzD[1]   = src16[7]  + src16[4];
    left_xyzD[2]   = src16[11] + src16[8];
    left_xyzD[3]   = src16[15] + src16[12];

    right_xyzD[0]  = src16[3]  - src16[0];
    right_xyzD[1]  = src16[7]  - src16[4];
    right_xyzD[2]  = src16[11] - src16[8];
    right_xyzD[3]  = src16[15] - src16[12];

    top_xyzD[0]    = src16[3]  - src16[1];
    top_xyzD[1]    = src16[7]  - src16[5];
    top_xyzD[2]    = src16[11] - src16[9];
    top_xyzD[3]    = src16[15] - src16[13];

    bottom_xyzD[0] = src16[3]  + src16[1];
    bottom_xyzD[1] = src16[7]  + src16[5];
    bottom_xyzD[2] = src16[11] + src16[9];
    bottom_xyzD[3] = src16[15] + src16[13];

    near_xyzD[0]   = src16[3]  + src16[2];
    near_xyzD[1]   = src16[7]  + src16[6];
    near_xyzD[2]   = src16[11] + src16[10];
    near_xyzD[3]   = src16[15] + src16[14];

    far_xyzD[0]    = src16[3]  - src16[2];
    far_xyzD[1]    = src16[7]  - src16[6];
    far_xyzD[2]    = src16[11] - src16[10];
    far_xyzD[3]    = src16[15] - src16[14];
}

float* mat4_transform_vec3(
    float       * restrict dst_xyz,
    float const * restrict src_xyz,
    float const * restrict t16)
{
    float vx = src_xyz[0];
    float vy = src_xyz[1];
    float vz = src_xyz[2];

    dst_xyz[0] = t16[0] * vx + t16[4] * vy + t16[8]  * vz + t16[12];
    dst_xyz[1] = t16[1] * vx + t16[5] * vy + t16[9]  * vz + t16[13];
    dst_xyz[2] = t16[2] * vx + t16[6] * vy + t16[10] * vz + t16[14];

    return dst_xyz;
}

float* mat4_transform_vec4(
    float       * restrict dst_xyzw,
    float const * restrict src_xyzw,
    float const * restrict t16)
{
    float vx = src_xyzw[0];
    float vy = src_xyzw[1];
    float vz = src_xyzw[2];
    float vw = src_xyzw[3];

    dst_xyzw[0] = t16[0] * vx + t16[4] * vy + t16[8]  * vz + t16[12] * vw;
    dst_xyzw[1] = t16[1] * vx + t16[5] * vy + t16[9]  * vz + t16[13] * vw;
    dst_xyzw[2] = t16[2] * vx + t16[6] * vy + t16[10] * vz + t16[14] * vw;
    dst_xyzw[3] = t16[3] * vx + t16[7] * vy + t16[11] * vz + t16[15] * vw;

    return dst_xyzw;
}

float* mat4_transform_point(
    float       * restrict dst_xyz,
    float const * restrict src_xyz,
    float const * restrict t16)
{
    float vx = src_xyz[0];
    float vy = src_xyz[1];
    float vz = src_xyz[2];

    // for points, w = 1.0
    dst_xyz[0] = t16[0] * vx + t16[4] * vy + t16[8]  * vz + t16[12];
    dst_xyz[1] = t16[1] * vx + t16[5] * vy + t16[9]  * vz + t16[13];
    dst_xyz[2] = t16[2] * vx + t16[6] * vy + t16[10] * vz + t16[14];

    return dst_xyz;
}

float* mat4_transform_vector(
    float       * restrict dst_xyz,
    float const * restrict src_xyz,
    float const * restrict t16)
{
    float vx = src_xyz[0];
    float vy = src_xyz[1];
    float vz = src_xyz[2];

    // for vectors, w = 0.0
    dst_xyz[0] = t16[0] * vx + t16[4] * vy + t16[8]  * vz;
    dst_xyz[1] = t16[1] * vx + t16[5] * vy + t16[9]  * vz;
    dst_xyz[2] = t16[2] * vx + t16[6] * vy + t16[10] * vz;

    return dst_xyz;
}

float* mat4_transform_array_vec4(
    float       * restrict dst_xyzw,
    float const * restrict src_xyzw,
    float const * restrict t16,
    size_t                 count)
{
    float  *dst = dst_xyzw;
    for (size_t i = 0; i < count; ++i)
    {
        float x = src_xyzw[0];
        float y = src_xyzw[1];
        float z = src_xyzw[2];
        float w = src_xyzw[3];

        dst_xyzw[0]  = t16[0] * x + t16[4] * y + t16[8]  * z + t16[12] * w;
        dst_xyzw[1]  = t16[1] * x + t16[5] * y + t16[9]  * z + t16[13] * w;
        dst_xyzw[2]  = t16[2] * x + t16[6] * y + t16[10] * z + t16[14] * w;
        dst_xyzw[3]  = t16[3] * x + t16[7] * y + t16[11] * z + t16[15] * w;

        src_xyzw += 4;
        dst_xyzw += 4;
    }
    return dst;
}

float* mat4_transform_array_point(
    float       * restrict dst_xyz,
    float const * restrict src_xyz,
    float const * restrict t16,
    size_t                 count)
{
    float  *dst = dst_xyz;
    for (size_t i = 0; i < count; ++i)
    {
        float x = src_xyz[0];
        float y = src_xyz[1];
        float z = src_xyz[2];

        dst_xyz[0] = t16[0] * x + t16[4] * y + t16[8]  * z + t16[12];
        dst_xyz[1] = t16[1] * x + t16[5] * y + t16[9]  * z + t16[13];
        dst_xyz[2] = t16[2] * x + t16[6] * y + t16[10] * z + t16[14];

        src_xyz += 3;
        dst_xyz += 3;
    }
    return dst;
}

float* mat4_transform_array_vector(
    float       * restrict dst_xyz,
    float const * restrict src_xyz,
    float const * restrict t16,
    size_t                 count)
{
    float  *dst = dst_xyz;
    for (size_t i = 0; i < count; ++i)
    {
        float x = src_xyz[0];
        float y = src_xyz[1];
        float z = src_xyz[2];

        dst_xyz[0] = t16[0] * x + t16[4] * y + t16[8]  * z;
        dst_xyz[1] = t16[1] * x + t16[5] * y + t16[9]  * z;
        dst_xyz[2] = t16[2] * x + t16[6] * y + t16[10] * z;

        src_xyz += 3;
        dst_xyz += 3;
    }
    return dst;
}
