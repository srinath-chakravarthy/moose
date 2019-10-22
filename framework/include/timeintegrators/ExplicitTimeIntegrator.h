//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "TimeIntegrator.h"
#include "MeshChangedInterface.h"

#include "libmesh/linear_solver.h"
#include "libmesh/preconditioner.h"
#include "libmesh/numeric_vector.h"

class ExplicitTimeIntegrator;
class LumpedPreconditioner;

template <>
InputParameters validParams<ExplicitTimeIntegrator>();

/**
 * Base class for explicit time integrators that are implemented without using
 * a nonlinear solver.
 */
class ExplicitTimeIntegrator : public TimeIntegrator, public MeshChangedInterface
{
public:
  ExplicitTimeIntegrator(const InputParameters & parameters);

  virtual void initialSetup() override;
  virtual void init() override;
  virtual void preSolve() override;
  virtual void meshChanged() override;

protected:
  enum SolveType
  {
    CONSISTENT,
    LUMPED,
    LUMP_PRECONDITIONED
  };

  /**
   * Solves a linear system using the chosen solve type
   *
   * @param[in] mass_matrix   Mass matrix
   */
  bool performExplicitSolve(SparseMatrix<Number> & mass_matrix);

  /**
   * Solves a linear system
   *
   * @param[in] mass_matrix   Mass matrix
   */
  bool solveLinearSystem(SparseMatrix<Number> & mass_matrix);

  /**
   * Check for the linear solver convergence
   */
  bool checkLinearConvergence();

  /// Solve type for how mass matrix is handled
  MooseEnum _solve_type;

  /// Residual used for the RHS
  NumericVector<Real> & _explicit_residual;

  /// Solution vector for the linear solve
  NumericVector<Real> & _solution_update;

  /// Diagonal of the lumped mass matrix (and its inversion)
  NumericVector<Real> & _mass_matrix_diag;

  /// Vector of 1's to help with creating the lumped mass matrix
  NumericVector<Real> * _ones;

  /// For computing the mass matrix
  TagID _Ke_time_tag;

  /// For solving with the consistent matrix
  std::unique_ptr<LinearSolver<Number>> _linear_solver;

  /// For solving with lumped preconditioning
  std::unique_ptr<LumpedPreconditioner> _preconditioner;

  /// Save off current time to reset it back and forth
  Real _current_time;
};

/**
 * Helper class to apply lumped mass matrix preconditioner
 */
class LumpedPreconditioner : public Preconditioner<Real>
{
public:
  LumpedPreconditioner(const NumericVector<Real> & diag_inverse)
    : Preconditioner(diag_inverse.comm()), _diag_inverse(diag_inverse)
  {
  }

  virtual void init() override
  {
    // No more initialization needed here
    _is_initialized = true;
  }

  virtual void apply(const NumericVector<Real> & x, NumericVector<Real> & y) override
  {
    y.pointwise_mult(_diag_inverse, x);
  }

protected:
  /// The inverse of the diagonal of the lumped matrix
  const NumericVector<Real> & _diag_inverse;
};
