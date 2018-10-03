#ifndef MPC_H
#define MPC_H

#include <vector>
#include "Eigen-3.3/Eigen/Core"
#include "poly.h"

using namespace std;

class MPC {
 public:
  std::vector<double> trajectory_x;
  std::vector<double> trajectory_y;
  std::vector<double> trajectory_psi;
  std::vector<double> trajectory_v;
  std::vector<double> trajectory_cte;
  std::vector<double> trajectory_epsi;

  MPC();

  virtual ~MPC();

  // Solve the model given an initial state and polynomial coefficients.
  // Return the first actuations.
  vector<double> Solve(const Eigen::VectorXd &state, const Eigen::VectorXd &coeffs, const double latency, const double alpha, const double v_ref);
};

#endif /* MPC_H */
