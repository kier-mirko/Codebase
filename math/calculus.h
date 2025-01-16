#ifndef BASE_CALCULUS
#define BASE_CALCULUS

#include <math.h>
#include "../base.h"

// Replace these function with something like:
// https://en.wikipedia.org/wiki/Cauchy%27s_integral_formula
inline F64 deriveEps(F64 (*func)(F64), F64 x, F64 h) {
  return (func(x + h) - func(x)) / h;
}

inline F64 derive(F64 (*func)(F64), F64 x) {
  return deriveEps(func, x, 1E-8);
}

inline F64 secondDerivativeEps(F64 (*func)(F64), F64 x, F64 h) {
  return (func(x) - 2 * func(x - h) + func(x - 2 * h)) / pow(h, 2);
}

inline F64 secondDerivative(F64 (*func)(F64), F64 x) {
  return secondDerivativeEps(func, x, 1E-8);
}

#endif
