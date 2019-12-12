//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ArrayKernel.h"

class ArrayCoupledForce : public ArrayKernel
{
public:
  static InputParameters validParams();

  ArrayCoupledForce(const InputParameters & parameters);

protected:
  virtual RealEigenVector computeQpResidual() override;

  virtual RealEigenMatrix computeQpOffDiagJacobian(MooseVariableFEBase & jvar) override;

private:
  unsigned int _v_var;
  const VariableValue & _v;
  RealEigenVector _coef;
};
