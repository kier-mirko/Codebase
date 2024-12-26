#ifndef BASE_SIMPLEX_H
#define BASE_SIMPLEX_H

// The input must be in the form:
// - All decision variables are non-negative
// - The goal is to maximize the objective function
// - All the constraints are <=
// When there isn't a single feasable solution an invalid one is returned
//   so you should always check if the vector returned from this function
//   is a feasable solution by checking `sol <= known_terms`.
template <usize Constraints, usize Variables>
fn Vector<f32, Variables> simplex(Vector<f32, Variables> objective_terms,
				  Matrix<f32, Constraints, Variables> constraints,
				  Vector<f32, Constraints> known_terms);

#endif
