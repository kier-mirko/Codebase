#ifndef BASE_MATRIX
#define BASE_MATRIX

template <typename T, usize R, usize C>
struct Matrix {
  T values[R][C];

  T& operator[](usize r, usize c) {
    return values[r][c];
  }

  static Matrix identity() {
    Matrix res = {0};

    for (usize r = 0; r < R; ++r) {
      res[r, r] = 1;
    }

    return res;
  }

  Matrix operator+(Matrix &other) {
    Matrix res = {0};

    for (usize r = 0; r < R; ++r) {
      for (usize c = 0; c < C; ++c) {
	res[r, c] = (*this)[r, c] + other[r, c];
      }
    }

    return res;
  }

  Matrix operator-(Matrix &other) {
    Matrix res = {0};

    for (usize r = 0; r < R; ++r) {
      for (usize c = 0; c < C; ++c) {
	res[r, c] = (*this)[r, c] - other.values[r, c];
      }
    }

    return res;
  }

  Matrix operator*(T scalar) {
    Matrix res = {0};

    for (usize r = 0; r < R; ++r) {
      for (usize c = 0; c < C; ++c) {
	res[r, c] = (*this)[r, c] * scalar;
      }
    }

    return res;
  }

  template <usize R1, usize C2>
  Matrix<T, R, C2> operator*(Matrix<T, R1, C2> &other) {
    Assert(R1 == C);
    Matrix<T, R, C2> res = {0};

    for (usize i = 0; i < R; ++i) {
      for (usize j = 0; j < C2; ++j) {
	for (usize r = 0; r < C; ++r) {
	  res[i, j] += (*this)[i, r] * other[r, j];
	}
      }
    }

    return res;
  }

  template <usize R2>
  Matrix operator*(Vector<T, R2> &vec) {
    Assert(C == R2);
    Matrix res = {0};

    for (usize i = 0; i < R; ++i) {
      for (usize j = 0; j < C; ++j) {
	res[i, j] *= vec[j];
      }
    }

    return res;
  }

  Matrix<T, C, R> transpose() {
    Matrix<T, C, R> res = {0};

    for (usize i = 0; i < R; ++i) {
      for (usize j = 0; j < C; ++j) {
	res[j, i] = (*this)[i, j];
      }
    }

    return res;
  }

  Matrix<T, R-1, C-1> submatrix(usize r, usize c) {
    Assert(r < R && c < C);
    Matrix<T, R-1, C-1> res = {0};

    for (usize i = 0, a = 0; a < R; ++a) {
      if (a == r) {continue;}
      for (usize j = 0, b = 0; b < C; ++b) {
	if (b == c) {continue;}
	res[i, j] = (*this)[a, b];
	++j;
      }

      ++i;
    }

    return res;
  }

  T trace() {
    Assert(R == C);
    T res = 0;

    for (usize i = 0; i < R; ++i) {
      res += (*this)[i, i];
    }

    return res;
  }

  T det() {
    Assert(R == C);

    if constexpr (R == 1) {
      return (*this)[0, 0];
    } else if constexpr (R == 2) {
      return (*this)[0, 0] * (*this)[1, 1]
	     - (*this)[0, 1] * (*this)[1, 0];
    } else {
      T res = 0;
      for (usize i = 0; i < R; ++i) {
	res += pow(-1, i) * (*this)[0, i]
	       * this->submatrix(0, i).det();
      }

      return res;
    }
  }

  usize rank() {
    constexpr local f64 eps (1E-9);
    usize rank = 0;

    u32 selected = 0;
    for (usize i = 0; i < C; ++i) {
      usize j = 0;
      for (; j < R; ++j) {
	if (!GetBit(selected, j) && Abs(((*this)[j, i])) > eps) {
	  break;
	}
      }

      if (j != R) {
	++rank;
	SetBit(selected, j, 1);
	for (usize p = i + 1; p < C; ++p) {
	  (*this)[j, p] /= (*this)[j, i];
	}

	for (usize k = 0; k < R; ++k) {
	  if (k != j && Abs(((*this)[k, i])) > eps) {
	    for (usize p = i + 1; p < C; ++p) {
	      (*this)[k, p] -= (*this)[j, p] * (*this)[k, i];
	    }
	  }
	}
      }
    }

    return rank;
  }

  // =======================================================
  // Operations on rows
  Matrix exchange(usize r1, usize r2) {
    Assert(r1 < R && r2 < R);
    Matrix res = {0};

    for (usize i = 0, old_i = 0, k = 0; i < R; ++i, ++k) {
      if (i == r1) {
	old_i = i;
	i = r2;
      } else if (i == r2) {
	old_i = i;
	i = r1;
      }

      for (usize j = 0; j < C; ++j) {
	res[i, j] = (*this)[k, j];
      }

      i = old_i++;
    }

    return res;
  }

  Matrix rowAdd(usize target, usize r) {
    Assert(target < R && r < R);
    Matrix res = {0};

    for (usize i = 0; i < R; ++i) {
      if (i == target) {
	for (usize j = 0; j < C; ++j) {
	  res[i, j] = (*this)[i, j] + (*this)[r, j];
	}
      } else {
	for (usize j = 0; j < C; ++j) {
	  res[i, j] = (*this)[i, j];
	}
      }
    }

    return res;
  }

  Matrix rowMult(usize target, T v) {
    Assert(target < R && v != 0);
    Matrix res = {0};

    for (usize i = 0; i < R; ++i) {
      if (i == target) {
	for (usize j = 0; j < C; ++j) {
	  res[i, j] = (*this)[i, j] * v;
	}
      } else {
	for (usize j = 0; j < C; ++j) {
	  res[i, j] = (*this)[i, j];
	}
      }
    }

    return res;
  }

  // =======================================================
  // Samething but for columns
  Matrix exchangeColumn(usize r1, usize r2) {
    Assert(r1 < C && r2 < C);
    Matrix res = {0};

    for (usize i = 0, old_j = 0; i < R; ++i) {
      for (usize j = 0, k = 0; j < C; ++j, ++k) {
	if (j == r1) {
	  old_j = j;
	  j = r2;
	} else if (j == r2) {
	  old_j = j;
	  j = r1;
	}

	res[i, j] = (*this)[i, k];

	j = old_j++;
      }
    }

    return res;
  }

  Matrix colAdd(usize target, usize c) {
    Assert(target < C && c < C);
    Matrix res = {0};

    for (usize i = 0; i < R; ++i) {
      for (usize j = 0; j < C; ++j) {
	res[i, j] = (j == target
		     ? (*this)[i, j] + (*this)[i, c]
		     : (*this)[i, j]);
      }
    }

    return res;
  }

  Matrix colMult(usize target, T v) {
    Assert(target < C && v != 0);
    Matrix res = {0};

    for (usize i = 0; i < R; ++i) {
      for (usize j = 0; j < C; ++j) {
	res[i, j] = (j == target
		     ? (*this)[i, j] * v
		     : (*this)[i, j]);
      }
    }

    return res;
  }
};

#endif
