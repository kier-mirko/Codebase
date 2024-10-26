#ifndef BASE_VEC2
#define BASE_VEC2

#include "base.h"

typedef union {
  i16 v[2];
  struct {
    i16 x;
    i16 y;
  };
} Vec2I16;

typedef union {
  i32 v[2];
  struct {
    i32 x;
    i32 y;
  };
} Vec2I32;

typedef union {
  i64 v[2];
  struct {
    i64 x;
    i64 y;
  };
} Vec2I64;

typedef union {
  f32 v[2];
  struct {
    f32 x;
    f32 y;
  };
} Vec2F32;

typedef union {
  f64 v[2];
  struct {
    f64 x;
    f64 y;
  };
} Vec2F64;

typedef union {
  f128 v[2];
  struct {
    f128 x;
    f128 y;
  };
} Vec2F128;

inline fn Vec2I16 addVec2I16(Vec2I16 v1, Vec2I16 v2);
inline fn Vec2I32 addVec2I32(Vec2I32 v1, Vec2I32 v2);
inline fn Vec2I64 addVec2I64(Vec2I64 v1, Vec2I64 v2);
inline fn Vec2F32 addVec2F32(Vec2F32 v1, Vec2F32 v2);
inline fn Vec2F64 addVec2F64(Vec2F64 v1, Vec2F64 v2);
inline fn Vec2F128 addVec2F128(Vec2F128 v1, Vec2F128 v2);

inline fn Vec2I16 subVec2I16(Vec2I16 v1, Vec2I16 v2);
inline fn Vec2I32 subVec2I32(Vec2I32 v1, Vec2I32 v2);
inline fn Vec2I64 subVec2I64(Vec2I64 v1, Vec2I64 v2);
inline fn Vec2F32 subVec2F32(Vec2F32 v1, Vec2F32 v2);
inline fn Vec2F64 subVec2F64(Vec2F64 v1, Vec2F64 v2);
inline fn Vec2F128 subVec2F128(Vec2F128 v1, Vec2F128 v2);

inline fn Vec2I16 multVec2I16(Vec2I16 v1, Vec2I16 v2);
inline fn Vec2I32 multVec2I32(Vec2I32 v1, Vec2I32 v2);
inline fn Vec2I64 multVec2I64(Vec2I64 v1, Vec2I64 v2);
inline fn Vec2F32 multVec2F32(Vec2F32 v1, Vec2F32 v2);
inline fn Vec2F64 multVec2F64(Vec2F64 v1, Vec2F64 v2);
inline fn Vec2F128 multVec2F128(Vec2F128 v1, Vec2F128 v2);

inline fn Vec2I16 divVec2I16(Vec2I16 v1, Vec2I16 v2);
inline fn Vec2I32 divVec2I32(Vec2I32 v1, Vec2I32 v2);
inline fn Vec2I64 divVec2I64(Vec2I64 v1, Vec2I64 v2);
inline fn Vec2F32 divVec2F32(Vec2F32 v1, Vec2F32 v2);
inline fn Vec2F64 divVec2F64(Vec2F64 v1, Vec2F64 v2);
inline fn Vec2F128 divVec2F128(Vec2F128 v1, Vec2F128 v2);

inline fn Vec2I16 scaleVec2I16(Vec2I16 v, i16 s);
inline fn Vec2I32 scaleVec2I32(Vec2I32 v, i32 s);
inline fn Vec2I64 scaleVec2I64(Vec2I64 v, i64 s);
inline fn Vec2F32 scaleVec2F32(Vec2F32 v, f32 s);
inline fn Vec2F64 scaleVec2F64(Vec2F64 v, f64 s);
inline fn Vec2F128 scaleVec2F128(Vec2F128 v, f128 s);

#endif
