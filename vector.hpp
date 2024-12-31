#ifndef BASE_VECTOR
#define BASE_VECTOR

#include "base.h"

#include <math.h>

template <typename T, usize D>
struct Vector {
  T values[D];

  inline T& x() {
    return values[0];
  }

  inline T& y() {
    return values[1];
  }

  inline T& z() {
    return values[2];
  }

  inline T& w() {
    return values[3];
  }

  T dot(Vector &other) {
    T res = 0;

    for (usize i = 0; i < D; ++i) {
      res += values[i] * other.values[i];
    }

    return res;
  }

  // the `||vector||` thing
  f32 magnitude() {
    f32 res = 0.f;
    for (usize i = 0; i < D; ++i) {
      res += Abs(values[i]) * Abs(values[i]);
    }

    return sqrtf(res);
  }

  f64 magnitude64() {
    f64 res = 0.f;
    for (usize i = 0; i < D; ++i) {
      res += Abs(values[i]) * Abs(values[i]);
    }

    return sqrt(res);
  }

  // the `vector / ||vector||` thing
  Vector normalize() {
    Vector res = {0};

    f32 length = magnitude();
    if (!length) {
      return res;
    }

    for (usize i = 0; i < D; ++i) {
      res.values[i] = values[i] / length;
    }

    return res;
  }

  Vector operator+(Vector &other) {
    Vector res;
    for (usize i = 0; i < D; ++i) {
      res.values[i] = values[i] + other.values[i];
    }

    return res;
  }

  void operator+=(Vector &other) {
    for (usize i = 0; i < D; ++i) {
      values[i] += other.values[i];
    }
  }

  Vector operator-(Vector &other) {
    Vector res;
    for (usize i = 0; i < D; ++i) {
      res.values[i] = values[i] - other.values[i];
    }

    return res;
  }

  void operator-=(Vector &other) {
    for (usize i = 0; i < D; ++i) {
      values[i] -= other.values[i];
    }
  }

  Vector operator*(Vector &other) {
    Vector res;
    for (usize i = 0; i < D; ++i) {
      res.values[i] = values[i] * other.values[i];
    }

    return res;
  }

  void operator*=(Vector &other) {
    for (usize i = 0; i < D; ++i) {
      values[i] *= other.values[i];
    }
  }

  Vector operator*(T scalar) {
    Vector res;
    for (usize i = 0; i < D; ++i) {
      res.values[i] = values[i] * scalar;
    }

    return res;
  }

  void operator*=(T scalar) {
    for (usize i = 0; i < D; ++i) {
      values[i] *= scalar;
    }
  }

  T& operator[](usize i) {
    Assert(i < D);
    return values[i];
  }
};

#define Vec2D(TYPE) Vector<TYPE, 2>
#define Vec3D(TYPE) Vector<TYPE, 3>

#endif
