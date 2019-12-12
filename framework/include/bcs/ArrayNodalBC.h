//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "NodalBCBase.h"
#include "MooseVariableInterface.h"

// Forward declarations
class ArrayNodalBC;

template <>
InputParameters validParams<ArrayNodalBC>();

/**
 * Base class for deriving any boundary condition that works at nodes on vector variables
 */
class ArrayNodalBC : public NodalBCBase, public MooseVariableInterface<RealEigenVector>
{
public:
  static InputParameters validParams();

  ArrayNodalBC(const InputParameters & parameters);

  /**
   * Gets the variable this BC is active on
   * @return the variable
   */
  virtual ArrayMooseVariable & variable() override { return _var; }
  virtual void computeResidual() override;
  virtual void computeJacobian() override;
  virtual void computeOffDiagJacobian(unsigned int jvar) override;

protected:
  ArrayMooseVariable & _var;

  /// current node being processed
  const Node * const & _current_node;

  /// Value of the unknown variable this BC is acting on
  const RealEigenVector & _u;

  virtual RealEigenVector computeQpResidual() = 0;

  /**
   * The user can override this function to compute the "on-diagonal"
   * Jacobian contribution for this VectorNodalBC.  If not overriden,
   * returns one.
   */
  virtual RealEigenVector computeQpJacobian();

  /**
   * This is the virtual that derived classes should override for
   * computing an off-diagonal jacobian component.
   */
  virtual RealEigenMatrix computeQpOffDiagJacobian(MooseVariableFEBase & jvar);
};
