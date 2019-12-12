//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Action.h"

#include "MooseEnum.h"

class RadiationTransferAction : public Action
{
public:
  RadiationTransferAction(const InputParameters & params);
  virtual void act() override;

protected:
  void addMeshGenerator() const;
  void addRadiationObject() const;
  void addViewFactorObject() const;
  void addRadiationBCs() const;

  std::vector<std::vector<std::string>> radiationPatchNames() const;
  UserObjectName viewFactorObjectName() const;
  UserObjectName radiationObjectName() const;

  /// the boundary ids participating in the radiative heat transfer
  std::vector<boundary_id_type> _boundary_ids;

  /// the number of radiation patches per boundary
  std::vector<unsigned int> _n_patches;
};

template <>
InputParameters validParams<RadiationTransferAction>();
