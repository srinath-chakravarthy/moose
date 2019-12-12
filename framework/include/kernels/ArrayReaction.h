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

class ArrayReaction;

template <>
InputParameters validParams<ArrayReaction>();

class ArrayReaction : public ArrayKernel
{
public:
  static InputParameters validParams();

  ArrayReaction(const InputParameters & parameters);

protected:
  virtual RealEigenVector computeQpResidual() override;
  virtual RealEigenVector computeQpJacobian() override;
  virtual RealEigenMatrix computeQpOffDiagJacobian(MooseVariableFEBase & jvar) override;

  /// scalar diffusion coefficient
  const MaterialProperty<Real> * _r;
  /// array diffusion coefficient
  const MaterialProperty<RealEigenVector> * _r_array;
  /// matrix diffusion coefficient
  const MaterialProperty<RealEigenMatrix> * _r_2d_array;
};
