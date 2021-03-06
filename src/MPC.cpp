#include "MPC.h"
#include <cppad/cppad.hpp>
#include <cppad/ipopt/solve.hpp>
#include "Eigen-3.3/Eigen/Core"

using CppAD::AD;

// TODO: Set the timestep length and duration
size_t N = 10 + 1; // +1 for latency calculation
double dt_global = 0.05;
double T = N * dt_global;

// This value assumes the model presented in the classroom is used.
//
// It was obtained by measuring the radius formed by running the vehicle in the
// simulator around in a circle with a constant steering angle and velocity on a
// flat terrain.
//
// Lf was tuned until the the radius formed by the simulating the model
// presented in the classroom matched the previous radius.
//
// This is the length from front to CoG that has a similar radius.
const double Lf = 2.67;

// The reference velocity the solver should try to reach
double ref_v = 60;
// The maximum velocity the solver should calculate the destination point
double max_v = ref_v * 2;

// The solver takes all the state variables and actuator
// variables in a singular vector. Thus, we should to establish
// when one variable starts and another ends to make our lifes easier.
size_t x_start = 0;
size_t y_start = x_start + N;
size_t psi_start = y_start + N;
size_t v_start = psi_start + N;
size_t cte_start = v_start + N;
size_t epsi_start = cte_start + N;
size_t delta_start = epsi_start + N;
size_t a_start = delta_start + N - 1;

class FG_eval {
 public:
  // Fitted polynomial coefficients
  Eigen::VectorXd coeffs;
  double latency;
  double max_lateral_acc;
  double v_ref;
  FG_eval(const Eigen::VectorXd &coeffs, const double latency, const double max_lateral_acc, const double v_ref) {
    this->coeffs = coeffs;
    this->latency = latency;
    this->max_lateral_acc = max_lateral_acc;
    this->v_ref = v_ref;
  }

  typedef CPPAD_TESTVECTOR(AD<double>) ADvector;
  void operator()(ADvector& fg, const ADvector& vars) {
    // TODO: implement MPC
    // `fg` a vector of the cost constraints
    // `vars` is a vector of variable values (state & actuators)
    // NOTE: You'll probably go back and forth between this function and
    // the Solver function below.

    // The cost is stored is the first element of `fg`.
    // Any additions to the cost should be added to `fg[0]`.
    fg[0] = 0;

    // Reference State Cost
    // Define the cost related the reference state and
    // any anything you think may be beneficial.

    for (int t = 0; t < N; ++t) {
      auto v = vars[v_start + t];
      auto cte = vars[cte_start + t];
      auto epsi = vars[epsi_start + t];

      // The part of the cost based on the reference state.
      fg[0] += CppAD::pow(cte, 2);
      fg[0] += CppAD::pow(epsi, 2);
      fg[0] += 0.5 * CppAD::pow(v - this->v_ref, 2);

      if (t + 1 < N) {
        auto delta = vars[delta_start + t];
        auto a = vars[a_start + t];

        // Minimize the use of actuators.
        fg[0] += CppAD::pow(delta, 2);
        fg[0] += CppAD::pow(a, 2);

        if (t + 2 < N) {
          auto delta_next = vars[delta_start + t + 1];
          auto a_next = vars[a_start + t + 1];

          // // Minimize the velocity in the curve.
          // auto r = Lf / (CppAD::abs(delta_next) + 0.000001);
          // auto v_max = CppAD::sqrt(this->max_lateral_acc * r);
          // fg[0] += 0.003 * CppAD::exp(v - v_max);

          // Minimize the value gap between sequential actuations.
          fg[0] += 400 * CppAD::pow(delta - delta_next, 2);
          // fg[0] += 70 * CppAD::pow(a - a_next, 2);
        }
      }
    }
    // normalize the errors
    fg[0] /= N;

    // The solver should try to reach a point far away from the current position
    auto x_dest = max_v * ((N - 1) * dt_global + this->latency);
    auto y_dest = polyeval(this->coeffs, x_dest);
    auto x_end = vars[x_start + N - 1];
    auto y_end = vars[y_start + N - 1];
    auto distance_to_dest = CppAD::sqrt(CppAD::pow(x_end - x_dest, 2) + CppAD::pow(y_end - y_dest, 2));
    fg[0] += 2 * distance_to_dest;
    // auto distance_to_dest_squared = CppAD::pow(x_end - x_dest, 2) + CppAD::pow(y_end - y_dest, 2);
    // fg[0] += distance_to_dest_squared;

    //
    // Setup Constraints
    //
    // NOTE: In this section you'll setup the model constraints.

    // Initial constraints
    //
    // We add 1 to each of the starting indices due to cost being located at
    // index 0 of `fg`.
    // This bumps up the position of all the other values.
    fg[1 + x_start] = vars[x_start];
    fg[1 + y_start] = vars[y_start];
    fg[1 + psi_start] = vars[psi_start];
    fg[1 + v_start] = vars[v_start];
    fg[1 + cte_start] = vars[cte_start];
    fg[1 + epsi_start] = vars[epsi_start];

    // The rest of the constraints
    for (int t = 0; t < N - 1; ++t) {
      auto dt = dt_global;
      if (t == 0) {
        dt = this->latency;
      }
      auto x0 = vars[x_start + t];
      auto y0 = vars[y_start + t];
      auto psi0 = vars[psi_start + t];
      auto v0 = vars[v_start + t];
      auto cte0 = vars[cte_start + t];
      auto epsi0 = vars[epsi_start + t];
      auto delta0 = vars[delta_start + t];
      auto a0 = vars[a_start + t];

      auto x1 = vars[x_start + t + 1];
      auto y1 = vars[y_start + t + 1];
      auto psi1 = vars[psi_start + t + 1];
      auto v1 = vars[v_start + t + 1];
      auto cte1 = vars[cte_start + t + 1];
      auto epsi1 = vars[epsi_start + t + 1];

      auto f0 = polyevalAD(this->coeffs, x0);
      auto psi_dest0 = CppAD::atan(polyevalAD(this->coeffs, x0, 1));

      // The idea here is to constraint this value to be 0.
      //
      // NOTE: The use of `AD<double>` and use of `CppAD`!
      // This is also CppAD can compute derivatives and pass
      // these to the solver.

      // Setup the model constraints
      fg[1 + x_start + t + 1] = x1 - (x0 + v0 * CppAD::cos(psi0) * dt);
      fg[1 + y_start + t + 1] = y1 - (y0 + v0 * CppAD::sin(psi0) * dt);
      fg[1 + psi_start + t + 1] = psi1 - (psi0 + v0/Lf * delta0 * dt);
      fg[1 + v_start + t + 1] = v1 - (v0 + a0 * dt);
      fg[1 + cte_start + t + 1] = cte1 - (f0 - y0 + v0 * CppAD::sin(epsi0) * dt);
      fg[1 + epsi_start + t + 1] = epsi1 - (psi0 - psi_dest0 + v0/Lf * delta0 * dt);
    }
  }
};

//
// MPC class definition implementation.
//
MPC::MPC() {}
MPC::~MPC() {}

vector<double> MPC::Solve(const Eigen::VectorXd &state, const Eigen::VectorXd &coeffs, const double latency, const double max_lateral_acc, const double v_ref) {
  bool ok = true;
  size_t i;
  typedef CPPAD_TESTVECTOR(double) Dvector;

  double x = state[0];
  double y = state[1];
  double psi = state[2];
  double v = state[3];
  double cte = state[4];
  double epsi = state[5];
  double delta = state[6];
  double a = state[7];

  // TODO: Set the number of model variables (includes both states and inputs).
  // For example: If the state is a 4 element vector, the actuators is a 2
  // element vector and there are 10 timesteps. The number of variables is:
  //
  // 4 * 10 + 2 * 9
  size_t n_vars = N * 6 + (N - 1) * 2;
  // TODO: Set the number of constraints
  size_t n_constraints = N * 6;

  // Initial value of the independent variables.
  // SHOULD BE 0 besides initial state.
  Dvector vars(n_vars);
  for (int i = 0; i < n_vars; i++) {
    vars[i] = 0;
  }

  // Set the initial variable values
  vars[x_start] = x;
  vars[y_start] = y;
  vars[psi_start] = psi;
  vars[v_start] = v;
  vars[cte_start] = cte;
  vars[epsi_start] = epsi;

  Dvector vars_lowerbound(n_vars);
  Dvector vars_upperbound(n_vars);
  // TODO: Set lower and upper limits for variables.

  // Set all non-actuators upper and lowerlimits
  // to the max negative and positive values.
  for (int i = 0; i < delta_start; i++) {
    vars_lowerbound[i] = -1.0e19;
    vars_upperbound[i] = 1.0e19;
  }

  // Set first actuators upper and lowerlimits
  // to the current values.
  vars_lowerbound[delta_start] = max(-0.436332, delta - 0.001);
  vars_upperbound[delta_start] = min(0.436332, delta + 0.001);
  vars_lowerbound[a_start] = max(-1.0, a - 0.001);
  vars_upperbound[a_start] = min(1.0, a + 0.001);

  // The upper and lower limits of delta are set to -25 and 25
  // degrees (values in radians).
  // NOTE: Feel free to change this to something else.
  for (int i = delta_start + 1; i < a_start; i++) {
    vars_lowerbound[i] = -0.436332;
    vars_upperbound[i] = 0.436332;
  }

  // Acceleration/decceleration upper and lower limits.
  // NOTE: Feel free to change this to something else.
  for (int i = a_start + 1; i < n_vars; i++) {
    vars_lowerbound[i] = -1.0;
    vars_upperbound[i] = 1.0;
  }

  // Lower and upper limits for the constraints
  // Should be 0 besides initial state.
  Dvector constraints_lowerbound(n_constraints);
  Dvector constraints_upperbound(n_constraints);
  for (int i = 0; i < n_constraints; i++) {
    constraints_lowerbound[i] = 0;
    constraints_upperbound[i] = 0;
  }
  constraints_lowerbound[x_start] = x;
  constraints_lowerbound[y_start] = y;
  constraints_lowerbound[psi_start] = psi;
  constraints_lowerbound[v_start] = v;
  constraints_lowerbound[cte_start] = cte;
  constraints_lowerbound[epsi_start] = epsi;

  constraints_upperbound[x_start] = x;
  constraints_upperbound[y_start] = y;
  constraints_upperbound[psi_start] = psi;
  constraints_upperbound[v_start] = v;
  constraints_upperbound[cte_start] = cte;
  constraints_upperbound[epsi_start] = epsi;

  // object that computes objective and constraints
  FG_eval fg_eval(coeffs, latency, max_lateral_acc, v_ref);

  //
  // NOTE: You don't have to worry about these options
  //
  // options for IPOPT solver
  std::string options;
  // Uncomment this if you'd like more print information
  options += "Integer print_level  0\n";
  // NOTE: Setting sparse to true allows the solver to take advantage
  // of sparse routines, this makes the computation MUCH FASTER. If you
  // can uncomment 1 of these and see if it makes a difference or not but
  // if you uncomment both the computation time should go up in orders of
  // magnitude.
  options += "Sparse  true        forward\n";
  options += "Sparse  true        reverse\n";
  // NOTE: Currently the solver has a maximum time limit of 0.5 seconds.
  // Change this as you see fit.
  options += "Numeric max_cpu_time          0.5\n";

  // place to return solution
  CppAD::ipopt::solve_result<Dvector> solution;

  // solve the problem
  CppAD::ipopt::solve<Dvector, FG_eval>(
      options, vars, vars_lowerbound, vars_upperbound, constraints_lowerbound,
      constraints_upperbound, fg_eval, solution);

  // Check some of the solution values
  ok &= solution.status == CppAD::ipopt::solve_result<Dvector>::success;

  // Cost
  auto cost = solution.obj_value;
  if (!ok) {
    std::cout << "\033[0;31m"; // switch to red text
    std::cout << "Error could not solve MPC problem!" << std::endl;
    std::cout << "\033[0m"; // reset colors
  }
  std::cout << "Cost " << cost << std::endl;

  trajectory_x.clear();
  trajectory_y.clear();
  trajectory_psi.clear();
  trajectory_v.clear();
  trajectory_cte.clear();
  trajectory_epsi.clear();
  for (int i = 0; i < N; ++i) {
    trajectory_x.push_back(solution.x[x_start + i]);
    trajectory_y.push_back(solution.x[y_start + i]);
    trajectory_psi.push_back(solution.x[psi_start + i]);
    trajectory_v.push_back(solution.x[v_start + i]);
    trajectory_cte.push_back(solution.x[cte_start + i]);
    trajectory_epsi.push_back(solution.x[epsi_start + i]);
  }

  // TODO: Return the first actuator values. The variables can be accessed with
  // `solution.x[i]`.
  //
  // {...} is shorthand for creating a vector, so auto x1 = {1.0,2.0}
  // creates a 2 element double vector.
  return {solution.x[x_start + 1],     solution.x[y_start + 1],
          solution.x[psi_start + 1],   solution.x[v_start + 1],
          solution.x[cte_start + 1],   solution.x[epsi_start + 1],
          solution.x[delta_start + 1], solution.x[a_start + 1]};
}
