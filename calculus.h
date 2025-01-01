#ifndef BASE_CALCULUS
#define BASE_CALCULUS

#include "base.h"

// Replace these function with something like:
// https://en.wikipedia.org/wiki/Cauchy%27s_integral_formula
inline fn f64 deriveEps(f64 (*func)(f64), f64 x, f64 h) {
  return (func(x + h) - func(x)) / h;
}

inline fn f64 derive(f64 (*func)(f64), f64 x) {
  return deriveEps(func, x, 1E-8);
}

inline fn f64 secondDerivativeEps(f64 (*func)(f64), f64 x, f64 h) {
  return (func(x) - 2 * func(x - h) + func(x - 2 * h)) / pow(h, 2);
}

inline fn f64 secondDerivative(f64 (*func)(f64), f64 x) {
  return secondDerivativeEps(func, x, 1E-8);
}

#endif
