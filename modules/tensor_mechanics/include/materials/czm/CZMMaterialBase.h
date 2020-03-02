//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "InterfaceMaterial.h"
class CZMMaterialBase;
/**
 * This is the base Material class for implementing a traction separation material model.
 * The responsibility of this class is to rotate the displacement jump from global to local
 * coordinate and rotate back traction and traction derivatives.
 * The local coordinate system assumes the following order: opening,
 * tangential1, tangential2. Note that tangential1, tangential2 are arbitrary and therefore the
 * interface assumes an in-plane isotropic behavior. By overriding computeTraction and
 * computeTractionDerivatives in aderived class, different traction separation laws can be
 * implemented. The computeTraction and computeTractionDerivatives methods assumes calculations are
 * performed in the local frame. CZM laws should always be implemented in 3D even if they are going
 * to be used in 2D or 1D simulations. This class assumes small deformations and that the traction
 * separation law is only dependent upon the the displacement jump.
 */
class CZMMaterialBase : public InterfaceMaterial
{
public:
  static InputParameters validParams();
  CZMMaterialBase(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  /// normal to the interface
  const MooseArray<Point> & _normals;

  /// number of displacement components
  const unsigned int _ndisp;

  /// the coupled displacement and neighbor displacement values
  ///@{
  std::vector<const VariableValue *> _disp;
  std::vector<const VariableValue *> _disp_neighbor;
  ///@}

  /// method returning the traction in the interface coordinate system.
  virtual RealVectorValue computeTraction() = 0;

  /// method returning the traction derivitaves wrt local displacement jump.
  virtual RankTwoTensor computeTractionDerivatives() = 0;

  /// the displacement jump in global and local coordiante
  ///@{
  MaterialProperty<RealVectorValue> & _displacement_jump_global;
  MaterialProperty<RealVectorValue> & _displacement_jump;
  ///@}

  /// the value of the traction in global and local coordinates
  ///@{
  MaterialProperty<RealVectorValue> & _traction_global;
  MaterialProperty<RealVectorValue> & _traction;
  ///@}

  /// the traction's derivatives wrt the displacement jump in global and local coordinates
  ///@{
  MaterialProperty<RankTwoTensor> & _traction_derivatives_global;
  MaterialProperty<RankTwoTensor> & _traction_derivatives;
  ///@}
};
