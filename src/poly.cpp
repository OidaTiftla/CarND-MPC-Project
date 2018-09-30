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
    double factor = 1.0;
    if (derivative > 0) {
      factor = factorial(i) / factorial(i - derivative);
    }
    result += factor * coeffs[i] * pow(x, i - derivative);
  }
  return result;
}

CppAD::AD<double> polyHelperPowAD(const CppAD::AD<double> &x, const uint pow) {
  // CppAD::pow cannot handle variables in the second argument
  // here the function is unrolled
  //
  // This is the error, if it is not unrolled:
  // cppad-20180000.0 error from a known source:
  // yq = f.Forward(q, xq): has a non-zero order Taylor coefficient
  // with the value nan (but zero order coefficients are not nan).
  // Error detected by false result for
  //     ok
  // at line 251 in the file
  //     /usr/include/cppad/core/forward.hpp
  // mpc: /usr/include/cppad/utility/error_handler.hpp:206: static void CppAD::ErrorHandler::Default(bool, int, const char*, const char*, const char*): Assertion `false' failed.
  // ./run.sh: line 9:  4436 Aborted                 (core dumped) ./mpc
  if (0 == pow) {
    return CppAD::pow(x, 0);
  }
  if (1 == pow) {
    return CppAD::pow(x, 1);
  }
  if (2 == pow) {
    return CppAD::pow(x, 2);
  }
  if (3 == pow) {
    return CppAD::pow(x, 3);
  }
  if (4 == pow) {
    return CppAD::pow(x, 4);
  }
  if (5 == pow) {
    return CppAD::pow(x, 5);
  }
  if (6 == pow) {
    return CppAD::pow(x, 6);
  }
  if (7 == pow) {
    return CppAD::pow(x, 7);
  }
  if (8 == pow) {
    return CppAD::pow(x, 8);
  }
  if (9 == pow) {
    return CppAD::pow(x, 9);
  }
  if (10 == pow) {
    return CppAD::pow(x, 10);
  }
  if (11 == pow) {
    return CppAD::pow(x, 11);
  }
  if (12 == pow) {
    return CppAD::pow(x, 12);
  }
  if (13 == pow) {
    return CppAD::pow(x, 13);
  }
  if (14 == pow) {
    return CppAD::pow(x, 14);
  }
  if (15 == pow) {
    return CppAD::pow(x, 15);
  }
  if (16 == pow) {
    return CppAD::pow(x, 16);
  }
  if (17 == pow) {
    std::cout << "Error (polyHelperPowAD): for loop must be unrolled for more iterations!!!" << std::endl;
  }
  return 0.0;
}

CppAD::AD<double> polyevalAD(const Eigen::VectorXd &coeffs, const CppAD::AD<double> &x, const uint derivative /*= 0*/) {
  CppAD::AD<double> result = 0.0;
  for (uint i = derivative; i < coeffs.size(); i++) {
    double factor = 1.0;
    if (derivative > 0) {
      factor = factorial(i) / factorial(i - derivative);
    }
    result += factor * coeffs[i] * polyHelperPowAD(x, i - derivative);
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
