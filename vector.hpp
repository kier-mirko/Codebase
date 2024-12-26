#ifndef BASE_VECTOR
#define BASE_VECTOR

#include "base.h"

#include <math.h>

template <typename T, usize D>
struct Vector {
  T values[D];

  template<typename... Ts>
  Vector(Ts... args) : values{(T)args...} {}

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

  f32 length() {
    return sqrt(dot(*this));
  }

  Vector norm() {
    Vector res;

    f32 length = this->length();
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

  Vector operator-(Vector &other) {
    Vector res;
    for (usize i = 0; i < D; ++i) {
      res.values[i] = values[i] - other.values[i];
    }

    return res;
  }

  Vector operator*(Vector &other) {
    Vector res;
    for (usize i = 0; i < D; ++i) {
      res.values[i] = values[i] * other.values[i];
    }

    return res;
  }

  Vector operator*(T scalar) {
    Vector res;
    for (usize i = 0; i < D; ++i) {
      res.values[i] = values[i] * scalar;
    }

    return res;
  }

  T& operator[](usize i) {
    Assert(i < D);
    return values[i];
  }
};

#define Vec2D(TYPE) Vector<TYPE, 2>
#define Vec3D(TYPE) Vector<TYPE, 3>

#endif
