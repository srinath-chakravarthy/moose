//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADComputeThermalExpansionEigenstrainBase.h"

#define usingComputeMeanThermalExpansionEigenstrainBaseMembers                                     \
  usingComputeThermalExpansionEigenstrainBaseMembers;                                              \
  using ADComputeMeanThermalExpansionEigenstrainBase<compute_stage>::referenceTemperature;         \
  using ADComputeMeanThermalExpansionEigenstrainBase<compute_stage>::meanThermalExpansionCoefficient

template <ComputeStage>
class ADComputeMeanThermalExpansionEigenstrainBase;

declareADValidParams(ADComputeMeanThermalExpansionEigenstrainBase);

/**
 * ADComputeMeanThermalExpansionEigenstrainBase is a base class for computing the
 * thermal expansion eigenstrain according to a temperature-dependent mean thermal
 * expansion defined in a derived class.  This is defined as the total
 * total linear strain (\f$\delta L / L\f$) at a given temperature relative to a
 * reference temperature at which \f$\delta L = 0\f$.
 *
 * Based on:
 * M. Niffenegger and K. Reichlin. The proper use of thermal expansion coefficients
 * in finite element calculations. Nuclear Engineering and Design, 243:356-359, Feb. 2012.
 */
template <ComputeStage compute_stage>
class ADComputeMeanThermalExpansionEigenstrainBase
  : public ADComputeThermalExpansionEigenstrainBase<compute_stage>
{
public:
  static InputParameters validParams();

  ADComputeMeanThermalExpansionEigenstrainBase(const InputParameters & parameters);

protected:
  /*
   * Compute the total thermal strain relative to the stress-free temperature at the
   * current temperature, as well as the current instantaneous thermal expansion coefficient.
   * param thermal_strain    The current total linear thermal strain (\f$\delta L / L\f$)
   */
  virtual void computeThermalStrain(ADReal & thermal_strain) override;

  /*
   * Get the reference temperature for the mean thermal expansion relationship.  This is
   * the temperature at which \f$\delta L = 0\f$.
   */
  virtual Real referenceTemperature() = 0;

  /*
   * Compute the mean thermal expansion coefficient relative to the reference temperature.
   * This is the linear thermal strain divided by the temperature difference:
   * \f$\bar{\alpha}=(\delta L / L)/(T - T_{ref})\f$.
   * param temperature  temperature at which this is evaluated
   */
  virtual ADReal meanThermalExpansionCoefficient(const ADReal & temperature) = 0;

  usingComputeThermalExpansionEigenstrainBaseMembers;
};
