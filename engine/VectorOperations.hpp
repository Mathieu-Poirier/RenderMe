#pragma once
#include "CameraSettings.hpp"
#include "DataTypes.hpp"
#include "EngineBounds.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>

using std::cout;

/*
    VectorOperations.hpp

    Description:
        This header defines a set of atomic vector operations (with safety checks)
        and globally-buffered vector functions optimized for data locality.

    Author:  Mathieu Poirier  
    Email:   mathieuworkemail@gmail.com
*/

inline Vec3_t VecAddAtomic(const Vec3_t& vector_base, const Vec3_t& vector_addend) {
    assert(vector_base.x < MAX_X_COORDINATE && "ATOMIC ADD: vector_base.x exceeds max x coordinate");
    assert(vector_base.y < MAX_Y_COORDINATE && "ATOMIC ADD: vector_base.y exceeds max y coordinate");
    assert(vector_base.z < MAX_Z_COORDINATE && "ATOMIC ADD: vector_base.z exceeds max z coordinate");

    assert(vector_addend.x < MAX_X_COORDINATE && "ATOMIC ADD: vector_addend.x exceeds max x coordinate");
    assert(vector_addend.y < MAX_Y_COORDINATE && "ATOMIC ADD: vector_addend.y exceeds max y coordinate");
    assert(vector_addend.z < MAX_Z_COORDINATE && "ATOMIC ADD: vector_addend.z exceeds max z coordinate");

    return {
        vector_base.x + vector_addend.x,
        vector_base.y + vector_addend.y,
        vector_base.z + vector_addend.z
    };
}


inline Vec3_t VecSubAtomic(const Vec3_t& minuend, const Vec3_t& subtrahend) {
    assert(minuend.x < MAX_X_COORDINATE && "ATOMIC SUBTRACTION: minuend.x exceeds max x coordinate");
    assert(minuend.y < MAX_Y_COORDINATE && "ATOMIC SUBTRACTION: minuend.y exceeds max y coordinate");
    assert(minuend.z < MAX_Z_COORDINATE && "ATOMIC SUBTRACTION: minuend.z exceeds max z coordinate");

    assert(subtrahend.x < MAX_X_COORDINATE && "ATOMIC SUBTRACTION: subtrahend.x exceeds max x coordinate");
    assert(subtrahend.y < MAX_Y_COORDINATE && "ATOMIC SUBTRACTION: subtrahend.y exceeds max y coordinate");
    assert(subtrahend.z < MAX_Z_COORDINATE && "ATOMIC SUBTRACTION: subtrahend.z exceeds max z coordinate");

    return {
        minuend.x - subtrahend.x,
        minuend.y - subtrahend.y,
        minuend.z - subtrahend.z
    };
}


inline Vec3_t VecScaleAtomic(const Vec3_t& vector_base, double scalar) {
    assert(std::isfinite(scalar) && "ATOMIC SCALE: scalar must be finite");

    assert(vector_base.x < MAX_X_COORDINATE && "ATOMIC SCALE: base.x too large");
    assert(vector_base.y < MAX_Y_COORDINATE && "ATOMIC SCALE: base.y too large");
    assert(vector_base.z < MAX_Z_COORDINATE && "ATOMIC SCALE: base.z too large");

    assert(vector_base.x * scalar < MAX_X_COORDINATE && "ATOMIC SCALE: x scaled exceeds max");
    assert(vector_base.y * scalar < MAX_Y_COORDINATE && "ATOMIC SCALE: y scaled exceeds max");
    assert(vector_base.z * scalar < MAX_Z_COORDINATE && "ATOMIC SCALE: z scaled exceeds max");

    return {
        vector_base.x * scalar,
        vector_base.y * scalar,
        vector_base.z * scalar
    };
}

inline double VecDotAtomic(const Vec3_t& vector_left_hand_side, const Vec3_t& vector_right_hand_side) {

    assert(vector_left_hand_side.x < MAX_X_COORDINATE && "ATOMIC DOT: vector_left_hand_side.x exceeds maximum");
    assert(vector_left_hand_side.y < MAX_Y_COORDINATE && "ATOMIC DOT: vector_left_hand_side.y exceeds maximum");
    assert(vector_left_hand_side.z < MAX_Z_COORDINATE && "ATOMIC DOT: vector_left_hand_side.z exceeds maximum");


    assert(vector_right_hand_side.x < MAX_X_COORDINATE && "ATOMIC DOT: vector_right_hand_side.x exceeds maximum");
    assert(vector_right_hand_side.y < MAX_Y_COORDINATE && "ATOMIC DOT: vector_right_hand_side.y exceeds maximum");
    assert(vector_right_hand_side.z < MAX_Z_COORDINATE && "ATOMIC DOT: vector_right_hand_side.z exceeds maximum");

    double dot_product =
        vector_left_hand_side.x * vector_right_hand_side.x +
        vector_left_hand_side.y * vector_right_hand_side.y +
        vector_left_hand_side.z * vector_right_hand_side.z;

    assert(std::isfinite(dot_product) && "ATOMIC DOT: result is not finite");
    return dot_product;
}


inline Vec3_t VecCrossAtomic(const Vec3_t& vector_base, const Vec3_t& vector_operand) {

    assert(vector_base.x < MAX_X_COORDINATE && "ATOMIC CROSS: vector_base.x exceeds maximum");
    assert(vector_base.y < MAX_Y_COORDINATE && "ATOMIC CROSS: vector_base.y exceeds maximum");
    assert(vector_base.z < MAX_Z_COORDINATE && "ATOMIC CROSS: vector_base.z exceeds maximum");

    assert(vector_operand.x < MAX_X_COORDINATE && "ATOMIC CROSS: vector_operand.x exceeds maximum");
    assert(vector_operand.y < MAX_Y_COORDINATE && "ATOMIC CROSS: vector_operand.y exceeds maximum");
    assert(vector_operand.z < MAX_Z_COORDINATE && "ATOMIC CROSS: vector_operand.z exceeds maximum");


    double vector_final_crossed_x = vector_base.y * vector_operand.z - vector_base.z * vector_operand.y;
    double vector_final_crossed_y = vector_base.z * vector_operand.x - vector_base.x * vector_operand.z;
    double vector_final_crossed_z = vector_base.x * vector_operand.y - vector_base.y * vector_operand.x;


    assert( std::isfinite( vector_final_crossed_x ) && "ATOMIC CROSS: vector_final_crossed_x result is not finite");
    assert( std::isfinite( vector_final_crossed_y ) && "ATOMIC CROSS: vector_final_crossed_y result is not finite");
    assert( std::isfinite( vector_final_crossed_z ) && "ATOMIC CROSS: vector_final_crossed_z  result is not finite");

    return { vector_final_crossed_x , vector_final_crossed_y, vector_final_crossed_z };
}


inline double VecLengthAtomic(const Vec3_t& vector_base) {
    assert(vector_base.x < MAX_X_COORDINATE && "ATOMIC LENGTH: vector_base.x exceeds maximum");
    assert(vector_base.y < MAX_Y_COORDINATE && "ATOMIC LENGTH: vector_base.y exceeds maximum");
    assert(vector_base.z < MAX_Z_COORDINATE && "ATOMIC LENGTH: vector_base.z exceeds maximum");

    double length_squared =
        vector_base.x * vector_base.x +
        vector_base.y * vector_base.y +
        vector_base.z * vector_base.z;

    assert(std::isfinite(length_squared) && "ATOMIC LENGTH: result is not finite");

    return std::sqrt(length_squared);
}

inline Vec3_t VecNormalizeAtomic(const Vec3_t& vector_base) {

    assert(vector_base.x < MAX_X_COORDINATE && "ATOMIC NORMALIZE: vector_base.x exceeds maximum");
    assert(vector_base.y < MAX_Y_COORDINATE && "ATOMIC NORMALIZE: vector_base.y exceeds maximum");
    assert(vector_base.z < MAX_Z_COORDINATE && "ATOMIC NORMALIZE: vector_base.z exceeds maximum");

    double vector_base_length = VecLengthAtomic(vector_base);

    if (vector_base_length == 0.0) {
        return {0.0, 0.0, 0.0};
    }

    return VecScaleAtomic(vector_base, 1.0 / vector_base_length);
}


inline Vec3_t VecNegateAtomic(const Vec3_t& vector_base) {
    assert(vector_base.x < MAX_X_COORDINATE && "ATOMIC NEGATE: vector_base.x exceeds maximum");
    assert(vector_base.y < MAX_Y_COORDINATE && "ATOMIC NEGATE: vector_base.y exceeds maximum");
    assert(vector_base.z < MAX_Z_COORDINATE && "ATOMIC NEGATE: vector_base.z exceeds maximum");

    return {
        -vector_base.x,
        -vector_base.y,
        -vector_base.z
    };
}

inline bool VecEqualsAtomic(const Vec3_t& vector_a,
                            const Vec3_t& vector_b,
                            double epsilon = 1e-6) {
    return std::fabs(vector_a.x - vector_b.x) < epsilon &&
           std::fabs(vector_a.y - vector_b.y) < epsilon &&
           std::fabs(vector_a.z - vector_b.z) < epsilon;
}


// === SoA (Vec3Buffer) versions for indexed batch math ===

inline void VecAddIndexed( const Vec3Buffer& buffer_lhs, size_t index_lhs, const Vec3Buffer& buffer_rhs, size_t index_rhs, Vec3Buffer& buffer_out) {
    // Bounds checks
    assert(index_lhs < buffer_lhs.size() && "INDEXED ADD: Left-hand index out of bounds");
    assert(index_rhs < buffer_rhs.size() && "INDEXED ADD: Right-hand index out of bounds");

    // Coordinate value checks before addition
    assert(buffer_lhs.x[index_lhs] < MAX_X_COORDINATE && "INDEXED ADD: LHS x exceeds max");
    assert(buffer_lhs.y[index_lhs] < MAX_Y_COORDINATE && "INDEXED ADD: LHS y exceeds max");
    assert(buffer_lhs.z[index_lhs] < MAX_Z_COORDINATE && "INDEXED ADD: LHS z exceeds max");

    assert(buffer_rhs.x[index_rhs] < MAX_X_COORDINATE && "INDEXED ADD: RHS x exceeds max");
    assert(buffer_rhs.y[index_rhs] < MAX_Y_COORDINATE && "INDEXED ADD: RHS y exceeds max");
    assert(buffer_rhs.z[index_rhs] < MAX_Z_COORDINATE && "INDEXED ADD: RHS z exceeds max");

    // Add and validate
    double result_x = buffer_lhs.x[index_lhs] + buffer_rhs.x[index_rhs];
    double result_y = buffer_lhs.y[index_lhs] + buffer_rhs.y[index_rhs];
    double result_z = buffer_lhs.z[index_lhs] + buffer_rhs.z[index_rhs];

    assert(result_x < MAX_X_COORDINATE && "INDEXED ADD: result_x exceeds max");
    assert(result_y < MAX_Y_COORDINATE && "INDEXED ADD: result_y exceeds max");
    assert(result_z < MAX_Z_COORDINATE && "INDEXED ADD: result_z exceeds max");

    assert(std::isfinite(result_x) && "INDEXED ADD: result_x is not finite");
    assert(std::isfinite(result_y) && "INDEXED ADD: result_y is not finite");
    assert(std::isfinite(result_z) && "INDEXED ADD: result_z is not finite");

    buffer_out.push_back(result_x, result_y, result_z);
}

inline void VecAddIndexedSingleBuffer(Vec3Buffer& buffer, size_t index_base, size_t index_addend) {
    assert(index_base < buffer.size() && "INDEXED IN-PLACE ADD: base index out of range");
    assert(index_addend < buffer.size() && "INDEXED IN-PLACE ADD: addend index out of range");

    double x_sum = buffer.x[index_base] + buffer.x[index_addend];
    double y_sum = buffer.y[index_base] + buffer.y[index_addend];
    double z_sum = buffer.z[index_base] + buffer.z[index_addend];

    assert(x_sum < MAX_X_COORDINATE && "INDEXED IN-PLACE ADD: x result exceeds max");
    assert(y_sum < MAX_Y_COORDINATE && "INDEXED IN-PLACE ADD: y result exceeds max");
    assert(z_sum < MAX_Z_COORDINATE && "INDEXED IN-PLACE ADD: z result exceeds max");

    assert(std::isfinite(x_sum) && "INDEXED IN-PLACE ADD: result x is not finite");
    assert(std::isfinite(y_sum) && "INDEXED IN-PLACE ADD: result y is not finite");
    assert(std::isfinite(z_sum) && "INDEXED IN-PLACE ADD: result z is not finite");

    buffer.x[index_base] = x_sum;
    buffer.y[index_base] = y_sum;
    buffer.z[index_base] = z_sum;
}



inline void VecSubIndexed(const Vec3Buffer& buffer_minuend, size_t index_minuend, const Vec3Buffer& buffer_subtrahend, size_t index_subtrahend, Vec3Buffer& buffer_out) {
    assert(index_minuend < buffer_minuend.size() && "INDEXED SUB: minuend index out of bounds");
    assert(index_subtrahend < buffer_subtrahend.size() && "INDEXED SUB: subtrahend index out of bounds");

    Vec3_t result = VecSubAtomic(
        {buffer_minuend.x[index_minuend], buffer_minuend.y[index_minuend], buffer_minuend.z[index_minuend]},
        {buffer_subtrahend.x[index_subtrahend], buffer_subtrahend.y[index_subtrahend], buffer_subtrahend.z[index_subtrahend]}
    );

    assert(std::isfinite(result.x) && "INDEXED SUB: result x is not finite");
    assert(std::isfinite(result.y) && "INDEXED SUB: result y is not finite");
    assert(std::isfinite(result.z) && "INDEXED SUB: result z is not finite");

    buffer_out.push_back(result.x, result.y, result.z);
}

inline void VecScaleIndexed(const Vec3Buffer& buffer_base, size_t index_base, double scalar, Vec3Buffer& buffer_out) {
    assert(index_base < buffer_base.size() && "INDEXED SCALE: base index out of bounds");

    Vec3_t result = VecScaleAtomic(
        {buffer_base.x[index_base], buffer_base.y[index_base], buffer_base.z[index_base]},
        scalar
    );

    assert(std::isfinite(result.x) && "INDEXED SCALE: result x is not finite");
    assert(std::isfinite(result.y) && "INDEXED SCALE: result y is not finite");
    assert(std::isfinite(result.z) && "INDEXED SCALE: result z is not finite");

    buffer_out.push_back(result.x, result.y, result.z);
}

inline double VecDotIndexed(const Vec3Buffer& buffer_left, size_t index_left, const Vec3Buffer& buffer_right, size_t index_right) {
    assert(index_left < buffer_left.size() && "INDEXED  DOT: left index out of bounds");
    assert(index_right < buffer_right.size() && "INDEXED DOT: right index out of bounds");

    double dot = VecDotAtomic(
        {buffer_left.x[index_left], buffer_left.y[index_left], buffer_left.z[index_left]},
        {buffer_right.x[index_right], buffer_right.y[index_right], buffer_right.z[index_right]}
    );

    assert(std::isfinite(dot) && "INDEXED DOT: result is not finite");
    return dot;
}

inline void VecCrossIndexed(const Vec3Buffer& buffer_base, size_t index_base, const Vec3Buffer& buffer_operand, size_t index_operand, Vec3Buffer& buffer_out) {
    assert(index_base < buffer_base.size() && "INDEXED CROSS: base index out of bounds");
    assert(index_operand < buffer_operand.size() && "INDEXED CROSS: operand index out of bounds");

    Vec3_t result = VecCrossAtomic(
        {buffer_base.x[index_base], buffer_base.y[index_base], buffer_base.z[index_base]},
        {buffer_operand.x[index_operand], buffer_operand.y[index_operand], buffer_operand.z[index_operand]}
    );

    assert(std::isfinite(result.x) && "INDEXED CROSS: result x is not finite");
    assert(std::isfinite(result.y) && "INDEXED CROSS: result y is not finite");
    assert(std::isfinite(result.z) && "INDEXED CROSS: result z is not finite");

    buffer_out.push_back(result.x, result.y, result.z);
}

inline double VecLengthIndexed(const Vec3Buffer& buffer, size_t index) {
    assert(index < buffer.size() && "INDEXED LENGTH: index out of bounds");

    double len = VecLengthAtomic(
        {buffer.x[index], buffer.y[index], buffer.z[index]}
    );

    assert(std::isfinite(len) && "INDEXED LENGTH: result is not finite");
    return len;
}

inline double VecDistanceIndexed(const Vec3Buffer& buffer_a, size_t index_a,
                                 const Vec3Buffer& buffer_b, size_t index_b) {
    assert(index_a < buffer_a.size() && "VEC DIST: index_a out of bounds");
    assert(index_b < buffer_b.size() && "VEC DIST: index_b out of bounds");

    Vec3_t delta = {
        buffer_a.x[index_a] - buffer_b.x[index_b],
        buffer_a.y[index_a] - buffer_b.y[index_b],
        buffer_a.z[index_a] - buffer_b.z[index_b]
    };

    double distance = VecLengthAtomic(delta);
    assert(std::isfinite(distance) && "VEC DIST: result is not finite");
    return distance;
}

inline void VecLerpIndexed(const Vec3Buffer& buffer_start, size_t index_start,
                           const Vec3Buffer& buffer_end, size_t index_end,
                           double t, Vec3Buffer& buffer_out) {
    assert(index_start < buffer_start.size() && "VEC LERP: start index out of bounds");
    assert(index_end < buffer_end.size() && "VEC LERP: end index out of bounds");

    Vec3_t start = {
        buffer_start.x[index_start],
        buffer_start.y[index_start],
        buffer_start.z[index_start]
    };
    Vec3_t end = {
        buffer_end.x[index_end],
        buffer_end.y[index_end],
        buffer_end.z[index_end]
    };
    Vec3_t result = {
        start.x + (end.x - start.x) * t,
        start.y + (end.y - start.y) * t,
        start.z + (end.z - start.z) * t
    };

    assert(std::isfinite(result.x) && "VEC LERP: result x is not finite");
    assert(std::isfinite(result.y) && "VEC LERP: result y is not finite");
    assert(std::isfinite(result.z) && "VEC LERP: result z is not finite");

    buffer_out.push_back(result.x, result.y, result.z);
}



inline void VecNormalizeIndexed(const Vec3Buffer &v, size_t vi,
                         Vec3Buffer &out) {
    Vec3_t res = VecNormalizeAtomic({v.x[vi], v.y[vi], v.z[vi]});
    out.push_back(res.x, res.y, res.z);
}

inline void VecProject(const Vec3Buffer &a, size_t ai,
                       const Vec3Buffer &b, size_t bi,
                       Vec3Buffer &out) {
    Vec3_t va = {a.x[ai], a.y[ai], a.z[ai]};
    Vec3_t vb = {b.x[bi], b.y[bi], b.z[bi]};
    double scale = VecDotAtomic(va, vb) / VecDotAtomic(vb, vb);
    Vec3_t res = VecScaleAtomic(vb, scale);
    out.push_back(res.x, res.y, res.z);
}

inline void VecAbs(const Vec3Buffer &v, size_t vi,
                   Vec3Buffer &out) {
    out.push_back(
        std::fabs(v.x[vi]),
        std::fabs(v.y[vi]),
        std::fabs(v.z[vi])
    );
}

inline void VecNegateAtomic(const Vec3Buffer &v, size_t vi,
                      Vec3Buffer &out) {
    Vec3_t res = VecNegateAtomic({v.x[vi], v.y[vi], v.z[vi]});
    out.push_back(res.x, res.y, res.z);
}

inline bool VecEqualsAtomic(const Vec3Buffer &a, size_t ai,
                      const Vec3Buffer &b, size_t bi,
                      double epsilon = 1e-6) {
    return VecEqualsAtomic({a.x[ai], a.y[ai], a.z[ai]}, {b.x[bi], b.y[bi], b.z[bi]}, epsilon);
}

constexpr double FovToFocalLength(const double fov) {
    return 1.0 / std::tan((fov / 2.0) * (M_PI / 180.0));
}
