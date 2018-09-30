#include <cppad/cppad.hpp>
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"

#ifndef POLY_HPP
#define POLY_HPP

uint factorial(uint n) {
  uint result = 1;
  for (uint i = 1; i <= n; ++i) {
    result *= i;
  }
  return result;
}

// Evaluate a polynomial.
// If derivative > 0 then the corresponding derivative is evaluated at point x.
double polyeval(Eigen::VectorXd coeffs, double x, uint derivative = 0) {
  double result = 0.0;
  for (int i = derivative; i < coeffs.size(); i++) {
    result += factorial(i) / factorial(i - derivative) * coeffs[i] * pow(x, i - derivative);
  }
  return result;
}

// Evaluate a polynomial.
// If derivative > 0 then the corresponding derivative is evaluated at point x.
CppAD::AD<double> polyevalAD(Eigen::VectorXd coeffs, CppAD::AD<double> x, uint derivative = 0) {
  CppAD::AD<double> result = 0.0;
  for (int i = derivative; i < coeffs.size(); i++) {
    result += factorial(i) / factorial(i - derivative) * coeffs[i] * CppAD::pow(x, i - derivative);
  }
  return result;
}

// Fit a polynomial.
// Adapted from
// https://github.com/JuliaMath/Polynomials.jl/blob/master/src/Polynomials.jl#L676-L716
Eigen::VectorXd polyfit(Eigen::VectorXd xvals, Eigen::VectorXd yvals,
                        int order) {
  assert(xvals.size() == yvals.size());
  assert(order >= 1 && order <= xvals.size() - 1);
  Eigen::MatrixXd A(xvals.size(), order + 1);

  for (int i = 0; i < xvals.size(); i++) {
    A(i, 0) = 1.0;
  }

  for (int j = 0; j < xvals.size(); j++) {
    for (int i = 0; i < order; i++) {
      A(j, i + 1) = A(j, i) * xvals(j);
    }
  }

  auto Q = A.householderQr();
  auto result = Q.solve(yvals);
  return result;
}

#endif /*POLY_HPP*/