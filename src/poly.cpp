#include "poly.h"

uint factorial(const uint n) {
  uint result = 1;
  for (uint i = 1; i <= n; ++i) {
    result *= i;
  }
  return result;
}

double polyeval(const Eigen::VectorXd &coeffs, double x, const uint derivative /*= 0*/) {
  double result = 0.0;
  for (uint i = derivative; i < coeffs.size(); i++) {
    result += factorial(i) / factorial(i - derivative) * coeffs[i] * pow(x, i - derivative);
  }
  return result;
}

CppAD::AD<double> polyevalAD(const Eigen::VectorXd &coeffs, const CppAD::AD<double> &x, const uint derivative /*= 0*/) {
  CppAD::AD<double> result = 0.0;
  for (uint i = derivative; i < coeffs.size(); i++) {
    result += factorial(i) / factorial(i - derivative) * coeffs[i] * CppAD::pow(x, i - derivative);
  }
  return result;
}

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
