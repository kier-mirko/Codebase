#include "vec2.h"

inline fn Vec2I16  addVec2I16(Vec2I16 v1, Vec2I16 v2)    {Vec2I16 res = {v1.x + v2.x, v1.y + v2.y};  return res;}
inline fn Vec2I32  addVec2I32(Vec2I32 v1, Vec2I32 v2)    {Vec2I32 res = {v1.x + v2.x, v1.y + v2.y};  return res;}
inline fn Vec2I64  addVec2I64(Vec2I64 v1, Vec2I64 v2)    {Vec2I64 res = {v1.x + v2.x, v1.y + v2.y};  return res;}
inline fn Vec2F32  addVec2F32(Vec2F32 v1, Vec2F32 v2)    {Vec2F32 res = {v1.x + v2.x, v1.y + v2.y};  return res;}
inline fn Vec2F64  addVec2F64(Vec2F64 v1, Vec2F64 v2)    {Vec2F64 res = {v1.x + v2.x, v1.y + v2.y};  return res;}
inline fn Vec2F128 addVec2F128(Vec2F128 v1, Vec2F128 v2) {Vec2F128 res = {v1.x + v2.x, v1.y + v2.y}; return res;}

inline fn Vec2I16  subVec2I16(Vec2I16 v1, Vec2I16 v2)    {Vec2I16 res = {v1.x - v2.x, v1.y - v2.y};  return res;}
inline fn Vec2I32  subVec2I32(Vec2I32 v1, Vec2I32 v2)    {Vec2I32 res = {v1.x - v2.x, v1.y - v2.y};  return res;}
inline fn Vec2I64  subVec2I64(Vec2I64 v1, Vec2I64 v2)    {Vec2I64 res = {v1.x - v2.x, v1.y - v2.y};  return res;}
inline fn Vec2F32  subVec2F32(Vec2F32 v1, Vec2F32 v2)    {Vec2F32 res = {v1.x - v2.x, v1.y - v2.y};  return res;}
inline fn Vec2F64  subVec2F64(Vec2F64 v1, Vec2F64 v2)    {Vec2F64 res = {v1.x - v2.x, v1.y - v2.y};  return res;}
inline fn Vec2F128 subVec2F128(Vec2F128 v1, Vec2F128 v2) {Vec2F128 res = {v1.x - v2.x, v1.y - v2.y}; return res;}

inline fn Vec2I16  multVec2I16(Vec2I16 v1, Vec2I16 v2)    {Vec2I16 res = {v1.x * v2.x, v1.y * v2.y};  return res;}
inline fn Vec2I32  multVec2I32(Vec2I32 v1, Vec2I32 v2)    {Vec2I32 res = {v1.x * v2.x, v1.y * v2.y};  return res;}
inline fn Vec2I64  multVec2I64(Vec2I64 v1, Vec2I64 v2)    {Vec2I64 res = {v1.x * v2.x, v1.y * v2.y};  return res;}
inline fn Vec2F32  multVec2F32(Vec2F32 v1, Vec2F32 v2)    {Vec2F32 res = {v1.x * v2.x, v1.y * v2.y};  return res;}
inline fn Vec2F64  multVec2F64(Vec2F64 v1, Vec2F64 v2)    {Vec2F64 res = {v1.x * v2.x, v1.y * v2.y};  return res;}
inline fn Vec2F128 multVec2F128(Vec2F128 v1, Vec2F128 v2) {Vec2F128 res = {v1.x * v2.x, v1.y * v2.y}; return res;}

inline fn Vec2I16  divVec2I16(Vec2I16 v1, Vec2I16 v2)    {Vec2I16 res = {v1.x / v2.x, v1.y / v2.y};  return res;}
inline fn Vec2I32  divVec2I32(Vec2I32 v1, Vec2I32 v2)    {Vec2I32 res = {v1.x / v2.x, v1.y / v2.y};  return res;}
inline fn Vec2I64  divVec2I64(Vec2I64 v1, Vec2I64 v2)    {Vec2I64 res = {v1.x / v2.x, v1.y / v2.y};  return res;}
inline fn Vec2F32  divVec2F32(Vec2F32 v1, Vec2F32 v2)    {Vec2F32 res = {v1.x / v2.x, v1.y / v2.y};  return res;}
inline fn Vec2F64  divVec2F64(Vec2F64 v1, Vec2F64 v2)    {Vec2F64 res = {v1.x / v2.x, v1.y / v2.y};  return res;}
inline fn Vec2F128 divVec2F128(Vec2F128 v1, Vec2F128 v2) {Vec2F128 res = {v1.x / v2.x, v1.y / v2.y}; return res;}

inline fn Vec2I16  scaleVec2I16(Vec2I16 v, i16 s)    {Vec2I16 res = {v.x * s, v.y * s};  return res;}
inline fn Vec2I32  scaleVec2I32(Vec2I32 v, i32 s)    {Vec2I32 res = {v.x * s, v.y * s};  return res;}
inline fn Vec2I64  scaleVec2I64(Vec2I64 v, i64 s)    {Vec2I64 res = {v.x * s, v.y * s};  return res;}
inline fn Vec2F32  scaleVec2F32(Vec2F32 v, f32 s)    {Vec2F32 res = {v.x * s, v.y * s};  return res;}
inline fn Vec2F64  scaleVec2F64(Vec2F64 v, f64 s)    {Vec2F64 res = {v.x * s, v.y * s};  return res;}
inline fn Vec2F128 scaleVec2F128(Vec2F128 v, f128 s) {Vec2F128 res = {v.x * s, v.y * s}; return res;}
