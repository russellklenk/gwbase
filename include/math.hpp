/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements a math library based on single-precision IEEE 754
/// floating-point numbers.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_MATH_HPP
#define GW_MATH_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Determines the smaller of two floating point values.
/// @param a The first value.
/// @param b The second value.
/// @return The smaller of a or b.
float min2(float a, float b);

/// @summary Determines the larger of two floating point values.
/// @param a The first value.
/// @param b The second value.
/// @return The larger of a or b.
float max2(float a, float b);

/// @summary Determines the smallest of three floating point values.
/// @param a The first value.
/// @param b The second value.
/// @param c The third value.
/// @return The smaller of a, b and c.
float min3(float a, float b, float c);

/// @summary Determines the largest of three floating point values.
/// @param a The first value.
/// @param b The second value.
/// @param c The third value.
/// @return The largest of a, b and c.
float max3(float a, float b, float c);

/// @summary Determines whether two floating point values are close enough to
/// be considered equal, using the same value for absolute and relative tolerance (FLT_EPSILON).
/// @param a The first value.
/// @param b The second value.
/// @return true if a and b can be considered equal.
bool eq(float a, float b);

/// @summary Determines whether two floating point values are close enough to
/// be considered equal, using the specified absolute tolerance. This test
/// fails when both a and b are very large.
/// @param a The first value.
/// @param b The second value.
/// @param tol The absolute tolerance value.
/// @return true if a and b can be considered equal.
bool eq_abs(float a, float b, float tol);

/// @summary Determines whether two floating point values are close enough to
/// be considered equal, using the specified relative tolerance. This test
/// fails when both a and b are very small.
/// @param a The first value.
/// @param b The second value.
/// @param tol The relative tolerance value.
/// @return true if a and b can be considered equal.
bool eq_rel(float a, float b, float tol);

/// @summary Determines whether two floating point values are close enough to
/// be considered equal, using the specified absolute and relative tolerance values.
/// @param a The first value.
/// @param b The second value.
/// @param tol_a The absolute tolerance value.
/// @param tol_r The relative tolerance value.
/// @return true if a and b can be considered equal.
bool eq_com(float a, float b, float tol_a, float tol_r);

/// @summary Determines whether a floating point value has the special Not A Number value.
/// @param a The value to check.
/// @return true if a is NaN.
bool is_nan(float a);

/// @summary Determines whether a floating point value is either positive or negative infinity.
/// @param a The value to check.
/// @return true if a is either the positive or negative infinity value.
bool is_inf(float a);

/// @summary Computes the reciporical value 1/a for a given value.
/// @param a The input value.
/// @return The value 1/a. The function does not check for divide-by-zero.
float rcp(float a);

/// @summary Converts a value specified in degrees to radians.
/// @param degrees The angle measure specified in degrees.
/// @return The angle measure specified in radians.
float rad(float degrees);

/// @summary Converts a value specified in radians to degrees.
/// @param radians The angle measure specified in radians.
/// @return The angle measure specified in degrees.
float deg(float radians);

/// @summary Performs linear interpolation between two scalar values.
/// @param a The value at t = 0.
/// @param b The value at t = 1.
/// @param t A normalized interpolation parameter.
/// @return The interpolated value.
float linear(float a, float b, float t);

/// @summary Performs Bezier interpolation between two scalar values.
/// @param a The value at t = 0.
/// @param b The value at t = 1.
/// @param in_t The tangent value (slope) coming into b.
/// @param out_t The tangent value (slope) coming out of b.
/// @param t A normalized interpolation parameter.
/// @return The interpolated value.
float bezier(float  a, float b, float in_t, float out_t, float t);

/// @summary Performs Hermite interpolation between two scalar values.
/// @param a The value at t = 0.
/// @param b The value at t = 1.
/// @param in_t The tangent value (slope) coming into b.
/// @param out_t The tangent value (slope) coming out of b.
/// @param t A normalized interpolation parameter.
/// @return The interpolated value.
float hermite(float a, float b, float in_t, float out_t, float t);

/// @summary Sets a 2-component vector or point value.
/// @param dst_xy Pointer to the destination storage.
/// @param x The x-component of the vector or point.
/// @param y The y-component of the vector or point.
/// @return The pointer dst_xy.
float* vec2_set_xy(float *dst_xy, float x, float y);

/// @summary Copies a 2-component vector or point value. The source and
/// destination values must not overlap.
/// @param dst_xy Pointer to the destination storage.
/// @param src_xy Pointer to the source value.
/// @return The pointer @a dst_xy.
float* vec2_set_vec2(float * restrict dst_xy, float const * restrict src_xy);

/// @summary Extracts the x- and y-components of a vector or point value into a
/// destination value. The source and destination values must not overlap.
/// @param dst_xy Pointer to the destination storage.
/// @param src_xyz Pointer to the source value.
/// @return The pointer dst_xy.
float* vec2_set_vec3(float * restrict dst_xy, float const * restrict src_xyz);

/// @summary Extracts the x- and y-components of a vector or point value into a
/// destination value. The source and destination values must not overlap.
/// @param dst_xy Pointer to the destination storage.
/// @param src_xyzw Pointer to the source value.
/// @return The pointer dst_xy.
float* vec2_set_vec4(float * restrict dst_xy, float const * restrict src_xyzw);

/// @summary Sets a 3-component vector or point value.
/// @param dst_xyz Pointer to the destination storage.
/// @param x The x-component of the vector or point.
/// @param y The y-component of the vector or point.
/// @param z The z-component of the vector or point.
/// @return The pointer dst_xyz.
float* vec3_set_xyz(float *dst_xyz, float x, float y, float z);

/// @summary Extracts the x- and y-components of a vector or point value into a
/// destination value with explicitly specified z-component. The source and
/// destination values must not overlap.
/// @param dst_xyz Pointer to the destination storage.
/// @param src_xy Pointer to the source storage from which the x- and y-components will be read.
/// @param z The z-component value.
/// @return The pointer to dst_xyz.
float* vec3_set_vec2(float * restrict dst_xyz, float const * restrict src_xy, float z);

/// @summary Copies a 3-component vector or point value. The source and
/// destination values must not overlap.
/// @param dst_xyz Pointer to the destination storage.
/// @param src_xyz Pointer to the source value.
/// @return The pointer dst_xyz.
float* vec3_set_vec3(float * restrict dst_xyz, float const * restrict src_xyz);

/// @summary Extracts the x- and y- and z-components of a vector or point value
/// into a destination value. The source and destination values must not overlap.
/// @param dst_xyz Pointer to the destination storage.
/// @param src_xyzw Pointer to the source storage from which the x- y- and z-components will be read.
/// @return The pointer to dst_xyz.
float* vec3_set_vec4(float * restrict dst_xyz, float const * restrict src_xyzw);

/// @summary Sets a 4-component vector or point value.
/// @param dst_xyzw Pointer to the destination storage.
/// @param x The x-component of the vector or point.
/// @param y The y-component of the vector or point.
/// @param z The z-component of the vector or point.
/// @param w The w-component of the vector or point. Vectors typically have a
/// w-component of zero; points typically have a w-component of one.
/// @return The pointer dst_xyzw.
float* vec4_set_xyzw(float *dst_xyzw, float x, float y, float z, float w);

/// @summary Extracts the x- and y-components of a vector or point value into a
/// destination value with explicitly specified z- and w-component. The source
/// and destination values must not overlap.
/// @param dst_xyz Pointer to the destination storage.
/// @param src_xy Pointer to the source storage from which the x- and y-components will be read.
/// @param z The z-component value.
/// @param w The w-component of the vector or point. Vectors typically have a
/// w-component of zero; points typically have a w-component of one.
/// @return The pointer dst_xyzw.
float* vec4_set_vec2(float * restrict dst_xyzw, float const * restrict src_xy, float z, float w);

/// @summary Extracts the x- y- and z-components of a vector or point value into
/// a destination value with explicitly specified w-component. The source and
/// destination values must not overlap.
/// @param dst_xyz Pointer to the destination storage.
/// @param src_xyz Pointer to the source storage from which the x- y- and z-components will be read.
/// @param w The w-component of the vector or point. Vectors typically have a
/// w-component of zero; points typically have a w-component of one.
/// @return The pointer dst_xyzw.
float* vec4_set_vec3(float * restrict dst_xyzw, float const * restrict src_xyz, float w);

/// @summary Copies a 4-component vector or point value. The source and destination values must not overlap.
/// @param dst_xyzw Pointer to the destination storage.
/// @param src_xyzw Pointer to the source value.
/// @return The pointer dst_xyzw.
float* vec4_set_vec4(float * restrict dst_xyzw, float const * restrict src_xyzw);

/// @summary Sets all elements of a vector to the IEEE-754 floating point Not-A-Number (NaN) value.
/// @param dst_xy Pointer to the destination storage.
/// @return The pointer dst_xy.
float* vec2_set_nan(float *dst_xy);

/// @summary Sets all elements of a vector to the IEEE-754 floating point Not-A-Number (NaN) value.
/// @param dst_xyz Pointer to the destination storage.
/// @return The pointer dst_xyz.
float* vec3_set_nan(float *dst_xyz);

/// @summary Sets all elements of a vector to the IEEE-754 floating point Not-A-Number (NaN) value.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_nan(float *dst_xyzw);

/// @summary Sets all elements of a vector to 1.0.
/// @param dst_xy Pointer to the destination storage.
/// @return The pointer dst_xy.
float* vec2_set_one(float *dst_xy);

/// @summary Sets all elements of a vector to 1.0.
/// @param dst_xyz Pointer to the destination storage.
/// @return The pointer dst_xyz.
float* vec3_set_one(float *dst_xyz);

/// @summary Sets all elements of a vector to 1.0.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_one(float *dst_xyzw);

/// @summary Sets all elements of a vector to 0.0.
/// @param dst_xy Pointer to the destination storage.
/// @return The pointer dst_xy.
float* vec2_set_zero(float *dst_xy);

/// @summary Sets all elements of a vector to 0.0.
/// @param dst_xyz Pointer to the destination storage.
/// @return The pointer dst_xyz.
float* vec3_set_zero(float *dst_xyz);

/// @summary Sets all elements of a vector to 0.0.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_zero(float *dst_xyzw);

/// @summary Sets all elements of a vector to the IEEE-754 floating point negative infinity value.
/// @param dst_xy Pointer to the destination storage.
/// @return The pointer dst_xy.
float* vec2_set_ninf(float *dst_xy);

/// @summary Sets all elements of a vector to the IEEE-754 floating point negative infinity value.
/// @param dst_xyz Pointer to the destination storage.
/// @return The pointer dst_xyz.
float* vec3_set_ninf(float *dst_xyz);

/// @summary Sets all elements of a vector to the IEEE-754 floating point negative infinity value.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_ninf(float *dst_xyzw);

/// @summary Sets all elements of a vector to the IEEE-754 floating point positive infinity value.
/// @param dst_xy Pointer to the destination storage.
/// @return The pointer dst_xy.
float* vec2_set_pinf(float *dst_xy);

/// @summary Sets all elements of a vector to the IEEE-754 floating point positive infinity value.
/// @param dst_xyz Pointer to the destination storage.
/// @return The pointer dst_xyz.
float* vec3_set_pinf(float *dst_xyz);

/// @summary Sets all elements of a vector to the IEEE-754 floating point positive infinity value.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_pinf(float *dst_xyzw);

/// @summary Sets the elements of a vector to the unit-length x-axis value <1,0>.
/// @param dst_xy Pointer to the destination storage.
/// @return The pointer dst_xy.
float* vec2_set_unit_x(float *dst_xy);

/// @summary Sets the elements of a vector to the unit-length x-axis value <1,0,0>.
/// @param dst_xyz Pointer to the destination storage.
/// @return The pointer dst_xyz.
float* vec3_set_unit_x(float *dst_xyz);

/// @summary Sets the elements of a vector to the unit-length x-axis value <1,0,0,0>.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_unit_x(float *dst_xyzw);

/// @summary Sets the elements of a vector to the unit-length y-axis value <0,1>.
/// @param dst_xy Pointer to the destination storage.
/// @return The pointer dst_xy.
float* vec2_set_unit_y(float *dst_xy);

/// @summary Sets the elements of a vector to the unit-length y-axis value <0,1,0>.
/// @param dst_xyz Pointer to the destination storage.
/// @return The pointer dst_xyz.
float* vec3_set_unit_y(float *dst_xyz);

/// @summary Sets the elements of a vector to the unit-length y-axis value <0,1,0,0>.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_unit_y(float *dst_xyzw);

/// @summary Sets the elements of a vector to the unit-length z-axis value <0,0,1>.
/// @param dst_xyz Pointer to the destination storage.
/// @return The pointer dst_xyz.
float* vec3_set_unit_z(float *dst_xyz);

/// @summary Sets the elements of a vector to the unit-length z-axis value <0,1,0,0>.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_unit_z(float *dst_xyzw);

/// @summary Sets the elements of a vector to the unit-length w-axis value <0,0,0,1>.
/// @param dst_xyzw Pointer to the destination storage.
/// @return The pointer dst_xyzw.
float* vec4_set_unit_w(float *dst_xyzw);

/// @summary Compares two vector values for equality.
/// @param a_xy The first vector value.
/// @param b_xy The second vector value.
/// @return true if a_xy and b_xy can be considered equal.
bool vec2_eq(float const * restrict a_xy, float const * restrict b_xy);

/// @summary Compares two vector values for equality.
/// @param a_xyz The first vector value.
/// @param b_xyz The second vector value.
/// @return true if a_xyz and b_xyz can be considered equal.
bool vec3_eq(float const * restrict a_xyz, float const * restrict b_xyz);

/// @summary Compares two vector values for equality.
/// @param a_xyzw The first vector value.
/// @param b_xyzw The second vector value.
/// @return true if a_xyzw and b_xyzw can be considered equal.
bool vec4_eq(float const * restrict a_xyzw, float const * restrict b_xyzw);

/// @summary Performs component-wise addition of two vector quantities, storing
/// the result in a third, such that dst = a + b.
/// @param dst_xy Pointer to the destination storage.
/// @param a_xy The first source value.
/// @param b_xy The second source value.
/// @return The pointer dst_xy.
float* vec2_add(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy);

/// @summary Performs component-wise addition of two vector quantities, storing
/// the result in a third, such that dst = a + b.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The first source value.
/// @param b_xyz The second source value.
/// @return The pointer dst_xyz.
float* vec3_add(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz);

/// @summary Performs component-wise addition of two vector quantities, storing
/// the result in a third, such that dst = a + b.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The first source value.
/// @param b_xyzw The second source value.
/// @return The pointer dst_xyzw.
float* vec4_add(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);

/// @summary Performs component-wise subtraction of two vector quantities,
/// storing the result in a third, such that dst = a - b.
/// @param dst_xy Pointer to the destination storage.
/// @param a_xy The first source value.
/// @param b_xy The second source value.
/// @return The pointer dst_xy.
float* vec2_sub(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy);

/// @summary Performs component-wise subtraction of two vector quantities,
/// storing the result in a third, such that dst = a - b.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The first source value.
/// @param b_xyz The second source value.
/// @return The pointer dst_xyz.
float* vec3_sub(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz);

/// @summary Performs component-wise subtraction of two vector quantities,
/// storing the result in a third, such that dst = a - b.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The first source value.
/// @param b_xyzw The second source value.
/// @return The pointer dst_xyzw.
float* vec4_sub(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);

/// @summary Performs component-wise multiplication of two vector quantities,
/// storing the result in a third, such that dst = a * b.
/// @param dst_xy Pointer to the destination storage.
/// @param a_xy The first source value.
/// @param b_xy The second source value.
/// @return The pointer to dst_xy.
float* vec2_mul(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy);

/// @summary Performs component-wise multiplication of two vector quantities,
/// storing the result in a third, such that dst = a * b.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The first source value.
/// @param b_xyz The second source value.
/// @return The pointer to dst_xyz.
float* vec3_mul(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz);

/// @summary Performs component-wise multiplication of two vector quantities,
/// storing the result in a third, such that dst = a * b.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The first source value.
/// @param b_xyzw The second source value.
/// @return The pointer to dst_xyzw.
float* vec4_mul(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);

/// @summary Performs component-wise division of two vector quantities, storing
/// the result in a third, such that dst = a / b.
/// @param dst_xy Pointer to the destination storage.
/// @param a_xy The first source value.
/// @param b_xy The second source value.
/// @return The pointer to dst_xy.
float* vec2_div(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy);

/// @summary Performs component-wise division of two vector quantities, storing
/// the result in a third, such that dst = a / b.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The first source value.
/// @param b_xyz The second source value.
/// @return The pointer to @a dst_xyz.
float* vec3_div(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz);

/// @summary Performs component-wise division of two vector quantities, storing
/// the result in a third, such that dst = a / b.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The first source value.
/// @param b_xyzw The second source value.
/// @return The pointer to dst_xyzw.
float* vec4_div(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);

/// @summary Multiplies each component of a vector value by a scalar.
/// @param dst_xy Pointer to the destination storage.
/// @param a_xy The source vector value.
/// @param b The scalar value.
/// @return The pointer to dst_xy.
float* vec2_scl(float * restrict dst_xy, float const * restrict a_xy, float b);

/// @summary Multiplies each component of a vector value by a scalar.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The source vector value.
/// @param b The scalar value.
/// @return The pointer to dst_xyz.
float* vec3_scl(float * restrict dst_xyz, float const * restrict a_xyz, float b);

/// @summary Multiplies each component of a vector value by a scalar.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The source vector value.
/// @param b The scalar value.
/// @return The pointer to dst_xyzw.
float* vec4_scl(float * restrict dst_xyzw, float const * restrict a_xyzw, float b);

/// @summary Multiplies each component of a vector value by a scalar. Only the
/// first three components of the vector are multiplied by the scalar value.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The source vector value.
/// @param b The scalar value.
/// @return The pointer to dst_xyzw.
float* vec4_scl3(float * restrict dst_xyzw, float const * restrict a_xyzw, float b);

/// @summary Negates each component of a vector value, preserving the magnitude
/// but reversing the direction.
/// @param dst_xy Pointer to the destination storage.
/// @param src_xy The source vector value.
/// @return The pointer to dst_xy.
float* vec2_neg(float * restrict dst_xy, float const * restrict src_xy);

/// @summary Negates each component of a vector value, preserving the magnitude
/// but reversing the direction.
/// @param dst_xyz Pointer to the destination storage.
/// @param src_xyz The source vector value.
/// @return The pointer to dst_xyz.
float* vec3_neg(float * restrict dst_xyz, float const * restrict src_xyz);

/// @summary Negates each component of a vector value, preserving the magnitude
/// but reversing the direction.
/// @param dst_xyzw Pointer to the destination storage.
/// @param src_xyzw The source vector value.
/// @return The pointer to dst_xyzw.
float* vec4_neg(float * restrict dst_xyzw, float const * restrict src_xyzw);

/// @summary Negates each component of a vector value, preserving the magnitude
/// but reversing the direction. Only the first three components are negated.
/// @param dst_xyzw Pointer to the destination storage.
/// @param src_xyzw The source vector value.
/// @return The pointer to dst_xyzw.
float* vec4_neg3(float * restrict dst_xyzw, float const * restrict src_xyzw);

/// @summary Computes the dot product of two vectors.
/// @param dst On return, this value is set to the dot product of a and b.
/// @param a_xy Vector value a.
/// @param b_xy Vector value b.
/// @return The dot product of the vectors.
float vec2_dot(float &dst, float const * restrict a_xy, float const * restrict b_xy);

/// @summary Computes the dot product of two vectors.
/// @param dst On return, this value is set to the dot product of a and b.
/// @param a_xyz Vector value a.
/// @param b_xyz Vector value b.
/// @return The dot product of the vectors.
float vec3_dot(float &dst, float const * restrict a_xyz, float const * restrict b_xyz);

/// @summary Computes the dot product of two vectors.
/// @param dst On return, this value is set to the dot product of a and b.
/// @param a_xyzw Vector value a.
/// @param b_xyzw Vector value b.
/// @return The dot product of the vectors.
float vec4_dot(float &dst, float const * restrict a_xyzw, float const * restrict b_xyzw);

/// @summary Computes the dot product of two vectors, considering only the first three components of each.
/// @param dst On return, this value is set to the dot product of a and b.
/// @param a_xyzw Vector value a.
/// @param b_xyzw Vector value b.
/// @return The dot product of the vectors.
float vec4_dot3(float &dst, float const * restrict a_xyzw, float const * restrict b_xyzw);

/// @summary Calculates the magnitude (length) of a vector.
/// @param dst On return, this value is set to the length of the vector.
/// @param a_xy The vector value.
/// @return The magnitude (length) of vector a_xy.
float vec2_len(float &dst, float const *a_xy);

/// @summary Calculates the magnitude (length) of a vector.
/// @param dst On return, this value is set to the length of the vector.
/// @param a_xyz The vector value.
/// @return The magnitude (length) of vector a_xyz.
float vec3_len(float &dst, float const *a_xyz);

/// @summary Calculates the magnitude (length) of a vector.
/// @param dst On return, this value is set to the length of the vector.
/// @param a_xyzw The vector value.
/// @return The magnitude (length) of vector a_xyzw.
float vec4_len(float &dst, float const *a_xyzw);

/// @summary Calculates the magnitude (length) of a vector. Only the first
/// three components of the vector are considered.
/// @param dst On return, this value is set to the length of the vector.
/// @param a_xyzw The vector value.
/// @return The magnitude (length) of vector a_xyzw.
float vec4_len3(float &dst, float const *a_xyzw);

/// @summary Calculates the squared magnitude (length) of a vector.
/// @param dst On return, this value is set to the squared length of the vector.
/// @param a_xy The vector value.
/// @return The squared magnitude (length) of vector a_xy.
float vec2_len_sq(float &dst, float const *a_xy);

/// @summary Calculates the squared magnitude (length) of a vector.
/// @param dst On return, this value is set to the squared length of the vector.
/// @param a_xyz The vector value.
/// @return The squared magnitude (length) of vector a_xyz.
float vec3_len_sq(float &dst, float const *a_xyz);

/// @summary Calculates the squared magnitude (length) of a vector.
/// @param dst On return, this value is set to the squared length of the vector.
/// @param a_xyzw The vector value.
/// @return The squared magnitude (length) of vector a_xyzw.
float vec4_len_sq(float &dst, float const *a_xyzw);

/// @summary Calculates the squared magnitude (length) of a vector.
/// @param dst On return, this value is set to the squared length of the vector.
/// @param a_xy The vector value.
/// @return The squared magnitude (length) of vector a_xy.
float vec4_len3_sq(float &dst, float const *a_xyzw);

/// @summary Calculates the normalized (unit-length) vector for a given vector
/// value. The normalized vector has the same direction, but magnitude 1.
/// @param dst_xy Pointer to the destination storage.
/// @param src_xy The source vector value.
/// @return The pointer dst_xy.
float* vec2_nrm(float * restrict dst_xy, float const * restrict src_xy);

/// @summary Calculates the normalized (unit-length) vector for a given vector
/// value. The normalized vector has the same direction, but magnitude 1.
/// @param dst_xyz Pointer to the destination storage.
/// @param src_xyz The source vector value.
/// @return The pointer dst_xyz.
float* vec3_nrm(float * restrict dst_xyz, float const * restrict src_xyz);

/// @summary Calculates the normalized (unit-length) vector for a given vector
/// value. The normalized vector has the same direction, but magnitude 1.
/// @param dst_xyzw Pointer to the destination storage.
/// @param src_xyzw The source vector value.
/// @return The pointer dst_xyzw.
float* vec4_nrm(float * restrict dst_xyzw, float const * restrict src_xyzw);

/// @summary Calculates the normalized (unit-length) vector for a given vector
/// value. The normalized vector has the same direction, but magnitude 1. Only
/// the first three components of src_xyzw are considered when computing the
/// length and normalizing the vector.
/// @param dst_xyzw Pointer to the destination storage.
/// @param src_xyzw The source vector value.
/// @return The pointer dst_xyzw.
float* vec4_nrm3(float * restrict dst_xyzw, float const * restrict src_xyzw);

/// @summary Calculates a vector perpendicular to a given vector, but with the same magnitude.
/// @param dst_xy Pointer to the destination storage.
/// @param src_xy The source vector value.
/// @return The pointer dst_xy.
float* vec2_perp(float * restrict dst_xy, float const * restrict src_xy);

/// @summary Calculates the cross product of two vectors, producing a third
/// vector that is orthogonal to the source vectors; the dot product of the
/// resulting vector with either of the source vectors is zero.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The first source vector.
/// @param b_xyz The second source vector.
/// @return The pointer dst_xyz.
float* vec3_cross(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz);

/// @summary Calculates the cross product of two vectors, producing a third
/// vector that is orthogonal to the source vectors; the dot product of the
/// resulting vector with either of the source vectors is zero. The w-component
/// of the resulting vector is always zero, since the cross product operation
/// always results in a vector value.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The first source vector.
/// @param b_xyzw The second source vector.
/// @return The pointer dst_xyzw.
float* vec4_cross(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);

/// @summary Performs a swizzle operation on a vector or point value to select
/// or change the order of components.
/// @param dst_xy Pointer to the destination storage.
/// @param src_xy The source value.
/// @param x The zero-based index of the source component that will be written
/// to the destination value at index 0.
/// @param y The zero-based index of the source component that will be written
/// to the destination value at index 1.
/// @return The pointer dst_xy.
float* vec2_swizzle(float * restrict dst_xy, float const * restrict src_xy, size_t x, size_t y);

/// @summary Performs a swizzle operation on a vector or point value to select
/// or change the order of components.
/// @param dst_xyz Pointer to the destination storage.
/// @param src_xyz The source value.
/// @param x The zero-based index of the source component that will be written
/// to the destination value at index 0.
/// @param y The zero-based index of the source component that will be written
/// to the destination value at index 1.
/// @param z The zero-based index of the source component that will be written
/// to the destination value at index 2.
/// @return The pointer dst_xyz.
float* vec3_swizzle(float * restrict dst_xyz, float const * restrict src_xyz, size_t x, size_t y, size_t z);

/// @summary Performs a swizzle operation on a vector or point value to select
/// or change the order of components.
/// @param dst_xyzw Pointer to the destination storage.
/// @param src_xyzw The source value.
/// @param x The zero-based index of the source component that will be written
/// to the destination value at index 0.
/// @param y The zero-based index of the source component that will be written
/// to the destination value at index 1.
/// @param z The zero-based index of the source component that will be written
/// to the destination value at index 2.
/// @param w The zero-based index of the source component that will be written
/// to the destination value at index 3.
/// @return The pointer dst_xyzw.
float* vec4_swizzle(float * restrict dst_xyzw, float const * restrict src_xyzw, size_t x, size_t y, size_t z, size_t w);

/// @summary Performs componentwise linear interpolation between two vector or point quantities.
/// @param dst_xy Pointer to the destination storage.
/// @param a_xy The value at @a t = 0.
/// @param b_xy The value at @a t = 1.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xy.
float* vec2_linear(float * restrict dst_xy, float const * restrict a_xy, float const * restrict b_xy, float t);

/// @summary Performs componentwise linear interpolation between two vector or point quantities.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The value at @a t = 0.
/// @param b_xyz The value at @a t = 1.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyz.
float* vec3_linear(float * restrict dst_xyz, float const * restrict a_xyz, float const * restrict b_xyz, float t);

/// @summary Performs componentwise linear interpolation between two vector or point quantities.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The value at @a t = 0.
/// @param b_xyzw The value at @a t = 1.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyzw.
float* vec4_linear(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float t);

/// @summary Performs componentwise linear interpolation between two vector or
/// point quantities. Only the first three components are interpolated.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The value at @a t = 0.
/// @param b_xyzw The value at @a t = 1.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyzw.
float* vec4_linear3(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float t);

/// @summary Performs componentwise Bezier interpolation between two vector or point quantities.
/// @param dst_xy Pointer to the destination storage.
/// @param a_xy The value at @a t = 0.
/// @param b_xy The value at @a t = 1.
/// @param itan_xy The incoming tangent value.
/// @param otan_xy The outgoing tangent value.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xy.
float* vec2_bezier(
    float       * restrict dst_xy,
    float const * restrict a_xy,
    float const * restrict b_xy,
    float const * restrict itan_xy,
    float const * restrict otan_xy,
    float                  t);

/// @summary Performs componentwise Bezier interpolation between two vector or point quantities.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The value at @a t = 0.
/// @param b_xyz The value at @a t = 1.
/// @param itan_xyz The incoming tangent value.
/// @param otan_xyz The outgoing tangent value.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyz.
float* vec3_bezier(
    float       * restrict dst_xyz,
    float const * restrict a_xyz,
    float const * restrict b_xyz,
    float const * restrict itan_xyz,
    float const * restrict otan_xyz,
    float                  t);

/// @summary Performs componentwise Bezier interpolation between two vector or point quantities.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The value at @a t = 0.
/// @param b_xyzw The value at @a t = 1.
/// @param itan_xyzw The incoming tangent value.
/// @param otan_xyzw The outgoing tangent value.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyzw.
float* vec4_bezier(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict itan_xyzw,
    float const * restrict otan_xyzw,
    float                  t);

/// @summary Performs componentwise Bezier interpolation between two vector or
/// point quantities. Only the first three components are interpolated.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The value at @a t = 0.
/// @param b_xyzw The value at @a t = 1.
/// @param itan_xyzw The incoming tangent value.
/// @param otan_xyzw The outgoing tangent value.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyzw.
float* vec4_bezier3(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict itan_xyz,
    float const * restrict otan_xyz,
    float                  t);

/// @summary Performs componentwise Hermite interpolation between two vector or point quantities.
/// @param dst_xy Pointer to the destination storage.
/// @param a_xy The value at @a t = 0.
/// @param b_xy The value at @a t = 1.
/// @param itan_xy The incoming tangent value.
/// @param otan_xy The outgoing tangent value.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xy.
float* vec2_hermite(
    float       * restrict dst_xy,
    float const * restrict a_xy,
    float const * restrict b_xy,
    float const * restrict itan_xy,
    float const * restrict otan_xy,
    float                  t);

/// @summary Performs componentwise Hermite interpolation between two vector or point quantities.
/// @param dst_xyz Pointer to the destination storage.
/// @param a_xyz The value at @a t = 0.
/// @param b_xyz The value at @a t = 1.
/// @param itan_xyz The incoming tangent value.
/// @param otan_xyz The outgoing tangent value.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyz.
float* vec3_hermite(
    float       * restrict dst_xyz,
    float const * restrict a_xyz,
    float const * restrict b_xyz,
    float const * restrict itan_xyz,
    float const * restrict otan_xyz,
    float                  t);

/// @summary Performs componentwise Hermite interpolation between two vector or point quantities.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The value at @a t = 0.
/// @param b_xyzw The value at @a t = 1.
/// @param itan_xyzw The incoming tangent value.
/// @param otan_xyzw The outgoing tangent value.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyzw.
float* vec4_hermite(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict itan_xyzw,
    float const * restrict otan_xyzw,
    float                      t);

/// @summary Performs componentwise Hermite interpolation between two vector or
/// point quantities. Only the first three components are interpolated.
/// @param dst_xyzw Pointer to the destination storage.
/// @param a_xyzw The value at @a t = 0.
/// @param b_xyzw The value at @a t = 1.
/// @param itan_xyz The incoming tangent value.
/// @param otan_xyz The outgoing tangent value.
/// @param t A value in the range [0, 1] specifying the interpolation parameter.
/// @return The pointer dst_xyzw.
float* vec4_hermite3(
    float       * restrict dst_xyzw,
    float const * restrict a_xyzw,
    float const * restrict b_xyzw,
    float const * restrict itan_xyz,
    float const * restrict otan_xyz,
    float                  t);

float* quat_set_xyzw(float *dst_xyzw, float x, float y, float z, float w);
float* quat_set_quat(float * restrict dst_xyzw, float const * restrict src_xyzw);
float* quat_set_nan(float *dst_xyzw);
float* quat_set_one(float *dst_xyzw);
float* quat_set_zero(float *dst_xyzw);
float* quat_set_ninf(float *dst_xyzw);
float* quat_set_pinf(float *dst_xyzw);
float* quat_set_ident(float *dst_xyzw);
bool   quat_eq(float const * restrict a_xyzw, float const * restrict b_xyzw);
float* quat_add(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);
float* quat_sub(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);
float* quat_mul(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);
float* quat_scl(float * restrict dst_xyzw, float const * restrict a_xyzw, float b);
float* quat_scl3(float * restrict dst_xyzw, float const * restrict a_xyzw, float b);
float* quat_neg(float * restrict dst_xyzw, float const * restrict src_xyzw);
float* quat_neg3(float * restrict dst_xyzw, float const * restrict src_xyzw);
float* quat_conj(float * restrict dst_xyzw, float const * restrict src_xyzw);
float  quat_dot(float const * restrict a_xyzw, float const * restrict b_xyzw);
float  quat_norm(float const *src_xyzw);
float  quat_len(float const *src_xyzw);
float  quat_len_sq(float const *src_xyzw);
float  quat_sel(float const *src_xyzw);
float* quat_inv(float * restrict dst_xyzw, float const * restrict src_xyzw);
float* quat_nrm(float * restrict dst_xyzw, float const * restrict src_xyzw);
float* quat_exp(float * restrict dst_xyzw, float const * restrict src_xyzw);
float* quat_log(float * restrict dst_xyzw, float const * restrict src_xyzw);
float* quat_closest(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw);
float* quat_orient_vec3(float *restrict dst_xyzw, float const * restrict from_xyz, float const * restrict to_xyz);
float* quat_orient_vec4(float * restrict dst_xyzw, float const * restrict from_xyzw, float const * restrict to_xyzw);
float* quat_set_mat4(float * restrict dst_xyzw, float const * restrict m16);
float* quat_set_euler_degree(float * restrict dst_xyzw, float deg_x, float deg_y, float deg_z);
float* quat_set_euler_radian(float * restrict dst_xyzw, float rad_x, float rad_y, float rad_z);
float* quat_set_angle_axis_degree_n(float * restrict dst_xyzw, float angle_deg, float const * restrict axis_xyz);
float* quat_set_angle_axis_radian_n(float * restrict dst_xyzw, float angle_rad, float const * restrict axis_xyz);
float* quat_set_angle_axis_degree_u(float * restrict dst_xyzw, float angle_deg, float const * restrict axis_xyz);
float* quat_set_angle_axis_radian_u(float * restrict dst_xyzw, float angle_rad, float const * restrict axis_xyz);
float* quat_linear(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float t);
float* quat_slerp(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float t);
float* quat_squad(float * restrict dst_xyzw, float const * restrict p_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float const * restrict q_xyzw, float t);
float* quat_spline(float * restrict dst_xyzw, float const * restrict a_xyzw, float const * restrict b_xyzw, float const * restrict c_xyzw);

float* mat4_set(float *dst16, float  r0c0, float r0c1, float r0c2, float r0c3, float  r1c0, float r1c1, float r1c2, float r1c3, float  r2c0, float r2c1, float r2c2, float r2c3, float  r3c0, float r3c1, float r3c2, float r3c3);
float* mat4_set_mat4(float * restrict dst16, float const * restrict src16);
float* mat4_set_nan(float *dst16);
float* mat4_set_one(float *dst16);
float* mat4_set_zero(float *dst16);
float* mat4_set_ninf(float *dst16);
float* mat4_set_pinf(float *dst16);
float* mat4_set_ident(float *dst16);
bool   mat4_eq(float const * restrict a16, float const * restrict b16);
bool   mat4_is_identity(float const *src16);
float* mat4_set_rows(float * restrict dst16, float const * restrict r0_xyzw, float const * restrict r1_xyzw, float const * restrict r2_xyzw, float const * restrict r3_xyzw);
float* mat4_set_cols(float * restrict dst16, float const * restrict c0_xyzw, float const * restrict c1_xyzw, float const * restrict c2_xyzw, float const * restrict c3_xyzw);
float* mat4_get_row(float * restrict dst_xyzw, size_t row, float const *restrict src16);
float* mat4_set_row(float * restrict dst16, size_t row, float const *restrict src_xyzw);
float* mat4_get_col(float * restrict dst_xyzw, size_t col, float const *restrict src16);
float* mat4_set_col(float * restrict dst16, size_t col, float const * restrict src_xyzw);
float  mat4_trace(float const *src16);
float  mat4_det(float const *src16);
float* mat4_transpose(float * restrict dst16, float const * restrict src16);
float* mat4_concat(float * restrict dst16, float const * restrict a16, float const * restrict b16);
float* mat4_inv_affine(float * restrict dst16, float const * restrict src16);
float* mat4_set_quat(float * restrict dst16, float const * restrict src_xyzw);
float* mat4_set_euler_degree_x(float *dst16, float deg_x);
float* mat4_set_euler_radian_x(float *dst16, float rad_x);
float* mat4_set_euler_degree_y(float *dst16, float deg_y);
float* mat4_set_euler_radian_y(float *dst16, float rad_y);
float* mat4_set_euler_degree_z(float *dst16, float deg_z);
float* mat4_set_euler_radian_z(float *dst16, float rad_z);
float* mat4_set_euler_degree(float *dst16, float deg_x, float deg_y, float deg_z);
float* mat4_set_euler_radian(float *dst16, float rad_x, float rad_y, float rad_z);
float* mat4_set_angle_axis_degree_n(float * restrict dst16, float angle_deg, float const * restrict axis_xyz);
float* mat4_set_angle_axis_radian_n(float * restrict dst16, float angle_rad, float const * restrict axis_xyz);
float* mat4_set_angle_axis_degree_u(float * restrict dst16, float angle_deg, float const * restrict axis_xyz);
float* mat4_set_angle_axis_radian_u(float * restrict dst16, float angle_rad, float const * restrict axis_xyz);
float* mat4_look_at(float * restrict dst16, float const * restrict pos_xyz, float const * restrict target_xyz, float const * restrict up_xyz);
float* mat4_scale(float *dst16, float scale_x, float scale_y, float scale_z);
float* mat4_trans(float *dst16, float trans_x, float trans_y, float trans_z);
float* mat4_ortho(float *dst16, float left, float right, float bottom, float top, float near, float far);
float* mat4_persp_degree(float *dst16, float fov_deg, float aspect, float near, float far);
float* mat4_persp_radian(float *dst16, float fov_rad, float aspect, float near, float far);
float* mat4_2d(float *dst16, float width, float height);
void   mat4_extract_frustum_n(float * restrict left_xyzD, float * restrict right_xyzD, float * restrict top_xyzD, float * restrict bottom_xyzD, float * restrict near_xyzD, float * restrict far_xyzD, float const * restrict proj16);
void   mat4_extract_frustum_u(float * restrict left_xyzD, float * restrict right_xyzD, float * restrict top_xyzD, float * restrict bottom_xyzD, float * restrict near_xyzD, float * restrict far_xyzD, float const * restrict proj16);
float* mat4_transform_vec3(float * restrict dst_xyz, float const * restrict src_xyz, float const * restrict t16);
float* mat4_transform_vec4(float * restrict dst_xyzw, float const * restrict src_xyzw, float const * restrict t16);
float* mat4_transform_point(float * restrict dst_xyz, float const * restrict src_xyz, float const * restrict t16);
float* mat4_transform_vector(float * restrict dst_xyz, float const * restrict src_xyz, float const * restrict t16);
float* mat4_transform_array_vec3(float * restrict dst_xyz, float const * restrict src_xyz, float const * restrict t16, size_t count);
float* mat4_transform_array_vec4(float * restrict dst_xyzw, float const * restrict src_xyzw, float const * restrict t16, size_t count);
float* mat4_transform_array_point(float * restrict dst_xyz, float const * restrict src_xyz, float const * restrict t16, size_t count);
float* mat4_transform_array_vector(float * restrict dst_xyz, float const * restrict src_xyz, float const * restrict t16, size_t count);

#endif /* !defined(GW_MATH_HPP) */
