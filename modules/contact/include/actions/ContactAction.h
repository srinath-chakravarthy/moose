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
#include "MooseTypes.h"
#include "MooseEnum.h"

enum class ContactModel
{
  FRICTIONLESS,
  GLUED,
  COULOMB,
};

enum class ContactFormulation
{
  KINEMATIC,
  PENALTY,
  AUGMENTED_LAGRANGE,
  TANGENTIAL_PENALTY,
  MORTAR
};

class ContactAction;

template <>
InputParameters validParams<ContactAction>();

class ContactAction : public Action
{
public:
  ContactAction(const InputParameters & params);

  virtual void act() override;

  using Action::addRelationshipManagers;
  virtual void addRelationshipManagers(Moose::RelationshipManagerType input_rm_type) override;

  static MooseEnum getModelEnum();
  static MooseEnum getFormulationEnum();
  static MooseEnum getSystemEnum();
  static MooseEnum getSmoothingEnum();

  static InputParameters commonParameters();

protected:
  const BoundaryName _master;
  const BoundaryName _slave;
  const MooseEnum _model;
  const MooseEnum _formulation;
  const MooseEnum _system;
  const MeshGeneratorName _mesh_gen_name;

private:
  void addMortarContact();
  void addNodeFaceContact();
  void addDiracContact();

  std::vector<VariableName> getDisplacementVarNames();
};
