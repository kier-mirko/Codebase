#include "simplex.hpp"

fn F64 minMaxmaketonMethod(F64 (*func)(F64), F64 x0, F64 epsilon) {
  F64 xk;
  for (USZ i = 0;; ++i, x0 = xk) {
    xk = x0 - (derive(func, x0) / secondDerivative(func, x0));
    if (Abs(xk - x0) <= epsilon) {
      return xk;
    }
  }
}

template <USZ Constraints, USZ Variables>
fn Vector<f32, Variables> simplex(Vector<f32, Variables> objective_terms,
                                  Matrix<f32, Constraints, Variables> constraints,
                                  Vector<f32, Constraints> known_terms) {
  Buffer<USZ, Constraints> basis;
  Matrix<f32, Constraints+1, Constraints+1 + Variables+1> A = {0};
  constexpr USZ B = Constraints + 1 + Variables;
  
  // Copy `objective` into `A`
  for (USZ i = 0; i < Constraints; ++i) {
    for (USZ j = 0; j < Variables; ++j) {
      A[i+1, j+1] = constraints[i, j];
    }
  }
  
  // Copy `bounds` into `B` leaving space for the simplex solution Z
  for (USZ i = 0; i < Constraints; ++i) {
    A[i+1, B] = known_terms[i];
  }
  
  // Setup the first row with `1` follow by `-objective`
  A[0, 0] = 1;
  for (USZ j = 0; j < Variables; ++j) {
    A[0, j+1] = -objective_terms[j];
  }
  
  // Setup the identity for the slack variables
  for (USZ i = 1; i < Constraints+1; ++i) {
    for (USZ j = Variables+1; j < Constraints+1 + Variables; ++j) {
      if (j - Variables == i) {
        A[i, j] = 1;
      }
    }
  }
  
  // Set the basis variables
  for (USZ i = 0; i < Constraints; ++i) {
    basis[i] = i + 1 + Variables;
  }
  
  // =========================================================================
  // The actual simplex algorithm
  while (1) {
    // Finding pivot element
    USZ pivot_column = 0, pivot_row = 0;
    for (USZ j = 1; j < Constraints + 1 + Variables; ++j) {
      if (A[0, j] < A[0, pivot_column] && A[0, j] < 0) {
        pivot_column = j;
      }
    }
    
    if (pivot_column == 0) { break; }
    
    for (USZ i = 1; i < Constraints+1; ++i) {
      if (!pivot_row ||
          (A[i, pivot_column] > 0 &&
           A[i, B]/A[i, pivot_column] < A[pivot_row, B]/A[pivot_row, pivot_column])) {
        pivot_row = i;
      }
    }
    
    if (pivot_row == 0) { break; }
    f32 pivot = A[pivot_row, pivot_column];
    basis[pivot_row - 1] = pivot_column;
    
    for (USZ j = 0; j <= B; ++j) {
      A[pivot_row, j] /= pivot;
    }
    
    for (USZ i = 0; i < Constraints+1; ++i) {
      if (i == pivot_row) { continue; }
      
      f32 update_val = -A[i, pivot_column];
      for (USZ j = 0; j < Constraints+1 + Variables+1; ++j) {
        A[i, j] += A[pivot_row, j] * update_val;
      }
    }
  }
  
  Vector<f32, Variables> res = {0};
  for (USZ i = 0; i < Constraints; ++i) {
    if (basis[i] <= Variables) {
      res[basis[i] - 1] = A[i+1, B];
    }
  }
  
  return res;
}
