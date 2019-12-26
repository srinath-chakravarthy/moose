//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADPowerLawCreepStressUpdate.h"

template <ComputeStage compute_stage>
class ADPowerLawCreepExceptionTest;

declareADValidParams(ADPowerLawCreepExceptionTest);

template <ComputeStage compute_stage>
class ADPowerLawCreepExceptionTest : public ADPowerLawCreepStressUpdate<compute_stage>
{
public:
  ADPowerLawCreepExceptionTest<compute_stage>(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual ADReal computeResidual(const ADReal & effective_trial_stress,
                                 const ADReal & scalar) override;
  virtual ADReal computeDerivative(const ADReal & effective_trial_stress,
                                   const ADReal & scalar) override;

  usingRadialReturnCreepStressUpdateBaseMembers;
};
