//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADMortarConstraint.h"

template <ComputeStage>
class TangentialMortarLMMechanicalContact;

template <ComputeStage compute_stage>
class TangentialMortarLMMechanicalContact : public ADMortarConstraint<compute_stage>
{
public:
  static InputParameters validParams();

  TangentialMortarLMMechanicalContact(const InputParameters & parameters);

protected:
  ADReal computeQpResidual(Moose::MortarType) final;

  const MooseVariableFE<Real> & _slave_disp_y;
  const MooseVariableFE<Real> & _master_disp_y;

  const MooseVariableFE<Real> & _contact_pressure_var;

  const ADVariableValue & _contact_pressure;

  const ADVariableValue & _slave_x_dot;
  const ADVariableValue & _master_x_dot;
  const ADVariableValue & _slave_y_dot;
  const ADVariableValue & _master_y_dot;

  const Real _friction_coeff;
  const Real _epsilon;

  const MooseEnum _ncp_type;

  const Real _c;

  usingMortarConstraintMembers;
};
