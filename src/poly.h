#include <cppad/cppad.hpp>
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"

#ifndef POLY_H
#define POLY_H

uint factorial(uint n);

// Evaluate a polynomial.
// If derivative > 0 then the corresponding derivative is evaluated at point x.
double polyeval(const Eigen::VectorXd &coeffs, double x, const uint derivative = 0);

// Evaluate a polynomial.
// If derivative > 0 then the corresponding derivative is evaluated at point x.
CppAD::AD<double> polyevalAD(const Eigen::VectorXd &coeffs, const CppAD::AD<double> &x, const uint derivative = 0);

// Fit a polynomial.
// Adapted from
// https://github.com/JuliaMath/Polynomials.jl/blob/master/src/Polynomials.jl#L676-L716
Eigen::VectorXd polyfit(Eigen::VectorXd xvals, Eigen::VectorXd yvals,
                        int order);

#endif /*POLY_H*/