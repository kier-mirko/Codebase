#ifndef BASE_MATRIX
#define BASE_MATRIX

#include <typeinfo>

template <typename T, USZ R, USZ C>
struct Matrix {
  T values[R][C];
  
  template <typename... Ts>
    static Matrix Init(Ts... args) {
    return (Matrix) {
      .values = { (T)(args)... },
    };
  }
  
  static Matrix Identity() {
    Matrix res = {0};
    for (USZ r = 0; r < R; ++r) {
      res[r, r] = 1;
    }
    
    return res;
  }
  
  template <USZ R1, USZ C2>
    Matrix<T, R, C2> mulElementWise(Matrix<T, R1, C2> &other) {
    Assert(R1 == C);
    Matrix<T, R, C2> res = *this;
    
    for (USZ r = 0; r < R; ++r) {
      for (USZ c = 0; c < C; ++c) {
        res[r, c] *= other[r, c];
      }
    }
    
    return res;
  }
  
  Matrix<T, C, R> transpose() {
    Matrix<T, C, R> res = {0};
    
    for (USZ i = 0; i < R; ++i) {
      for (USZ j = 0; j < C; ++j) {
        res[j, i] = (*this)[i, j];
      }
    }
    
    return res;
  }
  
  // Also called a `minor`
  Matrix<T, R-1, C-1> submatrix(USZ r, USZ c) {
    Assert(r < R && c < C);
    Matrix<T, R-1, C-1> res = {0};
    
    for (USZ i = 0, a = 0; a < R; ++a) {
      if (a == r) {continue;}
      for (USZ j = 0, b = 0; b < C; ++b) {
        if (b == c) {continue;}
        res[i, j] = (*this)[a, b];
        ++j;
      }
      
      ++i;
    }
    
    return res;
  }
  
  template <USZ R1, USZ C1>
    Matrix<T, R1, C1> downsize() {
    Assert(R1 <= R && C1 <= C);
    Matrix<T, R1, C1> res = {0};
    for (USZ i = 0; i < R1; ++i) {
      for (USZ j = 0; j < C1; ++j) {
        res[i, j] = (*this)[i, j];
      }
    }
    
    return res;
  }
  
  Vector<T, C> getRow(USZ i) {
    Assert(i < C);
    Vector<T, C> res = {0};
    for (USZ j = 0; j < C; ++j) {
      res[j] = (*this)[i, j];
    }
    
    return res;
  }
  
  Vector<T, R> getCol(USZ i) {
    Assert(i < R);
    Vector<T, R> res = {0};
    for (USZ j = 0; j < R; ++j) {
      res[j] = (*this)[j, i];
    }
    
    return res;
  }
  
  T trace() {
    Assert(R == C);
    T res = 0;
    
    for (USZ i = 0; i < R; ++i) {
      res += (*this)[i, i];
    }
    
    return res;
  }
  
  I32 det() {
    Assert(R == C);
    
    if constexpr (R == 1) {
      return (*this)[0, 0];
    } else if constexpr (R == 2) {
      return (*this)[0, 0] * (*this)[1, 1]
        - (*this)[0, 1] * (*this)[1, 0];
    } else {
      I32 res = 0;
      for (USZ i = 0; i < R; ++i) {
        res += pow(-1, i) * (*this)[0, i]
          * this->submatrix(0, i).det();
      }
      
      return res;
    }
  }
  
  USZ rank() {
    constexpr local F64 eps (1E-9);
    Matrix tmp = *this;
    USZ rank = 0;
    
    U32 selected = 0;
    for (USZ i = 0; i < C; ++i) {
      USZ j = 0;
      for (; j < R; ++j) {
        if (!GetBit(selected, j) && Abs((tmp[j, i])) > eps) {
          break;
        }
      }
      
      if (j != R) {
        ++rank;
        SetBit(selected, j, 1);
        for (USZ p = i + 1; p < C; ++p) {
          tmp[j, p] /= tmp[j, i];
        }
        
        for (USZ k = 0; k < R; ++k) {
          if (k != j && Abs((tmp[k, i])) > eps) {
            for (USZ p = i + 1; p < C; ++p) {
              tmp[k, p] -= tmp[j, p] * tmp[k, i];
            }
          }
        }
      }
    }
    
    return rank;
  }
  
  Matrix inverse() {
    I32 det = this->det();
    Assert(det != 0);
    
    if constexpr (R == 2) {
      Matrix res = {0};
      res[0, 0] = (*this)[1, 1] / det;
      res[0, 1] = -(*this)[0, 1] / det;
      res[1, 0] = -(*this)[1, 0] / det;
      res[1, 1] = (*this)[0, 0] / det;
      
      return res;
    }
    
    Matrix cofactors = {0};
    for (USZ r = 0; r < R; ++r) {
      for (USZ c = 0; c < R; ++c) {
        Matrix<T, R-1, C-1> minor = this->submatrix(r, c);
        cofactors[c, r] = (::pow(-1, r + c) * minor.det()) / det;
      }
    }
    
    return cofactors;
  }
  
  Matrix gramSchmidt() {
    Matrix res = {0};
    Vector<T, R> v1 = getCol(0);
    for (USZ i = 0; i < R; ++i) {
      res[i, 0] = v1[i];
    }
    
    for (USZ i = 1, k = 1; i < R; ++i) {
      Vector<T, R> vk = getCol(i);
      Vector<T, R> uk = vk;
      if (vk == Vector<T, R>()) {
        continue;
      }
      
      for (USZ j = 0; j < k; ++j) {
        Vector<T, R> uj = res.getCol(j);
        uk -= uj.proj(vk);
      }
      
      for (USZ i = 0; i < R; ++i) {
        res[i, k] = uk[i];
      }
      
      ++k;
    }
    
    for (USZ k = 0; k < C; ++k) {
      Vector<T, C> uk = res.getCol(k).normalize();
      for (USZ j = 0; j < R; ++j) {
        res[j, k] = uk[j];
      }
    }
    return res;
  }
  
  String8 toString(Arena *arena, const char *format_for_each_elem) {
    String8List ss = {0};
    for (USZ i = 0; i < R; ++i) {
      for (USZ j = 0; j < C; ++j) {
        stringstreamAppend(arena, &ss,
                           str8_format(arena, format_for_each_elem, (*this)[i, j]));
      }
      
      stringstreamAppend(arena, &ss, Strlit("\n"));
    }
    
    String8 str = str8_list_join(arena, &ss);
    return str8_format(arena, "%ld×%ld Matrix<%s>\n%.*s",
                       R, C, typeid(T).name(), Strexpand(str));
  }
  
  // =======================================================
  // Operations on rows
  Matrix swap(USZ r1, USZ r2) {
    Assert(r1 < R && r2 < R);
    Matrix res = {0};
    
    for (USZ i = 0, old_i = 0, k = 0; i < R; ++i, ++k) {
      if (i == r1) {
        old_i = i;
        i = r2;
      } else if (i == r2) {
        old_i = i;
        i = r1;
      }
      
      for (USZ j = 0; j < C; ++j) {
        res[i, j] = (*this)[k, j];
      }
      
      i = old_i++;
    }
    
    return res;
  }
  
  Matrix rowAdd(USZ target, USZ r) {
    Assert(target < R && r < R);
    Matrix res = {0};
    
    for (USZ i = 0; i < R; ++i) {
      if (i == target) {
        for (USZ j = 0; j < C; ++j) {
          res[i, j] = (*this)[i, j] + (*this)[r, j];
        }
      } else {
        for (USZ j = 0; j < C; ++j) {
          res[i, j] = (*this)[i, j];
        }
      }
    }
    
    return res;
  }
  
  Matrix rowMult(USZ target, T v) {
    Assert(target < R && v != 0);
    Matrix res = {0};
    
    for (USZ i = 0; i < R; ++i) {
      if (i == target) {
        for (USZ j = 0; j < C; ++j) {
          res[i, j] = (*this)[i, j] * v;
        }
      } else {
        for (USZ j = 0; j < C; ++j) {
          res[i, j] = (*this)[i, j];
        }
      }
    }
    
    return res;
  }
  
  // =======================================================
  // Operations on columns
  Matrix swapColumn(USZ r1, USZ r2) {
    Assert(r1 < C && r2 < C);
    Matrix res = {0};
    
    for (USZ i = 0, old_j = 0; i < R; ++i) {
      for (USZ j = 0, k = 0; j < C; ++j, ++k) {
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
  
  Matrix colAdd(USZ target, USZ c) {
    Assert(target < C && c < C);
    Matrix res = {0};
    
    for (USZ i = 0; i < R; ++i) {
      for (USZ j = 0; j < C; ++j) {
        res[i, j] = (j == target
                     ? (*this)[i, j] + (*this)[i, c]
                     : (*this)[i, j]);
      }
    }
    
    return res;
  }
  
  Matrix colMult(USZ target, T v) {
    Assert(target < C && v != 0);
    Matrix res = {0};
    
    for (USZ i = 0; i < R; ++i) {
      for (USZ j = 0; j < C; ++j) {
        res[i, j] = (j == target
                     ? (*this)[i, j] * v
                     : (*this)[i, j]);
      }
    }
    
    return res;
  }
  
  // ===========================================================================
  // Operators
  T& operator[](USZ r, USZ c) {
    return values[r][c];
  }
  
  Matrix operator+(Matrix &other) {
    Matrix res = *this;
    
    for (USZ r = 0; r < R; ++r) {
      for (USZ c = 0; c < C; ++c) {
        res[r, c] += other[r, c];
      }
    }
    
    return res;
  }
  
  Matrix operator-(Matrix &&other) { return (*this) - other; }
  Matrix operator-(Matrix &other) {
    Matrix res = *this;
    
    for (USZ r = 0; r < R; ++r) {
      for (USZ c = 0; c < C; ++c) {
        res[r, c] -= other[r, c];
      }
    }
    
    return res;
  }
  
  Matrix operator*(T scalar) {
    Matrix res = *this;
    
    for (USZ r = 0; r < R; ++r) {
      for (USZ c = 0; c < C; ++c) {
        res[r, c] *= scalar;
      }
    }
    
    return res;
  }
  
  template <USZ R1, USZ C2>
    Matrix<T, R, C2> operator*(Matrix<T, R1, C2> &other) {
    Assert(R1 == C);
    Matrix<T, R, C2> res = {0};
    
    for (USZ i = 0; i < R; ++i) {
      for (USZ j = 0; j < C2; ++j) {
        for (USZ r = 0; r < C; ++r) {
          res[i, j] += (*this)[i, r] * other[r, j];
        }
      }
    }
    
    return res;
  }
  
  void operator*=(Matrix &other) {
    *this = (*this) * other;
  }
  
  B32 operator==(Matrix &other) {
    for (USZ i = 0; i < R; ++i) {
      for (USZ j = 0; j < C; ++j) {
        if ((*this)[i, j] != other[i, j]) {
          return false;
        }
      }
    }
    
    return true;
  }
  
  B32 operator!=(Matrix &other) {
    return !(*this == other);
  }
};

template <typename T, USZ R, USZ C>
fn void fixRoundingErrors(Matrix<T, R, C> *matrix, F64 epsilon = 1E-5) {
  for (USZ i = 0; i < R; ++i) {
    for (USZ j = 0; j < C; ++j) {
      F64 rounded = round((*matrix)[i, j]);
      if (Abs(((*matrix)[i, j]) - rounded) < epsilon) {
        (*matrix)[i, j] = rounded;
      }
    }
  }
}

template <typename T, USZ R, USZ C>
fn Vector<T, R> eigvals(Matrix<T, R, C> *m, F64 epsilon = 1E-8) {
  Matrix q = m->gramSchmidt();
  Matrix r = q.transpose() * (*m);
  Matrix b = r * q;
  
  T last_eig = b[R-1, R-1];
  T diff = 1;
  
  for (USZ iter = 0; iter < 5000 && diff > epsilon; ++iter) {
    q = b.gramSchmidt();
    r = q.transpose() * b;
    b = r * q;
    
    T tot = 0;
    for (USZ i = 0; i < R; ++i) {
      tot += Abs((b[i, i]));
    }
    
    diff = Abs(diff - tot);
  }
  
  Vector<T, R> res;
  for (USZ i = 0; i < R; ++i) {
    res[i] = b[i, i];
  }
  
  return res;
}

#endif
