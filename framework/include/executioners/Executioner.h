//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MooseObject.h"
#include "UserObjectInterface.h"
#include "PostprocessorInterface.h"
#include "Restartable.h"
#include "PerfGraphInterface.h"
#include "FEProblemSolve.h"
#include "PicardSolve.h"

// System includes
#include <string>

class Problem;
class Executioner;

template <>
InputParameters validParams<Executioner>();

/**
 * Executioners are objects that do the actual work of solving your problem.
 */
class Executioner : public MooseObject,
                    public UserObjectInterface,
                    public PostprocessorInterface,
                    public Restartable,
                    public PerfGraphInterface
{
public:
  /**
   * Constructor
   *
   * @param parameters The parameters object holding data for the class to use.
   */
  Executioner(const InputParameters & parameters);

  virtual ~Executioner() {}

  static InputParameters validParams();

  /**
   * Initialize the executioner
   */
  virtual void init() {}

  /**
   * Pure virtual execute function MUST be overridden by children classes.
   * This is where the Executioner actually does it's work.
   */
  virtual void execute() = 0;

  /**
   * Override this for actions that should take place before execution
   */
  virtual void preExecute() {}

  /**
   * Override this for actions that should take place after execution
   */
  virtual void postExecute() {}

  /**
   * Override this for actions that should take place before execution, called by PicardSolve
   */
  virtual void preSolve() {}

  /**
   * Override this for actions that should take place after execution, called by PicardSolve
   */
  virtual void postSolve() {}

  /**
   * Deprecated:
   * Return a reference to this Executioner's Problem instance
   */
  virtual Problem & problem();

  /**
   * Return a reference to this Executioner's FEProblemBase instance
   */
  FEProblemBase & feProblem();

  /** The name of the TimeStepper
   * This is an empty string for non-Transient executioners
   * @return A string of giving the TimeStepper name
   */
  virtual std::string getTimeStepperName() { return std::string(); }

  /**
   * Can be used by subsclasses to call parentOutputPositionChanged()
   * on the underlying FEProblemBase.
   */
  virtual void parentOutputPositionChanged() {}

  /**
   * Whether or not the last solve converged.
   */
  virtual bool lastSolveConverged() const = 0;

  /// Return the underlining FEProblemSolve object.
  FEProblemSolve & feProblemSolve() { return _feproblem_solve; }

  /// Return underlining PicardSolve object.
  PicardSolve & picardSolve() { return _picard_solve; }

  /// Augmented Picard convergence check that to be called by PicardSolve and can be overridden by derived executioners
  virtual bool augmentedPicardConvergenceCheck() const { return false; }

  /**
   * Get the verbose output flag
   * @return The verbose output flag
   */
  const bool & verbose() const { return _verbose; }

  /**
   * Get the number of grid sequencing steps
   */
  unsigned int numGridSteps() const { return _num_grid_steps; }

protected:
  /**
   * Adds a postprocessor that the executioner can directly assign values to
   * @param name The name of the postprocessor to create
   * @param initial_value The initial value of the postprocessor value
   * @return Reference to the postprocessor data that to be used by this executioner
   */
  virtual PostprocessorValue & addAttributeReporter(const std::string & name,
                                                    Real initial_value = 0);

  FEProblemBase & _fe_problem;

  FEProblemSolve _feproblem_solve;
  PicardSolve _picard_solve;

  // Restart
  std::string _restart_file_base;

  /// True if printing out additional information
  const bool & _verbose;

  /// The number of steps to perform in a grid sequencing algorithm. This is one
  /// less than the number of grids requested by the user in their input,
  /// e.g. if they requested num_grids = 1, then there won't be any steps
  /// because we only need to perform one solve per time-step. Storing this
  /// member in this way allows for easy boolean operations, e.g. if (_num_grid_steps)
  /// as opposed to if (_num_grids)
  const unsigned int _num_grid_steps;
};
