//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "libmesh/libmesh_common.h"
#include "libmesh/petsc_macro.h"

#ifdef LIBMESH_HAVE_PETSC
#if PETSC_VERSION_LESS_THAN(3, 3, 0)
#else

#include "LineSearch.h"

#include <map>

using namespace libMesh;

class PetscProjectSolutionOntoBounds;
class GeometricSearchData;
class PenetrationLocator;
class NonlinearSystemBase;
class DisplacedProblem;

namespace libMesh
{
template <typename>
class PetscNonlinearSolver;
}

template <>
InputParameters validParams<PetscProjectSolutionOntoBounds>();

/**
 *  Petsc implementation of the contact line search (based on the Petsc LineSearchShell)
 */
class PetscProjectSolutionOntoBounds : public LineSearch
{
public:
  PetscProjectSolutionOntoBounds(const InputParameters & parameters);

  void initialSetup() override;
  virtual void lineSearch() override;

protected:
  NonlinearSystemBase & _nl;
  PetscNonlinearSolver<Real> * _solver;
  DisplacedProblem * _displaced_problem;
  const GeometricSearchData * _geometric_search_data;
  const std::map<std::pair<unsigned int, unsigned int>, PenetrationLocator *> *
      _pentration_locators;
};

#endif // PETSC_VERSION_LESS_THAN(3, 3, 0)
#endif // LIBMESH_HAVE_PETSC
